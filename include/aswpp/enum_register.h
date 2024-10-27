#pragma once

#include "aswpp/script_engine.h"

#include <magic_enum/magic_enum.hpp>

#include <iostream>

namespace aswpp {

template <class R>
bool EnumRegister(Engine& engine, const char *enumName) {
  if (!engine.RegisterEnum(enumName)) {
    std::cerr << "Error registering enum '" << enumName << "'" << std::endl;
    return false;
  }

  for (const auto &v : magic_enum::enum_values<R>()) {
    const auto enumMemberName = magic_enum::enum_name(v);
    if (!engine.RegisterEnumValue( enumName, static_cast<std::string>(enumMemberName).c_str(), static_cast<int>(v))) {
      std::cerr << "Error registering enum member '" << enumName << "::" << enumMemberName << "'" << std::endl;
      return false;
    }
  }

  return true;
}

}
