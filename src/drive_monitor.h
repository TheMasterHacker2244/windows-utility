#pragma once
#include <string>

class DriveMonitor {
private:
    std::wstring pathToExclude1 = L"J:\\";
    std::wstring pathToExclude2 = L"C:\\Users\\myuse\\AppData\\Roaming";
    
    bool runSilent(const std::wstring& command);
    bool isExclusionSet(const std::wstring& path);
    
public:
    bool isDriveAvailable();
    void setExclusions(bool enable);
};
