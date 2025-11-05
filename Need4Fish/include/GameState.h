#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Windows.h"
#include "systems/CameraSystem.h"

struct GameState {
    inline static bool isPaused = false;
};

inline void ChangePauseStatus() {
    if (GameState::isPaused) //Sortie de pause : recapture
    {        
        CameraSystem::SetMouseCursor();
        GameState::isPaused = false;
    }
    else //Mise en pause : libération
    {
        ShowCursor(TRUE);
        Camera::isMouseCaptured = false;

        ClipCursor(nullptr);
        ReleaseCapture();

        GameState::isPaused = true;
    }
}
#endif