SELF := $(abspath $(lastword $(MAKEFILE_LIST)))
HERE := $(dir $(SELF))
export PROJECT_ROOT := $(realpath $(HERE)/..)

RPM_PACKAGE_NAME := qv2ray
RPM_VERSION := $(shell cat $(PROJECT_ROOT)/makespec/VERSION)
RPM_RELEASE :=
RPM_BUILDROOT := default

SPEC_FILE = $(PROJECT_ROOT)/makespec/$(RPM_PACKAGE_NAME).spec
TARBALL_OUTDIR := $(or $(outdir), $(PROJECT_ROOT)/build/tarball)
RPM_OUTDIR := $(or $(outdir), $(PROJECT_ROOT)/build/rpm)
SRPM_OUTDIR :=$(or $(outdir), $(PROJECT_ROOT)/build/srpm)
export TARBALL_BASENAME = $(RPM_PACKAGE_NAME)-$(RPM_VERSION)
export TARBALL_OUTDIR_ABS = $(abspath $(TARBALL_OUTDIR))

HAS_GIT := $(shell command -v git 2> /dev/null)
ifndef HAS_GIT
deps:
	dnf -y install git
else
deps:
endif

tarball: deps
	mkdir -p "$${TARBALL_OUTDIR_ABS}"
	# archive qv2ray
	cd $(PROJECT_ROOT) && git archive HEAD --format=tar --prefix="$${TARBALL_BASENAME}/" \
		-o "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar"
	# archive submodules
	cd $(PROJECT_ROOT) && git submodule update --init && \
	    git submodule foreach --quiet 'set -e ;\
		mkdir -p "$$(dirname -- $${TARBALL_OUTDIR_ABS}/$${path})" ;\
		git archive HEAD --format=tar \
			--prefix="$${TARBALL_BASENAME}/$${path}/" \
			-o "$${TARBALL_OUTDIR_ABS}/$${path}-$${sha1}.tar" ;\
		tar -n --concatenate --file="$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar" \
			"$${TARBALL_OUTDIR_ABS}/$${path}-$${sha1}.tar"'
	# delete `3rdparty/zxing-cpp/test` from the source tarball
	tar -f "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar" --delete "$${TARBALL_BASENAME}/3rdparty/zxing-cpp/test" || true
	# compress
	gzip -c "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar" > "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar.gz"
	# clean-up
	cd $(PROJECT_ROOT) && git submodule foreach --quiet 'rm "$${TARBALL_OUTDIR_ABS}/$${path}-$${sha1}.tar"'
	rm "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar"
	find "$${TARBALL_OUTDIR_ABS}" -type d -empty -delete
	# workaround https://pagure.io/copr/copr/issue/1258
	(id mockbuild && chown mockbuild: -R "$(PROJECT_ROOT)") || true

spec: deps
	export RPM_RELEASE=$(RPM_RELEASE) ;\
	if [ -z "$$RPM_RELEASE" ]; then RPM_RELEASE=0.git.$$(git rev-parse --short=7 HEAD)%{?dist}; fi;\
	sed -e "s/@NAME@/$(RPM_PACKAGE_NAME)/g" \
		-e "s/@VERSION@/$(RPM_VERSION)/g" \
		-e "s/@RELEASE@/$${RPM_RELEASE}/g" \
		-e "s/@SOURCE0@/$(TARBALL_BASENAME).tar.gz/g" \
		-e "s/@NAME_VERSION@/$(TARBALL_BASENAME)/g" \
		"$(SPEC_FILE).in" > "$(SPEC_FILE)"

srpm: spec tarball
	rm -rf "$(SRPM_OUTDIR)"/*.src.rpm
	mkdir -p "$(SRPM_OUTDIR)/rpmbuild/SOURCES"
	ln -sf "$${TARBALL_OUTDIR_ABS}/$${TARBALL_BASENAME}.tar.gz" "$(SRPM_OUTDIR)/rpmbuild/SOURCES/$${TARBALL_BASENAME}.tar.gz"
	rpmbuild -bs "$(SPEC_FILE)" --undefine "dist" --define "%_topdir $(SRPM_OUTDIR)/rpmbuild"
	mv "$(SRPM_OUTDIR)"/rpmbuild/SRPMS/*.src.rpm "$(SRPM_OUTDIR)/"
	rm -rf "$(SRPM_OUTDIR)/rpmbuild"

rpm: srpm
	mkdir -p "$(RPM_OUTDIR)"
	mock -r "$(RPM_BUILDROOT)" --resultdir="$(RPM_OUTDIR)" --rebuild "$(SRPM_OUTDIR)/"*.src.rpm

clean:
	rm -rf "$(RPM_OUTDIR)" "$(SRPM_OUTDIR)" "$${TARBALL_OUTDIR_ABS}" "$(SPEC_FILE)"


.PHONY: tarball spec srpm rpm clean
