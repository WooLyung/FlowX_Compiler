#include "stdafx.h"
#include "SourceReader.h"

namespace flowx
{
    std::string SourceReader::ReadFile(const std::filesystem::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        if (!input)
            throw std::runtime_error("Failed to open file: " + path.string());

        std::string source;
        char buffer[4096];
        while (input.read(buffer, sizeof(buffer)) || input.gcount() > 0)
            source.append(buffer, static_cast<std::size_t>(input.gcount()));
        if (input.bad())
            throw std::runtime_error("Failed to read file: " + path.string());

        return source;
    }
}
