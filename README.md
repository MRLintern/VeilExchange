# VeilExchange
--------------

## Introduction
---------------

* ___VeilExchange___ is a __Peer-to-Peer__ (__P2P__) communication platform written in __Modern C++__  which enables users to securely exchanged __encrypted messages__ and __files__ without relying on a __central server__.

## Technology Stack
-------------------
* `Modern C++`.
* Compiler: `gcc/g++ 15.1.0`.
* Developed on `Linux (Ubuntu 20.04)`.
* Developed using `MS VSCode`. Note: If you wish to edit any code yourself, any text editor will do.
* `CMake`. Note: version: `4.0.2`.
* `Boost.Asio` for `Networking`.
* The `TLS protocol` for `encryption`.
* Note: `PlantUML` was used for the ` Software Architecture Diagrams`.

## VeilExchange Architecture
---------------------------

### 1. Application Architecture
-------------------------------
![Application-Architecture Diagram](Architecture/Images/01-Application-Architecture.png)

### 2. Class Diagram
--------------------
![Class Diagram](Architecture/Images/02-Class-Diagram.png)

### 3. Project Structure
------------------------
![VeilExchange Structure Diagram](Architecture/Images/03-Project-Structure.png)
### 4. Message Sending Sequence
-------------------------------
![Message Sending Structure Diagram](Architecture/Images/04-Send-Message-Sequence.png)
### 5. Receive Message Sequence
-------------------------------
![Receive Message Sequence Diagram](Architecture/Images/05-Receive-Message-Sequence.png)
### 6. Send File Sequence
-------------------------
![Send File Sequence Diagram](Architecture/Images/06-Send-File-Sequence.png)
### 7. Receive Message Sequence
-------------------------------
![Receive Message Sequence Diagram](Architecture/Images/07-Receive-File-Sequence.png)
### 8. Connection State
-----------------------
![Connection State Diagram](Architecture/Images/08-Connection-State.png)
### 9. Use Case
---------------
![Use Case Diagram](Architecture/Images/09-Use-Case.png)
### 10. Deployment
------------------
![VeilExchange Deployment Diagram](Architecture/Images/10-Deployment.png)
### 11. Message Protocol
------------------------
![Message Protocol Diagram](Architecture/Images/11-Message-Protocol.png)
### 12. Thread Model
--------------------
![Thread Model Diagram](Architecture/Images/12-Thread-Model.png)

## Building the Application
---------------------------
* `$ git clone git@github.com:MRLintern/VeilExchange.git`
* `$ cd VeilExchange`
* `$ mkdir -p build && cd build`
* `$ cmake ..`
* `$ cmake --build .`
* If you want to experiment with the file, `fluid_flow.dat` file, place it inside the `build` directory.
## Running the Application
--------------------------
* In `VSCode`, split the terminal in two.
* In one terminal: `$ ./VeilExchange listen 5000`
* In the other terminal: `$ ./VeilExchange connect 127.0.0.1 5000`
* Now peers can chat.
* To send a file from 1 peer to another: `$ /file fluid_flow.dat`.
* To end the conversation/disconnect: `$ /quit`
## References
-------------

* [The C++ Alliance](https://docs.cppalliance.org/user-guide/task-networking.html).
* ___Network Programming with C++: Build Efficient Communication Systems___, by __Robert Johnson__.
* ___C++ Networking 101: Unlocking Sockets, Protocols, VPNs, and Asynchronous I/O with 75+ Sample Programs___, by __Anais Sutherland__.
* ___Boost.Asio C++ Network Programming: Enhance your Skills with Practical Examples for C++ Network Programming___, by __John Torjo__, __PACKT Publishing (Open Source)__. 
