#include "obstacleGrid.hpp"

#include "Logs/logs.hpp"

namespace ph {

ObstacleGrid::ObstacleGrid(size_t columns, size_t rows)
	: mColumns(columns)
	, mRows(rows)
{
	mObstacles.resize(mColumns * mRows, false);
}

ObstacleGrid::ObstacleGrid(std::vector<bool> obstacles, size_t columns, size_t rows)
	: mColumns(columns)
	, mRows(rows)
{
	PH_ASSERT_ERROR(obstacles.size() == rows * columns, "rows=" + std::to_string(rows) + ",columns=" + std::to_string(columns) +
		" don't fit vector size");
	mObstacles = std::move(obstacles);
}


ObstacleGrid::ObstacleGrid(std::vector<std::vector<bool>> obstacles, size_t columns, size_t rows)
	: mColumns(columns)
	, mRows(rows)
{
	PH_ASSERT_ERROR(obstacles.size() == columns, "columns=" + std::to_string(columns) + " don't fit vector size");

	mObstacles.resize(mColumns * mRows, false);

	for (auto& column : obstacles)
	{
		PH_ASSERT_ERROR(column.size() == rows, "rows=" + std::to_string(rows) + " don't fit vector size");
	}

	for (size_t column = 0; column < mColumns; ++column)
		for (size_t row = 0; row < mRows; ++row)
			mObstacles.at(internalIndex(column, row)) = obstacles.at(column).at(row);
}

void ObstacleGrid::registerObstacle(size_t column, size_t row)
{
	mObstacles.at(internalIndex(column, row)) = true;
}

bool ObstacleGrid::isObstacle(size_t column, size_t row) const
{
	return mObstacles.at(internalIndex(column, row));
}

size_t ObstacleGrid::getColumnsCount() const
{
	return mColumns;
}

size_t ObstacleGrid::getRowsCount() const
{
	return mRows;
}

size_t ObstacleGrid::internalIndex(size_t column, size_t row) const
{
	return column + row * mColumns;
}

}
