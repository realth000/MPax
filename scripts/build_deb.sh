#!/bin/bash
set -x

pkg_name=$(grep 'Package' ../packaging/debian/control | cut -d' ' -f 2)
pkg_version=$(grep 'Version' ../packaging/debian/control | cut -d' ' -f 2)
build_dir="${pkg_name}-${pkg_version}"

if [ -e "${build_dir}" ];then
  rm -rf "${build_dir}" || (echo "can not clear build directory ${build_dir}";exit 1)
fi

mkdir -p "${build_dir}"/opt/MPax/translation
mkdir -p "${build_dir}"/usr/share/applications/
cp -raf ../packaging/debian/ "${build_dir}"/DEBIAN
cp -raf ../packaging/assets/mpax.desktop "${build_dir}"/usr/share/applications/
cp -raf ../cmake-build-release/MPax "${build_dir}"/opt/MPax/
cp -raf ../cmake-build-release/translation/*.qm "${build_dir}"/opt/MPax/translation/
cp -raf ../resource/pic/logo/* "${build_dir}"/opt/MPax/
chmod +x "${build_dir}"/DEBIAN/postinst
chmod 644 "${build_dir}"/usr/share/applications/mpax.desktop

dpkg -b "${build_dir}" "${pkg_name}_${pkg_version}-1ubuntu2204_amd64.deb"