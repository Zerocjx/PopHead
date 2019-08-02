#include <catch.hpp>

#include "AI/RandomPathAlgorithm.hpp"
#include <algorithm>

namespace ph {

ObstacleGrid getWalkableGrid11x11();
ObstacleGrid getGridWithObstacles11x11();

TEST_CASE("Random path finding algorithm works properly with no obstacles", "[AI][RandomPathAlgorithm]")
{
	auto obstacleGrid = getWalkableGrid11x11();
	RandomPathAlgorithm rpa(obstacleGrid, {6, 6});
	Path path = rpa.getRandomPath();
	
	REQUIRE((
		( path == Path{Direction::east, Direction::east} ) ||
		( path == Path{Direction::east, Direction::east, Direction::east} ) ||
		( path == Path{Direction::east, Direction::east, Direction::east, Direction::east} ) ||
		( path == Path{Direction::east, Direction::east, Direction::east, Direction::east, Direction::east} ) ||
		( path == Path{Direction::west, Direction::west} ) ||
		( path == Path{Direction::west, Direction::west, Direction::west}) ||
		( path == Path{Direction::west, Direction::west, Direction::west, Direction::west}) ||
		( path == Path{Direction::west, Direction::west, Direction::west, Direction::west, Direction::west}) ||
		( path == Path{Direction::north, Direction::north} ) ||
		( path == Path{Direction::north, Direction::north, Direction::north} ) ||
		( path == Path{Direction::north, Direction::north, Direction::north, Direction::north} ) ||
		( path == Path{Direction::north, Direction::north, Direction::north, Direction::north, Direction::north} ) ||
		( path == Path{Direction::south, Direction::south} ) ||
		( path == Path{Direction::south, Direction::south, Direction::south} ) ||
		( path == Path{Direction::south, Direction::south, Direction::south, Direction::south} ) ||
		( path == Path{Direction::south, Direction::south, Direction::south, Direction::south, Direction::south} )
	));
}

ObstacleGrid getWalkableGrid11x11()
{
	std::vector<bool> column;
	column.resize(11);
	std::fill(column.begin(), column.end(), false);
	ObstacleGrid grid;
	grid.resize(11);
	std::fill(grid.begin(), grid.end(), column);
	return grid;
}

TEST_CASE("Random path finding algorithm works properly with obstacles", "[AI][RandomPathAlgorithm]")
{
	for(int i = 0; i < 10; ++i) {
		auto obstacleGrid = getGridWithObstacles11x11();
		RandomPathAlgorithm rpa(obstacleGrid, {5, 5});
		Path path = rpa.getRandomPath();
		
		CHECK((
			(path == Path{Direction::east, Direction::east}) ||
			(path == Path{Direction::east, Direction::east, Direction::east}) ||
			(path == Path{Direction::east, Direction::east, Direction::east, Direction::east}) ||
			(path == Path{Direction::west, Direction::west}) ||
			(path == Path{Direction::west, Direction::west, Direction::west}) ||
			(path == Path{Direction::north, Direction::north})
		));
	}
}

ObstacleGrid getGridWithObstacles11x11()
{
	std::vector<bool> column;
	column.resize(11);
	std::fill(column.begin(), column.end(), false);
	ObstacleGrid grid;
	grid.resize(11);
	std::fill(grid.begin(), grid.end(), column);
	grid[10][5] = true;
	grid[1][5] = true;
	grid[5][2] = true;
	grid[5][7] = true;
	return grid;
}

}