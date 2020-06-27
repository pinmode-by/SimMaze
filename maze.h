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
  
using RowType = int;
using ColType = int;
using CellType = std::pair<ColType, RowType>; 
using uchar = unsigned char;

enum WALL_CELL {
  NORTH   = 0x1,
  EAST    = 0x2,
  SOUTH   = 0x4,
  WEST    = 0x8,
  VISITED = 0x10,
  ONROUTE = 0x20
};

extern bool isMouseEvent;

struct MouseEvent {
  int typeEvent = 0;
  int x;
  int y; 
};

extern MouseEvent mEvent;

const int MAZE_WIDTH = 16;

class SolverMaze;

class Maze {
public:
  Maze() = default;
  Maze(cv::Mat& base, cv::Size s, cv::Point p, int goal,
       int pW, int wW);
  cv::Size getSize() const { return cv::Size(mazeW, mazeH);}
  
  int cellN(int x, int y) const {
    return MAZE_WIDTH * x + y;
  }
  void setWall(int col, int row, int wall);
  void clearWall(int col, int row, int wall);
  void setVisited(int col, int row) {
    mapMaze[cellN(col, row)] |= VISITED;
  }
  void setCell(int cell, uchar value) {
    mapMaze[cell] = value;
  };
  uchar getCell(int cell) {
    return mapMaze[cell];
  };
  
  void buildNew();
  void clean();
  void edit();
  bool save();
  void generate();
  void update();
  bool isWall(cv::Point p);
  void randSearch(Maze *origin);
  void printMaze();
  CellType getStartPosition() {return {0, 0}; }
  std::vector<CellType> getGoalPosition() {
    return goalPositions;
  }
  void setColorVisited(cv::Scalar newColor) {
     colorVISITED = cv::Scalar(newColor); }
  void setColorCurrent(cv::Scalar newColor) {
     colorCURRENT = cv::Scalar(newColor); }   
  ~Maze() {}
  friend SolverMaze;
  
private:
  using pCallback = void(Maze::*)();
  pCallback onMazeUpdate = nullptr;

  void onCreate();
  void onEdit();
  void drawStandMaze();
  void drawCell(int col, int row);
  void algGeneration();
  void algRandSearch();
  bool isGoal(CellType cell);
  void setStartGoalCells();
  void setStandardGoal();
 
  std::vector<CellType> goalPositions {};
 
  cv::Point offset; // offset from SimMaze 
  
  std::vector<uchar> mapMaze;
  
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
