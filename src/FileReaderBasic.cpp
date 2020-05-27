/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/FileReaderBasic.h"
#include "vrb/Logger.h"

#include "vrb/ConcreteClass.h"

#include <fstream>
#include <vector>

namespace vrb {

struct FileReaderBasic::State {
  int trackingHandleCount;
  int imageTargetHandle;
  FileHandlerPtr imageTarget;
  State()
      : trackingHandleCount(0)
      , imageTargetHandle(0)
  {}

  int nextHandle() {
    trackingHandleCount++;
    return trackingHandleCount;
  }

  void readRawFile(const std::string& aFileName, FileHandlerPtr aHandler) {
    const int handle = nextHandle();
    aHandler->BindFileHandle(aFileName, handle);
    std::ifstream input(aFileName, std::ios::binary);
    if (!input) {
      std::string message("Unable to load file: ");
      message += aFileName;
      aHandler->LoadFailed(handle, message);
      return;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    if (buffer.size() > 0) {
      aHandler->ProcessRawFileChunk(handle, buffer.data(), buffer.size());
      aHandler->FinishRawFile(handle);
    } else {
      aHandler->LoadFailed(handle, "Error while reading file");
    }
  }

private:
  State(const State&) = delete;
  State& operator=(const State&) = delete;
};

FileReaderBasicPtr
FileReaderBasic::Create() {
  return std::make_shared<ConcreteClass<FileReaderBasic, FileReaderBasic::State> >();
}

void
FileReaderBasic::ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  m.readRawFile(aFileName, aHandler);
}

void
FileReaderBasic::ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  if (!aHandler) {
    return;
  }
  m.imageTarget = aHandler;
  m.imageTargetHandle = m.nextHandle();
  m.imageTarget->BindFileHandle(aFileName, m.imageTargetHandle);

  ImageFileLoadFailed(m.imageTargetHandle, "No Image Loader");
}

void
FileReaderBasic::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]> &aImage, const uint64_t aImageLength, const int aWidth, const int aHeight, const GLenum aFormat) {
  if (m.imageTargetHandle != aFileHandle) {
    return;
  }

  m.imageTargetHandle = 0;
  if (!m.imageTarget) {
    return;
  }

  m.imageTarget->ProcessImageFile(aFileHandle, aImage, aImageLength, aWidth, aHeight, aFormat);
  m.imageTarget = nullptr;
}


void
FileReaderBasic::ImageFileLoadFailed(const int aFileHandle, const std::string& aReason) {
  if (!m.imageTarget || (m.imageTargetHandle != aFileHandle)) {
    return;
  }

  m.imageTarget->LoadFailed(aFileHandle, aReason);
  m.imageTargetHandle = 0;
  m.imageTarget = nullptr;
}

FileReaderBasic::FileReaderBasic(State& aState) : m(aState) {}
FileReaderBasic::~FileReaderBasic() {}

} // namespace vrb
