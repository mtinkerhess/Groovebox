#pragma once

#include "Modules/SignalModule.h"
#include "Voice/Envelope.h"
#include "Voice/Oscillator.h"

template<class BackingObjectType>
class BackingObjectModule : public SignalModule
{
private:
	BackingObjectType* m_backingObject = nullptr;

public:
	BackingObjectModule() { }

	BackingObjectModule(const std::string& name, BackingObjectType* backingObject)
		: SignalModule(name, nullptr),
		m_backingObject(backingObject)
	{
	}

	BackingObjectType& GetBackingObject() { return *this->m_backingObject; }
	void SetBackingObject(BackingObjectType* backingObject) { this->m_backingObject = backingObject; }

	virtual double CalculateValue(double deltaSeconds) override
	{
		return this->m_backingObject->Next(deltaSeconds);
	}
};

using OscillatorModule = BackingObjectModule<Oscillator<false>>;
using EnvelopeModule = BackingObjectModule<Envelope>;