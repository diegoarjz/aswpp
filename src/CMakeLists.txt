add_library(
  aswpp
  STATIC
    script_engine.cpp
    script_module.cpp
)

target_include_directories(
  aswpp
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/AngelScript/sdk/angelscript/include
    ${CMAKE_SOURCE_DIR}/third_party/AngelScript/sdk/add_on/scriptbuilder
)

target_link_libraries(
  aswpp
  PUBLIC
    magic_enum::magic_enum
    angelscript
  PRIVATE
    AngelScript_ScriptBuilder_AddOn
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
