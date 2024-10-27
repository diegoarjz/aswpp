#pragma once

#include "aswpp/script_module.h"

#include <memory>

namespace aswpp {
/**
 * Entry point for scripting.
 */
class Engine {
public:

  //! Default ctor
  Engine();
  ~Engine();

  //! Checks if the Engine is in a valid state.
  bool IsValid() const;

  //----------------------------------------
  //! \section module-api Module API
  
  //! Creates a and attaches Module with the give \a name and \a source.
  //! Returns nullptr if attaching the module fails.
  ModulePtr CreateModule(const std::string& name, const std::string& source);

  //! Attaches a Module previously created to the engine.
  bool Attach(const ModulePtr& module);

  //----------------------------------------
  //! \section run-methods Run Methods

  //! Runs the given function with the arguments.
  template <class... ArgTypes>
  bool Run(const std::string& moduleName, const std::string& function, ArgTypes &... args) {
    if (!prepare(moduleName, function)) {
      return false;
    }
    if (!setArgs<sizeof...(ArgTypes)>(args...)) {
      return false;
    }
    if (!run(moduleName, function)) {
      return false;
    }
    return release();
  }

  //! Runs the given function with the arguments and the return value.
  template <class Return, class... ArgTypes>
  bool Run(const std::string& moduleName, const std::string& function, Return* ret, ArgTypes &... args) {
    if (!prepare(moduleName, function)) {
      return false;
    }
    if (!setArgs<sizeof...(ArgTypes)>(args...)) {
      return false;
    }
    if (!run(moduleName, function)) {
      return false;
    }
    if constexpr (std::is_enum<Return>::value) {
      int64_t returnValue;
      getReturnValue<int64_t>(&returnValue);
      *ret = static_cast<Return>(returnValue);
    }
    else {
      getReturnValue(ret);
    }
    return release();
  }

  //----------------------------------------
  //! \section Register Enum Methods
  bool RegisterEnum(const std::string& enumName);
  bool RegisterEnumValue(const std::string& enumName, const std::string& valueName, int value);
private:

  bool prepare(const std::string& moduleName, const std::string& function);
  bool run(const std::string& moduleName, const std::string& function);
  bool release();

  bool setFunctionObjectArg(int i, void* val);

  template<typename T>
  bool setFunctionArg(int i, T val) {
    return setFunctionObjectArg(i, &val);
  }

  template<std::size_t Count>
  bool setArgs() { return true; }

  template <std::size_t Count, class ArgType, class... ArgTypes>
  bool setArgs(ArgType &arg, ArgTypes &...args) {
    return setArgs<Count>(arg) && setArgs<Count - 1>(args...);
  }

  template <std::size_t Count, class ArgType>
  bool setArgs(ArgType &arg) {
    return setFunctionArg(Count - 1, arg);
  }

  template<typename T>
  void getReturnValue(T* value);

  class Impl;
  std::unique_ptr<Impl> m_impl;
};

//----------------------------------------
// Specializations
template<> bool Engine::setFunctionArg<int64_t>(int i, int64_t val);
template<> bool Engine::setFunctionArg<uint64_t>(int i, uint64_t val);
template<> bool Engine::setFunctionArg<int32_t>(int i, int32_t val);
template<> bool Engine::setFunctionArg<uint32_t>(int i, uint32_t val);
template<> bool Engine::setFunctionArg<int16_t>(int i, int16_t val);
template<> bool Engine::setFunctionArg<uint16_t>(int i, uint16_t val);
template<> bool Engine::setFunctionArg<int8_t>(int i, int8_t val);
template<> bool Engine::setFunctionArg<uint8_t>(int i, uint8_t val);
template<> bool Engine::setFunctionArg<char>(int i, char val);
template<> bool Engine::setFunctionArg<float>(int i, float val);
template<> bool Engine::setFunctionArg<double>(int i, double val);

template<> void Engine::getReturnValue(int64_t* value);
template<> void Engine::getReturnValue(uint64_t* value);
template<> void Engine::getReturnValue(int32_t* value);
template<> void Engine::getReturnValue(uint32_t* value);
template<> void Engine::getReturnValue(int16_t* value);
template<> void Engine::getReturnValue(uint16_t* value);
template<> void Engine::getReturnValue(int8_t* value);
template<> void Engine::getReturnValue(uint8_t* value);
template<> void Engine::getReturnValue(float* value);
template<> void Engine::getReturnValue(double* value);
}
