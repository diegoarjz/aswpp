#pragma once

#include <angelscript.h>
#include <magic_enum/magic_enum.hpp>

#include <iostream>

namespace aswpp {
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
      std::cerr << "[Script Engine] object released too many times."
                << std::endl;
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

//----------------------------------------
// Method Holders
template <class C, typename R, typename... Args> class MethodHolder {
public:
  using Fptr_t = R (C::*)(Args...);

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

template <class C, typename R, typename... Args> class ConstMethodHolder {
public:
  using Fptr_t = R (C::*)(Args...) const;

  static ConstMethodHolder<C, R, Args...> *Create(Fptr_t f) {
    auto m = new ConstMethodHolder<C, R, Args...>;
    m->m_fptr = f;
    s_holders.emplace_back(m);
    return m;
  }

  Fptr_t GetFptr() const { return m_fptr; }

private:
  Fptr_t m_fptr{nullptr};

  static std::vector<ConstMethodHolder<C, R, Args...> *> s_holders;
};
template <class C, typename R, typename... Args>
std::vector<ConstMethodHolder<C, R, Args...> *>
    ConstMethodHolder<C, R, Args...>::s_holders;

//! Return an argument of a specific type form \a gen
template <typename T> T arg(asIScriptGeneric *gen, uint8_t argNum) {
  return *static_cast<T *>(gen->GetAddressOfArg(argNum));
}
template <> int64_t arg<int64_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> uint64_t arg<uint64_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> int32_t arg<int32_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> uint32_t arg<uint32_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> int16_t arg<int16_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> uint16_t arg<uint16_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> int8_t arg<int8_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> uint8_t arg<uint8_t>(asIScriptGeneric *gen, uint8_t argNum);
template <> char arg<char>(asIScriptGeneric *gen, uint8_t argNum);
template <> float arg<float>(asIScriptGeneric *gen, uint8_t argNum);
template <> double arg<double>(asIScriptGeneric *gen, uint8_t argNum);

//----------------------------------------
// Calls methods with a specific number of args
template <class C, class R>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)()) {
  return (*object.*fptr)();
}
template <class C, class R, typename A1>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1)) {
  return (*object.*fptr)(arg<A1>(gen, 0));
}
template <class C, class R, typename A1, typename A2>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1, A2)) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1));
}
template <class C, class R, typename A1, typename A2, typename A3>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1, A2, A3)) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1, A2, A3, A4)) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4,
          typename A5>
R callMethod(asIScriptGeneric *gen, C *object,
             R (C::*fptr)(A1, A2, A3, A4, A5)) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3), arg<A5>(gen, 4));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
R callMethod(asIScriptGeneric *gen, C *object,
             R (C::*fptr)(A1, A2, A3, A4, A5, A6)) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3), arg<A5>(gen, 4), arg<A6>(gen, 5));
}

// Const versions
template <class C, class R>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)() const) {
  return (*object.*fptr)();
}
template <class C, class R, typename A1>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1) const) {
  return (*object.*fptr)(arg<A1>(gen, 0));
}
template <class C, class R, typename A1, typename A2>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1, A2) const) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1));
}
template <class C, class R, typename A1, typename A2, typename A3>
R callMethod(asIScriptGeneric *gen, C *object, R (C::*fptr)(A1, A2, A3) const) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4>
R callMethod(asIScriptGeneric *gen, C *object,
             R (C::*fptr)(A1, A2, A3, A4) const) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4,
          typename A5>
R callMethod(asIScriptGeneric *gen, C *object,
             R (C::*fptr)(A1, A2, A3, A4, A5) const) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3), arg<A5>(gen, 4));
}
template <class C, class R, typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
R callMethod(asIScriptGeneric *gen, C *object,
             R (C::*fptr)(A1, A2, A3, A4, A5, A6) const) {
  return (*object.*fptr)(arg<A1>(gen, 0), arg<A2>(gen, 1), arg<A3>(gen, 2),
                         arg<A4>(gen, 3), arg<A5>(gen, 4), arg<A6>(gen, 5));
}

template <class C, typename R, typename... Args>
void GenericCreateFromValue(asIScriptGeneric *gen) {
  using Fptr_t = R (C::*)(Args...);

  // Cast the object to the actual class
  C *object = static_cast<C *>(gen->GetObject());
  auto holder = static_cast<MethodHolder<C, R, Args...> *>(gen->GetAuxiliary());
  Fptr_t fptr = holder->GetFptr();
  R ret = callMethod<C, R, Args...>(gen, object, fptr);

  auto newPtr = ObjectFactory<R>::Create();
  *newPtr = ret;
  gen->SetReturnAddress(newPtr);
}

template <class C, typename R, typename... Args>
void ConstGenericCreateFromValue(asIScriptGeneric *gen) {
  using Fptr_t = R (C::*)(Args...) const;

  // Cast the object to the actual class
  C *object = static_cast<C *>(gen->GetObject());
  auto holder =
      static_cast<ConstMethodHolder<C, R, Args...> *>(gen->GetAuxiliary());
  Fptr_t fptr = holder->GetFptr();
  R ret = callMethod<C, R, Args...>(gen, object, fptr);

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
public:
  ClassRegister(asIScriptEngine *engine, const char *className)
      : m_engine(engine), m_className(className) {

    m_valid = O::SetObjectType(engine, className);
    m_valid |= F::SetFactoryBehaviour(engine, className);
    m_valid |= Ref::SetRefBehaviour(engine, className);

    if (!m_valid) {
      std::cerr << "Error registering class '" << className << "'" << std::endl;
    }
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
      std::cerr << "Error registering method '" << decl << "' of class '"
                << m_className << "'" << std::endl;
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
      std::cerr << "Error registering method '" << decl << "' of class '"
                << m_className << "'" << std::endl;
    }

    return *this;
  }

  template <typename R, typename... Args>
  ClassRegister &RegisterValueObjectMethod(const char *decl,
                                           R (C::*f)(Args...)) {
    if (!m_valid) {
      return *this;
    }

    auto methodHolder = MethodHolder<C, R, Args...>::Create(f);

    m_valid |= m_engine->RegisterObjectMethod(
        m_className, decl, asFUNCTION((GenericCreateFromValue<C, R, Args...>)),
        asCALL_GENERIC, methodHolder);

    if (!m_valid) {
      std::cerr << "Error registering method '" << decl << "' of class '"
                << m_className << "'" << std::endl;
    }

    return *this;
  }

  // Const method version
  template <typename R, typename... Args>
  ClassRegister &RegisterValueObjectMethod(const char *decl,
                                           R (C::*f)(Args...) const) {
    if (!m_valid) {
      return *this;
    }

    auto methodHolder = ConstMethodHolder<C, R, Args...>::Create(f);

    m_valid |= m_engine->RegisterObjectMethod(
        m_className, decl,
        asFUNCTION((ConstGenericCreateFromValue<C, R, Args...>)),
        asCALL_GENERIC, methodHolder);

    if (!m_valid) {
      std::cerr << "Error registering method '" << decl << "' of class '"
                << m_className << "'" << std::endl;
    }

    return *this;
  }

  template <typename V>
  ClassRegister &RegisterObjectProperty(const char *decl, V C::*propPtr) {
    if (!m_valid) {
      return *this;
    }

    m_valid |= m_engine->RegisterObjectProperty(m_className, decl,
                                                member_offset(propPtr));

    return *this;
  }

private:
  asIScriptEngine *m_engine;
  const char *m_className;
  bool m_valid{false};

  template <typename U> std::ptrdiff_t member_offset(U C::*member) {
    return reinterpret_cast<std::ptrdiff_t>(
        &(reinterpret_cast<C const volatile *>(NULL)->*member));
  }
};
} // namespace aswpp
