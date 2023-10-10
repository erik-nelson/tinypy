#include <iostream>
#include <string>

#include "interpreter.h"
#include "version.h"

int main(int argc, char** argv) {
  Interpreter interpreter;

  // Startup info.
  std::cout << "Tinypy version " << VersionInfo::ToString() << std::endl;
  std::cout << "https://github.com/erik-nelson/tinypy" << std::endl;
  std::cout << "Type \"exit()\" to exit." << std::endl;

  while (true) {
    // Prompt the user for input.
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);

    // Catch request to exit.
    if (input == "exit()") {
      break;
    }

    // Interpret user input.
    try {
      interpreter.Interpret(input);
    } catch (const std::exception& ex) {
      std::cerr << "Caught exception:\n\t" << ex.what();
      // TODO(erik): Handle exceptions.
    }
  }

  return EXIT_SUCCESS;
}