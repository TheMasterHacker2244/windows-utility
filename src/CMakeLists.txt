cmake_minimum_required(VERSION 3.15)
project(WindowsUtility)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Windows API
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Executable
add_executable(windows_utility
    src/main.cpp
    src/drive_monitor.cpp
    src/process_manager.cpp
    src/registry_cleaner.cpp
    src/cursor_manager.cpp
)

# Link libraries
if(WIN32)
    target_link_libraries(windows_utility user32 advapi32 shell32)
endif()
