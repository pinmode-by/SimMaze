#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include "simMaze.h"
#include "utils.h"

using namespace cv;
using namespace std;
using namespace std::chrono;

std::atomic_bool isCmd = false;

int main(int argc, char** argv) {
  std::cout.sync_with_stdio(false);
  const int wMaze = 6;
  const int hMase = 6;
  cv::Size sizeMaze(wMaze, hMase); 
  int gm = GoalDiagonal; // GoalCenter; 
 
  // check to exist fileMaze  
  
  // check command line and parse
  if (argc > 1) {
    optCmdLineMaze optMaze;
    if (auto isParse = parseCommamdLine(argc, argv, optMaze); isParse ) {
      sizeMaze = optMaze.cmdSize;
      gm = optMaze.goal;
    } else {
      return -1;
    }
  }
 
  sm::SimMaze Sim(sizeMaze, gm);
  Sim.run();
  
  return 0;
}

//circle(line_bin, cp, 5, Scalar(0, 255, 0), FILLED);
