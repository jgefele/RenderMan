#! /bin/bash
cd "$(dirname "$0")"/Documentation

###### TEST ######
if [[ "$TRAVIS" == "" ]]; then
    INTERACTIVE_NB_DIFF=--nbdime;
fi
python --version
python2 --version
which python
which python2
which pip2
ls -l /usr/bin
ls -l /usr/local/bin
python2 -c "import librenderman as rm; import utils; dexed=utils.download_plugins(); print(dexed); e = rm.RenderEngine(44100,512,512); rc = e.load_plugin(dexed); assert(rc)" || exit $?
py.test --nbval $INTERACTIVE_NB_DIFF || exit $?
