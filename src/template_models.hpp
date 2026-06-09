#pragma once

#include <string>
#include <vector>

struct CodeTemplate {
    std::wstring name;
    std::string code;
};

struct ToolVariant {
    std::wstring name;
    std::string code;
    std::wstring usage;
};

struct FillTool {
    std::wstring name;
    std::vector<ToolVariant> variants;
};
