#! /bin/bash
cd "$(dirname "$0")"

###### BUILD ######
if [[ "$(uname)" == "Darwin" ]]; then
    xcodebuild -project Builds/MacOSX/RenderMan.xcodeproj/ || exit $?
else
    make --directory=Builds/LinuxMakefile/ || exit $?
fi

###### TEST ######
if [[ "$TRAVIS" == "" ]]; then
    INTERACTIVE_NB_DIFF=--nbdime;
fi
py.test Documentation --nbval $INTERACTIVE_NB_DIFF || exit $?
