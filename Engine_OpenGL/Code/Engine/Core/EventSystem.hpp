#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "map"


typedef NamedStrings EventArgs;
typedef bool( *EventCallbackFunction )(EventArgs& args);

class EventSubscription {
    friend class EventSystem;

    private:
    explicit EventSubscription( EventCallbackFunction callback );

    EventCallbackFunction m_callbackFunction = nullptr;
};


class EventSystem {
    typedef std::vector<EventSubscription*> SubscriptionList;

    public:
    EventSystem();
    ~EventSystem();

    void Startup();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void SubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback );
    void UnsubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback );

    int FireEvent( const std::string& eventName );
    int FireEvent( const std::string& eventName, EventArgs& args );

    void GetSubscribedEventsList( Strings& eventsList ) const;

    private:
    std::map<std::string, SubscriptionList> m_subscriptionsByEvent;
};
