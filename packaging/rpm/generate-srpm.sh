#!/usr/bin/bash

# Specific branch, tag, or commit that you want to build
committish=${committish:-"dev"}
# Package release number
pkgrel=${pkgrel:-"1"}
# Install git-archive-all and initialize repository
pip install git-archive-all
git clone https://github.com/sixg0000d/Qv2ray.git
pushd Qv2ray
git checkout ${committish}
# Get git and version specs
TAG=$(git tag -l --points-at HEAD)
COMMIT=$(git rev-parse HEAD)
VERSION=$(cat makespec/VERSION)
VERSIONSUFFIX=$(cat makespec/VERSIONSUFFIX)
# Generate spec variables
rpm_version=${VERSION}${VERSIONSUFFIX//"-"/"~"}
if [ -n "${TAG}" ]; then
    rpm_release=${pkgrel}%{?dist}
    rpm_name_version=Qv2ray-"${TAG##v}"
else
    rpm_release=${pkgrel}.$(git log -1 --format="%cd" --date=format:"%Y%m%d")git${COMMIT:0:7}%{?dist}
    rpm_name_version=Qv2ray-"${COMMIT}"
fi
rpm_source0="${rpm_name_version}".tar.gz
# Generate spec file
sed -e "s/@VERSION@/${rpm_version}/g" \
    -e "s/@RELEASE@/${rpm_release}/g" \
    -e "s/@SOURCE0@/${rpm_source0}/g" \
    -e "s/@NAME_VERSION@/${rpm_name_version}/g" \
    packaging/rpm/qv2ray.spec.in >../qv2ray.spec
# Generate source tarball
git archive-all --force-submodules ../"${rpm_source0}"
# Generate source rpm
popd
rpmbuild \
    --define "_sourcedir $(pwd)" \
    --define "_srcrpmdir $(pwd)" \
    -bs qv2ray.spec
# Clean working directory
rm -rf Qv2ray
