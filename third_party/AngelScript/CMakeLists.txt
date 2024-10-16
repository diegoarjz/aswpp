#----------------------------------------
# Build AngelScript library
add_subdirectory(sdk/angelscript/projects/cmake)

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
