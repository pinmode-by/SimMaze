
#include <memory>
#include <iomanip>
#include <thread>
#include <chrono>
#include "solverMaze.h"
#include "maze.h"
#include "utils.h"

using namespace std::chrono_literals;

namespace sm {


SolverMaze::SolverMaze(std::shared_ptr<Maze> oMaze,
  std::shared_ptr<Maze> sMaze) 
  : originM(oMaze), solveM(sMaze) {
  mapDistances.assign(MAZE_AREA, MAX_DISTANCE);
} 

void SolverMaze::update() {
  if (onSolverUpdate != nullptr) {
    (this->*onSolverUpdate)();
  }
}


void SolverMaze::initAlgorithm() {
  
  solveM->clean();
  // get start position
  auto [startC, startR] = originM->getStartPosition();
  solveM->setVisited(startC, startR);
  solveM->setWall(startC, startR, EAST);
  solveM->setColorCurrent({0, 255, 0});
  solveM->goalPositions = originM->getGoalPosition();
  if ( solveM->goalPositions.empty()) {
    // standart positions
    solveM->setStandardGoal();
  }  
  solveM->goalMaze  = originM->goalMaze;
  if (solveM->goalMaze == GoalDiagonal) {
    solveM->clearWall(solveM->mazeW - 1,  solveM->mazeH - 1, SOUTH);
    solveM->setWall(solveM->mazeW - 1,  solveM->mazeH - 1, WEST);
  }
}

void SolverMaze::floodFill() {
  initAlgorithm();
  
  // set handler - pointer to 
  onSolverUpdate = &SolverMaze::algFloodFill;
  setDistances();
  originM->printMaze();
  printMapDistances();
}

void SolverMaze::algFloodFill() {

}

void SolverMaze::advFloodFill() {

}

compassDir SolverMaze::nextCompassDirection(stepDir step) {
  if (step == LEFT) {
    if (currentDir == EASTC)
      return NORTHC;
    if (currentDir == SOUTHC)
      return EASTC;
    if (currentDir == WESTC)
      return SOUTHC;
    return WESTC;
  } else if (step == RIGHT) {
    if (currentDir == EASTC)
      return SOUTHC;
    if (currentDir == SOUTHC)
      return WESTC;
    if (currentDir == WESTC)
      return NORTHC;
    return EASTC;
  } else if (step == FRONT) {
    return currentDir;
  }
  return static_cast<compassDir>(-currentDir);
}

bool SolverMaze::currentSideWall(stepDir side) {

  uchar bit = 0;
  compassDir direction = nextCompassDirection(side);
  std::cout <<  "next direction  : " << direction << std::endl;
  switch (direction) {
  case EASTC:
    bit = EAST;
    break;
  case SOUTHC:
    bit = SOUTH;
    break;
  case WESTC:
    bit = WEST;
    break;
  case NORTHC:
    bit = NORTH;
    break;
	default:
    break;
  }
  return bit;
}

bool SolverMaze::isNeighbourVisited(int col, int row, uchar wall) {
  switch (wall) {
    case NORTH:
        return (solveM->getCell(col, row + 1) & VISITED) != 0;
      break;
    case EAST:
        return (solveM->getCell(col + 1, row) & VISITED) != 0;
      break;
    case WEST:
        return (solveM->getCell(col - 1, row ) & VISITED) != 0;
      break; 
    case SOUTH:
        return (solveM->getCell(col, row - 1) & VISITED) != 0;
      break;   
  }
  return false;
}

void SolverMaze::algWallFollower() {
  std::this_thread::sleep_for(100ms);
  const auto [col, row] = solveM->stackMaze.top();
  if (!solveM->isGoal({col, row})) {  
    // set walls and neighbours
    int valueCell = originM->getCell(col, row);
    // set walls if not visited
    solveM->setWalls(col, row, valueCell);
    
    // create a set of neighbors
    std::vector<int> neighbour;
    int wall0  = currentSideWall(RIGHT);
    std::cout <<  "wall 0 : " << wall0 << std::endl;
    
    if (int wall = currentSideWall(RIGHT);
        !isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      std::cout <<  "wall 1 : " << wall << std::endl;
      neighbour.push_back(wall);
    } else if (int wall = currentSideWall(FRONT); 
        !isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      std::cout <<  "wall 2: " << wall << std::endl;
      neighbour.push_back(wall);
    } else if (int wall = currentSideWall(LEFT);
        !isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      std::cout <<  "wall 3: " << wall << std::endl;
      neighbour.push_back(wall);
    } 
    
    if (!neighbour.empty()) {
      
      int nextDir = neighbour[0];
      std::cout <<  "nextDir : " << nextDir << std::endl;
      switch (nextDir) {
        case NORTH: 
          solveM->stackMaze.push({col, row + 1});
          solveM->setVisited(col, row + 1);
          currentDir = NORTHC;
          break;
        case WEST: 
          solveM->stackMaze.push({col - 1, row});
          solveM->setVisited(col -1, row);
          currentDir = WESTC;
          break; 
        case EAST: 
          solveM->stackMaze.push({col + 1, row});
          solveM->setVisited(col + 1, row);
          currentDir = EASTC;
          break; 
        case SOUTH: 
          solveM->stackMaze.push({col, row - 1});
          solveM->setVisited(col, row - 1);
          currentDir = SOUTHC;
          break;  
      }
        
    } else {
      // No available neighbours so backtrack!
      solveM->stackMaze.pop();
    }
    
    
    /*
    if (valueCell & NORTH) {
      solveM->setWall(col, row, NORTH);
    } else if (prevDir != -NORTHN && (row < solveM->mazeH - 1) &&
    ((solveM->getCell(col - 1, row) & VISITED) == 0)) {
      // North neigbour
      neighbours.push_back(NORTHN);
    }

    if (valueCell & WEST) {
      solveM->setWall(col, row, WEST);
    } else if (prevDir != WESTN && (row < solveM->mazeH - 1) &&
    ((solveM->getCell(col - 1, row) & VISITED) == 0)) {
      // West neigbour
      neighbours.push_back(WESTN);
    }
    
    if (valueCell & SOUTH) {
      solveM->setWall(col, row, SOUTH);
    } else if (prevDir != SOUTHN && (row < solveM->mazeH - 1) &&
    ((solveM->getCell(col - 1, row) & VISITED) == 0)) {
      // South neigbour
      neighbours.push_back(SOUTHN); 
    }
    
    if (valueCell & EAST) {
      solveM->setWall(col, row, EAST);
    } else if (prevDir != EASTN && (row < solveM->mazeH - 1) &&
    ((solveM->getCell(col - 1, row) & VISITED) == 0)) {
      // East neigbour
      neighbours.push_back(EASTN); 
    }
    
    if (!neighbours.empty()) {
      int nextDir = neighbours[rand() % neighbours.size()];
    
    
    } else {
      // No available neighbours so backtrack!
      solveM->stackMaze.pop();
    }
    */
  
  } else {
     onSolverUpdate = nullptr;
  }
}
 

void SolverMaze::wallFollower() {
  initAlgorithm();
  currentDir = NORTHC;
  solveM->stackMaze.push({solveM->getStartPosition()});
  // set handler - pointer to 
  onSolverUpdate = &SolverMaze::algWallFollower;
}



void SolverMaze::randSearch() {
  //initAlgorithm();
}
 
void SolverMaze::resetDistancesAndQueue(void) {
  for (int i = 0; i < MAZE_AREA; ++i) {
    mapDistances[i] = MAX_DISTANCE;
  }
  // clear queue
  std::queue<CellType> empty;
  std::swap(queueD, empty);
} 
 
void SolverMaze::setDistances() {
  resetDistancesAndQueue();
  for (auto [col, row] : solveM->goalPositions) {
    mapDistances[solveM->cellN(col, row)] = 0;
    queueD.push({col, row});
  }
  updateDistances();
}
 
void SolverMaze::updateDistances() {
  while (!queueD.empty()) {
    auto [col, row] = queueD.front();
    queueD.pop();
    int distance = mapDistances[solveM->cellN(col, row)] + 1;
    
    if (!isWallExists(col, row, EAST))
      queuePushDistances(col + 1, row, distance);
    if (!isWallExists(col, row, SOUTH))
      queuePushDistances(col, row - 1, distance);
    if (!isWallExists(col, row, WEST))
      queuePushDistances(col - 1, row, distance);
    if (!isWallExists(col, row, NORTH))
      queuePushDistances(col, row + 1, distance);
  }
}
 
void SolverMaze::queuePushDistances(int col, int row, int distance) {
  
  uchar dist = mapDistances[solveM->cellN(col, row)];
  if (dist <= distance)
    return;
  mapDistances[solveM->cellN(col, row)] = distance;
  queueD.push({col, row});
}
 
bool SolverMaze::isWallExists(int col, int row, uchar wall) {
  return solveM->mapMaze[solveM->cellN(col, row)] & wall;
}
   
void SolverMaze::printMapDistances() {
  std::cout << std::dec;
  for (int i = 0; i < solveM->mazeH; ++i) {
    for (int j = 0; j < solveM->mazeW; ++j) {
      std::cout << std::setw(4) <<
      static_cast<int>(mapDistances[solveM->cellN(j, i)]);
    }
    std::cout << std::endl;
  }
  std::cout << std::dec << "\n\n";
}

}
