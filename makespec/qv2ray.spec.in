%if 0%{?rhel}
%global use_system_grpc 0
%else
%global use_system_grpc 1
%endif

Name:           @NAME@
Version:        @VERSION@
Release:        @RELEASE@
Summary:        A Qt frontend for V2Ray, written in c++.

License:        GPLv3
URL:            https://github.com/Qv2ray/Qv2ray
Source0:        @SOURCE0@

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qttools-devel
BuildRequires:  protobuf-compiler
BuildRequires:  protobuf-devel

%if 0%{?use_system_grpc}
BuildRequires:  grpc-devel
BuildRequires:  grpc-plugins
%endif

%description
A Qt based cross-platform GUI fontend for V2Ray.

%prep
%setup -q -n @NAME_VERSION@


%build
%cmake -DCMAKE_BUILD_TYPE:STRING=Release -DUSE_LIBQVB:BOOL=OFF -DEMBED_TRANSLATIONS:BOOL=ON .
%make_build


%install
rm -rf "$RPM_BUILD_ROOT"
%make_install
install -D LICENSE "$RPM_BUILD_ROOT"/%{_datadir}/licenses/qv2ray/LICENSE
install -D README.md "$RPM_BUILD_ROOT"/%{_docdir}/qv2ray/README.md


%files
%license LICENSE
%doc README.md
%{_bindir}/qv2ray
%{_datadir}/icons/*
%{_datadir}/applications/*
%{_datadir}/metainfo/*


%changelog
