# PERFORMANCE
-   find a way to keep track of all loaded sprites, and only unload at the end (at the moment, the sprites are loaded + unloaded every single time they are drawn)
# VISUAL
- make it **isometric**
- fade between tiles on ground
# GAMEPLAY
-   inventory
-   "spawn in" menu (debug mode)
-   tools
-   break + place tiles
# WORLD MANAGEMENT
-   save world
-   load world from save on start
-   multiple layers (object layer)
# DEV MODE:
-   ctrl + r will now reload the world rather than regenerating
-   ctrl + shift + r will regenerate the world, but will prompt the user to confirm their action before doing so
# ART:
-   basic player animation
-   better grass texture
-   better dirt texture
# UI:
-   custom UI library for use across the game and in DEV MODE
    -   spawn & edit panels
    -   alerts (fading after Xms)
    -   display image
    -   display text
