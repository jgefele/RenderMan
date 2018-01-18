#! /bin/bash
exit 1

cd "$(dirname "$0")"

###### BUILD ######
if [[ "$(uname)" == "Darwin" ]]; then
    xcodebuild -project Builds/MacOSX/RenderMan.xcodeproj/ || exit $?
else
    make --directory=Builds/LinuxMakefile/ || exit $?
fi
