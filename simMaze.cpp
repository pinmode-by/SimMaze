#include "simMaze.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

static void onMouse(int event, int x, int y, int flags, void* param) {
  switch(event) {
    case cv::EVENT_LBUTTONDOWN: {
      if (sm::SimMaze::getStatus() == sm::Status::Edit) {
        sm::isMouseEvent = true;
        sm::mEvent.x = x;
        sm::mEvent.y = y;
      } else if (sm::SimMaze::getStatus() == sm::Status::Idle) {
        //std::cout << "Idle :" << "at (" << x << "," << y << ") value is "
        // << (pm->at<cv::Vec3b>(cv::Point(x,y))) << std::endl;       
      }
    break;
    }
  }
}


namespace sm {

SimMaze::SimMaze (cv::Size sz, int goal) {
  const int sMax = std::max(sz.width, sz.height);
  int pWidth = (sMax < 9) ? 60 : ((sMax < 12) ? 45 : 30);
  int wWidth = 6; 
  const int cWidth = pWidth + wWidth;
  int smW = 60 + sz.width * cWidth + 60 + sz.width * cWidth + 180;
  int smH = 60 + sz.height * cWidth + 60;
  matSimMaze = Mat(cv::Size(smW, smH), CV_8UC3, cv::Scalar(230, 230, 230));
  sMaze = std::make_shared<Maze>(matSimMaze, sz, Point(60, 50),
          goal, pWidth, wWidth);
  oMaze = std::make_shared<Maze>(matSimMaze, sz,
          Point(60 + sz.width * cWidth + 60, 50), goal, pWidth, wWidth);
  solver = std::make_shared<SolverMaze>(oMaze, sMaze);      
  winSim += std::to_string(sz.height) + " x " + std::to_string(sz.width);
  namedWindow(winSim, WINDOW_AUTOSIZE);

  std::cout << SimMaze::getNameWinSim() << std::endl;
  cv::setMouseCallback(SimMaze::getNameWinSim(), onMouse,
     (void*)&matSimMaze);
}

void SimMaze::run() {
  const char kbCmdExit = 27;
  const char kbCmdNew = 'n';
  const char kbCmdGen = 'g';
  const char kbCmdWrite = 's';
  const char kbCmdLoad = 'l';
  const char kbCmdEdit = 'e';
  //const char kbCmdSolve = 's';
  const char kbCmdRand = 'r';
  const char kbCmdFlood = 'f';
  const char kbCmdWallFollower = 'w';
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
          std::cout << "Write current Maze to output file <simMaze.txt>" <<
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
        case kbCmdRand:
          std::cout << "Solve the current maze by random search algorithm" <<
                        std::endl;
          randSearchSim();
          break;
        case kbCmdFlood:
          std::cout << "Solve the current maze by flood fill algorithm" <<
                        std::endl;
          floodFillSim();
          break;
        case kbCmdWallFollower:
          std::cout << "Solve the current maze by wall follower" \
                      "(right rule ) algorithm" << std::endl;
          wallFollowerSim();
          break;  
        case kbCmdAdvFlood:
          std::cout << "Solve the current maze by advanced flood fill algorithm" <<
                        std::endl;
          break; 
        case kbCmdTest:
            std::cout << "Load test Maze " << std::endl;
          break;
      }   //switch 
    }
    updateMazez();
  }
}

void SimMaze::updateMazez() {
  solver->update();
  oMaze->update();
  sMaze->update();
  imshow(winSim, matSimMaze);
}

void SimMaze::newMaze() {
  oMaze->buildNew();
  sMaze->clean();
}

void SimMaze::genMaze() {
  status = Status::Gen;
  sMaze->clean();
  oMaze->generate();
}

void SimMaze::editMaze() {
  status = Status::Edit;
  sMaze->clean();
  oMaze->edit();
}

bool SimMaze::saveMazeToFile() {

  return true;
}

bool SimMaze::loadMazeFromFile() {
  oMaze->clean();
  sMaze->clean();
  std::string fileMaze ("simMaze.maz"); 
  std::ifstream ifs(fileMaze, std::ifstream::binary);
  if (!ifs) {
    std::cerr << "Error in <" << __PRETTY_FUNCTION__ << "> : " 
              << std::strerror(errno) << std::endl;
    return false;
  }
  
  int read_bytes = 0;
  std::cout << "Reading maze file... <" << fileMaze << "> : " << std::endl;
  while (true) {
    char ch;
    ifs.read(&ch, 1);
    if (ifs.eof()) {
      break;
    }
    oMaze->setCell(read_bytes, static_cast<uchar>(ch));
    ++read_bytes;
    /*
    std::cout << std::hex << static_cast<int>(ch) << ' ' ;
    if (read_bytes % 16 == 0) {
      std::cout << std::endl;
    } 
    */
  }
  std::cout << "\n\n" << std::endl;
  oMaze->printMaze();
  return true;
}

void SimMaze::randSearchSim() {
  status = Status::RandSearch;
  sMaze->randSearch(oMaze.get());
}

void SimMaze::wallFollowerSim() {
  status = Status::WallFollowerSearch;
  solver->wallFollower();
}


void SimMaze::floodFillSim() {
  status = Status::FloodSearch;
  solver->floodFill();

}


} // namespace sm
