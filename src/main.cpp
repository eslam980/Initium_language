#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "utils/parsing.hpp"
#include "utils/generation.hpp"
#include "utils/generation_win.hpp"

enum class Platforms {
    _linux,
    _windows,
    _macos,
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        // commands going to be like this ./main.inm linux/win
        std::cerr << "Initium <MAIN.inm> <platform[windows/linux]>" << std::endl;
        return EXIT_FAILURE;
    }

    // base platform going to be linux 
    // because linux is the only fully functional platform right now
    std::string platform = "linux";
    
    if (argc >= 2) {
        platform = argv[2];
        // basic checking error
        if(platform == "linux" || platform == "windows"){
            std::cout << "Built " << platform << "Platform Success" << std::endl;
        }else{
            std::cerr << "Incorrect platform, please the supported platforms" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        std::cerr << "Invalid Program Value!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string system_combo = "nasm -felf64 output.asm";

    if(platform == "linux")
    {
        LinuxGenerator generator(prog.value());
        std::fstream file("output.asm", std::ios::out);
        file << generator.gen_prog();

        system_combo = "nasm -felf64 output.asm";
    }
    else if (platform == "windows") {
        WindowsGenerator generator(prog.value());
        std::fstream file("output.asm", std::ios::out);

        // compilation to asm
        system_combo = "nasm -fwin64 output.asm";
        // TODO - Adding GOlinker or any other window linker to finish the compilation
    }

    system(system_combo.c_str());
    system("ld -o output output.o");

    return EXIT_SUCCESS;
}