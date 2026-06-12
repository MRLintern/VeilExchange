# VeilExchange

## Introduction

__VeilExchange__ is a __Peer-to-Peer__ (__P2P__) messaging application which allows users to __exchange encrypted messages__ and __files__
directly over a network without a centralised server. The key difference between a __server-peer__ and a __P2P__ application is that both computers run the same program.
__One peer__ must __initiate__ the __connection__ using the __other peer’s IP address__ and __port__. Once connected, __both peers__ can __send__ and __receive messages asynchronously__.


## Development Tools & Requirements
### Requirements
* Compiler: `gcc/g++ 8 or later` for `C++17` support.
* `CMake` for building and running the application.
### Develoment Tools
* `Linux (Ubuntu 20.04)`.
* Text editor: `MS VSCode` is being used. Note: any text editor will do, should you wish to change parts of the software.
* [Boost.Asio](https://www.boost.org/doc/user-guide/task-networking.html) library for networking.
* [PlantUML](https://plantuml.com/) for `UML CLass` and related diagrams. This requires `Java`. `Java` requires: [OpenJDK](https://openjdk.org/?utm_source=chatgpt.com).
* [Catch2](https://catch2.org/) for `Unit Testing`.
* Possible `UI`: [ncurses](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/). If not, then the `CLI` will be used.

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
