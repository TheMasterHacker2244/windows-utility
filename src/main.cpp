#include <windows.h>
#include <iostream>
#include "drive_monitor.h"
#include "process_manager.h"
#include "registry_cleaner.h"
#include "cursor_manager.h"

int main() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
    
    // Initialize components
    DriveMonitor driveMonitor;
    ProcessManager processManager;
    RegistryCleaner registryCleaner;
    CursorManager cursorManager;
    
    // Initial setup
    bool lastDriveState = driveMonitor.isDriveAvailable();
    driveMonitor.setExclusions(lastDriveState);
    
    // Set initial cursor state
    if (lastDriveState) {
        cursorManager.replaceCursors();
    }
    
    // Main loop
    bool prevPgUpState = false;
    bool prevPgDnState = false;
    bool isabelleOnJDrive = false;
    
    while (true) {
        // Check drive state
        bool currentDriveState = driveMonitor.isDriveAvailable();
        if (currentDriveState != lastDriveState) {
            driveMonitor.setExclusions(currentDriveState);
            lastDriveState = currentDriveState;
            
            // Update cursor based on drive state
            if (currentDriveState) {
                cursorManager.replaceCursors();
            } else {
                cursorManager.restoreCursors();
            }
        }
        
        // Check key states
        bool currentPgUpState = (GetAsyncKeyState(VK_PRIOR) & 0x8000) != 0;
        bool currentPgDnState = (GetAsyncKeyState(VK_NEXT) & 0x8000) != 0;
        
        // PG UP pressed
        if (currentPgUpState && !prevPgUpState) {
            if (isabelleOnJDrive) {
                processManager.moveIsabelleFolder(false);
                isabelleOnJDrive = false;
            }
            
            if (processManager.isProcessRunning(L"RobloxCrashHandler.exe")) {
                processManager.startIsabelle();
            }
            registryCleaner.cleanAll();
        }
        
        // PG DN pressed
        if (currentPgDnState && !prevPgDnState) {
            processManager.stopIsabelle();
            if (lastDriveState) {
                processManager.moveIsabelleFolder(true);
                isabelleOnJDrive = true;
            }
            registryCleaner.cleanAll();
        }
        
        prevPgUpState = currentPgUpState;
        prevPgDnState = currentPgDnState;
        
        Sleep(100);
    }
    
    return 0;
}
