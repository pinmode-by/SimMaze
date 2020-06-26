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
#include <memory>
#include "maze.h"

namespace sm {

enum class Status {
  Idle,
  Gen,
  Edit,
  RandSearch,
  FloodSearch
};

using namespace cv;

class SimMaze {
public:
  SimMaze(cv::Size s, int goal);
  void run();
  static std::string getNameWinSim() { return winSim; }
  static Status getStatus() { return status; }
  ~SimMaze() {}
private:
  cv::Mat matSimMaze;
  static inline std::string winSim {"pinMode Simulator Maze "}; 
  static inline Status status = Status::Idle;
  std::shared_ptr<Maze> oMaze;
  std::shared_ptr<Maze> sMaze;
  // private methods
  void updateMazez();
  void BuildMaze();
  void editMaze();
  bool saveMazeToFile();
  bool loadMazeFromFile();
  void randSearchMaze();
  void floodMaze();
  void newMaze();
  void genMaze();
};

}
