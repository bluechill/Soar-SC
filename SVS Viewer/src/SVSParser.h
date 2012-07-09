//
//  SVSParser.h
//  Application
//
//  Created by Alex Turner on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSParser_h
#define Application_SVSParser_h

#include "zenilib.h"

#include <vector>
#include <string>

#include "SVSScene.h"

class SVSParser
{
	SVSParser();
	~SVSParser();
	
public:
	static bool parse_add(std::vector<std::string> &parts, SVSScene &scene);
	static bool parse_change(std::vector<std::string> &parts, SVSScene &scene);
	static bool parse_delete(std::vector<std::string> &parts, SVSScene &scene);
	
private:
#ifdef _WIN32
#undef DELETE
#endif

	typedef enum {
		ADD = 2,
		CHANGE = 1,
		DELETE = 0
	} parse_type;
	
	static bool parse_mods(std::vector<std::string> &parts, SVSScene &scene, parse_type type);
	
	static bool parse_verts(std::vector<std::string> &parts, std::vector<Zeni::Point3f> &verts);
	static bool parse_vector3(std::vector<std::string> &parts, Zeni::Point3f &vert);
};

#endif
