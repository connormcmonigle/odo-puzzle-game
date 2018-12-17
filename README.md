# odo-puzzle-game

This project is a simple puzzle game in which the player attempts to prevent a node within a graph from reaching another specified node within graph by deleting nodes. It uses SDL to draw lines and nodes using the solely the CPU and is not GPU accelerated at this point in time. Game play consists of clicking on nodes to delete at each step after which the moving node will take a step through the graph towards the destination node along the new optimal path. The objective is to prevent the red node from reaching the blue node within graph.

# Usage

The ```odo.cpp``` can be compiled with the command:

```g++ odo.cpp -o odo --std=c++17 -O4 -lSDL2```

It depends on the SDL 2 library. A game can be initiated by running the program and pointing it to the desired game text file. A text file for testing purposes can be found in the included ```text.txt``` file. Upon program start up, enter the file name into the console.


