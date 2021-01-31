#pragma once

#include <functional>

using SimpleSynthParamGetter = std::function<class SynthVoiceParam& ()>;
using SynthParamGetter = std::function<class SynthVoiceParam& (class SynthVoiceParams&)>;
using SynthOptionalParamGetter = std::function<class SynthVoiceOptionalParam& (class SynthVoiceOptionalParams&)>;
using SynthModulationShortParamGetter = std::function<short& (class SynthVoiceModulationShortParams&)>;
