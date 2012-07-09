//
//  Scene.h
//  Application
//
//  Created by Alex Turner on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSScene_h
#define Application_SVSScene_h

#include "zenilib.h"

#include <string>
#include <vector>

#include "SVSObject.h"

class SVSScene
{
public:
	SVSScene(std::string name);
	SVSScene(const SVSScene &source);
	
	SVSScene& operator=(const SVSScene &source);
	
	~SVSScene();
	
	bool add_object(std::string name, std::string parent, std::vector<Zeni::Point3f> vertices, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale);
		
	bool update_object(std::string name, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale);
	
	bool delete_object(std::string name);
	
	SVSObject* get_object_by_name(std::string name);
	
	std::string get_scene_name();
	
	void render();
	void render_wireframe();
	
private:
	std::string scene_name;
	
	std::vector<SVSObject*> objects;
};

#endif
