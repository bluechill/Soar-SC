//
//  SVSObject.h
//  Application
//
//  Created by Alex Turner on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSObject_h
#define Application_SVSObject_h

#include "zenilib.h"

#include <vector>

class SVSObject
{
public:
	SVSObject(std::string name, const std::vector<Zeni::Point3f> verts, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale, SVSObject* parent = NULL);
	SVSObject(const SVSObject& source);
	
	SVSObject& operator=(const SVSObject& source);
	
	~SVSObject();
	
	std::string get_name() { return name; }
	SVSObject* get_parent() { return parent; }
	
	Zeni::Point3f get_center_position() { return center; }
	Zeni::Point3f get_scale() { return scale; }
	Zeni::Quaternion get_rotation() { return rotation; }
	
	void transform_position(Zeni::Point3f amount);
	void transform_scale(Zeni::Point3f amount);
	void transform_rotation(Zeni::Quaternion amount);
	
	void transform(Zeni::Matrix4f transformation_matrix);
	
	void render();
	void render_wireframe();
	
	bool is_a_group() { return is_group; }
	
	Zeni::Matrix4f get_transformation_matrix() { return transformation_matrix; }
	
	static const float global_scale;

private:
	static std::vector<std::vector<int> > verts_for_faces(const std::vector<Zeni::Point3f> verts);
	
	std::string name;
	SVSObject* parent;
	
	Zeni::Point3f center;
	Zeni::Point3f scale;
	Zeni::Quaternion rotation;
	
	Zeni::Matrix4f transformation_matrix;
	
	std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>* > triangles;
	std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>* > quadrilaterals;
	
	Zeni::Vertex_Buffer* buffer;
	
	SDL_mutex* mu;
	
	bool is_group;
};

#endif
