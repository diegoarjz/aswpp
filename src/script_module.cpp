#include "aswpp/script_module.h"

#include "scriptbuilder.h"

namespace aswpp {
//----------------------------------------
// Module class

class Module::Impl {
public:
  Impl(const std::string &name, const std::string &source)
      : m_name{name}, m_source{source} {}

  Impl(const std::string &name) : m_name{name} {}

  ~Impl() {}

  bool PrepareModule(asIScriptEngine *engine) {
    CScriptBuilder builder;
    const auto name = m_name.c_str();
    if (builder.StartNewModule(engine, name) != 0) {
      return false;
    }

    if (builder.AddSectionFromMemory("section", m_source.c_str(),
                                     m_source.size(), 0) != 1) {
      engine->DiscardModule(name);
      return false;
    }

    if (builder.BuildModule() != 0) {
      engine->DiscardModule(name);
      return false;
    }

    m_module = engine->GetModule(name);

    return true;
  }

  std::string m_name;
  std::string m_source;

  Engine *m_attachedTo{nullptr};
  asIScriptModule *m_module;
};

Module::Module(const std::string &name, const std::string &source)
    : m_impl{std::make_unique<Impl>(name, source)} {}

Module::~Module() {}

const std::string &Module::GetName() const { return m_impl->m_name; }

const std::string &Module::GetSource() const { return m_impl->m_source; }

std::size_t Module::GetFunctionCount() const {
  return m_impl->m_module->GetFunctionCount();
}

bool Module::IsAttached() const { return m_impl->m_attachedTo != nullptr; }

void Module::attachTo(Engine *engine) { m_impl->m_attachedTo = engine; }

bool Module::prepareModule(asScriptEngineHandle *handle) {
  return m_impl->PrepareModule(reinterpret_cast<asIScriptEngine *>(handle));
}

void *Module::getGlobalVarAddress(const std::string &name) const {
  const auto index = m_impl->m_module->GetGlobalVarIndexByName(name.c_str());
  return m_impl->m_module->GetAddressOfGlobalVar(index);
}

} // namespace aswpp
