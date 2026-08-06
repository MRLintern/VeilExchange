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
![Application-Architecture Diagram](VeilExchange/Images/01-Application-Architecture.png)

### 2. Class Diagram
--------------------
![Class Diagram](VeilExchange/Images/02-Class-Diagram.png)

### 3. Project Structure
------------------------
![VeilExchange Structure Diagram](VeilExchange/Images/03-Project-Structure.png)
### 4. Message Sending Sequence
-------------------------------
![Message Sending Structure Diagram](VeilExchange/Images/04-Send-Message-Sequence.png)
### 5. Receive Message Sequence
-------------------------------
![Receive Message Sequence Diagram](VeilExchange/Images/05-Receive-Message-Sequence.png)
### 6. Send File Sequence
-------------------------
![Send File Sequence Diagram](VeilExchange/Images/06-Send-File-Sequence.png)
### 7. Receive Message Sequence
-------------------------------
![Receive Message Sequence Diagram](VeilExchange/Images/07-Receive-File-Sequence.png)
### 8. Connection State
-----------------------
![Connection State Diagram](VeilExchange/Images/08-Connection-State.png)
### 9. Use Case
---------------
![Use Case Diagram](VeilExchange/Images/09-Use-Case.png)
### 10. Deployment
------------------
![VeilExchange Deployment Diagram](VeilExchange/Images/10-Deployment.png)
### 11. Message Protocol
------------------------
![Message Protocol Diagram](VeilExchange/Images/11-Message-Protocol.png)
### 12. Thread Model
--------------------
![Thread Model Diagram](VeilExchange/Images/12-Thread-Model.png)

## Building the Application
---------------------------
* `$ git clone git@github.com:MRLintern/VeilExchange.git`
* `$ cd VeilExchange`
* `$ mkdir -p build && cd build`
* `$ cmake ..`
* `$ cmake --build .`
## Running the Application
--------------------------

## References
-------------

* [The C++ Alliance](https://docs.cppalliance.org/user-guide/task-networking.html).
* ___Network Programming with C++: Build Efficient Communication Systems___, by __Robert Johnson__.
* ___C++ Networking 101: Unlocking Sockets, Protocols, VPNs, and Asynchronous I/O with 75+ Sample Programs___, by __Anais Sutherland__.
* ___Boost.Asio C++ Network Programming: Enhance your Skills with Practical Examples for C++ Network Programming___, by __John Torjo__, __PACKT Publishing (Open Source)__. 
