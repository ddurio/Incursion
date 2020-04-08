#pragma once
#include "Engine/Input/XboxController.hpp"


constexpr int MAX_CONTROLLERS = 4;

class InputSystem {
    public:
	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

    const XboxController& GetController( int controllerID ) const;

    private:
    XboxController m_controllers[MAX_CONTROLLERS] = {
        XboxController( 0 ),
        XboxController( 1 ),
        XboxController( 2 ),
        XboxController( 3 )
    };
};
