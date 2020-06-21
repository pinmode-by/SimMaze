#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "maze.h"
#include "utils.h"

using namespace std::chrono_literals;
 
namespace sm {
  
Maze::Maze (cv::Mat& base, cv::Size s, cv::Point p, int goal,
            int pW, int wW) {
  
  mazeW = s.width;
  mazeH = s.height;
  pathWidth = pW;
  wallWidth = wW;
  goalMaze = goal;
  sizeMatMaze = cv::Size(mazeW * (pathWidth + wallWidth) + wallWidth,
                      mazeH * (pathWidth + wallWidth) + wallWidth);
  matMaze = cv::Mat(base(cv::Rect(p, sizeMatMaze)));
  // assign matrix mapMaze size: mazeH x mazeW 
  mapMaze.resize(mazeH);
  for (int i = 0; i < mazeH; ++i) {
    mapMaze[i].assign(mazeW, 0);
  }
  srand(clock());
  onCreate();

}

void Maze::onCreate() {
  // clear mat
  matMaze = cv::Scalar(50, 50, 50);
  // assign outside walls Maze
  for (int i = 0; i < mazeW; ++i) { 
    setWall(0, i, SOUTH);  
    setWall(mazeH - 1, i, NORTH);  
  }
  for (int i = 0; i < mazeH; ++i) { 
    setWall(i, 0, WEST); 
    setWall(i, mazeW - 1, EAST); 
  }
  drawStandMaze();
}

void Maze::setStartGoalCells() {
  // start cell - assign EAST wall
  setWall(0, 0, EAST); 
  if (goalMaze == GoalDiagonal) {
    // target - diagonal cell: assign WEST wall
    setWall(mazeH - 1, mazeW - 1, WEST);
  } else {
    // target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    std::vector<CellType> goalCells {
      {ROW, COL}, {ROW, COL + 1},
      {ROW + 1, COL}, {ROW + 1, COL + 1}
    };
    std::vector<int> goalWalls {
      WEST, SOUTH, SOUTH, EAST, WEST, NORTH, NORTH, EAST 
    };
    // random entry to target cells
    entryInGoal = rand() % goalWalls.size();
    goalWalls[entryInGoal] = 0;
    int i = 0;
    for (const auto& [row, col] : goalCells) {
      setWall(row, col, goalWalls[i]); 
      setWall(row, col, goalWalls[i + 1]); 
      i += 2;
    }
  }
}


void Maze::drawStandMaze() {
  static const int cellWidth = pathWidth + wallWidth;
  for (int i = 0; i < mazeH + 1; ++i) {
    for (int j = 0; j < mazeW + 1; ++j) {
      rectangle(matMaze,
        cv::Point(j * cellWidth, i * cellWidth),
        cv::Point(j * cellWidth + wallWidth - 1,
                  i * cellWidth + wallWidth - 1),
        cv::Scalar(255, 255, 255),
        cv::FILLED,
        cv::LINE_8);
    }
  }
}

void Maze::setWall(int row, int col, int wall){
  
  mapMaze[row][col] |= wall;
  // set wall at neighbor cells
  switch (wall) {
    case NORTH:
      if (row < mazeH - 1) {
         mapMaze[row+1][col] |= SOUTH;
      }
      break;
    case WEST:
      if (col > 0 ) {
         mapMaze[row][col-1] |= EAST;
      }
      break;
    case EAST:
      if (col < mazeW - 1 ) {
         mapMaze[row][col+1] |= WEST;
      }
      break;
    case SOUTH:
      if (row > 0) {
         mapMaze[row-1][col] |= NORTH;
      }
      break;
  }
}

void Maze::clearWall(int row, int col, int wall){
  
  mapMaze[row][col] &= ~wall;
  // clear wall at neighbor cells
  switch (wall) {
    case NORTH:
      if (row < mazeH - 1) {
         mapMaze[row+1][col] &= ~SOUTH;
      }
      break;
    case WEST:
      if (col > 0 ) {
         mapMaze[row][col-1] &= ~EAST;
      }
      break;
    case EAST:
      if (col < mazeW - 1 ) {
         mapMaze[row][col+1] &= ~WEST;
      }
      break;
    case SOUTH:
      if (row > 0) {
         mapMaze[row-1][col] &= ~NORTH;
      }
      break;
    }
}

void Maze::drawCell(int row, int col) {
  static const int cellWidth = pathWidth + wallWidth;
  
  const auto cX = col * cellWidth; 
  const auto cY = sizeMatMaze.height - row * cellWidth - 1;
  // Draw South wall
  if (mapMaze[row][col] & SOUTH) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY),
        cv::Point(cX + cellWidth - 1, cY - wallWidth + 1),
        cv::Scalar(0, 0, 255), // Red color
        cv::FILLED,
        cv::LINE_8 );
  } else {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY),
        cv::Point(cX + cellWidth - 1, cY - wallWidth + 1),
        cv::Scalar(50, 50, 50), // No wall
        cv::FILLED,
        cv::LINE_8 );
  }
  // Draw West wall
  if (mapMaze[row][col] & WEST) {
    rectangle(matMaze,
        cv::Point(cX, cY - wallWidth),
        cv::Point(cX + wallWidth - 1, cY - cellWidth + 1),
        cv::Scalar( 0, 0, 255 ), // Red color
        cv::FILLED,
        cv::LINE_8 );
  } else {
    rectangle(matMaze,
        cv::Point(cX, cY - wallWidth),
        cv::Point(cX + wallWidth - 1, cY - cellWidth + 1),
        cv::Scalar(50, 50, 50), // No wall
        cv::FILLED,
        cv::LINE_8 );
  }
  
  // NORTH outside walls
  if (row == mazeH - 1) {
      rectangle(matMaze,
        cv::Point(cX + wallWidth, 0),
        cv::Point(cX + cellWidth - 1, wallWidth - 1),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  } 
  // EAST outside walls
  if (col == mazeW - 1) {
      rectangle(matMaze,
        cv::Point(sizeMatMaze.width - wallWidth , cY - cellWidth),
        cv::Point(sizeMatMaze.width - 1, cY - wallWidth),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  }
  
  if (mapMaze[row][col] & VISITED) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY - wallWidth),
        cv::Point(cX + cellWidth - 1, cY - cellWidth + 1),
        colorVISITED,
        cv::FILLED,
        cv::LINE_8 );
  }
  
  // if stack is not empty - CURRENT
  if (!stackMaze.empty()) {
    auto [rowC, colC] = stackMaze.top();
    if (row == rowC && col == colC) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY - wallWidth),
        cv::Point(cX + cellWidth - 1, cY - cellWidth + 1),
        cv::Scalar(0, 255, 255),
        cv::FILLED,
        cv::LINE_8 );
    }
  }
}

void Maze::update() {
   
  if (onUserUpdate != nullptr) {
    // call background function 
    (this->*onUserUpdate)();
  }
  // Draw Maze
  for (int x = 0; x < mazeH; ++x) { 
    for (int y = 0; y < mazeW; ++y) { 
      drawCell(x, y);
    }
  }
}

void Maze::buildNew() {
  clean();
  setStartGoalCells();
}

void Maze::clean() {
  for (int i = 0; i < mazeH; ++i) {
    for (int j = 0; j < mazeW; ++j) {
      mapMaze[i][j] = 0;
    }
  }
  onCreate();
}

void Maze::algGeneration() {
  if (nVisitedCells < mazeH * mazeW) {
    // Slow down for animation
    std::this_thread::sleep_for(5ms);
    
    // create a set of unvisited neighbors
    std::vector<int> neighbours;
    
    const auto [row, col] = stackMaze.top();
    // if start and finish cells - different way
    if (row == 0 && col == 0) {
      // now, only for start cell
      neighbours.push_back(NORTHN);
    } else {
      // North neigbour
      if (row < mazeH - 1 && (mapMaze[row +1][col] & VISITED) == 0) {
        neighbours.push_back(NORTHN);
      }
      // East neigbour
      if (col < mazeW - 1 && (mapMaze[row][col + 1] & VISITED) == 0) {
        neighbours.push_back(EASTN);
      }
      // South neigbour
      if (row > 0 && (mapMaze[row - 1][col] & VISITED) == 0) {
        neighbours.push_back(SOUTHN);
      }
       // West neigbour
      if (col > 0 && (mapMaze[row][col-1] & VISITED) == 0) {
        neighbours.push_back(WESTN);
      }
    }
    
    // Are there any neighbours available?
    if (!neighbours.empty()) {
      // get one available neighbour at random
      int nextDir = neighbours[rand() % neighbours.size()];
      // Create a path between the neighbour and the current cell
      switch (nextDir) {
        case NORTHN: // North
          mapMaze[row +1][col] |= VISITED;
          clearWall(row, col, NORTH);
          stackMaze.push({row +1, col});
          break;
        case EASTN:  // East
          mapMaze[row][col + 1] |= VISITED;
          clearWall(row, col, EAST);
          stackMaze.push({row, col + 1});
          break;
        case SOUTHN: // South
          mapMaze[row - 1][col] |= VISITED;
          clearWall(row, col, SOUTH);
          stackMaze.push({row - 1, col});
          break;
        case WESTN:  // West
          mapMaze[row][col - 1] |= VISITED;
          clearWall(row, col, WEST);
          stackMaze.push({row, col - 1});
          break;
      }
      nVisitedCells++;
      //setColorVisited(cv::Scalar(0, 255, 5 * nVisitedCells));
    } else {
      // No available neighbours so backtrack!
      stackMaze.pop();
    }
    
  } else {
    onUserUpdate = nullptr;

    std::cout << "Size of stack : " << stackMaze.size() << std::endl;
    stackMaze = std::stack<CellType>();
    //std::cout << "Size of stack : " << stackMaze.size() << std::endl;
  }
}

void Maze::generate() {
  // Seed random number generator
  srand(clock());
  
  for (int i = 0; i < mazeH; ++i) {
    for (int j = 0; j < mazeW; ++j) {
      mapMaze[i][j] = NORTH | EAST | SOUTH | WEST;
    }
  }
  matMaze = cv::Scalar(255, 40, 40);
  drawStandMaze();
  // for start cell
  mapMaze[0][0] |= VISITED;
  // for finish cells
  if (goalMaze == GoalDiagonal) {
    clearWall(mazeH -1, mazeW - 1, SOUTH);
    mapMaze[mazeH -1][mazeW - 1] |= VISITED;
    nVisitedCells = 2;
  } else {
  // for target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    clearWall(ROW, COL, EAST);
    clearWall(ROW, COL, NORTH);
    clearWall(ROW + 1, COL + 1 , WEST);
    clearWall(ROW + 1, COL + 1, SOUTH);
    mapMaze[ROW][COL] |= VISITED;
    mapMaze[ROW +  1][COL] |= VISITED;  
    mapMaze[ROW + 1][COL + 1] |= VISITED;  
    mapMaze[ROW][COL + 1] |= VISITED;  
    std::vector<CellType> goalCells {
      {ROW, COL}, {ROW, COL + 1},
      {ROW + 1, COL}, {ROW + 1, COL + 1}
    };
    std::vector<int> goalWalls {
      WEST, SOUTH, SOUTH, EAST, WEST, NORTH, NORTH, EAST 
    };
    // random entry to target cells
    entryInGoal = rand() % goalWalls.size();
    const auto [nr, nc ] = goalCells[entryInGoal / 2];
    clearWall(nr, nc, goalWalls[entryInGoal]);
    nVisitedCells = 5;
  }
  stackMaze.push({0,0});
  // pointer to member function
  onUserUpdate = &Maze::algGeneration;
}

void Maze::edit() {

}

bool Maze::save() {
  return true;
}

bool Maze::load() {
  return true;
}

void Maze::floodFill() {
  onUserUpdate = &Maze::floodFill;
}

void Maze::advFloodFill() {
  onUserUpdate = &Maze::advFloodFill;
}


} // namespace
