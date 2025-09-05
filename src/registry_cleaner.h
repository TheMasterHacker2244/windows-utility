#pragma once
#include <vector>
#include <string>

class RegistryCleaner {
private:
    std::vector<std::wstring> keywords;
    std::vector<std::wstring> whitelist;
    std::vector<std::wstring> registryKeys;
    
    std::wstring toLower(const std::wstring& str);
    bool containsKeywords(const std::wstring& str);
    void loadKeywords();
    
public:
    RegistryCleaner();
    void cleanRegistry();
    void cleanPrefetch();
    void cleanAll();
};
