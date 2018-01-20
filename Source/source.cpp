/*
  ==============================================================================

    source.cpp
    Created: 18 Feb 2017 6:37:01pm
    Author:  tollie

  ==============================================================================
*/

#include <boost/python.hpp>
#include "RenderEngine.h"
#include "PatchGenerator.h"

// Could also easily be namespace crap.
namespace wrap
{
    //==========================================================================
    // Converts a std::vector or std::array to a Python list.
    template <typename ForwardIterable>
    boost::python::list containerToPythonList (ForwardIterable container)
    {
    	boost::python::list list;
    	for (const auto& element : container)
            list.append(element);
    	return list;
    }

    template <typename T1, typename T2>
    boost::python::tuple pairToTuple (std::pair<T1, T2> pair)
    {
        return boost::python::make_tuple (pair.first, pair.second);
    }

    template <typename T1, typename T2>
    boost::python::list vectorOfPairsToListOfTuples(std::vector <std::pair <T1,T2>> vector)
    {
    	boost::python::list list;
        for (const auto& pair : vector)
    		list.append(pairToTuple(pair));
    	return list;
    }

    //==========================================================================
    // std::vector <std::array <double, 13>> MFCCFeatures;
    boost::python::list mfccFramesToListOfLists (MFCCFeatures frames)
    {
        boost::python::list list;
        for (const auto& frame : frames)
    		list.append(containerToPythonList(frame));
    	return list;
    }

    //==========================================================================
    PluginPatch listOfTuplesToPluginPatch (boost::python::list listOfTuples)
    {
        PluginPatch patch;
        const int size = boost::python::len (listOfTuples);
        patch.reserve (size);
        std::pair <int, float> parameter;
        for (int i = 0; i < size; ++i)
        {
            boost::python::tuple tup;
            tup = boost::python::extract<boost::python::tuple> (listOfTuples[i]);
            int   index = int (boost::python::extract<float> (tup[0]));
            float value = float (boost::python::extract<float> (tup[1]));
            parameter = std::make_pair (index, value);
            patch.push_back (parameter);
        }
        return patch;
    }

    //==========================================================================
    class RenderEngineWrapper : public RenderEngine
    {
    public:
        RenderEngineWrapper (int sr, int bs, int ffts) :
            RenderEngine (sr, bs, ffts)
        { }

        void wrapperSetPatch (boost::python::list listOfTuples)
        {
            PluginPatch patch = listOfTuplesToPluginPatch (listOfTuples);
            RenderEngine::setPatch(patch);
        }

        boost::python::list wrapperGetPatch()
        {
            return vectorOfPairsToListOfTuples(RenderEngine::getPatch());
        }

        void wrapperRenderPatch (int    midiNote,
                                 int    midiVelocity,
                                 double noteLength,
                                 double renderLength)
        {
            if (midiNote > 255) midiNote = 255;
            if (midiNote < 0) midiNote = 0;
            if (midiVelocity > 255) midiVelocity = 255;
            if (midiVelocity < 0) midiVelocity = 0;
            RenderEngine::renderPatch(midiNote,
                                      midiVelocity,
                                      noteLength,
                                      renderLength);
        }

        boost::python::list wrapperGetMFCCFrames()
        {
            return mfccFramesToListOfLists (RenderEngine::getMFCCFrames());
        }

        boost::python::list wrapperGetPluginParameters()
        {
            return vectorOfPairsToListOfTuples(RenderEngine::getPluginParameters());
        }

        boost::python::list wrapperGetAudioFrames()
        {
            return containerToPythonList (RenderEngine::getAudioFrames());
        }

        boost::python::list wrapperGetRMSFrames()
        {
            return containerToPythonList (RenderEngine::getRMSFrames());
        }
    };

    //==========================================================================
    class PatchGeneratorWrapper : public PatchGenerator
    {
    public:
        PatchGeneratorWrapper (RenderEngine& engine, uint64_t seed=0) :
            PatchGenerator (engine, seed)
        { }

        boost::python::tuple wrapperGetRandomParameter (int index)
        {
            return pairToTuple (PatchGenerator::getRandomParameter (index));
        }

        boost::python::list wrapperGetRandomPatch()
        {
            return vectorOfPairsToListOfTuples(PatchGenerator::getRandomPatch());
        }
    };
}

//==============================================================================
BOOST_PYTHON_MODULE(librenderman)
{
    using namespace boost::python;
    using namespace wrap;

    class_<RenderEngineWrapper, boost::noncopyable>("RenderEngine", init<int, int, int>())
    .def("load_plugin", &RenderEngineWrapper::loadPlugin)
    .def("set_patch", &RenderEngineWrapper::wrapperSetPatch)
    .def("get_patch", &RenderEngineWrapper::wrapperGetPatch)
    .def("render_patch", &RenderEngineWrapper::wrapperRenderPatch)
    .def("get_mfcc_frames", &RenderEngineWrapper::wrapperGetMFCCFrames)
    .def("get_plugin_parameters_description", &RenderEngineWrapper::wrapperGetPluginParameters)
    .def("override_plugin_parameter", &RenderEngineWrapper::overridePluginParameter)
    .def("remove_overriden_plugin_parameter", &RenderEngineWrapper::removeOverridenParameter)
    .def("get_audio_frames", &RenderEngineWrapper::wrapperGetAudioFrames)
    .def("get_rms_frames", &RenderEngineWrapper::wrapperGetRMSFrames)
    .def("write_to_wav", &RenderEngineWrapper::writeToWav);

    class_<PatchGeneratorWrapper>("PatchGenerator", init<RenderEngineWrapper&, optional<uint64_t>>())
    .def("get_random_parameter", &PatchGeneratorWrapper::wrapperGetRandomParameter)
    .def("get_random_patch", &PatchGeneratorWrapper::wrapperGetRandomPatch);
}
