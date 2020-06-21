/*
 * maze.h
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
#include <iostream>
#include <stack>

namespace sm {
  
enum WALL_CELL {
  NORTH   = 0x1,
  EAST    = 0x2,
  SOUTH   = 0x4,
  WEST    = 0x8,
  VISITED = 0x10,
  ONROUT  = 0x20, 
  CURRENT = 0x40
};

class Maze {
public:
  Maze() = default;
  Maze(cv::Mat& base, cv::Size s, cv::Point p, int goal,
       int pW, int wW);
  cv::Size getSize() const { return cv::Size(mazeW, mazeH);}
  void setStartGoalCells();
  void setWall(int row, int col, int wall);
  void clearWall(int row, int col, int wall);
  void setColorVisited(cv::Scalar newColor) {
     colorVISITED = cv::Scalar(newColor); }
  void buildNew();
  void clean();
  void edit();
  bool save();
  bool load();
  void generate();
  void update();
  ~Maze() {}
  
private:
  using pCallback = void(Maze::*)();
  pCallback onUserUpdate = nullptr;

  void onCreate();
  void drawStandMaze();
  void drawCell(int row, int col);
  void algGeneration();
  void floodFill();
  void advFloodFill();
  
  std::vector<std::vector<int>> mapMaze;
  int pathWidth;
  int wallWidth;
  int goalMaze;
  int mazeW;
  int mazeH;
  cv::Mat matMaze;
  cv::Size sizeMatMaze;
  cv::Scalar colorVISITED {50, 50, 50};
   
  // Algorithm generations mazes variables
  int  nVisitedCells;
  int entryInGoal = 0;
  using RowType = int;
  using ColType = int;
  using CellType = std::pair<RowType, ColType>;
  std::stack <CellType> stackMaze; 
  enum { NORTHN, EASTN, SOUTHN, WESTN };
  
};

}
