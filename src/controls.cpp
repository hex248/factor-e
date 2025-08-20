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
            int monitorWidth = GetMonitorWidth(config.preferredMonitor);
            int monitorHeight = GetMonitorHeight(config.preferredMonitor);

            if (IsWindowFullscreen())
            {
                // toggle off fullscreen, move window, then toggle back on
                ToggleBorderlessWindowed();
                SetWindowPosition((int)monitorPos.x + (monitorWidth - config.windowWidth) / 2,
                                  (int)monitorPos.y + (monitorHeight - config.windowHeight) / 2);
                ToggleBorderlessWindowed();
            }
            else
            {
                SetWindowPosition((int)monitorPos.x + (monitorWidth - config.windowWidth) / 2,
                                  (int)monitorPos.y + (monitorHeight - config.windowHeight) / 2);
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
