# CapsLockLanguageSwitch

## English

### Overview
CapsLockLanguageSwitch is a utility that enhances the remote desktop experience for macOS users accessing Windows systems via apps like Parsec. macOS users are accustomed to using the CapsLock key for language input switching, but on Windows.
This is an different version of the application that was re-writtern to interface directly with Windows IME, providing language keyboard conversion for multi-keyboard language (i.e. Japanese)

Language switching is done with Capslock, whilst input methods are done with Shift combiations (i.e. Left Shift + Right Shift). This is non-standard for macOS, but it's also a safe combination that will not block any known keybinds on Windows applications.
Many of the comments and other features were translated into English for easier understanding for myself, but it's fairly simple to operate.

Currently the application works with en-US, ja-JP, and ko-KR language inputs.

Written in C++, this application is built using CMake and runs in the Windows system tray. It provides a context menu (right-click on the tray icon) to enable/disable the functionality, set the app to launch on Windows startup, or exit the application.

### Features
- Converts Langauge via CapsLock key, and input method via Shift combination.
- System tray integration with a context menu for:
  - Enabling/disabling the functionality.
  - Setting the app to run on Windows startup.
  - Exiting the application.
- Lightweight and efficient, built with C++ and CMake.

### Installation
**The following is executed on a Windows desktop.**
1. Download `CapsLockLanguageSwitcher.exe` from the [Releases page](https://github.com/Socrad/CapsLockLanguageSwitch/releases).
2. Run the executable to start the application.
3. The app will appear in the system tray.

### Or build from source code: 
**The following is executed on a Windows desktop.**

1. **Prerequisites**:
   - Windows 10 or later.
   - CMake 3.10 or higher.
   - A C++ compiler (e.g., MSVC, MinGW).
2. **Clone the repository**:
   ```bash
   git clone https://github.com/Socrad/CapsLockLanguageSwitch.git
   cd CapsLockLanguageSwitch
   ```
3. **Build the application**:

   **Using MSVC (Visual Studio)**:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
   **Using MinGW**:
   ```bash
   mkdir build && cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake
   cmake --build .
   ```

4. **Run the application**:
   - Navigate to the build directory and run the generated executable (e.g., `CapsLockLanguageSwitcher.exe`).
   - The app will appear in the system tray.

### Usage
1. Launch `CapsLockLanguageSwitcher.exe`.
2. The app runs in the system tray (bottom-right corner of the Windows taskbar).
3. Right-click the tray icon to access the menu:
   - **Enable/Disable**: Toggle the CapsLock Switch functionality.
   - **Run at Startup**: Add or remove the app from Windows startup.
   - **Exit**: Close the application.
4. Press CapsLock to toggle between language inputs.

### License
This project is licensed under the [MIT License](LICENSE).
Full credits to the original author, my contributions are merely to make it more usable with other languages.

---