
#----------------------------------------
# Download and build Angelscript
set(Angelscript_zip_file_name "angelscript_${Angelscript_VERSION}.zip")
set(Angelscript_zip_path "${Angelscript_PATH}/${Angelscript_zip_file_name}")
set(Angelscript_url "https://www.angelcode.com/angelscript/sdk/files/${Angelscript_zip_file_name}")
set(Angelscript_build_dir "${Angelscript_PATH}/build")
set(Angelscript_install_prefix "${Angelscript_build_dir}/install")
set(Angelscript_cmake_project_path "${Angelscript_PATH}/sdk/angelscript/projects/cmake")

file(DOWNLOAD ${Angelscript_url} ${Angelscript_zip_path})
file(MAKE_DIRECTORY ${Angelscript_build_dir})


message(STATUS "Unpacking Angelscript")
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${Angelscript_zip_path}"
  WORKING_DIRECTORY ${Angelscript_PATH}
)

execute_process(
  COMMAND ${CMAKE_COMMAND} 
    ${Angelscript_cmake_project_path}
    "-B"
    ${Angelscript_build_dir}
    "-DCMAKE_BUILD_TYPE=Release"
    WORKING_DIRECTORY ${Angelscript_PATH}
)

execute_process(
  COMMAND ${CMAKE_COMMAND} 
    "--build"
    ${Angelscript_build_dir}
    "--config" "Release"
    "--target" "install"
    WORKING_DIRECTORY ${Angelscript_PATH}
)

execute_process(
  COMMAND ${CMAKE_COMMAND} 
    "--install"
    ${Angelscript_build_dir}
    "--prefix"
    ${Angelscript_install_prefix}
    WORKING_DIRECTORY ${Angelscript_PATH}
)

#----------------------------------------
# Build Script Builder Addon
file(GLOB AngelScript_AddOn_ScriptBuilder_SOURCES ${Angelscript_PATH}/sdk/add_on/scriptbuilder/*.cpp)
add_library(
  AngelScript_ScriptBuilder_AddOn
  STATIC
    ${AngelScript_AddOn_ScriptBuilder_SOURCES}
)
target_include_directories(
  AngelScript_ScriptBuilder_AddOn
  PRIVATE
    ${Angelscript_PATH}/sdk/angelscript/include
)
set_property(TARGET AngelScript_ScriptBuilder_AddOn PROPERTY POSITION_INDEPENDENT_CODE ON)

#----------------------------------------
# Build stdstring Addon
file(GLOB AngelScript_AddOn_ScriptStdString_SOURCES ${Angelscript_PATH}/sdk/add_on/scriptstdstring/*.cpp)
add_library(
  AngelScript_ScriptStdString_AddOn
  STATIC
    ${AngelScript_AddOn_ScriptStdString_SOURCES}
)
target_include_directories(
  AngelScript_ScriptStdString_AddOn
  PRIVATE
    ${Angelscript_PATH}/sdk/angelscript/include
)
set_property(TARGET AngelScript_ScriptStdString_AddOn PROPERTY POSITION_INDEPENDENT_CODE ON)
