# Mega Man Mini
## Project




## Team members
Adrià Belinchon Pérez

Claudia Ruiz Muñoz

### Github acount

Belin1234

https://github.com/Belin1234


cla4d1a

https://github.com/cla4d1a

## Description

This game is inspired by Mega Man video games; it’s a platformer where you must dodge and jump over various obstacles to reach the end of the level.

## Key Features

 - Move Right, left, to advance.
 - Jump across the different platforms.


## Controls

 - Key left-> move left
 - Key right  -> move right
 - Key up -> move up in god mode
 - Key down -> move down god mode
 - H -> Show/ hides the help menu with debug controls.
 - F9 -> Displays the colliders/ game logic.
 - F10 -> Activates " God Mode".
 - F11 -> Enables/ disables FPS cap at 30.
 
## Features implemented
 - Follow Camera: The camera follows the main character troughout the map.
 - TMX-based Map (Tiled): The map is loaded from a .tmx file, which includes layers for terrain and colliders.
 - Dynamic Colliders; Platform colliders are created dynamically upon loading the map using Box2D.
 - Player Animations: The player has animations for walking,jumping and dying.
 - Controls : WASD keys for movement and space key to jump.
 - External Configuration: initial position, player speed, amimations are loaded from external files.
 - FPS Optimization :The game maintains a stable cap of 60 FPS without vsync, and the window displays current FPS, average 
                     FPS, and ms per frame
 - Normalized Movement: All game movement is normalized using deltaTime, ensuring the same movement speed across different CPU speeds.
 
