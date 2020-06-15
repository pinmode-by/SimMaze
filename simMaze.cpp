#include "simMaze.h"
#include <iostream>
#include <string>

namespace sm {

SimMaze::SimMaze (cv::Size sz, int goal) {
  
  matSim = Mat(sizeSim, CV_8UC3, cv::Scalar(230, 230, 230));
  sMaze = Maze(matSim, sz, Point(80,50), 1);
  oMaze = Maze(matSim, sz, Point(520,50), 1);
  winSim += std::to_string(sz.height) + " x " + std::to_string(sz.width);
  namedWindow(winSim, WINDOW_AUTOSIZE);

}

void SimMaze::run() {
	const char kbESC = 27;

  for (;;) {
    updateMazez();
    
    char key = (char) waitKey(1);
    if (key > 0 ) {
			std::cout << std::hex << (int)key << std::endl;
			switch (key) {
				case kbESC:
					std::cout << "Exit simMaze" << std::endl;
					return;
					break;
			}    
    }
  }
}


void SimMaze::updateMazez() {
  oMaze.update();
  sMaze.update();
  imshow(winSim, matSim);
}

}
