#include "simMaze.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

static void onMouse(int event, int x, int y, int flags, void* param) {
  if (sm::SimMaze::getStatus() != sm::Status::Edit) {
    return;
  }
  sm::mEvent.x = x;
  sm::mEvent.y = y;
  switch(event) {
    case cv::EVENT_LBUTTONDOWN: 
      sm::isMouseEvent = true;   
      sm::mEvent.typeEvent = sm::LEFTDOWN; 
      break;
    case cv::EVENT_RBUTTONDOWN:
      sm::isMouseEvent = true;
      sm::mEvent.typeEvent = sm::RIGHTDOWN;
      break;
  } //switch
}


namespace sm {

std::string fileMaze = "maze.sim";

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
  const char kbCmdDisplay = 'd';

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
          std::cout << "Write current Maze to output file" << std::endl;
          saveMazeToFile();
          break;
        case kbCmdLoad:
          std::cout << "Load Maze from input file" << std::endl;
          loadMazeFromFile();
          break;
        case kbCmdDisplay:
          std::cout << "Display walls of current Maze on screen.\n" <<
                      std::endl;
          std::cout << "In memory: \n";
          oMaze->printMaze(',');      
          std::cout << "Visually:\n";      
          oMaze->printWalls(' ');
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
                      "(right/left rule ) algorithm" << std::endl;
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
  std::ofstream ofs(fileMaze, std::ifstream::binary);
  if (!ofs) {
    std::cerr << "Error in <" << __PRETTY_FUNCTION__ << "> : " 
              << std::strerror(errno) << std::endl;
    return false;
  }
  std::cout << "Writing maze to file... <" <<
    fileMaze << "> : " << std::endl;
  cv::Size sz = oMaze->getSize();
  for (int i = 0; i < sz.height * sz.width; ++i) {
    char ch = static_cast<char>(oMaze->getCell(i));
    ofs.write(&ch, 1);
  }
  return true;
}

bool SimMaze::loadMazeFromFile() {
  oMaze->clean();
  sMaze->clean();
  //std::string fileMaze ("simMaze.maz"); 
  std::ifstream ifs(fileMaze, std::ifstream::binary);
  if (!ifs) {
    std::cerr << "Error in <" << __PRETTY_FUNCTION__ << "> : " 
              << std::strerror(errno) << std::endl;
    return false;
  }
  
  int read_bytes = 0;
  std::cout << "Reading maze file... <" <<
    fileMaze << "> : " << std::endl;
  while (true) {
    char ch;
    ifs.read(&ch, 1);
    if (ifs.eof()) {
      break;
    }
    oMaze->setCell(read_bytes, static_cast<uchar>(ch));
    ++read_bytes;
  }
  std::cout << "\n\n" << std::endl;
  oMaze->printMaze(' ');
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
