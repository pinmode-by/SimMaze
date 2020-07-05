#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <docopt.h>
#include "simMaze.h"
#include "utils.h"

static constexpr auto USAGE =
R"(A Micromouse Maze Solving Simulator.

  Usage:
    simMaze [options]

  Options:
    --help              Show this screen.
    --version           Show version.
    --width <WIDHT>     Width maze [default: 6].
    --height <HEIGHT>   Height maze [default: 6].
    -t <TARGET>         Target cel(diagonal, center) [default: d]
    -f <FILE>           File of maze [default: maze.sim]
)"; 

int main(int argc, const char** argv) {
  std::cout.sync_with_stdio(false);
  
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
      { std::next(argv), std::next(argv, argc) } ,
      true,            // show help if requested
      "simMaze 1.0");  // version string
 
  for(auto const& arg : args) {
      std::cout << arg.first << ": " << arg.second << std::endl;
  }
  
  // check Size range 
  const int wMaze = static_cast<int>(args["--width"].asLong());
  const int hMase = static_cast<int>(args["--height"].asLong());
  if (wMaze < 4 || wMaze > 16 || hMase < 4 || hMase > 16) {
    std::cerr << "Command line options are out of the range.\n";
    for(auto const& arg : args) {
      std::cout << arg.first << ": " << arg.second << std::endl;
    }
    abort();
  }
  
  // check target
  const std::string goal = args["-t"].asString();
  if (goal != "c" && goal != "d") {
    std::cerr << "Bad command line options.\n";
    for(auto const& arg : args) {
      std::cout << arg.first << ": " << arg.second << std::endl;
    }
    abort();
  }
  
  const cv::Size sizeMaze(wMaze, hMase); 
  const int target = (goal == "c") ? GoalCenter : GoalDiagonal;
  
  // check to exist fileMaze  
  
  sm::SimMaze Sim(sizeMaze, target);
  Sim.run();
  
  return 0;
}
