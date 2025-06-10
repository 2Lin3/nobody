
This is a 2D flat-plane physics collision game where the player is controlled with the mouse to move and collide with enemies to defeat them.

# main.cpp

## Initialization and Shutdown
The `Game` class handles the initialization and shutdown of various components, such as SDL libraries, window, renderer, audio manager, and a Box2D physics world.

## Game Loop
It implements a game loop (`Loop` method) that includes handling events, updating game logic, and drawing graphics to the screen. The loop continues while the game is running (`mIsRunning`).

## Input Handling
The code processes player input from the keyboard and mouse, including movements and actions.

## Game Object Management
It manages game objects (`GameObject` instances), including creation, updating, and removal. This includes the player (`mPlayer`), enemies, and other entities in the game world.

## Collision Handling
The game uses the Box2D physics engine for collision detection and response. A `Collision` class is used as a contact listener for handling collisions.

## Rendering
The code handles rendering sprites and text to the screen, drawing the game's graphical elements like the player, enemies, background, and UI elements such as score, life bar, and boost bar.

## Audio Management
The game includes audio functionality, using SDL_mixer for playing music and sound effects.

## Game States and Transitions
It manages different game states, such as starting, pausing, and ending the game, including displaying messages and handling transitions between these states.
Commit 1 line
