#pragma once

#include <functional>
#include <vector>

template <typename CallbackType, class DelegateType>
class MulticastDelegateEventHandler
{
private:
    friend DelegateType;

    typename DelegateType::Callback m_callback;

    DelegateType& m_delegate;
    
public:
    MulticastDelegateEventHandler(DelegateType& delegate, typename DelegateType::Callback callback)
        : m_delegate(delegate),
        m_callback(callback)
    {
        m_delegate.Subscribe(*this);
    }

    ~MulticastDelegateEventHandler()
    {
        m_delegate.Unsubscribe(*this);
    }

private:
    template<typename... Args>
    void Handle(Args... args) { m_callback(*args...); }
};

template<typename... CallbackArgTypes>
class MulticastDelegate
{
public:
    typedef std::function<void(CallbackArgTypes ...)> Callback;
    typedef MulticastDelegateEventHandler<Callback, MulticastDelegate> EventHandler;

private:
    std::vector<EventHandler*> m_handlers;

public:
    void Subscribe(EventHandler& handler) { m_handlers.push_back(&handler); };
    void Unsubscribe(EventHandler& handler) { m_handlers.erase(std::remove(m_handlers.begin(), m_handlers.end(), &handler), m_handlers.end()); };

    void Invoke(CallbackArgTypes... callbackArgs)
    {
        for (EventHandler* handler : m_handlers)
        {
            handler->Handle(&callbackArgs...);
        }
    }
};

template<typename EventType>
class PendingEvent
{
public:
    EventType Event;
    double FireTime;

    PendingEvent(EventType event, double fireTime) : Event(event), FireTime(fireTime) { }
};

template <typename EventType>
class MulticastDelegateWithPendingEvents
{
public:
    MulticastDelegate<EventType> MulticastDelegate;
    std::vector<PendingEvent<EventType>> PendingEvents;

public:
    void ProcessEvents(double deltaSeconds, double now)
    {
        for (size_t i = 0; i < PendingEvents.size(); ++i)
        {
            if (PendingEvents[i].FireTime <= now)
            {
                MulticastDelegate.Invoke(PendingEvents[i].Event);
                PendingEvents.erase(PendingEvents.begin() + i);
                --i;
            }
        }
    }

    void PushEvent(EventType event, int samplesPerSecond, int previousSamplesProcessed, int processingSampleIndex)
    {
        double samplesPerSecondFloat = (double)samplesPerSecond;
        double timeAtStartOfBuffer = previousSamplesProcessed / samplesPerSecondFloat;
        double timeIntoBuffer = processingSampleIndex / samplesPerSecondFloat;
        PendingEvents.push_back(PendingEvent<EventType>(event, timeAtStartOfBuffer + timeIntoBuffer));
    }
};
