#ifndef VRB_FILE_READER_DOT_H
#define VRB_FILE_READER_DOT_H

#include <string>
#include <memory>

namespace vrb {

class FileHandler;
typedef std::shared_ptr<FileHandler> FileHandlerPtr;
class FileReader;
typedef std::shared_ptr<FileReader> FileReaderPtr;

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
  FileHandler(const FileHandler&) = delete;
  FileHandler& operator=(const FileHandler&) = delete;
};

class FileReader {
public:
  virtual void ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) = 0;
  virtual void ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) = 0;
protected:
  FileReader() {}
  virtual ~FileReader() {}
private:
  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;
};

}

#endif // VRB_FILE_READER_DOT_H
