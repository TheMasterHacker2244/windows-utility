#pragma once

class CursorManager {
private:
    void* hArrow = nullptr;
    bool cursorReplaced = false;
    
    // Cursor constants
    static const int OCR_NORMAL = 32512;
    static const int OCR_APPSTARTING = 32650;
    static const int OCR_WAIT = 32514;
    
public:
    CursorManager();
    void replaceCursors();
    void restoreCursors();
    bool isCursorReplaced() const { return cursorReplaced; }
};
