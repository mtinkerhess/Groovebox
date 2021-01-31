#pragma once

#include <limits>

namespace Constants
{
    const int NumTracks = 8;
    const int Pitch0Frequency = 440;
    const int SamplesPerSecond = 44100;
    const int SamplesPerChunk = SamplesPerSecond / 10;
    const int MinSampleValue = SHRT_MIN;
    const int MaxSampleValue = SHRT_MAX;
    const double PI = 3.141592653589793238462643383279502884;
}