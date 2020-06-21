#include "simMaze.h"
#include <iostream>
#include <string>

namespace sm {

SimMaze::SimMaze (cv::Size sz, int goal) {
  const int sMax = std::max(sz.width, sz.height);
  int pWidth = (sMax < 9) ? 55 : ((sMax < 12) ? 45 : 30);
  int wWidth = 5; 
  const int cWidth = pWidth + wWidth;
  smW = 60 + sz.width * cWidth + 60 + sz.width * cWidth + 180;
  smH = 60 + sz.height * cWidth + 60;
  matSim = Mat(cv::Size(smW, smH), CV_8UC3, cv::Scalar(230, 230, 230));
  sMaze = Maze(matSim, sz, Point(60, 50), goal, pWidth, wWidth);
  oMaze = Maze(matSim, sz, Point(60 + sz.width * cWidth + 60, 50), goal,
              pWidth, wWidth);
  winSim += std::to_string(sz.height) + " x " + std::to_string(sz.width);
  namedWindow(winSim, WINDOW_AUTOSIZE);
  //BuildMaze();
}

void SimMaze::BuildMaze() {
  // for test 
	
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
  oMaze.setWall(1, 4, EAST);
  oMaze.setWall(2, 4, EAST);
  oMaze.setWall(2, 4, NORTH);
  oMaze.setWall(0, 3, EAST);
  oMaze.setWall(1, 3, EAST);
}

void SimMaze::run() {
	const char kbCmdExit = 27;
  const char kbCmdNew = 'n';
  const char kbCmdGen = 'g';
  const char kbCmdSave = 'w';
  const char kbCmdLoad = 'l';
  const char kbCmdEdit = 'e';
  const char kbCmdTest = 't';

  for (;;) {
    // event loop
    char key = (char) waitKey(1);
    if (key > 0 ) {
      //std::cout << std::hex << (int)key << std::endl;
      switch (key) {
        case kbCmdExit:
          std::cout << "Exit simMaze" << std::endl;
          return;
          break;
        case kbCmdNew:
          std::cout << "New empty Maze for build" << std::endl;
          newMaze();
          break;
        case kbCmdGen:
          std::cout << "Generation new random Maze" << std::endl;
          genMaze();
          break;
        case kbCmdSave:
          std::cout << "Save current Maze to output file <maze.txt>" <<
                      std::endl;
          saveMazeToFile();
          break;
        case kbCmdLoad:
          std::cout << "Load Maze from input file <maze.txt>" <<
                      std::endl;
          loadMazeFromFile();
          break;
        case kbCmdEdit:
          std::cout << "Edit current Maze" << std::endl;
          editMaze();
          break;
        case kbCmdTest:
          if (oMaze.getSize() == Size(6, 6)) {
            std::cout << "Load test Maze 6 x 6" << std::endl;
            BuildMaze();
          }
          break;
      }   //switch 
    }
    updateMazez();
  }
}

void SimMaze::updateMazez() {
  oMaze.update();
  sMaze.update();
  imshow(winSim, matSim);
}

void SimMaze::newMaze() {
  oMaze.buildNew();
  sMaze.clean();
}

void SimMaze::genMaze() {
  sMaze.clean();
  oMaze.generate();
}

void SimMaze::editMaze() {

}

bool SimMaze::saveMazeToFile() {
  return true;
}

bool SimMaze::loadMazeFromFile() {
  return true;
}

} // namespace sm
