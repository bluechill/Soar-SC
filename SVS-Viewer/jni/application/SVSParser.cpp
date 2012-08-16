//
//  SVSParser.cpp
//  Application
//
//  Created by Alex Turner on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>

#include <iostream>

bool SVSParser::parse_add(std::vector<std::string> &parts, SVSScene &scene)
{
	return parse_mods(parts, scene, ADD);
}

bool SVSParser::parse_change(std::vector<std::string> &parts, SVSScene &scene)
{
	return parse_mods(parts, scene, CHANGE);
}

bool SVSParser::parse_delete(std::vector<std::string> &parts, SVSScene &scene)
{
	if (parts.size() != 1)
		return false;
	
	if (!scene.delete_object(parts[0]))
		return false;
	
	return true;
}

bool SVSParser::parse_mods(std::vector<std::string> &parts, SVSScene &scene, parse_type type)
{
	std::vector<Zeni::Point3f> vertices;
	Zeni::Point3f position;
	Zeni::Quaternion rotation;
	Zeni::Point3f scale(1,1,1);
	
	if (parts.size() < 1) // Must have at least a name
		return false;
	
	std::string name = parts[0];
	std::string parent_name;

	if (parts.size() == 1 && name == "clear")
	{
		scene.clear_objects();
		return true;
	}
	
	if (parts.size() > 1  && type != CHANGE)
		parent_name = parts[1];
	
	int start = 1;
	
	if (parts.size() > 1 && type != CHANGE)
		start = 2;
	
	for (unsigned int i = start;i < parts.size();i++)
	{
		for (;i < parts.size();i++)
		{
			if (parts[i].size() == 1 && !isdigit(parts[i][0]))
				break;
		}
		
		if (i == parts.size())
			break;

		char character = parts[i][0];
		
		switch (character) {
			case 'p':
			{
				if (i + 3 >= parts.size())
				{
					std::cout << "Invalid position vector" << std::endl;
					return false;
				}
				
				std::vector<std::string> pos_as_string(parts.begin() + i + 1, parts.begin() + i + 4);
				if (!parse_vector3(pos_as_string, position))
					return false;
				
				break;
			}
			case 'r':
			{
				if (i + 3 >= parts.size())
				{
					std::cout << "Invalid rotation vector" << std::endl;
					return false;
				}
				
				Zeni::Point3f temp_rotation;
				
				std::vector<std::string> pos_as_string(parts.begin() + i + 1, parts.begin() + i + 4);
				if (!parse_vector3(pos_as_string, temp_rotation))
					return false;
				
				Zeni::Quaternion* temp_quaternion;
				
				temp_quaternion = new Zeni::Quaternion(temp_rotation.y, temp_rotation.x, temp_rotation.z);
				
				rotation = (*temp_quaternion);
				
				delete temp_quaternion;
				
				break;
			}
			case 's':
			{
				if (i + 3 >= parts.size())
				{
					std::cout << "Invalid scale vector" << std::endl;
					return false;
				}
				
				std::vector<std::string> pos_as_string(parts.begin() + i + 1, parts.begin() + i + 4);
				if (!parse_vector3(pos_as_string, scale))
					return false;
				
				break;
			}
			case 'v':
			{
				unsigned int j;
				for (j = (i+1);j < parts.size() && parts[j].size() == 1 && isdigit(parts[j][0]);j++);
				
				//j is now the next alphanumeric character, go up to j not *to*, *upto*!
				std::vector<std::string> subvector(parts.begin() + i + 1, parts.begin() + j);
				if (!parse_verts(subvector, vertices))
					return false;
				
				break;
			}
			default:
			{
				std::cout << "Unknown option: '" << character << "'" << std::endl;
				break;
			}
		}
	}
	
	if (type == CHANGE)
	{
		if (!scene.update_object(name, position, rotation, scale))
			return false;
		
		return true;
	}
	else if (type == ADD)
	{
		if (!scene.add_object(name, parent_name, vertices, position, rotation, scale))
			return false;
		
		return true;
	}
	else
		return false;
}

bool SVSParser::parse_verts(std::vector<std::string> &parts, std::vector<Zeni::Point3f> &verts)
{
	if (parts.size() % 3 != 0)
		return false;
	
	for (unsigned int i = 0;i < parts.size();i += 3)
	{
		Zeni::Point3f vert;
		
		std::vector<std::string> subparts(parts.begin() + i, parts.begin() + i + 3);
		if (!parse_vector3(subparts, vert))
			return false;
		
		verts.push_back(vert);
	}
	
	return true;
}

bool SVSParser::parse_vector3(std::vector<std::string> &parts, Zeni::Point3f &vert)
{
	if (parts.size() != 3)
		return false;
	
	std::string test1 = parts[0];
	std::string test2 = parts[1];
	std::string test3 = parts[2];
	
	if (test1.size() > 1 && test1[0] == '-')
		test1 = test1.substr(1);
	
	if (test2.size() > 1 && test2[0] == '-')
		test2 = test2.substr(1);
	
	if (test3.size() > 1 && test3[0] == '-')
		test3 = test3.substr(1);
	
	if (!(test1.find_first_not_of( "0123456789." ) == std::string::npos) ||
		!(test2.find_first_not_of( "0123456789." ) == std::string::npos) ||
		!(test3.find_first_not_of( "0123456789." ) == std::string::npos))
		return false;
	
	if (std::count(test1.begin(), test1.end(), '.') > 1 ||
		std::count(test2.begin(), test2.end(), '.') > 1 ||
		std::count(test3.begin(), test3.end(), '.') > 1)
		return false;
	
	char *end;
	vert.x = (float)strtod(parts[0].c_str(), &end);
	if (*end != '\0')
		return false;
	
	vert.y = (float)strtod(parts[1].c_str(), &end);
	if (*end != '\0')
		return false;
	
	vert.z = (float)strtod(parts[2].c_str(), &end);
	if (*end != '\0')
		return false;
	
	return true;
}
