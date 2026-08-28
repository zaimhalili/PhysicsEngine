#include "../include/grid.hpp"

Grid::Grid(int tableWidth, int tableHeight, float cellSize, float originX,
           float originY)
    : cellSize(cellSize), originX(originX), originY(originY) {
  cols = std::ceil(tableWidth / cellSize); // round up
  rows = std::ceil(tableHeight / cellSize);

  cells.resize(cols * rows);
  for (auto &bucket : cells) {
    bucket.reserve(8);
  }
}

std::pair<int, int> Grid::GetCellCoords(Vector2D pos) const {
  float localX = pos.x - originX;
  float localY = pos.y - originY;

  int col = static_cast<int>(std::floor(localX / cellSize));
  int row = static_cast<int>(std::floor(localY / cellSize));

  return {col, row};
}

int Grid::GetCellIndex(int col, int row) const { return row * cols + col; }

void Grid::Clear() {
  for (auto &bucket : cells) {
    bucket.clear();
  }
}

void Grid::Insert(Ball &ball) {
  auto [col, row] = GetCellCoords(ball.position);
  if (col < 0 || col >= cols || row < 0 || row >= rows) {
    return;
  }

  cells[GetCellIndex(col, row)].push_back(&ball);
}

void Grid::GetPossibleCollisions(const Ball &ball,
                                 vector<Ball *> &outCandidates) const {
  outCandidates.clear();
  auto [centerCol, centerRow] = GetCellCoords(ball.position);

  for (int dR = -1; dR <= 1; ++dR) {
    int targetRow = centerRow + dR;
    if (targetRow < 0 || targetRow >= rows)
      continue;

    for (int dC = -1; dC <= 1; ++dC) {
      int targetCol = centerCol + dC;
      if (targetCol < 0 || targetCol >= cols)
        continue;

      int targetIndex = GetCellIndex(targetCol, targetRow);
      for (Ball *other : cells[targetIndex]) {
        if (other != &ball) {
          outCandidates.push_back(other);
        }
      }
    }
  }
}