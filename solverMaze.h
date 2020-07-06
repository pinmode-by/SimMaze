/*
 * solverMaze.h
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
#include <queue>
#include "maze.h"

namespace sm {

constexpr int MAZE_AREA = MAZE_WIDTH * MAZE_WIDTH;
constexpr int MAX_DISTANCE = MAZE_AREA - 1;

enum stepDir { 
  NONE = -1, LEFT = 0, FRONT = 1, RIGHT = 2, BACK = 3
};

enum compassDir {
  EASTC = 1,
  SOUTHC = -MAZE_WIDTH,
  WESTC = -1,
  NORTHC = MAZE_WIDTH
};


class SolverMaze {
public:
  SolverMaze(std::shared_ptr<Maze> oMaze, std::shared_ptr<Maze> sMaze);
  ~SolverMaze() {}
  void update();
  void algFloodFill();
  void floodFill();
  void advFloodFill();
  void algWallFollower();
  void wallFollower();
  void randSearch();
private:
  
  using solverCallback = void(SolverMaze::*)();
  solverCallback onSolverUpdate = nullptr;
  std::vector<uchar> mapDistances;
  std::shared_ptr<Maze> originM;
  std::shared_ptr<Maze> solveM;
  std::queue<CellType> queueD;
  std::stack<compassDir> stackDir;
  compassDir currentDir = NORTHC;
  bool isRightRule = true;
  compassDir nextCompassDirection(stepDir step);
  void initAlgorithm();
  bool isNeighbourVisited(int col, int row, uchar wall);
  uchar currentSideWall(stepDir side); 
  void resetDistancesAndQueue();
  void setDistances();
  void updateDistances();
  void queuePushDistances(int col, int row, int distance);
  void setSolverCallback(solverCallback handler) {
    onSolverUpdate = handler;
  }
  void printMapDistances();
};

}
