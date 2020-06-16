#include "simMaze.h"
#include <iostream>
#include <string>

namespace sm {

SimMaze::SimMaze (cv::Size sz, int goal) {
  
  smW = 80 + sz.width * 70 + 80 + sz.width * 70 + 80;
  smH = 60 + sz.height * 70 + 60;
  matSim = Mat(cv::Size(smW, smH), CV_8UC3, cv::Scalar(230, 230, 230));
  sMaze = Maze(matSim, sz, Point(80, 50), 1);
  oMaze = Maze(matSim, sz, Point(80 + sz.width * 70 + 80, 50), 1);
  winSim += std::to_string(sz.height) + " x " + std::to_string(sz.width);
  namedWindow(winSim, WINDOW_AUTOSIZE);
  BuildMaze();
}

void SimMaze::BuildMaze() {
  // for test 
	oMaze.setWall(0, 3, WEST);
	oMaze.setWall(1, 3, WEST);
  oMaze.setWall(1, 2, SOUTH);
  oMaze.setWall(2, 3, WEST);
  oMaze.setWall(3, 3, WEST);
  //oMaze.setWall(4, 3, WEST);
  oMaze.setWall(2, 0, EAST);
  oMaze.setWall(3, 0, EAST);
  oMaze.setWall(4, 0, EAST);

  oMaze.setWall(4, 4, WEST);
  oMaze.setWall(1, 1, NORTH);
  oMaze.setWall(5, 1, EAST);
  oMaze.setWall(4, 1, EAST);
  oMaze.setWall(3, 1, EAST);
  oMaze.setWall(5, 2, EAST);
  oMaze.setWall(5, 4, SOUTH);
  oMaze.setWall(4, 4, EAST);
  oMaze.setWall(3, 4, EAST);
}

void SimMaze::run() {
	const char kbCmdExit = 27;
  const char kbCmdNew = 'n';
  const char kbCmdGen = 'g';
  const char kbCmdSave = 'w';
  const char kbCmdTest = 't';

  for (;;) {
    updateMazez();
    
    char key = (char) waitKey(1);
    if (key > 0 ) {
      std::cout << std::hex << (int)key << std::endl;
      switch (key) {
        case kbCmdExit:
          std::cout << "Exit simMaze" << std::endl;
          return;
          break;
        case kbCmdNew:
          std::cout << "New empty Maze for build" << std::endl;
          break;
        case kbCmdGen:
          std::cout << "Generation new random Maze" << std::endl;
          break;
        case kbCmdSave:
          std::cout << "Save current Maze to output file" << std::endl;
          break;
        case kbCmdTest:
          if (oMaze.getSize() == Size(6, 6)) {
            std::cout << "Load test Maze 6 x 6" << std::endl;
          }
          break;
      }    
    }
  }
}


void SimMaze::updateMazez() {
  oMaze.update();
  //sMaze.update();
  imshow(winSim, matSim);
}

}
