#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"


EventSystem* g_theEventSystem = new EventSystem();

EventSubscription::EventSubscription( EventCallbackFunction callback ) :
    m_callbackFunction(callback) {
}

EventSystem::EventSystem() {
    
}


EventSystem::~EventSystem() {

}


void EventSystem::Startup() {

}


void EventSystem::Shutdown() {

}


void EventSystem::BeginFrame() {

}


void EventSystem::EndFrame() {

}


void EventSystem::SubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback ) {
    SubscriptionList& subs = m_subscriptionsByEvent[eventName];
    EventSubscription* sub = new EventSubscription( callback );
    subs.push_back( sub );
}


void EventSystem::UnsubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback ) {
    SubscriptionList subs = m_subscriptionsByEvent[eventName];
    SubscriptionList::iterator subIter;

    for( subIter = subs.begin(); subIter != subs.end(); subIter++ ) {
        if( (*subIter)->m_callbackFunction == callback ) {
            subs.erase( subIter );
            return;
        }
    }
}


int EventSystem::FireEvent( const std::string& eventName ) {
    EventArgs* emptyArgs = new EventArgs();
    return FireEvent( eventName, *emptyArgs );
}


int EventSystem::FireEvent( const std::string& eventName, EventArgs& args ) {
    SubscriptionList subs = m_subscriptionsByEvent[eventName];

    int numSubs = (int)subs.size(); // This seems dangerous since each sub can (un)sub callbacks during the loop
    int subIndex;

    for( subIndex = 0; subIndex < numSubs; subIndex++ ) {
        EventSubscription* sub = subs[subIndex];
        bool consumed = sub->m_callbackFunction( args );

        // No one else needs to be alerted
        if( consumed ) {
            subIndex++;
            break;
        }
    }

    return subIndex;
}


void EventSystem::GetSubscribedEventsList( Strings& eventsList ) const {
    std::map<std::string, SubscriptionList>::const_iterator listIter;

    for( listIter = m_subscriptionsByEvent.begin(); listIter != m_subscriptionsByEvent.end(); listIter++ ) {
        const SubscriptionList& subList = listIter->second;
        int numSubs = (int)subList.size();

        if( numSubs > 0 ) {
            eventsList.push_back( listIter->first );
        }
    }
}
