#!/bin/bash
set -x

[ "$USER" != "root" ] && echo "need to run as root" >&2 && exit

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

pkg_name=$(grep 'Package' ../packaging/debian/control | cut -d' ' -f 2)
pkg_version=$(grep 'Version' ../packaging/debian/control | cut -d' ' -f 2)
build_dir="${pkg_name}-${pkg_version}"

if [ -e "${build_dir}" ];then
  rm -rf "${build_dir}" || (echo "can not clear build directory ${build_dir}";exit 1)
fi

mkdir -p "${build_dir}"/opt/MPax/translation
mkdir -p "${build_dir}"/usr/share/applications/
cp -rf ../packaging/debian/ "${build_dir}"/DEBIAN
cp -rf ../packaging/assets/mpax.desktop "${build_dir}"/usr/share/applications/
cp -rf ../cmake-build-release/MPax "${build_dir}"/opt/MPax/
cp -rf ../cmake-build-release/translation/*.qm "${build_dir}"/opt/MPax/translation/
cp -rf ../resource/pic/logo/* "${build_dir}"/opt/MPax/
chown -R root.root "${build_dir}"
chmod -R 755 "${build_dir}"/DEBIAN

dpkg -b "${build_dir}" "${pkg_name}_${pkg_version}-1${distro_type}_amd64.deb"
