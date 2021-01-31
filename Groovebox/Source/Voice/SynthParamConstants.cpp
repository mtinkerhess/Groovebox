#include "SynthParamConstants.h"

#include "MyMath.h"

short SynthParamConstants::ClampShort(short value) { return Math::Clamp(value, MinShortValue, MaxShortValue); }
double SynthParamConstants::ClampDouble(double value) { return Math::Clamp(value, MinDoubleValue, MaxDoubleValue); }

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Synth##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/SynthParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Oscillator##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/OscillatorParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Filter##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/FilterParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Envelope##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/EnvelopeParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Delay##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/DelayParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
SynthParamConstants SynthParamConstants::Lfo##name(L#name, L#shortName, min, max, initial, doubleMin, doubleMax);
#include "Voice/Macros/LfoParamMacros.h"
#undef PARAM