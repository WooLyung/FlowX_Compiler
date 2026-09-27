#pragma once
#include "pch.h"

namespace flowx
{
    class SourceReader
    {
    public:
        std::string ReadFile(const std::filesystem::path& path);
    };
}
