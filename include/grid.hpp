#ifndef GRID_HPP
#define GRID_HPP

#include "ball.hpp"
#include "vector2.hpp"
#include <utility>
#include <vector>

using namespace std;

class Grid {
private:
  int rows;
  int cols;
  float cellSize;
  float originX, originY;
  vector<vector<Ball *>> cells;
  pair<int, int> GetCellCoords(Vector2D pos) const;
  int GetCellIndex(int row, int col) const;

public:
  Grid(int tableWidth, int tableHeight, float cellSize, float originX = 0.0f,
       float originY = 0.0f);

  void Clear();
  void Insert(Ball &ball);
  void GetPossibleCollisions(const Ball &ball,
                             vector<Ball *> &outCandidates) const;
};

#endif // !GRID_HPP
