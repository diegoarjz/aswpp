#----------------------------------------
# Build AngelScript library
add_subdirectory(sdk/angelscript/projects/cmake)

#----------------------------------------
# Build Script Builder Addon
file(GLOB AngelScript_AddOn_ScriptBuilder_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sdk/add_on/scriptbuilder/*.cpp)
add_library(
  AngelScript_ScriptBuilder_AddOn
  STATIC
    ${AngelScript_AddOn_ScriptBuilder_SOURCES}
)
target_include_directories(
  AngelScript_ScriptBuilder_AddOn
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/angelscript/include
)
set_property(TARGET AngelScript_ScriptBuilder_AddOn PROPERTY POSITION_INDEPENDENT_CODE ON)

#----------------------------------------
# Build stdstring Addon
file(GLOB AngelScript_AddOn_ScriptStdString_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sdk/add_on/scriptstdstring/*.cpp)
add_library(
  AngelScript_ScriptStdString_AddOn
  STATIC
    ${AngelScript_AddOn_ScriptStdString_SOURCES}
)
target_include_directories(
  AngelScript_ScriptStdString_AddOn
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/angelscript/include
)
set_property(TARGET AngelScript_ScriptStdString_AddOn PROPERTY POSITION_INDEPENDENT_CODE ON)
