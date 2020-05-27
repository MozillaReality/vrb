/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_FILE_READER_BASIC_DOT_H
#define VRB_FILE_READER_BASIC_DOT_H

#include "vrb/FileReader.h"
#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"
#include <memory>

namespace vrb {

class FileReaderBasic : public FileReader {
public:
  static FileReaderBasicPtr Create();
  void ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const uint64_t aImageLength, const int aWidth, const int aHeight, const GLenum aFormat);
  void ImageFileLoadFailed(const int aFileHandle, const std::string& aReason);
protected:
  struct State;
  FileReaderBasic(State& aState);
  ~FileReaderBasic();
private:
  State& m;
  FileReaderBasic() = delete;
  VRB_NO_DEFAULTS(FileReaderBasic)
};

} // namespace vrb


#endif // VRB_FILE_READER_BASIC_DOT_H
