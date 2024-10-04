#pragma once

#include <angelscript.h>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

#include <iostream>

namespace alpha::scripting {
//------------------------------------------------------------
// Enum Register
//------------------------------------------------------------

template <class R> class EnumRegister {
public:
  EnumRegister(asIScriptEngine *engine, const char *enumName) {

    bool valid = engine->RegisterEnum(enumName) >= 0;
    if (!valid) {
      spdlog::error("Error registering enum '{}'", enumName);
      return;
    }

    for (const auto &v : magic_enum::enum_values<R>()) {
      const auto enumMemberName = magic_enum::enum_name(v);
      valid = engine->RegisterEnumValue(
                  enumName, static_cast<std::string>(enumMemberName).c_str(),
                  static_cast<int>(v)) >= 0;

      if (!valid) {
        spdlog::error("Error registering enum member '{}::{}'", enumName,
                      enumMemberName);
        return;
      }
    }
  }
};

//------------------------------------------------------------
// Class Register
//------------------------------------------------------------

template <class C> struct RefObjectType {
  static bool SetObjectType(asIScriptEngine *engine, const char *className) {
    return engine->RegisterObjectType(className, 0, asOBJ_REF) >= 0;
  }
};

template <class C> struct ValueObjectType {
  static bool SetObjectType(asIScriptEngine *engine, const char *className) {
    return engine->RegisterObjectType(className, sizeof(C), asOBJ_VALUE) >= 0;
  }
};

template <class C> class ObjectFactory {
public:
  static C *Create() {
    C *ptr = new C;
    s_referenceCounts.emplace(ptr, 1);
    return ptr;
  }

  static void AddRef(C *ptr) {
    assert(s_referenceCounts.find(ptr) != s_referenceCounts.end());
    s_referenceCounts[ptr]++;
  }

  static void Release(C *ptr) {
    assert(s_referenceCounts.find(ptr) != s_referenceCounts.end());
    s_referenceCounts[ptr]--;
    if (s_referenceCounts[ptr] <= 0) {
      s_referenceCounts.erase(ptr);
      delete ptr;
    } else if (s_referenceCounts[ptr] < 0) {
      std::cout << "--> " << ptr << " <--" << std::endl;
      spdlog::error("[Script Engine] object released too many times.");
    }
  }

private:
  static std::unordered_map<C *, int> s_referenceCounts;
};

template <class C>
std::unordered_map<C *, int> ObjectFactory<C>::s_referenceCounts;

template <class C> C *GenericRefFactory() { return ObjectFactory<C>::Create(); }

template <class C> struct FactoryBehaviour {
  static bool SetFactoryBehaviour(asIScriptEngine *engine,
                                  const char *className) {
    return engine->RegisterObjectBehaviour(
               className, asBEHAVE_FACTORY,
               (std::string(className) + "@ f()").c_str(),
               asFUNCTION(GenericRefFactory<C>), asCALL_CDECL) >= 0;
  }
};

template <class C> struct NoFactoryBehaviour {
  static bool SetFactoryBehaviour(asIScriptEngine *engine,
                                  const char *className) {
    return true;
  }
};

template <class C> void GenericAddRef(asIScriptGeneric *gen) {
  C *object = static_cast<C *>(gen->GetObject());
  ObjectFactory<C>::AddRef(object);
}

template <class C> void GenericRelease(asIScriptGeneric *gen) {
  C *object = static_cast<C *>(gen->GetObject());
  ObjectFactory<C>::Release(object);
}

template <class C, typename R, typename... Args> class MethodHolder {
public:
  using Fptr_t = R (C::*)(Args...) const;

  static MethodHolder<C, R, Args...> *Create(Fptr_t f) {
    auto m = new MethodHolder<C, R, Args...>;
    m->m_fptr = f;
    s_holders.emplace_back(m);
    return m;
  }

  Fptr_t GetFptr() const { return m_fptr; }

private:
  Fptr_t m_fptr{nullptr};

  static std::vector<MethodHolder<C, R, Args...> *> s_holders;
};
template <class C, typename R, typename... Args>
std::vector<MethodHolder<C, R, Args...> *>
    MethodHolder<C, R, Args...>::s_holders;

template <class C, typename R, typename... Args>
void GenericCreateFromValue(asIScriptGeneric *gen) {
  C *object = static_cast<C *>(gen->GetObject());
  auto holder = static_cast<MethodHolder<C, R, Args...> *>(gen->GetAuxiliary());
  auto fptr = holder->GetFptr();
  R ret = (*object.*fptr)();

  auto newPtr = ObjectFactory<R>::Create();
  *newPtr = ret;
  gen->SetReturnAddress(newPtr);
}

template <class C> struct RefCountBehaviour {
  static bool SetRefBehaviour(asIScriptEngine *engine, const char *className) {
    return (engine->RegisterObjectBehaviour(
               className, asBEHAVE_ADDREF, "void f()",
               asFUNCTION(GenericAddRef<C>), asCALL_GENERIC)) &&
           (engine->RegisterObjectBehaviour(
               className, asBEHAVE_RELEASE, "void f()",
               asFUNCTION(GenericRelease<C>), asCALL_GENERIC));
  }
};

template <class C> struct NoRefCountBehaviour {
  static bool SetRefBehaviour(asIScriptEngine *engine, const char *className) {
    return true;
  }
};

template <class C> void ClassCtor(void *memory) { new (memory) C(); }
template <class C> void ClassDtor(void *memory) { ((C *)memory)->~C(); }

template <class C, class O = RefObjectType<C>, class Ref = RefCountBehaviour<C>,
          class F = FactoryBehaviour<C>>
class ClassRegister {
private:
public:
  ClassRegister(asIScriptEngine *engine, const char *className)
      : m_engine(engine), m_className(className) {

    m_valid = O::SetObjectType(engine, className);
    m_valid |= F::SetFactoryBehaviour(engine, className);
    m_valid |= Ref::SetRefBehaviour(engine, className);

    if (!m_valid) {
      spdlog::error("Error registering class '{}'", className);
    }
  }

  template <typename... Args>
  ClassRegister RegisterConstructor(const char *decl, Args... args) {
    if (!m_valid) {
      return *this;
    }

    m_valid |= m_engine->RegisterObjectBehaviour(
        m_className, asBEHAVE_CONSTRUCT, decl, asFUNCTION(ClassCtor<C>),
        asCALL_CDECL_OBJLAST);

    if (!m_valid) {
      spdlog::error("Error registering constructor '{}' of class '{}'", decl,
                    m_className);
    }

    return *this;
  }

  ClassRegister RegisterDestructor() {
    if (!m_valid) {
      return *this;
    }

    m_valid |= m_engine->RegisterObjectBehaviour(
        m_className, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ClassDtor<C>),
        asCALL_CDECL_OBJLAST);

    if (!m_valid) {
      spdlog::error("Error registering destructor of class '{}'", m_className);
    }

    return *this;
  }

  template <typename R, typename... Args>
  ClassRegister &RegisterObjectMethod(const char *decl, R (C::*f)(Args...)) {
    if (!m_valid) {
      return *this;
    }

    m_valid |=
        m_engine->RegisterObjectMethod(
            m_className, decl, asSMethodPtr<sizeof(void(C::*)())>::Convert(f),
            asCALL_THISCALL) >= 0;

    if (!m_valid) {
      spdlog::error("Error registering method '{}' of class '{}'", decl,
                    m_className);
    }

    return *this;
  }

  // Const method version
  template <typename R, typename... Args>
  ClassRegister &RegisterObjectMethod(const char *decl,
                                      R (C::*f)(Args...) const) {
    if (!m_valid) {
      return *this;
    }

    m_valid |=
        m_engine->RegisterObjectMethod(
            m_className, decl, asSMethodPtr<sizeof(void(C::*)())>::Convert(f),
            asCALL_THISCALL) >= 0;

    if (!m_valid) {
      spdlog::error("Error registering method '{}' of class '{}'", decl,
                    m_className);
    }

    return *this;
  }

  template <typename R, typename... Args>
  ClassRegister &RegisterValueObjectMethod(const char *decl,
                                           R (C::*f)(Args...) const) {
    if (!m_valid) {
      return *this;
    }

    auto methodHolder = MethodHolder<C, R, Args...>::Create(f);

    m_valid |= m_engine->RegisterObjectMethod(
        m_className, decl, asFUNCTION((GenericCreateFromValue<C, R, Args...>)),
        asCALL_GENERIC, methodHolder);

    if (!m_valid) {
      spdlog::error("Error registering method '{}' of class '{}'", decl,
                    m_className);
    }

    return *this;
  }

private:
  asIScriptEngine *m_engine;
  const char *m_className;
  bool m_valid{false};
};
} // namespace alpha::scripting
