#include "controls.h"
#include "config.h"
#include "main.h"

bool showDebug = DEV;

void HandleControls()
{
    if (DEV)
    {
        if (IsKeyDown(KEY_LEFT_CONTROL) && (IsKeyPressed(KEY_R) || IsKeyPressedRepeat(KEY_R)))
        {
            ReloadGame();
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && (IsKeyPressed(KEY_R) || IsKeyPressedRepeat(KEY_R)))
        {
            RegenerateWorld();
        }

        if (IsKeyPressed(KEY_F3))
            showDebug = !showDebug;
    }
    if (IsKeyPressed(KEY_F10))
    {
        int monitorCount = GetMonitorCount();
        if (monitorCount > 1)
        {
            config.preferredMonitor = (config.preferredMonitor + 1) % monitorCount;
            Vector2 monitorPos = GetMonitorPosition(config.preferredMonitor);
            float monitorWidth = (float)GetMonitorWidth(config.preferredMonitor);
            float monitorHeight = (float)GetMonitorHeight(config.preferredMonitor);

            if (IsWindowFullscreen())
            {
                // toggle off fullscreen, move window, then toggle back on
                ToggleBorderlessWindowed();
                SetWindowPosition((int)(monitorPos.x + (monitorWidth - config.windowWidth) / 2),
                                  (int)(monitorPos.y + (monitorHeight - config.windowHeight) / 2));
                ToggleBorderlessWindowed();
            }
            else
            {
                SetWindowPosition((int)(monitorPos.x + (monitorWidth - config.windowWidth) / 2),
                                  (int)(monitorPos.y + (monitorHeight - config.windowHeight) / 2));
            }
            configChanged = true;
        }
    }
    if (IsKeyPressed(KEY_F11))
    {
        ToggleBorderlessWindowed();
        config.borderlessFullscreen = IsWindowFullscreen();
        configChanged = true;
    }
}
