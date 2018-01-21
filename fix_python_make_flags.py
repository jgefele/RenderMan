#! /usr/bin/python

import os
import subprocess
import sys

os.chdir(os.path.dirname(__file__))

linker_flags=subprocess.check_output(["python2-config", "--ldflags"]).strip()
include_flags=subprocess.check_output(["python2-config", "--includes"]).strip()
include_list=include_flags.replace("-I", "").split()

from sys import platform
if platform == "linux" or platform == "linux2":
  linux_makefile="Builds/LinuxMakefile/Makefile"
  subprocess.check_call(["sed", "-i", "", 's@-I/usr/include/python2.7@'+include_flags+'@g', linux_makefile])
  subprocess.check_call(["sed", "-i", "", "s@-lpython2.7@"+linker_flags+"@g", linux_makefile])
elif platform == "darwin":
  xcode_project="Builds/MacOSX/RenderMan.xcodeproj/project.pbxproj"
  xcode_includes=' '.join(['"%s",'%i for i in include_list])
  subprocess.check_call(["sed", "-i", "", 's@"/usr/include/python2.7",@'+xcode_includes+'@g', xcode_project])
  subprocess.check_call(["sed", "-i", "", "s@-lpython2.7@"+linker_flags+"@g", xcode_project])
else:
  raise Exception("Unexpected platform: " + platform)
