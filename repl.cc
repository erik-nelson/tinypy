#include <iostream>
#include <string>
#include <cassert>
#include <unistd.h>
#include <termios.h>

#include "interpreter.h"
#include "version.h"

namespace {
// Hard coded ASCII keys used in the REPL.
static constexpr char kKeyBackspace = 8;
static constexpr char kKeyReturn = 10;
static constexpr char kKeyEscape = 27;
static constexpr char kKeyDelete = 127;
static constexpr char kClearSequence[] = "\033[2K\r";

class TerminalReader {
  public:
    TerminalReader() {
      // Turn off buffering. Read one keystroke at a time.
      assert(tcgetattr(STDIN_FILENO, &settings_) >= 0);
      settings_.c_lflag &= ~(ICANON | ECHO);
      assert(tcsetattr(STDIN_FILENO, TCSANOW, &settings_) >= 0);
    }

    // Read a line from the terminal. Handles up/down arrows for history.
    // Input specifies whether this is part of a multi-line statement.
    std::string GetLine(bool multiline) {
      std::string input;
      
      while(true) {
        std::cout << kClearSequence; // clear previous output.
        std::cout << (multiline ? "... " : ">>> ");
        std::cout << (idx_ >= 0 ? history_[idx_] : input);
        fflush(stdout);
        const char c = GetChar();

        // When the user presses enter, accept the line.
        if (c == kKeyReturn) {
          std::cout << std::endl;
          if (idx_ >= 0) input = history_[idx_];
          idx_ = -1;
          break;
       } 
       
       // Handle escape keys, like up and down arrow keypresses.
       else if (c == kKeyEscape) {
          if (GetChar() != '[') continue;

          char escape_c = GetChar();
          if (escape_c == 'A') {
            // We read '^[[A'.
            if (!history_.empty()) {
              idx_ = std::min(idx_ + 1, static_cast<int>(history_.size() - 1));
            }
          } else if (escape_c == 'B') {
            // We read '^[[B'.
            if (!history_.empty()) {
              idx_ = std::max(idx_ - 1, -1);
            }
          } 
        } 
        
        // Handle backspace and delete - clear one character from output.
        else if (c == kKeyBackspace || c == kKeyDelete) {
          if (!input.empty()) {
            input.pop_back();
            std::cout << kKeyBackspace << " " << kKeyBackspace;
          }
        } 
        
        // Otherwise append the character to the input line.
        else {
          input.push_back(c);
          std::cout << c;
        }      
      }

      AppendHistory(input);
      return input;
    }

  private:
    // Get a single character.
    char GetChar() const {
      char buf = 0;
      assert(read(STDIN_FILENO, &buf, 1) >= 0);
      return buf;
    }

    void AppendHistory(std::string str) {
      for (auto c : str) {
        if (!std::isspace(c)) {
          history_.emplace_front(std::move(str));
          return;
        }
      }
    }

    struct termios settings_ = {0};
    std::deque<std::string> history_;
    int idx_ = -1;
};


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
  TerminalReader reader;
  Interpreter interpreter;

  // Startup info.
  std::cout << "Tinypy version " << VersionInfo::ToString() << std::endl;
  std::cout << "https://github.com/erik-nelson/tinypy" << std::endl;
  std::cout << "Type \"exit()\" to exit." << std::endl;

  bool multiline = false;
  std::vector<std::string> lines;

  while (true) {
    // Prompt the user for input.
    std::string input = reader.GetLine(multiline);

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