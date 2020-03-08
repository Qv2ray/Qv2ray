# -*- Mode:Python; indent-tabs-mode:nil; tab-width:4 -*-
#
# Copyright (C) 2015-2016 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""The cmake plugin is useful for building cmake based parts.

These are projects that have a CMakeLists.txt that drives the build.
The plugin requires a CMakeLists.txt in the root of the source tree.

If the part has a list of build-snaps listed, the part will be set up in
such a way that the paths to those snaps are used as paths for find_package
and find_library by use of `CMAKE_FIND_ROOT_PATH``.
This plugin uses the common plugin keywords as well as those for "sources".
For more information check the 'plugins' topic for the former and the
'sources' topic for the latter.

Additionally, this plugin uses the following plugin-specific keywords:

    - configflags:
      (list of strings)
      configure flags to pass to the build using the common cmake semantics.
"""

import logging
import os
from typing import List, Optional
import subprocess

import snapcraft
from snapcraft.internal import errors


logger = logging.getLogger(name=__name__)


class _Flag:
    def __str__(self) -> str:
        if self.value is None:
            flag = self.name
        else:
            flag = "{}={}".format(self.name, self.value)
        return flag

    def __init__(self, flag: str) -> None:
        parts = flag.split("=")
        self.name = parts[0]
        try:
            self.value: Optional[str] = parts[1]
        except IndexError:
            self.value = None


class CMakePlugin(snapcraft.BasePlugin):
    @classmethod
    def schema(cls):
        schema = super().schema()
        schema["properties"]["configflags"] = {
            "type": "array",
            "minitems": 1,
            "uniqueItems": True,
            "items": {"type": "string"},
            "default": [],
        }
        # For backwards compatibility
        schema["properties"]["make-parameters"] = {
            "type": "array",
            "minitems": 1,
            "uniqueItems": True,
            "items": {"type": "string"},
            "default": [],
        }
        schema["required"] = ["source"]

        return schema

    @classmethod
    def get_build_properties(cls):
        # Inform Snapcraft of the properties associated with building. If these
        # change in the YAML Snapcraft will consider the build step dirty.
        return super().get_build_properties() + ["configflags"]

    def __init__(self, name, options, project):
        super().__init__(name, options, project)
        self.build_packages.append("cmake")
        self.build_packages.append("ninja-build")
        self.out_of_source_build = True

        cmd = '''
        echo 'deb http://archive.neon.kde.org/unstable bionic main' > /etc/apt/sources.list.d/neon.list
        echo 'deb http://ppa.launchpad.net/ymshenyu/grpc-1/ubuntu bionic main' > /etc/apt/sources.list.d/grpc.list
        sudo apt-key adv --keyserver keyserver.ubuntu.com --recv E6D4736255751E5D
        sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 281f24e574404629aa3bda1a4f10c386c55cdb04
        sudo apt update -qq
        '''

        subprocess.call(cmd, shell=True)

        if project.info.get_build_base() not in ("core", "core16", "core18"):
            raise errors.PluginBaseError(
                part_name=self.name, base=project.info.get_build_base()
            )

        if options.make_parameters:
            logger.warning("make-paramaters is deprecated, ignoring.")

    def build(self):
        source_subdir = getattr(self.options, "source_subdir", None)
        if source_subdir:
            sourcedir = os.path.join(self.sourcedir, source_subdir)
        else:
            sourcedir = self.sourcedir

        env = self._build_environment()
        configflags = self._get_processed_flags()

        self.run(["cmake", sourcedir, "-DCMAKE_INSTALL_PREFIX="] + configflags, env=env)

        # TODO: there is a better way to specify the job count on newer versions of cmake
        # https://github.com/Kitware/CMake/commit/1ab3881ec9e809ac5f6cad5cd84048310b8683e2
        self.run(
            ["cmake", "--build", ".", "--", "-j{}".format(self.parallel_build_count)],
            env=env,
        )

        self.run(["cmake", "--build", ".", "--target", "install"], env=env)

    def _get_processed_flags(self) -> List[str]:
        # Return the original if no build_snaps are in options.
        if not self.options.build_snaps:
            return self.options.configflags

        build_snap_paths = [
            os.path.join(os.path.sep, "snap", snap_name.split("/")[0], "current")
            for snap_name in self.options.build_snaps
        ]

        flags = [_Flag(f) for f in self.options.configflags]
        for flag in flags:
            if flag.name == "-DCMAKE_FIND_ROOT_PATH":
                flag.value = "{};{}".format(flag.value, ";".join(build_snap_paths))
                break
        else:
            flags.append(
                _Flag("-DCMAKE_FIND_ROOT_PATH={}".format(";".join(build_snap_paths)))
            )

        return [str(f) for f in flags]

    def _build_environment(self):
        env = os.environ.copy()
        env["DESTDIR"] = self.installdir
        env["CMAKE_PREFIX_PATH"] = "$CMAKE_PREFIX_PATH:{}".format(
            self.project.stage_dir
        )
        env["CMAKE_INCLUDE_PATH"] = "$CMAKE_INCLUDE_PATH:" + ":".join(
            ["{0}/include", "{0}/usr/include", "{0}/include/{1}", "{0}/usr/include/{1}"]
        ).format(self.project.stage_dir, self.project.arch_triplet)
        env["CMAKE_LIBRARY_PATH"] = "$CMAKE_LIBRARY_PATH:" + ":".join(
            ["{0}/lib", "{0}/usr/lib", "{0}/lib/{1}", "{0}/usr/lib/{1}"]
        ).format(self.project.stage_dir, self.project.arch_triplet)

        return env
