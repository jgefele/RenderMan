#! /bin/bash
cd "$(dirname "$0")"/Documentation

###### TEST ######
if [[ "$TRAVIS" == "" ]]; then
    INTERACTIVE_NB_DIFF=--nbdime;
fi
python -c "import librenderman as rm; import utils; dexed=utils.download_plugins(); print(dexed); e = rm.RenderEngine(44100,512,512); rc = e.load_plugin(dexed); assert(rc)" || exit $?
py.test --nbval $INTERACTIVE_NB_DIFF || exit $?
