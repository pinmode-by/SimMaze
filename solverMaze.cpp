
#include <memory>
#include "solverMaze.h"
#include "maze.h"
#include "utils.h"


namespace sm {
  
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
}

void SolverMaze::algFloodFill() {

}

void SolverMaze::advFloodFill() {

}

void SolverMaze::followWalls() {

}

void SolverMaze::randSearch() {

}
 

}
