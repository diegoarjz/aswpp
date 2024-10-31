#include "aswpp/class_register.h"

namespace aswpp {
template<> int64_t  arg< int64_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgQWord(argNum); }
template<> uint64_t arg<uint64_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgQWord(argNum); }
template<> int32_t  arg< int32_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgDWord(argNum); }
template<> uint32_t arg<uint32_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgDWord(argNum); }
template<> int16_t  arg< int16_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgWord(argNum); }
template<> uint16_t arg<uint16_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgWord(argNum); }
template<> int8_t   arg<  int8_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgByte(argNum); }
template<> uint8_t  arg< uint8_t>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgByte(argNum); }
template<> char     arg<    char>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgByte(argNum); }
template<> float    arg<   float>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgFloat(argNum); }
template<> double   arg<  double>(asIScriptGeneric* gen, uint8_t argNum) { return gen->GetArgDouble(argNum); }
}
