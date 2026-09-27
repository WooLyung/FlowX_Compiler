#pragma once
#include "stdafx.h"

namespace flowx
{
    class SourceReader
    {
    public:
        std::string ReadFile(const std::filesystem::path& path);
    };
}
