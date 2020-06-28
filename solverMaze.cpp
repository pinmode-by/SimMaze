
#include <memory>
#include <iomanip>
#include "solverMaze.h"
#include "maze.h"
#include "utils.h"


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

void SolverMaze::floodFill() {
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
  if (solveM->goalMaze == GoalDiagonal) {
    solveM->clearWall(solveM->mazeW - 1,  solveM->mazeH - 1, SOUTH);
    solveM->setWall(solveM->mazeW - 1,  solveM->mazeH - 1, WEST);
  }
  // set handler - pointer to 
  onSolverUpdate = &SolverMaze::algFloodFill;
  setDistances();
  printMapDistances();
}

void SolverMaze::algFloodFill() {

}

void SolverMaze::advFloodFill() {

}

void SolverMaze::wallFollower() {

}

void SolverMaze::randSearch() {

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
