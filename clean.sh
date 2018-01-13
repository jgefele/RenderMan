#! /bin/sh
cd "$(dirname "$0")"

###### CLEAN ######
if [[ "$(uname)" == "Darwin" ]]; then
    xcodebuild -project Builds/MacOSX/RenderMan.xcodeproj/ clean -alltargets
else
    make --directory=Builds/LinuxMakefile/ clean
fi
rm -rf .cache/ Builds/MacOSX/RenderMan.xcodeproj/project.xcworkspace/ Builds/MacOSX/RenderMan.xcodeproj/xcuserdata/ Builds/MacOSX/build/ Documentation/librenderman.so Documentation/plugins Documentation/.cache/ Documentation/__pycache__/ 2>/dev/null
