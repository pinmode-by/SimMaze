#include "maze.h"

namespace sm {
  
Maze::Maze (cv::Mat& base, cv::Size s, cv::Point p, int goal) {
  
  mazeW = s.width;
  mazeH = s.height;
  goalMaze = goal;
  sizeMaze = cv::Size(mazeW * (pathWidth + wallWidth) + wallWidth,
                      mazeH * (pathWidth + wallWidth) + wallWidth);
  matMaze =cv::Mat(base(cv::Rect(p, sizeMaze)));
  matMaze = cv::Scalar(50, 50, 50);
  // assign matrix mapMaze size: mazeH x mazeW 
	mapMaze.resize(mazeH);
  for (int i = 0; i < mazeH; ++i) {
    mapMaze[i].assign(mazeW, 0);
  }
  
  onCreate();

}

void Maze::onCreate() {
	
  // assign outside walls Maze
  for (int i = 0; i < mazeW; ++i) { 
    setWall(0, i, SOUTH);  
    setWall(mazeH - 1, i, NORTH);  
  }
  for (int i = 0; i < mazeH; ++i) { 
    setWall(i, 0, WEST); 
    setWall(i, mazeW-1, EAST); 
  }
  
  // start cell - assign EAST wall
  setWall(0, 0, EAST); 
  // target cell - assign WEST wall
  if (goalMaze) {
    setWall(mazeH - 1, mazeW - 1, WEST);
  } else {
    // target - center of Maze: 4 cells
  }
  
  drawStandMaze();
}

void Maze::drawStandMaze() {
  std::cout << "mazeH : " << mazeH << " mazeW : " << mazeW << std::endl;
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

void Maze::drawCell(int row, int col) {
	static const int cellWidth = pathWidth + wallWidth;
	static const int H = mazeH * cellWidth + wallWidth; 
	//static const int W = mazeW * cellWidth + wallWidth;
	
	const auto cX = col * cellWidth; 
	const auto cY = H - row * cellWidth - 1;

  if (mapMaze[row][col] & SOUTH) {
    rectangle(matMaze,
        cv::Point(cX + wallWidth, cY),
        cv::Point(cX + cellWidth - 1, cY - wallWidth + 1),
        cv::Scalar(0, 0, 255),
        cv::FILLED,
        cv::LINE_8 );
  }
  
  if (mapMaze[row][col] & WEST) {
    rectangle(matMaze,
        cv::Point(cX, cY - wallWidth),
        cv::Point(cX + wallWidth - 1, cY - cellWidth + 1),
        cv::Scalar( 0, 0, 255 ),
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
			 cv::Point(H - wallWidth , cY - cellWidth),
			 cv::Point(H - 1, cY - wallWidth),
			 cv::Scalar(0, 0, 255),
			 cv::FILLED,
			 cv::LINE_8 );
  }

}

void Maze::update() {
   
  // Draw Maze
  for (int x = 0; x < mazeH; ++x) { 
    for (int y = 0; y < mazeW; ++y) { 
      drawCell(x, y);
    }
  }
}

void Maze::clean() {
	for (int i = 0; i < mazeH; ++i) {
		for (int j = 0; j < mazeW; ++j) {
			mapMaze[i][j] = 0;
		}
  }
  matMaze = cv::Scalar(50, 50, 50);
	onCreate();
}

} // namespace
