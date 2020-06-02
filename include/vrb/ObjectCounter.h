#ifndef VRB_OBJECT_COUNTER_DOT_H
#define VRB_OBJECT_COUNTER_DOT_H

#include <cstddef>

#if defined(VRB_COUNT_OBJECTS)
#  define VRB_ADD_TO_OBJECT_COUNT AddObject(typeid(*this).hash_code(), typeid(*this).name())
#  define VRB_REMOVE_FROM_OBJECT_COUNT RemoveObject(typeid(*this).hash_code())
#else
#  define VRB_ADD_TO_OBJECT_COUNT
#  define VRB_REMOVE_FROM_OBJECT_COUNT
#endif

namespace vrb {

void InitializeObjectCounter();
void LogObjectCount();
void ShutdownObjectCounter();
void AddObject(std::size_t aHandle, const char* aName);
void RemoveObject(std::size_t aHandle);

} // vrb

#endif //  VRB_OBJECT_COUNTER_DOT_H
