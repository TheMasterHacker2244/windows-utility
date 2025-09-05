#include "registry_cleaner.h"
#include <windows.h>
#include <algorithm>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

RegistryCleaner::RegistryCleaner() {
    // Load keywords from file or use defaults
    loadKeywords();
    
    // Whitelisted applications
    whitelist = {
        L"NVIDIA APP.EXE", L"obs64.exe", L"obs32.exe", L"obs.exe", L"obs", 
        L"obs-studio", L"obsproject"
    };
    
    // Registry keys to clean
    registryKeys = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FeatureUsage\\AppSwitched",
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store",
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers"
    };
}

void RegistryCleaner::loadKeywords() {
    // Try to load from file first
    std::ifstream file("assets/keywords.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                keywords.push_back(std::wstring(line.begin(), line.end()));
            }
        }
        file.close();
    } else {
        // Default keywords
        keywords = {
            L"matcha", L"evolve", L"software", L"loader", L"exploit", L"hack", 
            L"mooze", L"isabelle", L"matrix", L"severe", L"assembly", L"tsar", 
            // ... (all the other keywords)
        };
    }
}

std::wstring RegistryCleaner::toLower(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool RegistryCleaner::containsKeywords(const std::wstring& str) {
    std::wstring lowerStr = toLower(str);
    
    for (const auto& keyword : keywords) {
        std::wstring lowerKeyword = toLower(keyword);
        if (lowerStr.find(lowerKeyword) != std::wstring::npos) {
            // Check if this matches any whitelisted term
            bool isWhitelisted = false;
            for (const auto& white : whitelist) {
                std::wstring lowerWhite = toLower(white);
                if (lowerStr.find(lowerWhite) != std::wstring::npos) {
                    isWhitelisted = true;
                    break;
                }
            }
            if (!isWhitelisted) {
                return true;
            }
        }
    }
    return false;
}

void RegistryCleaner::cleanRegistry() {
    HKEY hKey;
    for (const auto& subKey : registryKeys) {
        if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            DWORD index = 0;
            wchar_t valueName[16383];
            DWORD valueNameSize;
            DWORD valueType;
            BYTE data[4096];
            DWORD dataSize;
            
            while (true) {
                valueNameSize = sizeof(valueName) / sizeof(wchar_t);
                dataSize = sizeof(data);
                
                LONG result = RegEnumValueW(hKey, index, valueName, &valueNameSize, 
                                           nullptr, &valueType, data, &dataSize);
                
                if (result == ERROR_NO_MORE_ITEMS) break;
                if (result != ERROR_SUCCESS) {
                    index++;
                    continue;
                }
                
                std::wstring nameStr(valueName);
                std::wstring dataStr;
                
                if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
                    dataStr = reinterpret_cast<wchar_t*>(data);
                }
                
                if (containsKeywords(nameStr) || containsKeywords(dataStr)) {
                    RegDeleteValueW(hKey, valueName);
                    index = 0;
                } else {
                    index++;
                }
            }
            
            RegCloseKey(hKey);
        }
    }
}

void RegistryCleaner::cleanPrefetch() {
    std::wstring prefetchPath = L"C:\\Windows\\Prefetch";
    
    try {
        if (fs::exists(prefetchPath) && fs::is_directory(prefetchPath)) {
            for (const auto& entry : fs::directory_iterator(prefetchPath)) {
                if (entry.is_regular_file()) {
                    std::wstring filename = entry.path().filename().wstring();
                    
                    if (containsKeywords(filename)) {
                        fs::remove(entry.path());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Error handling
    }
}

void RegistryCleaner::cleanAll() {
    cleanRegistry();
    cleanPrefetch();
}
