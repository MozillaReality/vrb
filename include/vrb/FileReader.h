/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_FILE_READER_DOT_H
#define VRB_FILE_READER_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <string>
#include <memory>

namespace vrb {

class FileHandler {
public:
  virtual void BindFileHandle(const std::string& aFileName, const int aFileHandle) = 0;
  virtual void LoadFailed(const int aFileHandle, const std::string& aReason) = 0;
  virtual void ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) = 0;
  virtual void FinishRawFile(const int aFileHandle) = 0;
  virtual void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) = 0;
protected:
  FileHandler() {}
  virtual ~FileHandler() {}
private:
  VRB_NO_DEFAULTS(FileHandler)
};

class FileReader {
public:
  virtual void ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) = 0;
  virtual void ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) = 0;
protected:
  FileReader() {}
  virtual ~FileReader() {}
private:
  VRB_NO_DEFAULTS(FileReader)
};

}

#endif // VRB_FILE_READER_DOT_H
