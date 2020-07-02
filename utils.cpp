
#include "utils.h"
#include "iostream"

void printUsage() {
  std::cout << "Bad command or options\n";
  std::cout << "Usage: simMaze [options]\n";
  std::cout << "Options: \n";
  std::cout << "  -s <w[:h]>\tSet size W x H of maze\n";
  std::cout << "  -g <d>,<c> \tSet target cell of maze\n";
  std::cout << "  -f <file>\tSet input file of maze\n";
}

bool parseCommamdLine(int argc, char** argv,
                      optCmdLineMaze& opt) {

  // parse command line 
  // if parse == true -> fill struct <optCmdLineMaze>
  // if parse == false -> cout USAGE 

  if (argc < 3 && argc % 2 == 0) {
    printUsage();
    return false;
  }
  int count {1};
  while (count < argc) {
    std::string option = argv[count]; 
    try {
      if (option == "-s") {
        ++count;
        // string -> integer
        int width = std::stoi(argv[count++]);
        opt.cmdSize = {width, width};
      } else if (option == "-g") {
        ++count;
      } else if (option == "-f") {
        ++count; 
      } else {
        throw std::runtime_error("error");
      }
    } catch (const std::exception& e) {
      printUsage();
      return false;
    }
  }
  return true;
}
