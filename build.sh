#!/bin/sh

aswpp_ROOT=${1}
if [ -z ${aswpp_ROOT} ]; then
  aswpp_ROOT=$(pwd)
fi

BUILD_ROOT=${aswpp_ROOT}/build

echo "########################################"
echo "Running build.sh"
echo "   aswpp root: ${aswpp_ROOT}"
echo "   build root: ${BUILD_ROOT}"
echo "########################################"

#----------------------------------------
# install dependencies
#----------------------------------------
python3 -m venv devenv
source devenv/bin/activate
pip3 install -r ${aswpp_ROOT}/requirements.txt

#----------------------------------------
# install conan dependencies
#----------------------------------------
git -C ${aswpp_ROOT} submodule update --init --recursive
conan profile detect
conan install                                     \
  -of ${BUILD_ROOT}                               \
  ${aswpp_ROOT}                                   \
  --build=missing

#----------------------------------------
# Build aswpp
#----------------------------------------
conan build -of ${BUILD_ROOT} ${aswpp_ROOT}
