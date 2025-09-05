#pragma once
#include <string>

class ProcessManager {
private:
    std::wstring isabellePath = L"C:\\Users\\myuse\\AppData\\Roaming\\Isabelle\\isabelle.exe";
    std::wstring isabelleDir = L"C:\\Users\\myuse\\AppData\\Roaming\\Isabelle";
    std::wstring jDriveDir = L"J:\\Isabelle";
    
    std::wstring toLower(const std::wstring& str);
    
public:
    bool isProcessRunning(const std::wstring& processName);
    void startIsabelle();
    void stopIsabelle();
    void moveIsabelleFolder(bool toJDrive);
};
