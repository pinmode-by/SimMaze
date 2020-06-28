#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "maze.h"
#include "utils.h"

using namespace std::chrono_literals;
 
namespace sm {

bool isMouseEvent = false;

MouseEvent mEvent;
 
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
  mapMaze.assign(MAZE_WIDTH * MAZE_WIDTH, 0);
  srand(clock());
  onCreate();

}

void Maze::onCreate() {
  // clear mat
  matMaze = cv::Scalar(50, 50, 50);
  // clear stack
  stackMaze = std::stack<CellType>();
  // clear pointers
  onMazeUpdate = nullptr;
  originMaze = nullptr;
  // assign outside walls Maze
  for (int i = 0; i < mazeW; ++i) { 
    setWall(i, mazeH - 1, NORTH);  
    setWall(i, 0, SOUTH);  
  }
  for (int i = 0; i < mazeH; ++i) { 
    setWall(0, i, WEST); 
    setWall(mazeW - 1, i, EAST); 
  }
  drawStandMaze();
  //printMaze();
}

void Maze::setStartGoalCells() {
  goalPositions.clear();
  // start cell - assign EAST wall
  setWall(0, 0, EAST); 
  if (goalMaze == GoalDiagonal) {
    // target - diagonal cell: assign WEST wall
    setWall(mazeW - 1, mazeW - 1, WEST);
    goalPositions.push_back({mazeW - 1, mazeW - 1});
  } else {
    // target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    std::vector<CellType> goalCells {
      {COL, ROW}, {COL, ROW + 1},
      {COL + 1, ROW}, {COL + 1, ROW + 1}
    };
    std::vector<int> goalWalls {
      WEST, SOUTH, WEST, NORTH, SOUTH, EAST, EAST, NORTH 
    };
    // random entry to target cells
    entryInGoal = rand() % goalWalls.size();
    goalWalls[entryInGoal] = 0;
    std::cout << "entryInGoal : " << entryInGoal << std::endl; 
    int i = 0;
    for (const auto& [col, row] : goalCells) {
      setWall(col, row, goalWalls[i]); 
      setWall(col, row, goalWalls[i + 1]); 
      i += 2;
    }
    goalPositions.push_back({COL, ROW});
    goalPositions.push_back({COL, ROW + 1});
    goalPositions.push_back({COL + 1, ROW});
    goalPositions.push_back({COL + 1, ROW + 1});
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

void Maze::setWall(int col, int row, int wall) {
  mapMaze[cellN(col, row)] |= wall;
  // set wall at neighbor cells
  switch (wall) {
    case NORTH:
      if (row < mazeH - 1) {
         mapMaze[cellN(col, row + 1)] |= SOUTH;
      }
      break;
    case WEST:
      if (col > 0) {
         mapMaze[cellN(col - 1, row)] |= EAST;
      }
      break;
    case EAST:
      if (col < mazeW - 1) {
         mapMaze[cellN(col + 1, row)] |= WEST;
      }
      break;
    case SOUTH:
      if (row > 0) {
         mapMaze[cellN(col, row -1)] |= NORTH;
      }
      break;
  }
}

void Maze::clearWall(int col, int row, int wall){
  
  mapMaze[cellN(col, row)] &= ~wall;
  // clear wall at neighbor cells
  switch (wall) {
    case NORTH:
      if (row < mazeH - 1) {
        mapMaze[cellN(col, row + 1)] &= ~SOUTH;
      }
      break;
    case WEST:
      if (col > 0 ) {
        mapMaze[cellN(col - 1, row)] &= ~EAST;
      }
      break;
    case EAST:
      if (col < mazeW - 1) {
         mapMaze[cellN(col + 1, row)] &= ~WEST;
      }
      break;
    case SOUTH:
      if (row > 0) {
         mapMaze[cellN(col, row -1)] &= ~NORTH;
      }
      break;
    }
}

void Maze::drawCell(int col, int row) {
  static const int cellWidth = pathWidth + wallWidth;
  
  const auto cX = col * cellWidth; 
  const auto cY = sizeMatMaze.height - row * cellWidth - 1;
   
  // Draw SOUTH wall
  if (mapMaze[cellN(col, row)] & SOUTH) {
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
  
  // Draw WEST wall
  if (mapMaze[cellN(col, row)] & WEST) {
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
        cv::Point(cX + cellWidth -1 , wallWidth - 1),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  } 
  
  // EAST outside walls
  if (col == mazeW - 1) {
      rectangle(matMaze,
        cv::Point(cX + cellWidth, cY - cellWidth + 1),
        cv::Point(sizeMatMaze.width - 1, cY - wallWidth),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  }
  
  if (mapMaze[cellN(col, row)] & VISITED) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY - wallWidth),
        cv::Point(cX + cellWidth - 1, cY - cellWidth + 1),
        colorVISITED,
        cv::FILLED,
        cv::LINE_8 );
  };
  
  // if stack is not empty - CURRENT
  if (!stackMaze.empty()) {
    auto [colC, rowC] = stackMaze.top();
    if (col == colC && row == rowC) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY - wallWidth),
        cv::Point(cX + cellWidth - 1, cY - cellWidth + 1),
        colorCURRENT,
        cv::FILLED,
        cv::LINE_8 );
    }
  }
}

void Maze::update() {
  if (onMazeUpdate != nullptr) {
    // call background function 
    (this->*onMazeUpdate)();
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
  mapMaze.assign(MAZE_WIDTH * MAZE_WIDTH, 0);
  onCreate();
}

void Maze::algGeneration() {
  if (nVisitedCells < mazeH * mazeW) {
    // Slow down for animation
    std::this_thread::sleep_for(4ms);
    
    // create a set of unvisited neighbors
    std::vector<int> neighbours;
    
    const auto [col, row] = stackMaze.top();
    // if start and finish cells - different way
    if (col == 0  && row == 0) {
      // now, only for start cell
      neighbours.push_back(NORTHN);
    } else {
      // North neigbour
      if (row < mazeH - 1 && (mapMaze[cellN(col, row + 1)] & VISITED) == 0) {
        neighbours.push_back(NORTHN);
      }
      // East neigbour
      if (col < mazeW - 1 && (mapMaze[cellN(col + 1, row)] & VISITED) == 0) {
        neighbours.push_back(EASTN);
      }
      // South neigbour
      if (row > 0  && (mapMaze[cellN(col, row - 1)] & VISITED) == 0) {
        neighbours.push_back(SOUTHN);
      }
       // West neigbour
      if (col > 0 && (mapMaze[cellN(col - 1, row)] & VISITED) == 0) {
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
          mapMaze[cellN(col, row + 1)] |= VISITED;
          clearWall(col, row, NORTH);
          stackMaze.push({col, row + 1});
          break;
        case EASTN:  // East
          mapMaze[cellN(col + 1, row)] |= VISITED;
          clearWall(col, row, EAST);
          stackMaze.push({col + 1, row});
          break;
        case SOUTHN: // South
          mapMaze[cellN(col, row - 1)] |= VISITED;
          clearWall(col, row, SOUTH);
          stackMaze.push({col, row - 1});
          break;
        case WESTN:  // West
          mapMaze[cellN(col - 1, row)] |= VISITED;
          clearWall(col, row, WEST);
          stackMaze.push({col - 1, row});
          break;
      }
      nVisitedCells++;
    } else {
      // No available neighbours so backtrack!
      stackMaze.pop();
    }
    
  } else {
    onMazeUpdate = nullptr;

    std::cout << "Size of stack : " << stackMaze.size() << std::endl;
    stackMaze = std::stack<CellType>();

  }
}

void Maze::generate() {
  // Seed random number generator
  srand(clock());
  
  for (int row = 0; row < mazeH; ++row) {
    for (int col = 0; col < mazeW; ++col) {
      mapMaze[cellN(col, row)] = NORTH | EAST | SOUTH | WEST;
    }
  }
  matMaze = cv::Scalar(255, 40, 40);
  drawStandMaze();
  // for start cell
  mapMaze[cellN(0, 0)] |= VISITED;
  // for finish cells
  if (goalMaze == GoalDiagonal) {
    clearWall(mazeW - 1, mazeH - 1, SOUTH);
    mapMaze[cellN(mazeW - 1, mazeH - 1)] |= VISITED;
    nVisitedCells = 2;
  } else {
  // for target - center of Maze: 4 cells
    const int ROW = (mazeH - 1) / 2;
    const int COL = (mazeW - 1) / 2; 
    clearWall(COL, ROW, EAST);
    clearWall(COL, ROW, NORTH);
    clearWall(COL + 1, ROW + 1, WEST);
    clearWall(COL + 1, ROW + 1, SOUTH);
    mapMaze[cellN(COL, ROW)] |= VISITED;
    mapMaze[cellN(COL, ROW + 1)] |= VISITED;  
    mapMaze[cellN(COL + 1, ROW)] |= VISITED;  
    mapMaze[cellN(COL + 1, ROW + 1)] |= VISITED;  
    std::vector<CellType> goalCells {
      {COL, ROW}, {COL, ROW + 1},
      {COL + 1, ROW}, {COL + 1, ROW + 1}
    };
    std::vector<int> goalWalls {
      WEST, SOUTH, WEST, NORTH, SOUTH, EAST, EAST, NORTH 
    };
    // random entry to target cells
    entryInGoal = rand() % goalWalls.size();
    const auto [nc, nr ] = goalCells[entryInGoal / 2];
    clearWall(nc, nr, goalWalls[entryInGoal]);
    nVisitedCells = 5;
  }
  setColorCurrent({0, 255, 255});
  stackMaze.push(getStartPosition()); // {0, 0}
  // pointer to member function
  onMazeUpdate = &Maze::algGeneration;
}


void Maze::onEdit() {
  if (sm::isMouseEvent) {
    if (isOriginMaze()) {
      std::cout << "Edit :" << "at (" << mEvent.x << "," << mEvent.y << 
         ")" << std::endl;
    }
    // flag off 
    isMouseEvent = false;
  
  }
}

bool Maze::isOriginMaze() {
  if ((mEvent.x >= offset.x && mEvent.x <= offset.x + matMaze.cols) &&
     (mEvent.y >= offset.y && mEvent.y <= offset.y + matMaze.rows)) {
      return true;
  }
  return false;
}

void Maze::edit() {
  onMazeUpdate = &Maze::onEdit;
}

bool Maze::isWall(cv::Point p) {

  return true ; //matMaze.contains
}

bool Maze::save() {
  return true;
}

// check if cell in goal array positions
bool Maze::isGoal(CellType cell) {
  for (const auto [col, row] : goalPositions) {
    if (col == cell.first && row == cell.second ) {
      return true;
    }
  }  
  return false;
}


void Maze::algRandSearch() { 

  std::this_thread::sleep_for(10ms);
  const auto [col, row] = stackMaze.top();
  
  if (!isGoal({col, row})) {   
    // set walls and neighbours
    int valueCell = originMaze->mapMaze[cellN(col, row)];
    // create a set of neighbors
    std::vector<int> neighbours;
    if (valueCell & NORTH) {
      setWall(col, row, NORTH);
    } else if (prevDir != NORTHN) {
      // North neigbour
      neighbours.push_back(NORTHN);
    }

    if (valueCell & WEST) {
      setWall(col, row, WEST);
    } else if (prevDir != WESTN) {
      // West neigbour
      neighbours.push_back(WESTN);
    }
    
    if (valueCell & SOUTH) {
      setWall(col, row, SOUTH);
    } else if (prevDir != SOUTHN) {
      // South neigbour
      neighbours.push_back(SOUTHN); 
    }
    
    if (valueCell & EAST) {
      setWall(col, row, EAST);
    } else if (prevDir != EASTN) {
      // East neigbour
      neighbours.push_back(EASTN); 
    }
    
    if (!neighbours.empty()) {
      // get one available neighbour at random
      int nextDir = neighbours[rand() % neighbours.size()];
      switch (nextDir) {
        case NORTHN: // North
          mapMaze[cellN(col, row + 1)] |= VISITED;
          stackMaze.push({col, row + 1});
          prevDir = SOUTHN;
          break;
        case EASTN:  // East
          mapMaze[cellN(col + 1, row)] |= VISITED;
          stackMaze.push({col + 1, row});
          prevDir = WESTN;
          break;
        case SOUTHN: // South
          mapMaze[cellN(col, row - 1)] |= VISITED;
          stackMaze.push({col, row - 1});
          prevDir = NORTHN;
          break;
        case WESTN:  // West
          mapMaze[cellN(col - 1, row)] |= VISITED;
          stackMaze.push({col - 1, row});
          prevDir = EASTN;
          break;
      } // switch
      
    } else {
      // No available neighbours so backtrack!
      // change oposite prevDir
      if (prevDir == NORTHN) {
        prevDir = SOUTHN;
      } else if (prevDir == SOUTHN) {
        prevDir = NORTHN;
      } else if (prevDir == WESTN) {
        prevDir = EASTN;
      } else {
        prevDir = WESTN;
      }
      stackMaze.pop();
    }
  } else {
    // finish randSearch
    onMazeUpdate = nullptr;
    // draw goal cell

    // clear stack 
    //stackMaze = std::stack<CellType>();
  }
}

void Maze::randSearch(Maze *origin) {
  clean();
  // copy pointer to origin Maze
  originMaze = origin;
  // get start position
  auto [startC, startR] = originMaze->getStartPosition();
  mapMaze[cellN(startC, startR)] |= VISITED;
  setWall(startC, startR, EAST);
  stackMaze.push({startC, startR});
  setColorCurrent({0, 255, 0});
  goalPositions = originMaze->getGoalPosition();
  if (goalPositions.empty()) {
    // standart positions
    setStandardGoal();
  }
  prevDir = SOUTHN;
  // set handler - pointer to algRandSearch
  onMazeUpdate = &Maze::algRandSearch;
  srand(clock());
}

// 
void Maze::printMaze() {
  std::cout << std::hex;
  for (int i = 0; i < mazeH; ++i) {
    for (int j = 0; j < mazeW; ++j) {
      std::cout << static_cast<int>(mapMaze[cellN(j, i)]) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::dec << "\n\n";
}

void Maze::setStandardGoal() { 
  if (goalPositions.empty()) {
    // standart positions
    if (goalMaze == GoalDiagonal) {
      goalPositions = {{mazeW - 1, mazeH - 1}};
      clearWall(mazeW - 1, mazeH - 1, SOUTH);
      setWall(mazeW - 1, mazeH - 1, WEST);
    } else {
      const int ROW = (mazeH - 1) / 2;
      const int COL = (mazeW - 1) / 2;
      goalPositions = {{COL, ROW},
        {COL, ROW + 1},{COL + 1, ROW}, {COL + 1, ROW + 1}};
    }
  }
}

} // namespace
