@echo off

set aswpp_ROOT=%1%
if "%~1" == "" (set aswpp_ROOT=%cd%)

set CONAN_PROFILE=%2%
if "%~2" == "" (set CONAN_PROFILE="%aswpp_ROOT%/tools/profiles/win_Rel.txt")

set BUILD_ROOT=%aswpp_ROOT%\build

echo "########################################"
echo "Running build.sh"
echo "    aswpp root: %aswpp_ROOT%"
echo "    build root: %BUILD_ROOT%"
echo " conan profile: %CONAN_PROFILE%"
echo "########################################"

REM ----------------------------------------
REM  install conan dependencies
REM ----------------------------------------
git -C %aswpp_ROOT% submodule update --init --recursive
conan install                                     ^
  -of %BUILD_ROOT%                                ^
  %aswpp_ROOT%                                    ^
  --profile:host=%CONAN_PROFILE%                  ^
  --profile:build=%CONAN_PROFILE%                 ^
  --build=missing

REM ----------------------------------------
REM  Build aswpp
REM ----------------------------------------
conan build                                       ^
  -of %BUILD_ROOT%                                ^
  %aswpp_ROOT%                                    ^
  --profile:host=%CONAN_PROFILE%                  ^
  --profile:build=%CONAN_PROFILE%                 ^
