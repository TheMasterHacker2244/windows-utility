# Windows Utility

A comprehensive Windows utility that provides:

1. Drive monitoring and Windows Defender exclusion management
2. Process management for Isabelle application
3. Registry and Prefetch cleaning for security
4. Cursor management based on drive status
5. File operations for moving application folders

## Features

- Automatically manages Windows Defender exclusions for specified paths
- Monitors J: drive status and adjusts system cursors accordingly
- Provides hotkey controls (PG UP/PG DN) for process management
- Cleans registry and Prefetch of suspicious entries
- Moves application folders between drives

## Building

1. Clone the repository
2. Create a build directory: `mkdir build && cd build`
3. Run CMake: `cmake ..`
4. Build the project: `cmake --build .`

## Usage

The application runs silently in the background. Use PG UP and PG DN keys to control the Isabelle application and perform cleaning operations.

## Configuration

Edit the `assets/keywords.txt` file to customize the detection patterns for registry and Prefetch cleaning.
