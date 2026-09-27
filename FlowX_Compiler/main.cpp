#include "stdafx.h"
#include "SourceReader.h"
#include "Lexer.h"
#include "Token.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: FlowX_Compiler <file-path>\n";
        return 1;
    }

    try
    {
        flowx::SourceReader reader;
        flowx::Lexer lexer(reader.ReadFile(argv[1]));
        const auto tokens = lexer.Tokenize();
        flowx::PrintTokens(tokens, std::cout);
    }
    catch (const flowx::LexerError& error)
    {
        const auto location = error.Location();
        std::cerr << argv[1] << ':' << location.line << ':' << location.column 
            << ": lexer error: " << error.what() << '\n';
        return 1;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
