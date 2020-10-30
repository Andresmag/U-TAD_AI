#include <stdafx.h>

#include "pathfinder.h"

bool compare_values(Node* first, Node* second)
{
	return first->Fvalue < second->Fvalue;
}

Pathfinder::Pathfinder() : MOAIEntity2D()
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	distance = distance2 = numeric_limits<float>::max();
}

Pathfinder::~Pathfinder()
{
	m_Path.clear();
	m_OpenNodes.clear();
	m_ClosedNodes.clear();
}

void Pathfinder::UpdatePath()
{
	// Clear current path
	m_Path.clear();

	// Clear lists
	m_OpenNodes.clear();
	m_ClosedNodes.clear();

	// Add the first node to the open list free
	m_OpenNodes.push_back(new Node(0, Heuristic(m_StartSquare.mX, m_StartSquare.mY), m_StartSquare));

	PathfindStep();
}


void Pathfinder::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	for (uint currentRow = 0; currentRow < m_Grid.size(); ++currentRow)
	{
	    for (uint currentCol = 0; currentCol < m_Grid[currentRow].size(); ++currentCol)
		{
			if (currentRow == m_StartSquare.mY && currentCol == m_StartSquare.mX) // Start square
			{
				gfxDevice.SetPenColor(1.0f, 1.0f, 0.0f, 0.5f); // Yellow
			}
			else if (currentRow == m_GoalSquare.mY && currentCol == m_GoalSquare.mX) // Goal square
			{
				gfxDevice.SetPenColor(1.0f, 0.0f, 1.0f, 0.5f); // Purple
			}
			else
			{
                switch (m_Grid[currentRow][currentCol])
                {
				case 'A':
					gfxDevice.SetPenColor(0.0f, 0.0f, 0.5f, 0.5f); // blue
					break;
				case 'B':
					gfxDevice.SetPenColor(0.0f, 0.5f, 0.0f, 0.5f); // green
					break;
				case 'C':
					gfxDevice.SetPenColor(0.9f, 0.9f, 0.9f, 0.5f); // pale white
					break;
				default:
					gfxDevice.SetPenColor(0.0f, 0.0f, 0.0f, 0.5f); // black
					break;
                }
			}

			MOAIDraw::DrawRectFill(-512.f + SQUARE_WIDTH * currentCol,- 384.f + SQUARE_HEIGHT * currentRow, -512.f + SQUARE_WIDTH * currentCol + SQUARE_WIDTH, -384.f + SQUARE_HEIGHT * currentRow + SQUARE_HEIGHT);
		}
	}

    // Draw grid
	USRect grid;
	grid.mXMin = -512;
	grid.mXMax = 512;
	grid.mYMin = -384;
	grid.mYMax = 384;
	gfxDevice.SetPenColor(0.0f, 0.0f, 0.0f, 0.5f);
	MOAIDraw::DrawGrid(grid, 32, 24);

	// Draw Open Nodes
	gfxDevice.SetPenColor(0.0f, 1.0f, 1.0f, 0.1f);
	for (auto& node : m_OpenNodes)
	{
		MOAIDraw::DrawRectFill(-512.f + SQUARE_WIDTH * node->square.mX, -384.f + SQUARE_HEIGHT * node->square.mY, -512.f + SQUARE_WIDTH * node->square.mX + SQUARE_WIDTH, -384.f + SQUARE_HEIGHT * node->square.mY + SQUARE_HEIGHT);
	}

	// Draw Closed Nodes
	gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.1f);
	for (auto& node : m_ClosedNodes)
	{
		MOAIDraw::DrawRectFill(-512.f + SQUARE_WIDTH * node->square.mX, -384.f + SQUARE_HEIGHT * node->square.mY, -512.f + SQUARE_WIDTH * node->square.mX + SQUARE_WIDTH, -384.f + SQUARE_HEIGHT * node->square.mY + SQUARE_HEIGHT);
	}

	// Draw path
	gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
	for (auto it = m_Path.begin(); it != m_Path.end(); ++it)
	{
		float horizontalCenter, verticalCenter, fatherHorizontalCenter, fatherVerticalCenter;
		if ((*it)->father)
		{
			horizontalCenter = ((-512.f + SQUARE_WIDTH * (*it)->square.mX) * 2 + SQUARE_WIDTH) / 2;
			verticalCenter = ((-384.f + SQUARE_HEIGHT * (*it)->square.mY) * 2 + SQUARE_HEIGHT) / 2;

			fatherHorizontalCenter = ((-512.f + SQUARE_WIDTH * (*it)->father->square.mX) * 2 + SQUARE_WIDTH) / 2;
			fatherVerticalCenter = ((-384.f + SQUARE_HEIGHT * (*it)->father->square.mY) * 2 + SQUARE_HEIGHT) / 2;

			MOAIDraw::DrawLine(USVec2D(horizontalCenter, verticalCenter), USVec2D(fatherHorizontalCenter, fatherVerticalCenter));
		}
	}
}

bool Pathfinder::PathfindStep()
{
	if (m_Path.empty())
	{
        // returns true if pathfinding process finished
	    m_OpenNodes.sort(compare_values); // sort list to make the first element the lowest f value
	    Node* currentExploringNode = m_OpenNodes.front(); // pick the first element
	    m_OpenNodes.pop_front(); // and delete it from the list

	    if (currentExploringNode->square.mX == m_GoalSquare.mX && currentExploringNode->square.mY == m_GoalSquare.mY) // we are exploring the goal node
	    {
		    BuildPath(currentExploringNode);
		    return true;
	    }

	    // if we are not in the goal node we have to expand the node
	    for (int rowIdx = currentExploringNode->square.mY - 1; rowIdx <= currentExploringNode->square.mY + 1; rowIdx++)
	    {
		    for (int colIdx = currentExploringNode->square.mX - 1; colIdx <= currentExploringNode->square.mX + 1; colIdx++)
		    {
			    if (rowIdx != currentExploringNode->square.mY || colIdx != currentExploringNode->square.mX) // Check it isn't the same node
			    {
				    if ((rowIdx >= 0 && rowIdx < 768 / SQUARE_HEIGHT) && (colIdx >= 0 && colIdx < 1024 / SQUARE_WIDTH)) // Check if the square is valid
				    {
					    if (GetSquareValue(m_Grid[rowIdx][colIdx]) >= 0) // Check if the square is passable
					    {
						    if (!CheckInClosedList(colIdx, rowIdx)) // if the neighbour node to explore is already in the closed list we can skip it
						    {
							    if (!CheckInOpenListAndUpdate(colIdx, rowIdx, currentExploringNode)) // if the neighbour node to explore isn't in the open list we add it
							    {
								    float g = currentExploringNode->Gvalue + GetSquareValue(m_Grid[rowIdx][colIdx]);
								    float h = Heuristic(colIdx, rowIdx);
								    m_OpenNodes.push_back(new Node(g, h, USVec2D(colIdx, rowIdx), currentExploringNode));
							    }
						    }
					    }
				    }
			    }
		    }
	    }

	    m_ClosedNodes.push_back(currentExploringNode);
	}
    return false;
}

void Pathfinder::BuildPath(Node* pathNode)
{
	// Set new path
	Node* currentNode = pathNode;
	while(currentNode->father)
	{
		m_Path.push_back(currentNode);
		currentNode = currentNode->father;
	}
}

bool Pathfinder::CheckInClosedList(float x, float y)	// Square position
{
	for (auto& node : m_ClosedNodes)
	{
		if (node->square.mX == x && node->square.mY == y)
			return true;
	}

	return false;
}

bool Pathfinder::CheckInOpenListAndUpdate(float x, float y, Node* newFather)
{
	for (auto& node : m_OpenNodes)
	{
		if (node->square.mX == x && node->square.mY == y)
		{
		    // Check values to know if we need to update the open list because it's a shorter path
		    if (newFather->Gvalue + GetSquareValue(m_Grid[y][x]) < node->Gvalue)	// y = rows, x = cols
		    {
				node->Gvalue = newFather->Gvalue + GetSquareValue(m_Grid[y][x]);	// Update Gvalue
				node->Hvalue = Heuristic(x, y); // Update Hvalue
				node->Fvalue = node->Gvalue + node->Hvalue; // Update Fvalue manually
				node->father = newFather; // Update father
		    }
			return true;
		}
	}

	return false;
}

void Pathfinder::ReadGrid(const char* filepath)
{
	string line;
	ifstream file(filepath);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			vector<char> newRow;
			for (auto& c : line)
			{
				newRow.push_back(toupper(c));
			}
			m_Grid.push_back(newRow);
		}
		file.close();
	}
	else
	{
		printf("ERROR: couldn't read grid file\n");
	}
}

void Pathfinder::ReadGridValues(const char* filepath)
{
	string line;
	ifstream file(filepath);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			char zone = line[0];
			int delimiterIdx = line.find("=");
			float value = stof(line.substr(++delimiterIdx));
			m_GridValues.insert(make_pair(toupper(zone), value));

			// Get lowest value
			if (value >= 0 && value < distance)
			{
				distance = value;
			}
		}
		distance2 = distance + 0.5f;

		file.close();
	}
	else
	{
		printf("ERROR: couldn't read grid values file\n");
	}
}

float Pathfinder::GetSquareValue(char& zone)
{
	try
	{
		return m_GridValues.at(zone);
	}
	catch (const out_of_range& oor)
    {
		printf("Out of Range error: %s\n", oor.what());
	}
	return -1;
}

float Pathfinder::Heuristic(float x, float y)
{
	// if distance = distance2 = 1 then we are using Chebyshev distance
    float dx = abs(x - m_GoalSquare.mX);
	float dy = abs(y - m_GoalSquare.mY);
	return distance * (dx + dy) + (distance2 - 2 * distance) * MIN(dx, dy);
}


//lua configuration ----------------------------------------------------------------//
void Pathfinder::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity::RegisterLuaFuncs(state);

	luaL_Reg regTable [] = {
		{ "setStartPosition",		_setStartPosition},
		{ "setEndPosition",		_setEndPosition},
        { "pathfindStep",			_pathfindStep},
	    {"setGrid",				_setGrid},
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Pathfinder::_setStartPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetStartPosition(pX, pY);
	return 0;
}

int Pathfinder::_setEndPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetEndPosition(pX, pY);
	return 0;
}

int Pathfinder::_pathfindStep(lua_State* L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    self->PathfindStep();
    return 0;
}

int Pathfinder::_setGrid(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	self->ReadGrid("../sample/grid.txt"); // Specify grid txt
	self->ReadGridValues("../sample/grid_values.txt"); // Specify grid value txt
	return 0;
}
