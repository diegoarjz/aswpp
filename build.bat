@echo off

set aswpp_ROOT=%1%
if "%~1" == "" (set aswpp_ROOT=%cd%)

set BUILD_ROOT=%aswpp_ROOT%\build

echo "########################################"
echo "Running build.sh"
echo "  aswpp root: %aswpp_ROOT%"
echo "   build root: %BUILD_ROOT%"
echo "########################################"

REM ----------------------------------------
REM  install dependencies
REM ----------------------------------------
pip3 install -r %aswpp_ROOT%/requirements.txt

REM ----------------------------------------
REM  install conan dependencies
REM ----------------------------------------
git -C %aswpp_ROOT% submodule update --init --recursive
conan profile detect
conan install                                     ^
  -of %BUILD_ROOT%                                ^
  %aswpp_ROOT%                                    ^
  --build=missing

REM ----------------------------------------
REM  Build aswpp
REM ----------------------------------------
conan build -of %BUILD_ROOT% %aswpp_ROOT%
