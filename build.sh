#!/bin/sh

aswpp_ROOT=${1}
if [ -z ${aswpp_ROOT} ]; then
  aswpp_ROOT=$(pwd)
fi
BUILD_ROOT=${aswpp_ROOT}/build

#----------------------------------------
# Detect OS
# Used to specify the conan profiles
#----------------------------------------
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     DETECTED_OS=linux;;
    Darwin*)    DETECTED_OS=mac;;
    *)          DETECTED_OS="UNKNOWN:${unameOut}"
esac

CONAN_PROFILE=${2}
if [ -z ${CONAN_PROFILE} ]; then
  CONAN_PROFILE="${aswpp_ROOT}/tools/profiles/${DETECTED_OS}_Rel.txt"
fi

echo "########################################"
echo "Running build.sh"
echo "            OS: ${DETECTED_OS}"
echo "    aswpp root: ${aswpp_ROOT}"
echo "    build root: ${BUILD_ROOT}"
echo " conan profile: ${CONAN_PROFILE}"
echo "########################################"

#----------------------------------------
# install conan dependencies
#----------------------------------------
git -C ${aswpp_ROOT} submodule update --init --recursive
conan install                                     \
  -of ${BUILD_ROOT}                               \
  ${aswpp_ROOT}                                   \
  --profile:host=${CONAN_PROFILE}                 \
  --profile:build=${CONAN_PROFILE}                \
  --build=missing

#----------------------------------------
# Build aswpp
#----------------------------------------
conan build                                       \
  -of ${BUILD_ROOT}                               \
  ${aswpp_ROOT}                                   \
  --profile:host=${CONAN_PROFILE}                 \
  --profile:build=${CONAN_PROFILE}
