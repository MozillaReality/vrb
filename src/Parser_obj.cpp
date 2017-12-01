#include "vrb/Parser_obj.h"
#include <string>
#include <vector>
#include <iostream>

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
TokenizeWhiteSpace(const std::string aBuffer, std::vector<std::string>& aTokens) {
   const size_t bufferLength = aBuffer.length();
   std::string result;
   std::string inProcess;
   size_t place = 0;
   bool first = true;

   while (place < bufferLength) {
     if ((aBuffer[place] == cSpace) || (aBuffer[place] == cTab)) {
       if (!inProcess.empty()) {
         if (first) {
           result = inProcess;
           first = false;
         } else { aTokens.push_back(inProcess); }
         inProcess.clear();
       }
     } else {
       inProcess.push_back(aBuffer[place]);
     }
     place++;
   }

   if (!inProcess.empty()) {
     if (first) { result = inProcess; }
     else { aTokens.push_back(inProcess); }
     inProcess.clear();
   }

   return result;
}

static size_t
TokenizeDelimiter(const std::string& aStr, const std::string& aDelimiter, std::vector<std::string>& aTokens) {
  bool done = false;
  size_t start = 0;
  size_t end = 0;
  while (!done) {
    end = aStr.find(aDelimiter, start);
    if (end == std::string::npos) {
      done = true;
      end = aStr.length();
    }

    std::string token = aStr.substr(start, end - start);
    aTokens.push_back(aStr.substr(start, end - start));

    start = end + aDelimiter.length();
  }
  return aTokens.size();

}

class LineParser {
public:
  virtual void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserver_obj& aObserver) = 0;
};

class VectorParser : public LineParser {
public:
  void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserver_obj& aObserver) override;
protected:
  VectorParser(const float aDefaultValue) : mDefaultValue(aDefaultValue) {}
  virtual void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserver_obj& aObserver) = 0;
  float GetValue(const std::vector<std::string>& aTokens, const size_t place);
  float mDefaultValue;

private:
  VectorParser() = delete;
  VectorParser(const VectorParser&) = delete;
  VectorParser& operator=(const VectorParser&) = delete;
};

class VertexParser : public VectorParser {
public:
  VertexParser() : VectorParser(0.0f) {}
  ~VertexParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserver_obj& aObserver) override {
    aObserver.AddVertex(aVector, aW);
  }
};

class NormalParser : public VectorParser {
public:
  NormalParser() : VectorParser(0.0f) {}
  ~NormalParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserver_obj& aObserver) override {
    aObserver.AddNormal(aVector);
  }
};

class UVParser : public VectorParser {
public:
  UVParser() : VectorParser(1.0f) {}
  ~UVParser() {}
protected:
  void SetVector(const vrb::Vector& aVector, const float aW, vrb::ParserObserver_obj& aObserver) override {
    aObserver.AddUV(aVector.x(), aVector.y(), aVector.z());
  }
};

class FaceParser : public LineParser {
public:
  FaceParser() {}
  ~FaceParser() {}
  void Parse(const std::vector<std::string>& aTokens, vrb::ParserObserver_obj& aObserver) override;
};

void
VectorParser::Parse(const std::vector<std::string>& aTokens, vrb::ParserObserver_obj& aObserver) {
  SetVector(vrb::Vector(GetValue(aTokens, 0), GetValue(aTokens, 1), GetValue(aTokens, 2)), GetValue(aTokens, 3), aObserver);
}

float
VectorParser::GetValue(const std::vector<std::string>& aTokens, const size_t place) {
  return aTokens.size() > place ? LocalStof(aTokens[place]) : mDefaultValue;
}

void
FaceParser::Parse(const std::vector<std::string>& aTokens, vrb::ParserObserver_obj& aObserver) {
  const size_t tokensSize = aTokens.size();
  std::vector<int> vertices;
  std::vector<int> uvs;
  std::vector<int> normals;
  for (size_t ix = 0; ix < tokensSize; ix++) {
    std::vector<std::string> vertexTokens;
    size_t vertexSize = TokenizeDelimiter(aTokens[ix], "/", vertexTokens);
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

struct Parser_obj::State {
  ParserObserver_obj *observer;
  std::string lineBuffer;
  VertexParser vertexParser;
  NormalParser normalParser;
  UVParser uvParser;
  FaceParser faceParser;

  State()
      : observer(nullptr)
    {}
};

Parser_obj::Parser_obj() : m(*(new State)) {

}

Parser_obj::~Parser_obj() {
  delete &m;
}

void
Parser_obj::SetObserver(ParserObserver_obj* aObserver) {
  m.observer = aObserver;
}

void
Parser_obj::Start() {
  m.lineBuffer.clear();
}

void
Parser_obj::ParseChunk(const char* aChunk, const size_t aSize) {
  size_t place = 0;
  size_t start = 0;
  while(place < aSize) {
    if ((aChunk[place] == cLF) || (aChunk[place] == cRF)) {
      if ((place - start) > 0) {
        m.lineBuffer.append(&(aChunk[start]), place - start);
      }
      Finish();
      start = place + 1;
    }
    place++;
  }

  if (start < aSize) {
    m.lineBuffer.append(&(aChunk[start]), aSize - start);
  }
}

void
Parser_obj::Finish() {
  if (!m.lineBuffer.empty() && m.observer) {
std::cout << "line: " << m.lineBuffer << std::endl;
    size_t end = m.lineBuffer.find("#");
    if (end != std::string::npos) { m.lineBuffer.erase(end); }
    std::vector<std::string> tokens;
    const std::string type = TokenizeWhiteSpace(m.lineBuffer, tokens);
    LineParser *currentParser = nullptr;
    if (type == "#") {
      // Found line comment;
    } else if (type == "v") {
      currentParser = &m.vertexParser;
    } else if (type == "vn") {
      currentParser = &m.normalParser;
    } else if (type == "vt") {
      currentParser = &m.uvParser;
    } else if (type == "f") {
      currentParser = &m.faceParser;
    } else if (type == "g") {
      m.observer->SetGroupNames(tokens);
    } else if (type == "o") {
      m.observer->SetObjectName(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "mtllib") {
      m.observer->LoadMaterialLibrary(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "usemtl") {
      m.observer->SetMaterialName(tokens.size() > 0 ? tokens[0] : "");
    } else if (type == "s") {
      std::string value = tokens.size() > 0 ? tokens[0] : "0";
      if (value == "off") { value = "0"; }
      int group = LocalStoi(value);
      m.observer->SetSmoothingGroup(group);
    } else {
      std::cout << "Unknown type: " << type << std::endl;
    }

    if (currentParser) {
      currentParser->Parse(tokens, *m.observer);
    }
  }
  m.lineBuffer.clear();
}

} // namespace vrb
