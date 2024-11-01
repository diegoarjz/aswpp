#pragma once

#include <memory>
#include <string>

namespace aswpp {
using EnginePtr = std::shared_ptr<class Engine>;

class asScriptEngineHandle;

class Module {
public:
  //! Creates a module with a source and name.
  Module(const std::string &name, const std::string &source);

  ~Module();

  //! Returns the \a Module name.
  const std::string &GetName() const;

  //! Returns the \a Module source.
  const std::string &GetSource() const;

  std::size_t GetFunctionCount() const;

  bool IsAttached() const;

  template <typename VarType>
  bool GetGlobalVar(const std::string &name, VarType *var) const {
    if (void *addr = getGlobalVarAddress(name)) {

      if (var != nullptr) {
        *var = *static_cast<VarType *>(addr);
      }

      return true;
    }
    return false;
  }

private:
  void *getGlobalVarAddress(const std::string &name) const;

  void attachTo(Engine *engine);

  bool prepareModule(asScriptEngineHandle *handle);

  class Impl;
  std::unique_ptr<Impl> m_impl;

  friend class Engine;
};
using ModulePtr = std::shared_ptr<Module>;

} // namespace aswpp
