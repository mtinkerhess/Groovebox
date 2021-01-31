#include "SynthParamReflection.h"

#include "MyMath.h"

using namespace SynthParamReflection;

Reflection* Reflection::s_instance = nullptr;

const Reflection& Reflection::Instance()
{
	if (s_instance == nullptr)
	{
		s_instance = new Reflection();
	}

	return *s_instance;
}