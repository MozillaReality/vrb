#ifndef VRB_MACRO_UTILS_DOT_H
#define VRB_MACRO_UTILS_DOT_H

#define VRB_NO_COPY(Class)                 \
  Class(const Class&) = delete;            \
  Class& operator=(const Class&) = delete;

#define VRB_NO_MOVE(Class)           \
  Class(Class&&) = delete;           \
  Class& operator=(Class&&) = delete;

#define VRB_NO_DEFAULTS(Class) \
  VRB_NO_COPY(Class)           \
  VRB_NO_MOVE(Class)

#endif //  VRB_MACRO_UTILS_DOT_H
