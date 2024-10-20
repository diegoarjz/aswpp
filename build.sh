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
