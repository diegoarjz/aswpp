include(ExternalProject)

set(AngelScript_URL
  "https://www.angelcode.com/angelscript/sdk/files/angelscript_${AngelScript_VERSION}.zip"
)
set(AngelScriptZip_DST
  "${CMAKE_CURRENT_SOURCE_DIR}/angelscript_${AngelScript_VERSION}.zip"
)
set(AngelScript_DST
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk"
)

add_custom_command(
  OUTPUT
    ${AngelScriptZip_DST}
  COMMAND
    wget ${AngelScript_URL}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT
    "Downloading ${AngelScript_URL} to ${AngelScriptZip_DST}"
)

add_custom_command(
  OUTPUT
    ${AngelScript_DST}
  COMMAND
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${AngelScriptZip_DST}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Unzipping ${AngelScriptZip_DST}"
)

add_custom_target(
  download_AngelScript
  ALL
  DEPENDS
    ${AngelScriptZip_DST}
    ${AngelScript_DST}
)
