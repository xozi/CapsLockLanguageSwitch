# Minimum CMake version requirement
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CapsLockLanguageSwitcher)

# Create the executable file
# The "WIN32" keyword means creating a GUI application without a console window.
add_executable(CapsLockLanguageSwitcher WIN32 
    main.cpp 
    resource.rc
    lang/kr.cpp
    lang/ja.cpp
    lang/en_us.cpp
 "lang/switch.cpp" "lang/switch.h")

# Add compiler definitions for Unicode build
target_compile_definitions(CapsLockLanguageSwitcher PRIVATE UNICODE _UNICODE)

# Link Windows API libraries
# user32.lib contains basic UI functions such as window management, messages, and keyboard input.
target_link_libraries(CapsLockLanguageSwitcher PRIVATE user32 shell32)