#!/bin/bash
set -x

#[ "$USER" != "root" ] && echo "need to run as root" >&2 && exit

while getopts d: OPT; do
  case ${OPT} in
    d) distro_type=${OPTARG}
       ;;
    \?)
       printf 'Usage: '"$(basename $0)"' -d [distro type]\n'  >&2
       exit 1
  esac
done

[ -z "${distro_type}" ] && echo 'error: distro type not set: -d [distro type]' >&2 && exit 1

WORK_DIR="$(pwd)"
SCRIPT_DIR=$(dirname "$0" | xargs realpath)
BUILD_DIR_NAME='cmake-build-release'
BUILD_DIR="../${BUILD_DIR_NAME}"

######### Make binary #########
function build_binary() {
    pushd "${SCRIPT_DIR}"
    if [ -e "${BUILD_DIR}" ];then
      rm -rf ${BUILD_DIR}
    fi
    mkdir ${BUILD_DIR} && pushd ${BUILD_DIR}
    cmake ../ && cmake --build . -j "$(nproc)"
    if [ -d "translation" ];then
      rm -rf "translation"
    fi
    mkdir translation && cp -af ../resource/translation/*.qm translation/
}

######### Build *.deb #########
function build_deb() {
  tag=$(git describe --abbrev=0 --tags)
  commit=$(git rev-parse HEAD)
  release_count=$(git log --pretty=oneline "${tag}"..."${commit}" | wc -l | xargs expr 1 +)

  [ -z "${release_count}" ] && release_count="1"

  pkg_name=$(grep 'Package' ../packaging/debian/control | cut -d' ' -f 2)
  pkg_version=$(grep "VERSION .* LANGUAGES" ../CMakeLists.txt  -o | cut -d' ' -f2)
  build_dir="${pkg_name}-${pkg_version}"
  deb_name="${pkg_name}_${pkg_version}-${release_count}${distro_type}_amd64.deb"

  popd
  if [ -d "${build_dir}" ];then
    rm -rf "${build_dir}" || (echo "can not clear build directory ${build_dir}";exit 1)
  fi

  mkdir -p "${build_dir}"/opt/MPax/translation
  mkdir -p "${build_dir}"/usr/share/applications/
  cp -raf ../packaging/debian/ "${build_dir}"/DEBIAN
  sed -i "s/@@deb_version@@/${pkg_version}-${release_count}${distro_type}/1" "${build_dir}"/DEBIAN/control
  cp -raf ../packaging/assets/mpax.desktop "${build_dir}"/usr/share/applications/
  cp -raf ../cmake-build-release/MPax "${build_dir}"/opt/MPax/
  cp -raf ../cmake-build-release/translation/*.qm "${build_dir}"/opt/MPax/translation/ || exit 0
  cp -raf ../resource/pic/logo/* "${build_dir}"/opt/MPax/

  dpkg -b "${build_dir}"  "${deb_name}" && rm -rf "${build_dir}"
  if [ "${WORK_DIR}" != "${SCRIPT_DIR}" ];then
    popd && mv "${SCRIPT_DIR}/${deb_name}" .
  fi
}

build_binary
build_deb