#ifndef VRB_FILE_READER_ANDROID_DOT_H
#define VRB_FILE_READER_ANDROID_DOT_H

#include "vrb/FileReader.h"
#include <jni.h>
#include <memory>

namespace vrb {

class FileReaderAndroid;
typedef std::shared_ptr<FileReaderAndroid> FileReaderAndroidPtr;

class FileReaderAndroid : public FileReader {
public:
  static FileReaderAndroidPtr Create();
  void ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) override;
  void SetAssetManager(JNIEnv* aEnv, jobject& aAssetManager);
  void ClearAssetManager();
protected:
  FileReaderAndroid();
  ~FileReaderAndroid();

  struct State;
  State* m;

private:
  FileReaderAndroid(const FileReaderAndroid&) = delete;
  FileReaderAndroid& operator=(const FileReaderAndroid&) = delete;
};

} // namespace vrb


#endif // VRB_FILE_READER_ANDROID_DOT_H
