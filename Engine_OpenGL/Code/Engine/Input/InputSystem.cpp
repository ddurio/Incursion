#include "Engine/Input/InputSystem.hpp"


void InputSystem::Startup() {

}


void InputSystem::Shutdown() {

}


void InputSystem::BeginFrame() {
    for( int i = 0; i < MAX_CONTROLLERS; i++ ) {
        m_controllers[i].UpdateInput();
    }
}


void InputSystem::EndFrame() {

}


const XboxController& InputSystem::GetController( int controllerID ) const {
    return m_controllers[controllerID];
}
