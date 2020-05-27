/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/FileReaderBasic.h"
#include "vrb/Logger.h"

#include "vrb/ConcreteClass.h"

#include <assert.h>
#include <cstring>
#include <fstream>
#include <vector>
#define GLIML_NO_DDS
#define GLIML_NO_PVR
#include "gliml/gliml.h"

namespace vrb {

struct FileReaderBasic::State {
  int trackingHandleCount;
  State()
      : trackingHandleCount(0)
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
  const int imageTargetHandle = m.nextHandle();
  aHandler->BindFileHandle(aFileName, imageTargetHandle);

  std::ifstream input(aFileName, std::ios::binary);
  if (!input) {
    std::string message("Unable to load file: ");
    aHandler->LoadFailed(imageTargetHandle, message + aFileName);
    return;
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  if (buffer.size() == 0) {
    std::string message("File is empty: ");
    aHandler->LoadFailed(imageTargetHandle, message + aFileName);
    return;
  }

  gliml::context loader;
  loader.enable_etc2(true);

  if (!loader.load_ktx(buffer.data(), buffer.size())) {
    std::string message("Failed to parse file: ");
    VRB_ERROR("Error code: %d", loader.error());
    aHandler->LoadFailed(imageTargetHandle, message + aFileName);
    return;
  }

  const size_t length = loader.image_size(0, 0);
  std::unique_ptr<uint8_t[]> image = std::make_unique<uint8_t[]>(length);
  memcpy(image.get(), loader.image_data(0, 0), length);
  aHandler->ProcessImageFile(imageTargetHandle, image, (uint64_t)length, loader.image_width(0, 0), loader.image_height(0, 0), (GLenum)loader.image_internal_format());
}

FileReaderBasic::FileReaderBasic(State& aState) : m(aState) {}
FileReaderBasic::~FileReaderBasic() {}

} // namespace vrb
