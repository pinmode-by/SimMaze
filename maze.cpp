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
  
  offset = p;
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
  // clear stack
  stackMaze = std::stack<CellType>();
  // clear pointers
  onUserUpdate = nullptr;
  originMaze = nullptr;
  // assign outside walls Maze
  for (int i = 0; i < mazeW; ++i) { 
    setWall(0, i, NORTH);  
    setWall(mazeH - 1, i, SOUTH);  
  }
  for (int i = 0; i < mazeH; ++i) { 
    setWall(i, 0, WEST); 
    setWall(i, mazeW - 1, EAST); 
  }
  drawStandMaze();
}

void Maze::setStartGoalCells() {
  goalPositipons.clear();
  // start cell - assign EAST wall
  setWall(mazeH - 1, 0, EAST); 
  if (goalMaze == GoalDiagonal) {
    // target - diagonal cell: assign WEST wall
    setWall(0, mazeW - 1, WEST);
    goalPositipons.push_back({0, mazeW - 1});
  } else {
    // target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    std::vector<CellType> goalCells {
      {ROW, COL}, {ROW, COL + 1},
      {ROW + 1, COL}, {ROW + 1, COL + 1}
    };
    std::vector<int> goalWalls {
      WEST, NORTH, NORTH, EAST, WEST, SOUTH, SOUTH, EAST 
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
    goalPositipons.push_back({ROW, COL});
    goalPositipons.push_back({ROW, COL + 1});
    goalPositipons.push_back({ROW + 1, COL});
    goalPositipons.push_back({ROW + 1, COL + 1});
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
      if (row > 0) {
         mapMaze[row - 1][col] |= SOUTH;
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
      if (row < mazeH - 1) {
         mapMaze[row + 1][col] |= NORTH;
      }
      break;
  }
}

void Maze::clearWall(int row, int col, int wall){
  
  mapMaze[row][col] &= ~wall;
  // clear wall at neighbor cells
  switch (wall) {
    case NORTH:
      if (row > 0) {
         mapMaze[row -1][col] &= ~SOUTH;
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
      if (row < mazeH - 1) {
         mapMaze[row + 1][col] &= ~NORTH;
      }
      break;
    }
}

void Maze::drawCell(int row, int col) {
  static const int cellWidth = pathWidth + wallWidth;
  
  const auto cX = col * cellWidth; 
  const auto cY = row * cellWidth;
  // Draw NORTH wall
  if (mapMaze[row][col] & NORTH) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY),
        cv::Point(cX + cellWidth - 1, cY + wallWidth - 1),
        cv::Scalar(0, 0, 255), // Red color
        cv::FILLED,
        cv::LINE_8 );
  } else {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY),
        cv::Point(cX + cellWidth - 1, cY + wallWidth - 1),
        cv::Scalar(50, 50, 50), // No wall
        cv::FILLED,
        cv::LINE_8 );
  }
  
  // Draw WEST wall
  if (mapMaze[row][col] & WEST) {
    rectangle(matMaze,
        cv::Point(cX, cY + wallWidth),
        cv::Point(cX + wallWidth - 1, cY + cellWidth - 1),
        cv::Scalar( 0, 0, 255 ), // Red color
        cv::FILLED,
        cv::LINE_8 );
  } else {
    rectangle(matMaze,
        cv::Point(cX, cY + wallWidth),
        cv::Point(cX + wallWidth - 1, cY + cellWidth - 1),
        cv::Scalar(50, 50, 50), // No wall
        cv::FILLED,
        cv::LINE_8 );
  }
  
  // SOUTH outside walls
  if (row == mazeH - 1) {
      rectangle(matMaze,
        cv::Point(cX + wallWidth, cY + cellWidth),
        cv::Point(cX + cellWidth -1 , cY + cellWidth + wallWidth - 1),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  } 
  
  // EAST outside walls
  if (col == mazeW - 1) {
      rectangle(matMaze,
        cv::Point(cX + cellWidth, cY + wallWidth),
        cv::Point(sizeMatMaze.width - 1, cY + cellWidth - 1),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  }
  
  if (mapMaze[row][col] & VISITED) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY + wallWidth),
        cv::Point(cX + cellWidth - 1, cY + cellWidth - 1),
        colorVISITED,
        cv::FILLED,
        cv::LINE_8 );
  };
  
  // if stack is not empty - CURRENT
  if (!stackMaze.empty()) {
    auto [rowC, colC] = stackMaze.top();
    if (row == rowC && col == colC) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY + wallWidth),
        cv::Point(cX + cellWidth - 1, cY + cellWidth - 1),
        colorCURRENT,
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
    if (row == mazeH - 1  && col == 0) {
      // now, only for start cell
      neighbours.push_back(NORTHN);
    } else {
      // North neigbour
      if (row > 0 && (mapMaze[row - 1][col] & VISITED) == 0) {
        neighbours.push_back(NORTHN);
      }
      // East neigbour
      if (col < mazeW - 1 && (mapMaze[row][col + 1] & VISITED) == 0) {
        neighbours.push_back(EASTN);
      }
      // South neigbour
      if (row < mazeH - 1  && (mapMaze[row + 1][col] & VISITED) == 0) {
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
          mapMaze[row - 1][col] |= VISITED;
          clearWall(row, col, NORTH);
          stackMaze.push({row - 1, col});
          break;
        case EASTN:  // East
          mapMaze[row][col + 1] |= VISITED;
          clearWall(row, col, EAST);
          stackMaze.push({row, col + 1});
          break;
        case SOUTHN: // South
          mapMaze[row + 1][col] |= VISITED;
          clearWall(row, col, SOUTH);
          stackMaze.push({row + 1, col});
          break;
        case WESTN:  // West
          mapMaze[row][col - 1] |= VISITED;
          clearWall(row, col, WEST);
          stackMaze.push({row, col - 1});
          break;
      }
      nVisitedCells++;
    } else {
      // No available neighbours so backtrack!
      stackMaze.pop();
    }
    
  } else {
    onUserUpdate = nullptr;

    std::cout << "Size of stack : " << stackMaze.size() << std::endl;
    stackMaze = std::stack<CellType>();

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
  mapMaze[mazeH - 1][0] |= VISITED;
  // for finish cells
  if (goalMaze == GoalDiagonal) {
    clearWall(0, mazeW - 1, SOUTH);
    mapMaze[0][mazeW - 1] |= VISITED;
    nVisitedCells = 2;
  } else {
  // for target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    clearWall(ROW, COL, EAST);
    clearWall(ROW, COL, SOUTH);
    clearWall(ROW + 1, COL + 1 , WEST);
    clearWall(ROW + 1, COL + 1, NORTH);
    mapMaze[ROW][COL] |= VISITED;
    mapMaze[ROW +  1][COL] |= VISITED;  
    mapMaze[ROW + 1][COL + 1] |= VISITED;  
    mapMaze[ROW][COL + 1] |= VISITED;  
    std::vector<CellType> goalCells {
      {ROW, COL}, {ROW, COL + 1},
      {ROW + 1, COL}, {ROW + 1, COL + 1}
    };
    std::vector<int> goalWalls {
      WEST, NORTH, NORTH, EAST, WEST, SOUTH, SOUTH, EAST 
    };
    // random entry to target cells
    entryInGoal = rand() % goalWalls.size();
    const auto [nr, nc ] = goalCells[entryInGoal / 2];
    clearWall(nr, nc, goalWalls[entryInGoal]);
    nVisitedCells = 5;
  }
  setColorCurrent({0, 255, 255});
  stackMaze.push(getStartPosition()); // {mazeH - 1, 0}
  // pointer to member function
  onUserUpdate = &Maze::algGeneration;
  prevDir = SOUTHN;
}

void Maze::edit() {
    
}

bool Maze::save() {
  return true;
}

void Maze::algRandSearch() { 

  std::this_thread::sleep_for(200ms);
  const auto [row, col] = stackMaze.top();
  
 
  if (!isGoal({row, col})) {
    
    // set walls and neighbours
    int valueCell = originMaze->mapMaze[row][col];
    // create a set of neighbors
    std::vector<int> neighbours;
    if (valueCell & NORTH) {
      setWall(row, col, NORTH);
    } else if (prevDir != NORTHN) {
      // North neigbour
      neighbours.push_back(NORTHN);
    }

    if (valueCell & WEST) {
      setWall(row, col, WEST);
    } else if (prevDir != WESTN) {
      // West neigbour
      neighbours.push_back(WESTN);
    }
    
    if (valueCell & SOUTH) {
      setWall(row, col, SOUTH);
    } else if (prevDir != SOUTHN) {
      // South neigbour
      neighbours.push_back(SOUTHN); 
    }
    
    if (valueCell & EAST) {
      setWall(row, col, EAST);
    } else if (prevDir != EASTN) {
      // East neigbour
      neighbours.push_back(EASTN); 
    }
    
    if (!neighbours.empty()) {
      // get one available neighbour at random
      int nextDir = neighbours[rand() % neighbours.size()];
      switch (nextDir) {
        case NORTHN: // North
          stackMaze.push({row - 1, col});
          prevDir = SOUTHN;
          break;
        case EASTN:  // East
          stackMaze.push({row, col + 1});
          prevDir = WESTN;
          break;
        case SOUTHN: // South
          stackMaze.push({row + 1, col});
          prevDir = NORTHN;
          break;
        case WESTN:  // West
          stackMaze.push({row, col - 1});
          prevDir = EASTN;
          break;
      } // switch
      
    } else {
      // No available neighbours so backtrack!
      stackMaze.pop();
    }
  } else {
    // finish randSearch
    onUserUpdate = nullptr;
    // clear stack 
    stackMaze = std::stack<CellType>();
  }
}

bool Maze::isGoal(CellType cell) {
  for (const auto [row, col] : goalPositipons) {
    if (row == cell.first && col == cell.second ) {
      return true;
    }
  }  
  return false;
  
}

void Maze::randSearch(Maze *origin) {
  clean();
  // 
  originMaze = origin;
  // get start position
  auto [startR, startC] = originMaze->getStartPosition();
  mapMaze[startR][startC] |= VISITED;
  setWall(startR, startC, EAST);
  stackMaze.push({startR, startC});
  setColorCurrent({0, 255, 0});
  // set handler - pointer to member function
  onUserUpdate = &Maze::algRandSearch;
  goalPositipons = originMaze->getGoalPosition();
  prevDir = SOUTHN;
}

void Maze::floodFill() {
  onUserUpdate = &Maze::floodFill;
}

void Maze::advFloodFill() {
  onUserUpdate = &Maze::advFloodFill;
}

bool Maze::isWall(cv::Point p) {
	
  return true ; //matMaze.contains
}

void Maze::printMaze() {
 for (int i = 0; i < mazeH; ++i) {
    for (int j = 0; j < mazeW; ++j) {
      std::cout << mapMaze[i][j] << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << "\n\n" << std::endl;
}

} // namespace
