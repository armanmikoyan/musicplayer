# Project Name

A simple C++ server using TCP sockets with a Qt 6 front end.

## Overview

This project consists of a C++ server implemented with TCP sockets and a Qt 6 front end for communication with the server. The server handles incoming TCP connections and performs basic operations.

## Prerequisites

- C++ compiler with C++17 support
- Qt 6 development libraries
- CMake (minimum version x.x.x)

## Build Instructions

### Server

1. Navigate to the `server/` directory.
2. Create a build directory: `mkdir build && cd build`
3. Configure the project: `cmake ..`
4. Build the project: `cmake --build .`

### Client (Qt 6 Front End)

1. Navigate to the `client/` directory.
2. Open the project in Qt Creator or your preferred IDE.
3. Build the project.

## Running the Server

1. Execute the server binary in the `server/build/` directory after building.

   ```bash
   cd server/build/
   ./ServerExecutable

