#ifndef VRB_PROGRAM_FACTORY_DOT_H
#define VRB_PROGRAM_FACTORY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ProgramFactory {
public:
  static ProgramFactorPtr Create(ContextWeak& aContext);
  ProgramPtr CreatePhongProgram(const bool aEnableTexturing);
  // TODO: Add PBR Program.
protected:
  struct State;
  ProgramFactory(ContextWeak& aContext);
private:
  State& m;
  ProgramFactory() = delete;
  VRB_NO_DEFAULTS(ProgramFactory)
};

} // namespace vrb

#endif // VRB_PROGRAM_FACTORY_DOT_H
