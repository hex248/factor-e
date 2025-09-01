#include "controls.h"
#include "config.h"
#include "main.h"
#include "player.h"

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

    // toolbelt slots
    if (IsKeyPressed(KEY_ONE))
    {
        if (player.selectedSlot == 0)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 0;
    }
    else if (IsKeyPressed(KEY_TWO))
    {
        if (player.selectedSlot == 1)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 1;
    }
    else if (IsKeyPressed(KEY_THREE))
    {
        if (player.selectedSlot == 2)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 2;
    }
    else if (IsKeyPressed(KEY_FOUR))
    {
        if (player.selectedSlot == 3)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 3;
    }
    else if (IsKeyPressed(KEY_FIVE))
    {
        if (player.selectedSlot == 4)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 4;
    }
    else if (IsKeyPressed(KEY_SIX))
    {
        if (player.selectedSlot == 5)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 5;
    }
    else if (IsKeyPressed(KEY_SEVEN))
    {
        if (player.selectedSlot == 6)
            player.selectedSlot = 7;
        else
            player.selectedSlot = 6;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player.selectedSlot < 7)
    {
        ItemStack current = player.inventory[player.selectedSlot];
        if (current.isTool)
        {
            Item toolItem = GetItemByID(current.itemID);
            Tool tool = GetToolByName(toolItem.name);

            player.UseTool(tool);
        }
        if (current.isPlaceable)
        {
            if (strcmp(player.hovering.name, "Empty") == 0 && player.hoveringIndex >= 0)
            {
                printf("Placing tile: %d\n", current.itemID);
                PlaceTile(player.hoveringIndex, current.itemID);
                player.inventory[player.selectedSlot].quantity--;
                if (player.inventory[player.selectedSlot].quantity == 0)
                {
                    player.inventory[player.selectedSlot] = {};
                }
            }
        }
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
