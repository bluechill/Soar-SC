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
	SVSObject(std::string name, const std::vector<Zeni::Point3f> verts, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale);
	SVSObject(const SVSObject& source);
	
	SVSObject& operator=(const SVSObject& source);
	
	~SVSObject();
	
	std::string get_name() { return name; }
	
	Zeni::Point3f get_center_position() { return center; }
	Zeni::Point3f get_scale() { return scale; }
	Zeni::Quaternion get_rotation() { return rotation; }
	
	void transform_position(Zeni::Point3f amount);
	void transform_scale(Zeni::Point3f amount);
	void transform_rotation(Zeni::Quaternion amount);
	
	void transform(Zeni::Matrix4f transformation_matrix, Zeni::Point3f pos, Zeni::Quaternion rotation, Zeni::Point3f scale);
	
	void render();
	void render_wireframe();
	
	const bool is_a_group() { return is_group; }
	const std::vector<SVSObject*> getChildren() { return children; }
	bool addChild(SVSObject* object);
	
	Zeni::Matrix4f get_transformation_matrix() { return transformation_matrix; }
	
	static const float global_scale;

private:
	static std::vector<std::vector<int> > verts_for_faces(const std::vector<Zeni::Point3f> verts);
	
	std::string name;
	std::vector<SVSObject*> children;
	
	Zeni::Point3f center;
	Zeni::Point3f scale;
	Zeni::Quaternion rotation;
	
	Zeni::Matrix4f transformation_matrix;
	
	std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>* > triangles;
	std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>* > quadrilaterals;
	
	std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>* > wireframe_triangles;
	std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>* > wireframe_quadrilaterals;

	Zeni::Vertex_Buffer* buffer;
	Zeni::Vertex_Buffer* wireframe_buffer;
	
	bool is_group;
};

#endif
