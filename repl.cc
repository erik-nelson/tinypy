#include <iostream>
#include <string>

#include "interpreter.h"
#include "version.h"

namespace {
// Whether this begins a multi-line statement. Currently only checks whether
// the line ends with a ':' character.
// TODO(erik): Add other multi-line conditions, such as string linebreaks.
bool BeginsMultiline(std::string line) {
  size_t endpos = line.find_last_not_of(" \t");
  if (std::string::npos != endpos) {
    line = line.substr(0, endpos + 1);
  }
  return !line.empty() && line.back() == ':';
}

// Interprets a set of input lines as a statement.
void ProcessStatement(Interpreter* interpreter,
                      std::vector<std::string>* lines) {
  std::string statement;
  for (auto& line : *lines) statement += std::move(line) + "\n";
  lines->clear();

  // Interpret user input.
  try {
    interpreter->Interpret(statement);
  } catch (const std::exception& ex) {
    std::cerr << "Caught exception:\n\t" << ex.what();
    throw ex;
    // TODO(erik): Handle exceptions.
  }
}
}  // namespace

int main(int argc, char** argv) {
  Interpreter interpreter;

  // Startup info.
  std::cout << "Tinypy version " << VersionInfo::ToString() << std::endl;
  std::cout << "https://github.com/erik-nelson/tinypy" << std::endl;
  std::cout << "Type \"exit()\" to exit." << std::endl;

  bool multiline = false;
  std::vector<std::string> lines;

  while (true) {
    // Prompt the user for input.
    std::string input;
    std::cout << (multiline ? "... " : ">>> ");
    std::getline(std::cin, input);

    // Catch request to exit.
    if (input == "exit()") {
      break;
    }

    lines.push_back(std::move(input));

    // Handle single or multi-line statements.
    if (!multiline) {
      if (BeginsMultiline(lines.back())) {
        multiline = true;
      } else {
        ProcessStatement(&interpreter, &lines);
      }
    } else if (multiline && lines.back().empty()) {
      ProcessStatement(&interpreter, &lines);
      multiline = false;
    }
  }

  return EXIT_SUCCESS;
}