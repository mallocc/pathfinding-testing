#pragma once

#include "mesh.h"
#include "glm.h"
#include <stdio.h>
#include <vector>


struct WalkMap
{
	std::vector<glm::vec2> nodes;
	std::vector<glm::vec2> tris;
	Mesh mesh;

	WalkMap()
	{
	}

	WalkMap(char * walkmap, Mesh mesh)
	{
		this->mesh = mesh;
	}

	WalkMap * add_node(glm::vec2 a)
	{
		nodes.push_back(a);
		return this;
	}
	WalkMap * add_tri_point(glm::vec2 a)
	{
		tris.push_back(a);
		return this;
	}

	bool is_left(glm::vec2 a, glm::vec2 b, glm::vec2 p)
	{
		// cross product
		return (b.x - a.x)*(p.y - a.y) > (b.y - a.y)*(p.x - a.x);
	}

	bool is_inside_tri(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p)
	{
		// check each side
		if (is_left(a, b, p))
			return false;
		if (is_left(b, c, p))
			return false;
		if (is_left(c, a, p))
			return false;
		return true;
	}
	bool is_outside(glm::vec2 a)
	{
		// if point is within at least one tri, 
		// then it is outside the walkable map
		for (int i = 0; i < tris.size(); i += 3)
			if (is_inside_tri(tris[i], tris[i + 1], tris[i + 2], a))
				return true;
		return false;
	}

	void extrapolate_nodes()
	{
		// extrapolates the position of the walkable border,
		// creates a 'rougher' board to stop walking through corners
		for (int i = 0; i < nodes.size()-1; i += 2)
		{
			glm::vec2 d = nodes[i + 1] - nodes[i];
			d = glm::normalize(d) * 1.0f;
			nodes[i] -= d;
			nodes[i+1] += d;
		}
	}

};

struct PathState
{
	glm::vec2 state;
	float g = 0, f = 0, hash = 0.0f;
	std::vector<glm::vec2> current_path;
	std::vector<glm::vec2> nodes_left;
	int node_id = -1;

	PathState(glm::vec2 state)
	{
		this->state = state;
	}

	PathState(PathState * s)
	{
		this->state = s->state;
		this->g = s->g;
		this->f = s->f;
		this->current_path = s->current_path;
		this->hash = s->hash;
		this->node_id = s->node_id;
		this->nodes_left = s->nodes_left;
	}
};
inline int	get_hash(PathState * s)
{
	float hash = glm::length(s->state);
	for (int i = 0; i < s->current_path.size(); ++i)
		hash += glm::length(s->current_path[i]) * i * 5.0f + glm::length(s->current_path[i]) * 31.0f;
	return hash;
}
bool equal_state(PathState s1, PathState s2)
{
	if (s1.state == s2.state)
		return true;
	return false;
}
bool intersect(glm::vec2 a1, glm::vec2 a2, glm::vec2 a3, glm::vec2 a4)
{
	float t1 = ((a1.x - a3.x) * (a3.y - a4.y) - (a1.y - a3.y)*(a3.x - a4.x)) / ((a1.x - a2.x) * (a3.y - a4.y) - (a1.y - a2.y)*(a3.x - a4.x));
	float t2 = -((a1.x - a2.x) * (a1.y - a3.y) - (a1.y - a2.y)*(a1.x - a3.x)) / ((a1.x - a2.x) * (a3.y - a4.y) - (a1.y - a2.y)*(a3.x - a4.x));
	return t1 > 0.0f && t1 < 1.0f && t2 > 0.0f && t2 < 1.0f;
}
bool intersect_eq(glm::vec2 a1, glm::vec2 a2, glm::vec2 a3, glm::vec2 a4)
{
	float t1 = ((a1.x - a3.x) * (a3.y - a4.y) - (a1.y - a3.y)*(a3.x - a4.x)) / ((a1.x - a2.x) * (a3.y - a4.y) - (a1.y - a2.y)*(a3.x - a4.x));
	float t2 = -((a1.x - a2.x) * (a1.y - a3.y) - (a1.y - a2.y)*(a1.x - a3.x)) / ((a1.x - a2.x) * (a3.y - a4.y) - (a1.y - a2.y)*(a3.x - a4.x));
	return t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f;
}
bool move_state(WalkMap map, PathState * s, glm::vec2 new_pos)
{
	for (int i = 1; i < map.nodes.size(); i+=2)
		if (intersect(s->state, new_pos, map.nodes[i - 1], map.nodes[i]))
			return false;

	for (int i = 0; i < s->current_path.size(); i++)
		if (s->current_path[i] == new_pos)
			return false;

	s->state = new_pos;
	s->g += glm::length(new_pos - s->current_path[s->current_path.size() - 1]);
	s->current_path.push_back(s->state);

	return true;
}
std::vector<glm::vec2> get_walkmap_path(glm::vec2 current_pos, glm::vec2 destination_pos, WalkMap map)
{	
	std::vector<PathState> 
		open, 
		closed;

	PathState
		current_state(current_pos);
	current_state.current_path.push_back(current_pos);
	current_state.nodes_left = map.nodes;
	PathState
		goal_state(destination_pos),
		copy_state = current_state;

	// local variables
	int
		skip = false,
		expanded = 0,
		no_route = true,
		spent_node = -1;

	while (!equal_state(current_state, goal_state))
	{
		no_route = true;

		// check goal directly
		skip = false;
		copy_state = PathState(&current_state);
		// check if move is legal
		skip = !move_state(map, &copy_state, goal_state.state);
		// break if path can be finished at goal
		if (!skip)
		{
			current_state = copy_state;
			break;
		}

		// check every node
		for (int i = 0; i < current_state.nodes_left.size(); i++)
		{
			// dont check the node that is the same as the current position
			if (current_state.state != current_state.nodes_left[i])
			{
				skip = false;
				copy_state = PathState(&current_state);
				// check if move is legal
				skip = !move_state(map, &copy_state, copy_state.nodes_left[i]);

				if (!skip)
				{
					copy_state.node_id = i;
					if (copy_state.nodes_left.size() > 0 && copy_state.node_id >= 0)
						copy_state.nodes_left.erase(copy_state.nodes_left.begin() + copy_state.node_id);

					// find the cost
					copy_state.f = copy_state.g + glm::length(copy_state.state - goal_state.state);
					// hash the state for effiecent searching
					copy_state.hash = get_hash(&copy_state);
					// skip if expanded state is in the open set and has higher cost
					for (int j = 0; j < open.size(); ++j)
						if (copy_state.hash == open[j].hash && copy_state.f > open[j].f)
							skip = true;
					if (!skip)
					{
						// skip if expanded state is in the closed set and has higher cost
						for (int j = 0; j < closed.size(); ++j)
							if (copy_state.hash == closed[j].hash && copy_state.f > closed[j].f)
								skip = true;
						// if not add to open set
						if (!skip)
						{
							// there is at least one move to the goal for this round
							no_route = false;
							// remove node used from the list of nodes left							
							open.push_back(&copy_state);
						}
					}
				}
			}
		}

		// find the least cost state for this round
		int minId = -1;
		float minValue = INT_MAX;
		for (int i = 0; i < open.size(); ++i)
			if (open[i].f < minValue)
			{
				minId = i;
				minValue = open[i].f;
			}

		// use the least cost state so far for the next round
		if (open.size() > 0)
		{
			current_state = PathState(&open[minId]);
			closed.push_back(current_state);
			open.erase(open.begin() + minId);
			expanded++;
			//printf("expanded = %i\n", expanded);
		}
		else
		{
			current_state.current_path = std::vector<glm::vec2>();
			current_state.current_path.push_back(current_pos);
			break;
		}	
	}
	open = closed = std::vector<PathState>();

	return current_state.current_path;
}



typedef std::vector<glm::vec2> polygon;

std::vector<glm::vec2> ptf(polygon p)
{
	std::vector<polygon> polygons_left;
	std::vector<glm::vec2> tris;

	polygons_left.push_back(p);
	
	while (!polygons_left.empty())
	{
		polygon current_poly = polygons_left.back();
		polygons_left.pop_back();
		
		int offset = 1;
		polygon new_poly;

		int point_count = 0;

		bool start_poly = false;
		bool old_start_poly = false;

		bool no_tris = true;

		int poly_length = 0;

		while (no_tris)
		{
			for (int i = offset; i < current_poly.size() + offset; ++i)
			{
				int mod_i = i % current_poly.size();
				old_start_poly = start_poly;
				start_poly = false;
				for (int j = 0; j < current_poly.size(); ++j)
					if (intersect(current_poly[offset], current_poly[mod_i], current_poly[j], current_poly[(j + 1) % current_poly.size()]))
					{
						start_poly = true;
						break;
					}

				if (!start_poly)
				{
					point_count++;
					if (old_start_poly)
					{
						for (int j = offset; j <= offset + poly_length+2; j++)
							new_poly.push_back(current_poly[j % current_poly.size()]);
						//new_poly.push_back(current_poly[offset]);
						polygons_left.push_back(new_poly);
						new_poly.erase(new_poly.begin() + new_poly.size() - 1);
					}
					poly_length = 0;
				}
				else
				{
					poly_length++;
					point_count = 1;
				}

				if (point_count == 3 && offset!= mod_i)
				{
					no_tris = false;
					tris.push_back(current_poly[offset]);
					tris.push_back(current_poly[mod_i - 1]);
					tris.push_back(current_poly[mod_i]);
					point_count = 1;
				}
			}
			offset++;
		}
	}

	return tris;
}

std::vector<glm::vec2>			polygon_triangulation_fan(std::vector<glm::vec2> loop)
{
	std::vector<int> lines;
	std::vector<int> tris;
	std::vector<int> available;
	for (int i = 0; i < loop.size(); ++i)
		available.push_back(i);

	int c = 1;
	int last_pivot = 0;
	while (available.size() > 0)
	{
		int loc = -1;
		bool skip = false;
		// find an free node add to list
		for (int i = 0; i < available.size(); ++i)
			if (available[i] == c)
			{
				loc = i;
				// check for intersections
				bool intersects = false;
				for (int j = 0; j < loop.size() - 1; ++j)
					if (intersect(loop[available[last_pivot]], loop[c], loop[j], loop[j + 1]))
					{
						intersects = true;
						break;
					}
				// if no intersections, add to list
				if (!intersects)
				{
					lines.push_back(last_pivot);
					lines.push_back(c);
					break;
				}
				else
				{
					c--;
					last_pivot = c;
					// make add some tris
					for (int j = 0; j < lines.size()-2; j+=2)
					{
						tris.push_back(lines[i    ]);
						tris.push_back(lines[i + 1]);
						tris.push_back(lines[i + 3]);
					}
					lines = std::vector<int>();
					skip = true;
				}

				break;
			}

		if(!skip)
			if (loc == -1)
			{
				break;
			}
			else
			{
				available.erase(available.begin() + loc);
			}

		c++;
	}

	for (int j = 0; j < lines.size() - 2; j += 2)
	{
		tris.push_back(lines[j]);
		tris.push_back(lines[j + 1]);
		tris.push_back(lines[j + 3]);
	}

	std::vector<glm::vec2> output;
	for (int i = 0; i < tris.size(); ++i)
		output.push_back(loop[tris[i]]);

	return output;
}