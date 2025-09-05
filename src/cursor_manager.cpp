#include "cursor_manager.h"
#include <windows.h>

CursorManager::CursorManager() {
    hArrow = LoadCursor(NULL, IDC_ARROW);
}

void CursorManager::replaceCursors() {
    if (hArrow == nullptr) return;
    
    SetSystemCursor(LoadCursor(NULL, IDC_ARROW), OCR_APPSTARTING);
    SetSystemCursor(LoadCursor(NULL, IDC_ARROW), OCR_WAIT);
    cursorReplaced = true;
}

void CursorManager::restoreCursors() {
    SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
    cursorReplaced = false;
}
