#include "process_manager.h"
#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>

namespace fs = std::filesystem;

std::wstring ProcessManager::toLower(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool ProcessManager::isProcessRunning(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    
    if (!Process32FirstW(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return false;
    }
    
    bool found = false;
    std::wstring targetName = toLower(processName);
    
    do {
        if (toLower(pe.szExeFile) == targetName) {
            found = true;
            break;
        }
    } while (Process32NextW(hSnapshot, &pe));
    
    CloseHandle(hSnapshot);
    return found;
}

void ProcessManager::startIsabelle() {
    if (!isProcessRunning(L"isabelle.exe")) {
        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        PROCESS_INFORMATION pi;
        
        std::wstring cmd = isabellePath;
        if (CreateProcessW(nullptr, &cmd[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

void ProcessManager::stopIsabelle() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;
    
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    
    if (!Process32FirstW(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return;
    }
    
    do {
        if (toLower(pe.szExeFile) == L"isabelle.exe") {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (hProcess) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
            break;
        }
    } while (Process32NextW(hSnapshot, &pe));
    
    CloseHandle(hSnapshot);
}

void ProcessManager::moveIsabelleFolder(bool toJDrive) {
    fs::path sourceDir = toJDrive ? isabelleDir : jDriveDir;
    fs::path targetDir = toJDrive ? jDriveDir : isabelleDir;
    
    // Create target directory if it doesn't exist
    if (!fs::exists(targetDir)) {
        fs::create_directories(targetDir);
    }
    
    // Stop Isabelle if running
    if (isProcessRunning(L"isabelle.exe")) {
        stopIsabelle();
        Sleep(1000);
    }
    
    try {
        if (fs::exists(sourceDir) && fs::is_directory(sourceDir)) {
            for (const auto& entry : fs::directory_iterator(sourceDir)) {
                fs::path targetPath = targetDir / entry.path().filename();
                
                if (fs::exists(targetPath)) {
                    if (fs::is_directory(targetPath)) {
                        fs::remove_all(targetPath);
                    } else {
                        fs::remove(targetPath);
                    }
                }
                
                fs::rename(entry.path(), targetPath);
            }
            
            if (fs::is_empty(sourceDir)) {
                fs::remove(sourceDir);
            }
        }
    } catch (const std::exception& e) {
        // Error handling
    }
}
