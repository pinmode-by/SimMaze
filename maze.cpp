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
  onCreate();
  
}

void Maze::onCreate() {
	// assign matrix mapMaze mazeH x mazeW 
	mapMaze.resize(mazeH);
  for (int i = 0; i < mazeH; ++i) {
    mapMaze[i].assign(mazeW, 0);
  }
  
  // assign outside walls Maze
  for (int i = 0; i < mazeW; ++i) { 
    mapMaze[0][i] |= SOUTH;
    mapMaze[mazeH-1][i] |= NORTH;
  }
  for (int i = 0; i < mazeH; ++i) { 
    mapMaze[i][0] |= WEST;
    mapMaze[i][mazeW-1] |= EAST;
  }
  
  // start cell - assign EAST wall
  mapMaze[0][0] |= EAST;
  mapMaze[0][1] |= WEST;
  // target cell - assign WEST wall
  if (goalMaze) {
		mapMaze[mazeH-1][mazeW-1] |= WEST;
		mapMaze[mazeH-1][mazeW-2] |= EAST;
	} else {
		// target - center of Maze: 4 cells
	}
	
	// for test 
	mapMaze[2][3] |= WEST;
	mapMaze[3][2] |= SOUTH;
	mapMaze[1][2] |= WEST;
}

void Maze::drawCell(int row, int col) {
	static const int cellWidth = pathWidth + wallWidth;
	static const int H = mazeH * cellWidth + wallWidth; 
	static const int W = mazeW * cellWidth + wallWidth;
	
	const auto cX = col * cellWidth; 
	const auto cY = H - row * cellWidth - 1;
		
  if (mapMaze[row][col] & SOUTH) {
    rectangle(matMaze,
			 cv::Point(cX, cY),
			 cv::Point(cX + cellWidth - 1, cY - wallWidth),
			 cv::Scalar(0, 0, 255),
			 cv::FILLED,
			 cv::LINE_8 );
  }
  
  if (mapMaze[row][col] & WEST) {
    rectangle(matMaze,
         cv::Point(cX, cY),
         cv::Point(cX + wallWidth -1, cY - cellWidth + 1),
         cv::Scalar( 0, 0, 255 ),
         cv::FILLED,
         cv::LINE_8 );
  }
  
  if (row == mazeH - 1) {
		if (mapMaze[row][col] & NORTH) {
    rectangle(matMaze,
			 cv::Point(cX, 0),
			 cv::Point(cX + cellWidth - 1, wallWidth - 1),
			 cv::Scalar(0, 0, 255),
			 cv::FILLED,
			 cv::LINE_8 );
		}
	} 
	 
  if (col == mazeW - 1) {
		if (mapMaze[row][col] & EAST) {
    rectangle(matMaze,
			 cv::Point(H - wallWidth , cY - cellWidth + 1 ),
			 cv::Point(H - 1, cY),
			 cv::Scalar(0, 0, 255),
			 cv::FILLED,
			 cv::LINE_8 );
		}
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

} // namespace
