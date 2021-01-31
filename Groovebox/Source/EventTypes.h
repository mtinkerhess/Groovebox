#pragma once

#include "MulticastDelegate.h"

class SequenceStep;

class NoteStartedEvent
{
public:
    int Track;
    double Pitch;
    double Velocity;

    NoteStartedEvent(int track, double pitch, double velocity) : Track(track), Pitch(pitch), Velocity(velocity)
    {
    }
};

class NoteEndedEvent
{
public:
    int Track;

    explicit NoteEndedEvent(int track) : Track(track) { }
};

class StepChangedEvent
{
public:
    int Track = 0;
    const SequenceStep* Step = nullptr;
    bool IsFirstStep = false;

    StepChangedEvent() { }
    StepChangedEvent(int track, const SequenceStep* step, bool isFirstStep) : Track(track), Step(step), IsFirstStep(isFirstStep) { }

    StepChangedEvent& operator =(const StepChangedEvent& other)
    {
        Track = other.Track;
        Step = other.Step;
        return *this;
    }
};

typedef MulticastDelegateWithPendingEvents<NoteStartedEvent> NoteStartedMulticastDelegate;
typedef MulticastDelegateWithPendingEvents<NoteEndedEvent> NoteEndedMulticastDelegate;
typedef MulticastDelegateWithPendingEvents<StepChangedEvent> StepChangedMulticastDelegate;

typedef decltype(NoteStartedMulticastDelegate::MulticastDelegate)::EventHandler NoteStartedHandler;
typedef decltype(NoteEndedMulticastDelegate::MulticastDelegate)::EventHandler NoteEndedHandler;
typedef decltype(StepChangedMulticastDelegate::MulticastDelegate)::EventHandler StepChangedHandler;

