/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ParserObj.h"

#include "vrb/ConcreteClass.h"
#include "vrb/CreationContext.h"
#include "vrb/Vector.h"

#include <string>
#include <vector>

namespace {

const char cLF = char(10);
const char cRF = char(13);
const char cSpace = ' ';
const char cTab = '\t';

static int
LocalStoi(const std::string& aValue) {
  int result = 0;
  try {
    result = std::stoi(aValue);
  } catch (std::invalid_argument& e) {
  } catch (std::out_of_range& e) {
  }
  return result;
}
static float
LocalStof(const std::string& aValue) {
  float result = 0;
  try {
    result = std::stof(aValue);
  } catch (std::invalid_argument& e) {
  } catch (std::out_of_range& e) {
  }
  return result;
}

static std::string
Trim(const std::string& aStr) {
  size_t start = aStr.find_first_not_of(" ");
  size_t end = aStr.find_last_not_of(" ") + 1;
  return aStr.substr(start, end - start);
}

static std::string
TokenizeWhiteSpace(const std::string& aBuffer, std::vector<std::string>& aTokens) {
  const size_t bufferLength = aBuffer.length();
  const char* buffer = aBuffer.c_str();
  std::string result;
  int charCount = 0;
  size_t begin = 0;
  size_t place = 0;
  bool first = true;

  while (place < bufferLength) {
    const char value = buffer[place];
    if ((value == cSpace) || (value == cTab)) {
      if (charCount > 0) {
        if (first) {
          result = aBuffer.substr(begin, place - begin);
          first = false;
        } else {
          aTokens.push_back(aBuffer.substr(begin, place - begin));
        }
        charCount = 0;
      }
    } else {
      if (charCount == 0) {
        begin = place;
      }
      charCount++;
    }
    place++;
  }

  if (charCount > 0) {
    if (first) { result = aBuffer.substr(begin); }
    else { aTokens.push_back(aBuffer.substr(begin)); }
  }
  return result;
}

static std::string
TokenizeBuffer(std::string& aBuffer, std::vector<std::string>& aTokens) {
  size_t end = aBuffer.find("#");
  if (end != std::string::npos) { aBuffer.erase(end); }
  return TokenizeWhiteSpace(aBuffer, aTokens);
}

static size_t
TokenizeDelimiter(const std::string& aStr, const char aDelimiter, std::vector<std::string>& aTokens) {
  const char* buffer = aStr.c_str();
  size_t start = 0;
  size_t place = 0;
  while (buffer[place] != '\0') {
    if (buffer[place] == aDelimiter) {
      aTokens.push_back(aStr.substr(start, place - start));
      start = place + 1;
    }
    place++;
  }
  aTokens.push_back(aStr.substr(start));
  return aTokens.size();
}

class LineParser {
public:
  virtual void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) = 0;
};

class VectorParser : public LineParser {
public:
  void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) override;
protected:
  VectorParser(const float aDefaultValue) : mDefaultValue(aDefaultValue) {}
  virtual void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) = 0;
  float GetValue(const std::vector<std::string>& aTokens, const int place);
  float mDefaultValue;

private:
  VectorParser() = delete;
  VectorParser(const VectorParser&) = delete;
  VectorParser& operator=(const VectorParser&) = delete;
};

class ColorParser : public VectorParser {
public:
  void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) override;

protected:
  ColorParser(const float aDefaultValue) : VectorParser(aDefaultValue) {}
};

class VertexParser : public VectorParser {
public:
  VertexParser() : VectorParser(0.0f) {}
  ~VertexParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.AddVertex(aVector, aW);
  }
};

class NormalParser : public VectorParser {
public:
  NormalParser() : VectorParser(0.0f) {}
  ~NormalParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.AddNormal(aVector);
  }
};

class UVParser : public VectorParser {
public:
  UVParser() : VectorParser(1.0f) {}
  ~UVParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.AddUV(aVector.x(), 1.0f - aVector.y(), aVector.z());
  }
};

class AmbientParser : public ColorParser {
public:
  AmbientParser() : ColorParser(1.0f) {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.SetAmbientColor(aVector);
  }
};

class DiffuseParser : public ColorParser {
public:
  DiffuseParser() : ColorParser(1.0f) {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.SetDiffuseColor(aVector);
  }
};

class SpecularParser : public ColorParser {
public:
  SpecularParser() : ColorParser(1.0f) {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserverObj& aObserver) override {
    aObserver.SetSpecularColor(aVector);
  }
};

class FaceParser : public LineParser {
public:
  FaceParser() {}
  ~FaceParser() {}
  void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) override;
};

void
VectorParser::Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) {
  SetVector(vrb::Vector(GetValue(aTokens, 0), GetValue(aTokens, 1), GetValue(aTokens, 2)), GetValue(aTokens, 3), aObserver);
}

void
ColorParser::Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) {
  SetVector(vrb::Vector(GetValue(aTokens, -3), GetValue(aTokens, -2), GetValue(aTokens, -1)), 1.0f, aObserver);
}

float
VectorParser::GetValue(const std::vector<std::string>& aTokens, const int place) {
  if (place >= 0) {
    return aTokens.size() > place ? LocalStof(aTokens[place]) : mDefaultValue;
  } else {
    // Since place < 0 then endPlace < aTokens.size()
    const int endPlace = aTokens.size() + place;
    return endPlace >= 0 ? LocalStof(aTokens[endPlace]) : mDefaultValue;
  }
}

void
FaceParser::Parse(const std::vector<std::string>& aTokens, vrb::ParserObserverObj& aObserver) {
  const size_t tokensSize = aTokens.size();
  std::vector<int> vertices;
  vertices.reserve(4);
  std::vector<int> uvs;
  uvs.reserve(4);
  std::vector<int> normals;
  normals.reserve(4);
  for (size_t ix = 0; ix < tokensSize; ix++) {
    std::vector<std::string> vertexTokens;
    vertexTokens.reserve(3);
    size_t vertexSize = TokenizeDelimiter(aTokens[ix], '/', vertexTokens);
    int index[3] = { 0, 0, 0 };
    for (size_t jy = 0; jy < vertexSize; jy++) {
      if ((jy < 3) && !vertexTokens[jy].empty()) { index[jy] = LocalStoi(vertexTokens[jy]); }
    }
    vertices.push_back(index[0]);
    uvs.push_back(index[1]);
    normals.push_back(index[2]);
  }
  aObserver.AddFace(vertices, uvs, normals);
}

} // namespace

namespace vrb {

struct ParserObj::State {
  std::weak_ptr<ParserObj> self;
  FileReaderPtr fileReader;
  std::weak_ptr<ParserObserverObj> weakObserver;
  std::string objFileName;
  std::string mtlFileName;
  int objFileHandle;
  int mtlFileHandle;
  std::string objLineBuffer;
  std::string mtlLineBuffer;
  VertexParser vertexParser;
  NormalParser normalParser;
  UVParser uvParser;
  FaceParser faceParser;
  AmbientParser ambientParser;
  DiffuseParser diffuseParser;
  SpecularParser specularParser;

  State()
      : objFileHandle(0)
      , mtlFileHandle(0)
    {
}

  std::string GetAbsolutePath(const std::string& aRelativePath) const;
  std::string* GetBuffer(const int aFileHandle);
  void Parse(const int aFileHandle, FileHandler& aFileHandler);
  void Finish(const int aFileHandle);
  void ParseObj(FileHandler& aFileHandler);
  void ParseMtl();
};

std::string
ParserObj::State::GetAbsolutePath(const std::string& aRelativePath) const  {
  size_t pos = objFileName.rfind("/");
  if (pos == std::string::npos) {
    return aRelativePath;
  }
  return objFileName.substr(0, pos + 1) + aRelativePath;
}

std::string*
ParserObj::State::GetBuffer(const int aFileHandle) {
  if (aFileHandle == objFileHandle) { return &objLineBuffer; }
  else if (aFileHandle == mtlFileHandle) { return &mtlLineBuffer; }
  else { return nullptr; }
}

void
ParserObj::State::Parse(const int aFileHandle, FileHandler& aFileHandler) {
  if (aFileHandle == objFileHandle) { ParseObj(aFileHandler); }
  else if (aFileHandle == mtlFileHandle) { ParseMtl(); }
  else {

  }
}

void
ParserObj::State::Finish(const int aFileHandle) {
  ParserObserverObjPtr observer = weakObserver.lock();
  if (aFileHandle == objFileHandle) {
    if (observer) { observer->FinishModel(); }
    objFileHandle = 0;
  } else if (aFileHandle == mtlFileHandle) {
    if (observer) { observer->FinishMaterialFile(); }
    mtlFileHandle = 0;
    mtlFileName.clear();
  }
}

void
ParserObj::State::ParseObj(FileHandler& aFileHandler) {
  ParserObserverObjPtr observer = weakObserver.lock();
  if (!objLineBuffer.empty() && observer) {
    std::vector<std::string> tokens;
    const std::string type = TokenizeBuffer(objLineBuffer, tokens);
    LineParser *currentParser = nullptr;
    if (type == "#") {
      // Found line comment;
    } else if (type == "v") {
      currentParser = &vertexParser;
    } else if (type == "vn") {
      currentParser = &normalParser;
    } else if (type == "vt") {
      currentParser = &uvParser;
    } else if (type == "f") {
      currentParser = &faceParser;
    } else if (type == "g") {
      observer->SetGroupNames(tokens);
    } else if (type == "o") {
      observer->SetObjectName(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "mtllib") {

      mtlFileName = tokens.size() > 0 ? GetAbsolutePath(tokens[0]) : "";
      if (fileReader) {
        fileReader->ReadRawFile(mtlFileName, self.lock());
      }
      observer->LoadMaterialLibrary(mtlFileName);
    } else if (type == "usemtl") {
      observer->SetMaterialName(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "s") {
      std::string value = tokens.size() > 0 ? tokens[0] : "0";
      if (value == "off") { value = "0"; }
      int group = LocalStoi(value);
      observer->SetSmoothingGroup(group);
    } else if (type.empty()) {
      // Ignore empty lines
    } else {
      VRB_WARN("Unknown OBJ type: '%s'", type.c_str());
    }

    if (currentParser) {
      currentParser->Parse(tokens, *observer.get());
    }
  }
  objLineBuffer.clear();
}

void
ParserObj::State::ParseMtl() {
  ParserObserverObjPtr observer = weakObserver.lock();
  if (!mtlLineBuffer.empty() && observer) {
    std::vector<std::string> tokens;
    const std::string type = TokenizeBuffer(mtlLineBuffer, tokens);
    LineParser *currentParser = nullptr;

    if (type == "#") {
      // Found line comment
    } else if (type == "newmtl") {
      observer->CreateMaterial(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "Ka") {
      currentParser = &ambientParser;
    } else if (type == "Ks") {
      currentParser = &specularParser;
    } else if (type == "Kd") {
      currentParser = &diffuseParser;
    } else if (type == "Ns") {
      observer->SetSpecularExponent(LocalStof(tokens.size() > 0 ? tokens[tokens.size() - 1] : "1.0"));
    } else if (type == "map_Ka") {
      observer->SetAmbientTexture(tokens.size() > 0 ? GetAbsolutePath(tokens[tokens.size() - 1]) : "");
    } else if (type == "map_Kd") {
      observer->SetDiffuseTexture(tokens.size() > 0 ? GetAbsolutePath(tokens[tokens.size() - 1]) : "");
    } else if (type == "map_Ks") {
      observer->SetSpecularTexture(tokens.size() > 0 ? GetAbsolutePath(tokens[tokens.size() - 1]) : "");
    } else if (type == "illum") {
      observer->SetIlluniationModel(LocalStoi(tokens.size() > 0 ? tokens[tokens.size() - 1] : "1"));
    }else {
      // Unhandled tag
      VRB_WARN("In file: '%s' Unhandled mtl option: '%s'", mtlFileName.c_str(), mtlLineBuffer.c_str());
    }

    if (currentParser) {
      currentParser->Parse(tokens, *observer.get());
    }
  }
  mtlLineBuffer.clear();
}

ParserObjPtr
ParserObj::Create(CreationContextPtr& aContext) {
  ParserObjPtr self = std::make_shared<ConcreteClass<ParserObj, ParserObj::State> >(aContext);
  self->m.self = self;
  return self;
}

void
ParserObj::BindFileHandle(const std::string& aFileName, const int aFileHandle) {
  ParserObserverObjPtr observer = m.weakObserver.lock();
  if (!m.mtlFileName.empty() && (aFileName == m.mtlFileName)) {
    m.mtlFileHandle = aFileHandle;
    m.mtlLineBuffer.clear();
    if (observer) { observer->StartMaterialFile(aFileName); }
  } else {
    m.objFileHandle = aFileHandle;
    m.objFileName = aFileName;
    m.objLineBuffer.clear();
    if (observer) { observer->StartModel(aFileName); }
  }
}

void
ParserObj::LoadFailed(const int aFileHandle, const std::string& aReason) {
  VRB_ERROR("Failed to load: %s", aReason.c_str());
  FinishRawFile(aFileHandle);
}

void
ParserObj::ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) {
  std::string* lineBuffer = m.GetBuffer(aFileHandle);

  if (!lineBuffer) {
    VRB_ERROR("Failed to find line buffer of file handle: %d", aFileHandle);
    return;
  }
  size_t place = 0;
  size_t start = 0;

  while(place < aSize) {
    if ((aBuffer[place] == cLF) || (aBuffer[place] == cRF)) {
      if ((place - start) > 0) {
        lineBuffer->append(&(aBuffer[start]), place - start);
      }
      m.Parse(aFileHandle, *this);
      start = place + 1;
    }
    place++;
  }

  if (start < aSize) {
    lineBuffer->append(&(aBuffer[start]), aSize - start);
  }
}

void
ParserObj::FinishRawFile(const int aFileHandle) {
  m.Parse(aFileHandle, *this);
  m.Finish(aFileHandle);

}

void
ParserObj::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const uint64_t aImageLength, const int aWidth, const int aHeight, const GLenum aFormat) {

}

void
ParserObj::LoadModel(const std::string& aFileName) {
  if (m.fileReader) {
    VRB_LOG("Loading file: '%s'", aFileName.c_str());
    m.fileReader->ReadRawFile(aFileName, m.self.lock());
  } else {
    VRB_ERROR("ParserObj unable to load file: '%s'. FileReader not set", aFileName.c_str());
  }
}

void
ParserObj::SetFileReader(FileReaderPtr aFileReader) {
  m.fileReader = aFileReader;
}

void
ParserObj::ClearFileReader() {
  m.fileReader = nullptr;
}

void
ParserObj::SetObserver(ParserObserverObjPtr aObserver) {
  m.weakObserver = aObserver;
}

ParserObj::ParserObj(State& aState, CreationContextPtr& aContext) : m(aState) {
  m.fileReader = aContext->GetFileReader();
}

ParserObj::~ParserObj() {}

} // namespace vrb
