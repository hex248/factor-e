# PERFORMANCE

-   find a way to keep track of all loaded sprites, and only unload at the end (at the moment, the sprites are loaded + unloaded every single time they are drawn)
-   render at 1920x1080 and scale up or down to screen resolution. this ensures that 1440p doesn't have more fov than 1080p, and 720p doesn't have less

# VISUAL

-   fade between tiles on ground

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

# ADMIN:

-   fix warnings
