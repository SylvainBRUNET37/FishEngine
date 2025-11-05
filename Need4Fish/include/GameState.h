#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Windows.h"
#include "components/camera/Camera.h"

struct GameState {
    inline static bool isPaused = false;
};

inline void SetMouseCursor()
{
    ShowCursor(FALSE);
    Camera::isMouseCaptured = true;

    HWND hwnd = GetActiveWindow();
    if (!hwnd)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    Camera::screenCenter.x = (rect.right - rect.left) / 2;
    Camera::screenCenter.y = (rect.bottom - rect.top) / 2;

    // Convertir en repère écran
    ClientToScreen(hwnd, &Camera::screenCenter);

    // Centrer le curseur
    SetCursorPos(Camera::screenCenter.x, Camera::screenCenter.y);

    // Initialiser les coordonnées de la caméra
    Camera::cursorCoordinates = Camera::screenCenter;
}

inline void ChangePauseStatus() {
    if (GameState::isPaused) //Sortie de pause : recapture
    {        
        SetMouseCursor();
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