/*
 * simMaze.h
 * 
 * Copyright 2020 pinMode <info@pinmode.by>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "maze.h"

namespace sm {

using namespace cv;

class SimMaze {
public:
  SimMaze(cv::Size s, int goal);
  void run();
  ~SimMaze() {}
private:
  void updateMazez();
  void BuildMaze();
  void newMaze();
  cv::Mat matSim;
  Maze oMaze;
  Maze sMaze;
  int smW = 960;
  int smH = 480;
  std::string winSim = "pinMode Simulator Maze "; 

};

}
