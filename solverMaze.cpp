
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
  // clear stack
  stackDir = std::stack<compassDir>();
  // get start position
  auto [startC, startR] = originM->getStartPosition();

  solveM->setWall(startC, startR, EAST);
  solveM->mapMaze[solveM->cellN(startC, startR)] |= START_CELL;
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
  for(const auto [col, row] : solveM->goalPositions) {
    solveM->mapMaze[solveM->cellN(col, row)] |= TARGET_CELL;
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

uchar SolverMaze::currentSideWall(stepDir side) {

  uchar bit = 0;
  compassDir direction = nextCompassDirection(side);
  //std::cout <<  "next direction  : " << direction << std::endl;
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
  std::this_thread::sleep_for(50ms);
  const auto [col, row] = solveM->stackMaze.top();  
  if (!solveM->isGoal({col, row})) {  
    int valueCell = originM->getCell(col, row);
    // set walls if not visited
    if ((solveM->getCell(col, row) & VISITED) == 0) {
      solveM->setWalls(col, row, valueCell);
      solveM->setVisited(col, row);
    }
    // create a set of neighbors
    std::vector<int> neighbour;
    
    
    if (int wall = currentSideWall(isRightRule ? RIGHT : LEFT);
        !solveM->isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      neighbour.push_back(wall);
    } else if (int wall = currentSideWall(FRONT); 
        !solveM->isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      neighbour.push_back(wall);
    } else if (int wall = currentSideWall(isRightRule ? LEFT : RIGHT);
        !solveM->isWallExists(col, row, wall) &&
        !isNeighbourVisited(col, row, wall)) {
      neighbour.push_back(wall);
    } 
    
    if (!neighbour.empty()) {
      
      int nextDir = neighbour[0];
      //std::cout <<  "nextDir : " << nextDir << std::endl;
      
      switch (nextDir) {
        case NORTH: 
          solveM->stackMaze.push({col, row + 1});
          //solveM->setVisited(col, row + 1);
          currentDir = NORTHC;
          break;
        case WEST: 
          solveM->stackMaze.push({col - 1, row});
          //solveM->setVisited(col -1, row);
          currentDir = WESTC;
          break; 
        case EAST: 
          solveM->stackMaze.push({col + 1, row});
          //solveM->setVisited(col + 1, row);
          currentDir = EASTC;
          break; 
        case SOUTH: 
          solveM->stackMaze.push({col, row - 1});
          //solveM->setVisited(col, row - 1);
          currentDir = SOUTHC;
          break;  
      }
      stackDir.push(currentDir);
        
    } else {
      // No available directions so backtrack!
      currentDir = static_cast<compassDir>(-stackDir.top()); 
      stackDir.pop();
      solveM->stackMaze.pop();   
    }
    
  } else {
     onSolverUpdate = nullptr;
  }
}
 

void SolverMaze::wallFollower() {
  initAlgorithm();
  currentDir = NORTHC;
  stackDir.push(currentDir);
  solveM->stackMaze.push({solveM->getStartPosition()});
  solveM->update();
  // set handler - pointer to 
  onSolverUpdate = &SolverMaze::algWallFollower;
  solveM->setColorCurrent({0, 255, 0});
  isRightRule = rand() % 2;
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
    
    if (!solveM->isWallExists(col, row, EAST))
      queuePushDistances(col + 1, row, distance);
    if (!solveM->isWallExists(col, row, SOUTH))
      queuePushDistances(col, row - 1, distance);
    if (!solveM->isWallExists(col, row, WEST))
      queuePushDistances(col - 1, row, distance);
    if (!solveM->isWallExists(col, row, NORTH))
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

void SolverMaze::printMapDistances() {
  std::cout << std::dec;
  for (int i = solveM->mazeH - 1; i >= 0; --i) {
    for (int j = 0; j < solveM->mazeW; ++j) {
      std::cout << std::setw(4) <<
      static_cast<int>(mapDistances[solveM->cellN(j, i)]);
    }
    std::cout << std::endl;
  }
  std::cout << std::dec << "\n\n";
}

}
