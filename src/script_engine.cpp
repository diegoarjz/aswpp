#include "aswpp/script_engine.h"

#include "scriptbuilder.h"
#include <iostream>

namespace aswpp {
//----------------------------------------
// Engine class
static void StaticMessageCallback(const asSMessageInfo *msg) {
  switch (msg->type) {
  case asMSGTYPE_ERROR:
    std::cerr << "[Error] (" << msg->section << "@" << msg->row << ":"
              << msg->col << ")" << msg->message << std::endl;
    break;
  case asMSGTYPE_WARNING:
    std::cerr << "[Warning] (" << msg->section << "@" << msg->row << ":"
              << msg->col << ")" << msg->message << std::endl;
    break;
  case asMSGTYPE_INFORMATION:
    std::cerr << "[Info] (" << msg->section << "@" << msg->row << ":"
              << msg->col << ")" << msg->message << std::endl;
    break;
  };
}

class Engine::Impl {
public:
  Impl(Engine &engine) : m_scriptEngine{engine} {
    m_asEngine = asCreateScriptEngine();

    int r = m_asEngine->SetMessageCallback(asFUNCTION(StaticMessageCallback), 0,
                                           asCALL_CDECL);

    m_asEngine->SetMessageCallback(asFUNCTION(StaticMessageCallback), nullptr,
                                   asCALL_CDECL);

    /*
    if (r == asINVALID_ARG) {
    } else if (r == asNOT_SUPPORTED) {
    }
    */

    m_context = m_asEngine->CreateContext();

    if (m_context == nullptr) {
      m_asEngine->ShutDownAndRelease();
      m_asEngine = nullptr;
    }
  }

  ~Impl() {
    if (m_asEngine != nullptr) {
      m_asEngine->ShutDownAndRelease();
    }
  }

  bool prepare(const std::string &moduleName, const std::string &function) {
    if (m_asEngine == nullptr) {
      std::cerr << "Tried running an invalid script." << std::endl;
      return false;
    }

    asIScriptModule *module = m_asEngine->GetModule(moduleName.c_str());

    auto *func = module->GetFunctionByDecl(function.c_str());
    if (func == nullptr) {
      std::cerr << "Could not find function '" << function << "'." << std::endl;
      return false;
    }

    if (m_context == nullptr) {
      std::cerr << "Could not create context." << std::endl;
      return false;
    }

    if (m_context->Prepare(func) != 0) {
      std::cerr << "Could not prepare context." << std::endl;
      m_context->Release();
      return false;
    }

    return true;
  }

  bool run(const std::string &moduleName, const std::string &function) {
    if (m_context->Execute() != asEXECUTION_FINISHED) {
      std::cerr << "Could not execute context." << std::endl;
      return false;
    }
    return true;
  }

  bool release() { return m_context->Release() == 0; }

  Engine &m_scriptEngine;
  asIScriptEngine *m_asEngine{nullptr};
  asIScriptContext *m_context{nullptr};
  std::map<std::string, ModulePtr> m_modules;
};

Engine::Engine() : m_impl(std::make_unique<Engine::Impl>(*this)) {}

Engine::~Engine() {}

bool Engine::IsValid() const { return m_impl->m_asEngine != nullptr; }

ModulePtr Engine::CreateModule(const std::string &name,
                               const std::string &source) {
  auto module = std::make_shared<Module>(name, source);
  if (!Attach(module)) {
    return nullptr;
  }
  return module;
}

bool Engine::Attach(const ModulePtr &module) {
  // Keep track of which engine this module is attached to
  if (module->IsAttached()) {
    return false;
  }

  const bool wasInserted =
      m_impl->m_modules.emplace(module->GetName(), module).second;
  if (!wasInserted) {
    return false;
  }
  module->attachTo(this);

  if (!module->prepareModule(
          reinterpret_cast<asScriptEngineHandle *>(m_impl->m_asEngine))) {
    return false;
  }

  return true;
}

//----------------------------------------
//! \section Register Enum Methods
bool Engine::RegisterEnum(const std::string &enumName) {
  const int r = m_impl->m_asEngine->RegisterEnum(enumName.c_str());
  return r >= 0;
}
bool Engine::RegisterEnumValue(const std::string &enumName,
                               const std::string &valueName, int value) {
  const int r = m_impl->m_asEngine->RegisterEnumValue(enumName.c_str(),
                                                      valueName.c_str(), value);
  return r >= 0;
}

bool Engine::prepare(const std::string &moduleName,
                     const std::string &function) {
  return m_impl->prepare(moduleName, function);
}

bool Engine::run(const std::string &module, const std::string &function) {
  return m_impl->run(module, function);
}

bool Engine::release() { return m_impl->release(); }

asIScriptEngine *Engine::engine() { return m_impl->m_asEngine; }

namespace {
void reportSetFunctionError(int error) {
  if (error != asSUCCESS) {
    if (error == asCONTEXT_NOT_PREPARED) {
      std::cerr << " Context not prepared" << std::endl;
    } else if (error == asINVALID_ARG) {
      std::cerr << " Argument number is larger than the number of arguments in "
                   "the prepared function."
                << std::endl;
    } else if (error == asINVALID_TYPE) {
      std::cerr << " The argument is not an object or handle." << std::endl;
    }
  }
}
} // namespace

template <> bool Engine::setFunctionArg<int64_t>(int i, int64_t val) {
  const int r = m_impl->m_context->SetArgQWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<uint64_t>(int i, uint64_t val) {
  const int r = m_impl->m_context->SetArgQWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<int32_t>(int i, int32_t val) {
  const int r = m_impl->m_context->SetArgDWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<uint32_t>(int i, uint32_t val) {
  const int r = m_impl->m_context->SetArgDWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<int16_t>(int i, int16_t val) {
  const int r = m_impl->m_context->SetArgWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<uint16_t>(int i, uint16_t val) {
  const int r = m_impl->m_context->SetArgWord(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<int8_t>(int i, int8_t val) {
  const int r = m_impl->m_context->SetArgByte(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<uint8_t>(int i, uint8_t val) {
  const int r = m_impl->m_context->SetArgByte(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<char>(int i, char val) {
  const int r = m_impl->m_context->SetArgByte(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<float>(int i, float val) {
  const int r = m_impl->m_context->SetArgFloat(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> bool Engine::setFunctionArg<double>(int i, double val) {
  const int r = m_impl->m_context->SetArgDouble(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

bool Engine::setFunctionObjectArg(int i, void *val) {
  const int r = m_impl->m_context->SetArgObject(i, val);
  if (r != asSUCCESS) {
    std::cerr << "Unable to set argument " << i << " to value at address "
              << val << std::endl;
    reportSetFunctionError(r);
  }
  return r == asSUCCESS;
}

template <> void Engine::getReturnValue(int64_t *value) {
  *value = m_impl->m_context->GetReturnQWord();
}

template <> void Engine::getReturnValue(uint64_t *value) {
  *value = m_impl->m_context->GetReturnQWord();
}

template <> void Engine::getReturnValue(int32_t *value) {
  *value = m_impl->m_context->GetReturnDWord();
}

template <> void Engine::getReturnValue(uint32_t *value) {
  *value = m_impl->m_context->GetReturnDWord();
}

template <> void Engine::getReturnValue(int16_t *value) {
  *value = m_impl->m_context->GetReturnWord();
}

template <> void Engine::getReturnValue(uint16_t *value) {
  *value = m_impl->m_context->GetReturnWord();
}

template <> void Engine::getReturnValue(int8_t *value) {
  *value = m_impl->m_context->GetReturnByte();
}

template <> void Engine::getReturnValue(uint8_t *value) {
  *value = m_impl->m_context->GetReturnByte();
}

template <> void Engine::getReturnValue(float *value) {
  *value = m_impl->m_context->GetReturnFloat();
}

template <> void Engine::getReturnValue(double *value) {
  *value = m_impl->m_context->GetReturnDouble();
}
} // namespace aswpp
