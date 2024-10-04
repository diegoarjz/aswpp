add_library(
  aswpp
  SHARED
    script_engine.cpp
)

target_include_directories(
  aswpp
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/AngelScript/sdk/angelscript/include
    ${CMAKE_SOURCE_DIR}/third_party/AngelScript/sdk/add_on/scriptbuilder
)

target_link_libraries(
  aswpp
  PRIVATE
    AngelScript
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
