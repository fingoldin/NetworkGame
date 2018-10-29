# Platformer game with C/C++ networking

A simple multiplayer platformer in the style of Smash Bros. The networking is implemented using C socket calls, the graphics is performed using the Irrlicht 3D engine.

## Demo
![Demo](https://www.github.com/fingoldin/NetworkGame/raw/master/game.gif "Two users")

## Usage
Run `make` in the `client` and `server` directories. Then, run `./main` in the appropriate directory if your machine is to be a client or server. Each server supports an arbitrary number of clients.
