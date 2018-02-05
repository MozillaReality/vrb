/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_FILE_READER_ANDROID_DOT_H
#define VRB_FILE_READER_ANDROID_DOT_H

#include "vrb/FileReader.h"
#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <jni.h>
#include <memory>

namespace vrb {

class FileReaderAndroid : public FileReader {
public:
  static FileReaderAndroidPtr Create(ContextWeak& aContext);
  void ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void Init(JNIEnv* aEnv, jobject& aAssetManager);
  void Shutdown();
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight);
  void ImageFileLoadFailed(const int aFileHandle, const std::string& aReason);
protected:
  struct State;
  FileReaderAndroid(State& aState, ContextWeak& aContext);
  ~FileReaderAndroid();
private:
  State& m;
  FileReaderAndroid() = delete;
  VRB_NO_DEFAULTS(FileReaderAndroid)
};

} // namespace vrb


#endif // VRB_FILE_READER_ANDROID_DOT_H
