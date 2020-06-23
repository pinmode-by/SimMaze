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
  EAST   = 0x1,
  SOUTH  = 0x2,
  WEST   = 0x4,
  NORTH   = 0x8,
  VISITED = 0x10,
  ONROUTE = 0x20
};

class Maze {
public:
  Maze() = default;
  Maze(cv::Mat& base, cv::Size s, cv::Point p, int goal,
       int pW, int wW);
  cv::Size getSize() const { return cv::Size(mazeW, mazeH);}
  void setStartGoalCells();
  void setWall(int row, int col, int wall);
  void setCell(int row, int col, int value) {
    mapMaze[row][col] = value;
  };
  void clearWall(int row, int col, int wall);
  
  void buildNew();
  void clean();
  void edit();
  bool save();
  void generate();
  void update();
  bool isWall(cv::Point p);
  void randSearch(Maze *origin);
  void printMaze();
  ~Maze() {}
  
private:
  using pCallback = void(Maze::*)();
  pCallback onUserUpdate = nullptr;
  using RowType = int;
  using ColType = int;
  using CellType = std::pair<RowType, ColType>;
  
  void onCreate();
  void drawStandMaze();
  void drawCell(int row, int col);
  void algRandSearch();
  void algGeneration();
  void floodFill();
  void advFloodFill();
  bool isGoal(CellType cell);
  void setColorVisited(cv::Scalar newColor) {
     colorVISITED = cv::Scalar(newColor); }
  void setColorCurrent(cv::Scalar newColor) {
     colorCURRENT = cv::Scalar(newColor); }   
  CellType getStartPosition() {return {mazeH - 1, 0}; }
  std::vector<CellType> getGoalPosition() {
    return goalPositions;
  }
  std::vector<CellType> goalPositions {};
  cv::Point offset; // offset from SimMaze 
  std::vector<std::vector<int>> mapMaze;
  Maze *originMaze = nullptr;
  int pathWidth;
  int wallWidth;
  int goalMaze; // type of goal
  int mazeW;
  int mazeH;
  cv::Mat matMaze;
  cv::Size sizeMatMaze;
  cv::Scalar colorVISITED {50, 50, 50};
  cv::Scalar colorCURRENT {0, 255, 255};
  int prevDir;
  // Algorithm generations mazes variables
  int  nVisitedCells;
  int entryInGoal = 0;
  
  std::stack <CellType> stackMaze; 
  enum { NORTHN, EASTN, SOUTHN, WESTN };
  
};

}
