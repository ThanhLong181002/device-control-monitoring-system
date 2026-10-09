# Device Control & Monitoring System

A C++17 client-server application for controlling and monitoring a simulated industrial device over TCP/IP.

## Features

- Simulates an industrial device with `IDLE`, `RUNNING`, `ERROR`, and `EMERGENCY_STOP` states.
- Supports `START`, `STOP`, `RESET`, `EMERGENCY_STOP`, and `STATUS` commands.
- Monitors device temperature in a background thread.
- Uses mutexes to protect shared device state.
- Supports multiple sequential client connections.
- Handles server shutdown with `Ctrl+C`.
- Uses CMake for project configuration and Git for version control.

## Architecture

Control Application
|
TcpClient
|
TCP/IP
|
TcpServer
|
CommandProcessor
|
Device
^
|
DeviceMonitor

## Technologies

- C++17
- TCP/IP and Windows Winsock
- Multithreading (`std::thread`)
- Synchronization (`std::mutex`, `std::atomic`)
- CMake
- Git

## Build

Requirements:

- A C++17-compatible compiler
- CMake
- Windows (the current networking implementation uses Winsock)

From the project root, run:

````bash
cmake -S . -B build
cmake --build build


## Run

Open two terminals in the project root.

Terminal 1 — Start the server:

powershell
.\build\Debug\device_control.exe server


Terminal 2 — Start the client:

```powershell
.\build\Debug\device_control.exe client


If your generator uses a different output directory, adjust the executable path accordingly.

Press `Ctrl+C` in the server terminal to request shutdown.

## Supported Commands

| Command          | Description                                         |
| ---------------- | --------------------------------------------------- |
| `START`          | Start the simulated device                          |
| `STOP`           | Stop the device                                     |
| `RESET`          | Reset an error or emergency-stop state              |
| `EMERGENCY_STOP` | Stop actuators and enter emergency-stop state       |
| `STATUS`         | Retrieve the current device state and sensor values |

## Demo Scenario

1. Start the server and client.
2. Send `START` and observe the device enter `RUNNING`.
3. Send `STATUS` and inspect the current temperature and actuator states.
4. Send `STOP` and observe the device return to `IDLE`.
5. Try `EMERGENCY_STOP` and `RESET`.
6. Exit the client, reconnect, and verify the server remains available.
7. Press `Ctrl+C` to shut down the server.

## Project Scope

This project is a software simulation intended to demonstrate device-control logic, TCP/IP communication, multithreading, synchronization, and basic resource management. It does not control physical industrial equipment.
````
