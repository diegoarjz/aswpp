add_library(
  aswpp
  STATIC
    script_engine.cpp
    script_module.cpp
    class_register.cpp
)

target_include_directories(
  aswpp
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${Angelscript_PATH}/sdk/add_on/scriptbuilder
    ${Angelscript_PATH}/sdk/add_on/scriptstdstring
)

target_link_libraries(
  aswpp
  PUBLIC
    magic_enum::magic_enum
    Angelscript::angelscript
  PRIVATE
    AngelScript_ScriptBuilder_AddOn
    AngelScript_ScriptStdString_AddOn
)

set(aswpp_public_headers
    ${CMAKE_SOURCE_DIR}/include/aswpp/script_engine.h
)

set_target_properties(
  aswpp
  PROPERTIES
  PUBLIC_HEADER
    "${aswpp_public_headers}"
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aswpp
)
