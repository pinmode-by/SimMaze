#include "simMaze.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

static void onMouse(int event, int x, int y, int flags, void* param) {
  cv::Mat *pm = (cv::Mat*)(param);
  switch(event) {
    case cv::EVENT_LBUTTONDOWN: {
      if (sm::SimMaze::getStatus() == sm::Status::Edit) {
        std::cout << "Edit :" << "at (" << x << "," << y << ") value is "
         << (pm->at<cv::Vec3b>(cv::Point(x,y))) << std::endl;
      } else if (sm::SimMaze::getStatus() == sm::Status::Idle) {
        std::cout << "Idle :" << "at (" << x << "," << y << ") value is "
         << (pm->at<cv::Vec3b>(cv::Point(x,y))) << std::endl;       
      }
    break;
    }
  }
}


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
  namedWindow(winSim, WINDOW_AUTOSIZE | WINDOW_GUI_EXPANDED);
  //BuildMaze();
  std::cout << SimMaze::getNameWinSim() << std::endl;
  cv::setMouseCallback(SimMaze::getNameWinSim(), onMouse, (void*)&matSim);
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
  const char kbCmdWrite = 'w';
  const char kbCmdLoad = 'l';
  const char kbCmdEdit = 'e';
  const char kbCmdSolve = 's';
  const char kbCmdRand = 'r';
  const char kbCmdFlood = 'f';
  const char kbCmdAdvFlood = 'a';
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
        case kbCmdWrite:
          std::cout << "Write current Maze to output file <maze.txt>" <<
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
        case kbCmdSolve:
          std::cout << "Solve the current maze by right rule algorithm" <<
                        std::endl;
          break;
        case kbCmdRand:
          std::cout << "Solve the current maze by random search algorithm" <<
                        std::endl;
          randSearchMaze();
          break;
        case kbCmdFlood:
          std::cout << "Solve the current maze by flood fill algorithm" <<
                        std::endl;
          break;
        case kbCmdAdvFlood:
          std::cout << "Solve the current maze by advanced flood fill algorithm" <<
                        std::endl;
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
  status = Status::Gen;
  sMaze.clean();
  oMaze.generate();
}

void SimMaze::editMaze() {
  status = Status::Edit;
  oMaze.edit();
}

bool SimMaze::saveMazeToFile() {
  return true;
}

bool SimMaze::loadMazeFromFile() {
  std::string fileMaze ("simMaze.maz"); 
  std::ifstream ifs(fileMaze, std::ifstream::binary);
  if (!ifs) {
    std::cerr << "Error in <" << __PRETTY_FUNCTION__ << "> : " 
              << std::strerror(errno) << std::endl;
    return false;
  }
  
  int read_bytes = 0;
  std::cout << "Reading maze file... <" << fileMaze << "> : " <<  std::endl;
  while (true) {
    char ch;
    ifs.read(&ch, 1);
    if (ifs.eof()) {
      break;
    }
    oMaze.setCell(read_bytes / 16, read_bytes % 16,
                  static_cast<int>(ch));
    ++read_bytes;
    std::cout << std::hex << static_cast<int>(ch) << ' ' ;
    if (read_bytes % 16 == 0) {
      std::cout << std::endl;
    } 
  }
  std::cout << "\n\n" << std::endl;
  oMaze.printMaze();
  
  return true;
}

void SimMaze::randSearchMaze() {
  status = Status::RandSearch;
  sMaze.randSearch(&oMaze);
}


} // namespace sm
