#include "pch.h"
#include "SourceReader.h"
#include "Lexer.h"
#include "Token.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: FlowX_Compiler <input.flowx> <output.ll>\n";
        return 1;
    }

    try
    {
        if (std::filesystem::exists(argv[2]) && std::filesystem::equivalent(argv[1], argv[2]))
            throw flowx::codegenerator::CodeGeneratorError("Input and LLVM output must be different files");

        flowx::SourceReader reader;
        flowx::lexer::Lexer lexer(reader.ReadFile(argv[1]));
        const auto tokens = lexer.Tokenize();

        flowx::parser::Parser parser(tokens);
        const auto parseTree = parser.Parse();

        flowx::semantic::SemanticAnalyzer analyzer;
        const auto model = analyzer.Analyze(*parseTree);

        flowx::codegenerator::CodeGenerator generator(model);
        generator.Generate(argv[2]);
    }
    catch (const flowx::lexer::LexerError& error)
    {
        const auto location = error.Location();
        std::cerr << argv[1] << ':' << location.line << ':' << location.column << ": lexer error: " << error.what() << '\n';
        return 1;
    }
    catch (const flowx::parser::ParserError& error)
    {
        const auto location = error.Location();
        std::cerr << argv[1] << ':' << location.line << ':' << location.column << ": parser error: " << error.what() << '\n';
        return 1;
    }
    catch (const flowx::semantic::SemanticError& error)
    {
        const auto location = error.Location();
        std::cerr << argv[1] << ':' << location.line << ':' << location.column << ": semantic error: " << error.what() << '\n';
        return 1;
    }
    catch (const flowx::codegenerator::CodeGeneratorError& error)
    {
        std::cerr << argv[2] << ": code generation error: " << error.what() << '\n';
        return 1;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
