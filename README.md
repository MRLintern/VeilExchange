# VeilExchange

## Introduction

__VeilExchange__ is a __Peer-to-Peer__ (__P2P__) messaging application which allows users to __exchange encrypted messages__ and __files__
directly over a network without a centralised server.

## Development Tools & Requirements
### Requirements
* Compiler: `gcc/g++ 8 or later` for `C++17` support.
* `CMake` for building and running the application.
### Develoment Tools
* `Linux (Ubuntu 20.04)`.
* Text editor: `MS VSCode` is being used. Note: any text editor will do, should you wish to change parts of the software.
* [Boost.Asio](https://www.boost.org/doc/user-guide/task-networking.html) library for networking.
* [PlantUML](https://plantuml.com/) for `UML CLass` and related diagrams. This requires `Java`. `Java requires: [OpenJDK](https://openjdk.org/?utm_source=chatgpt.com).
* `Catch2` for unit testing.

## Running/Using the Application
### Using the Application
* `$ mkdir build && cd build`
* `$ cmake ..`
* `$ cmake --build .`
* `$ ./main`

### Testing
* `$ mkdir build && cd build`
* `$ cmake ..`
* `$ cmake --build .`
* `$ ctest`
  
## Other
* Note: this project was developed as part of a training program with [WithYouWithMe](https://withyouwithme.com/).
