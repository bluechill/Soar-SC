//
//  SVSObject.cpp
//  Application
//
//  Created by Alex Turner on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSObject.h"

#include <fstream>
#include <sstream>

const float SVSObject::global_scale = 25.0f;

SVSObject::SVSObject(std::string name, const std::vector<Zeni::Point3f> verts, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale)
{	
	using namespace Zeni;

	this->name = name;

	Zeni::String color_str = "magenta";

	if (name.find("Enemy") != Zeni::String::npos)
		color_str = "red";
	else if (name.find("Terrain") != Zeni::String::npos)
		color_str = "blue";
	else if (name.find("Resource") != Zeni::String::npos)
		color_str = "green";
	else if (name.find("Fog") != Zeni::String::npos)
		color_str = "violet";
	
	transformation_matrix = Matrix4f::Translate(position) * Matrix4f::Rotate(rotation) * Matrix4f::Scale(scale);

	if (verts.size() == 0)
	{
		is_group = true;
				
		this->center = position;
		
		this->scale = scale;
		this->rotation = rotation;
		
		return;
	}
	
	this->buffer = new Zeni::Vertex_Buffer();
	this->wireframe_buffer = new Zeni::Vertex_Buffer();
	
	std::vector<std::vector<int> > faces = verts_for_faces(verts);
	
	int i = 0;
	
	for (std::vector<std::vector<int> >::iterator it = faces.begin();it != faces.end();++it)
	{
		++i;
		
		if (it->size() == 3)
		{
			//Triangle
			Zeni::Color color = Zeni::get_Colors()[color_str];
			
			Zeni::Point3f position1 = verts.at(it->at(0));
			Zeni::Point3f position2 = verts.at(it->at(1));
			Zeni::Point3f position3 = verts.at(it->at(2));
						
			Zeni::Vertex3f_Color vert1(position1, color);
			Zeni::Vertex3f_Color vert2(position2, color);
			Zeni::Vertex3f_Color vert3(position3, color);
			
			Zeni::Triangle<Zeni::Vertex3f_Color> *triag = new Zeni::Triangle<Zeni::Vertex3f_Color>(vert1, vert2, vert3);
			triag->give_Material(new Material());
			triangles.push_back(triag);
			buffer->fax_Triangle(triag);

			Zeni::Triangle<Zeni::Vertex3f_Color> *wireframe_triag = new Zeni::Triangle<Zeni::Vertex3f_Color>(*triag);
			color = Zeni::get_Colors()["white"];

			wireframe_triag->a.set_Color(color);
			wireframe_triag->b.set_Color(color);
			wireframe_triag->c.set_Color(color);

			wireframe_triag->give_Material(new Material());
			wireframe_triangles.push_back(wireframe_triag);
			wireframe_buffer->fax_Triangle(wireframe_triag);
		}
		else if (it->size() == 4)
		{
			//Quad
			Zeni::Color color = Zeni::get_Colors()[color_str];
			
			Zeni::Point3f position1 = verts.at(it->at(0));
			Zeni::Point3f position2 = verts.at(it->at(1));
			Zeni::Point3f position3 = verts.at(it->at(2));
			Zeni::Point3f position4 = verts.at(it->at(3));
			
			Zeni::Vertex3f_Color vert1(position1, color);
			Zeni::Vertex3f_Color vert2(position2, color);
			Zeni::Vertex3f_Color vert3(position3, color);
			Zeni::Vertex3f_Color vert4(position4, color);
						
			Zeni::Quadrilateral<Zeni::Vertex3f_Color> *quad = new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(vert1, vert2, vert3, vert4);
			quad->give_Material(new Material());
			quadrilaterals.push_back(quad);
			buffer->fax_Quadrilateral(quad);

			color = Zeni::get_Colors()["white"];

			Zeni::Quadrilateral<Zeni::Vertex3f_Color> *wireframe_quad = new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*quad);
			wireframe_quad->a.set_Color(color);
			wireframe_quad->b.set_Color(color);
			wireframe_quad->c.set_Color(color);
			wireframe_quad->d.set_Color(color);

			wireframe_quad->give_Material(new Material());
			wireframe_quadrilaterals.push_back(wireframe_quad);
			wireframe_buffer->fax_Quadrilateral(wireframe_quad);
		}
		else
		{
			throw Zeni::Error("Invalid Number of Vertices for faces");
			
			return;
		}
	}
	
	this->center = position;

	is_group = false;
}

SVSObject::SVSObject(const SVSObject& source)
{
	for (unsigned int i = 0;i < source.quadrilaterals.size();i++)
		quadrilaterals.push_back(new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*source.quadrilaterals[i]));
	
	for (unsigned int i = 0;i < source.triangles.size();i++)
		triangles.push_back(new Zeni::Triangle<Zeni::Vertex3f_Color>(*source.triangles[i]));

	for (unsigned int i = 0;i < source.wireframe_quadrilaterals.size();i++)
		wireframe_quadrilaterals.push_back(new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*source.wireframe_quadrilaterals[i]));
	
	for (unsigned int i = 0;i < source.wireframe_triangles.size();i++)
		wireframe_triangles.push_back(new Zeni::Triangle<Zeni::Vertex3f_Color>(*source.wireframe_triangles[i]));
	
	this->name = source.name;
	
	this->center = source.center;
	this->scale = source.scale;
	this->rotation = source.rotation;
	
	this->transformation_matrix = source.transformation_matrix;
	
	this->buffer = new Zeni::Vertex_Buffer();
	this->wireframe_buffer = new Zeni::Vertex_Buffer();
	
	for (unsigned int i = 0;i < this->triangles.size();i++)
		buffer->fax_Triangle(this->triangles[i]);
	
	for (unsigned int i = 0;i < this->quadrilaterals.size();i++)
		buffer->fax_Quadrilateral(this->quadrilaterals[i]);

	for (unsigned int i = 0;i < this->wireframe_triangles.size();i++)
		wireframe_buffer->fax_Triangle(this->wireframe_triangles[i]);
	
	for (unsigned int i = 0;i < this->wireframe_quadrilaterals.size();i++)
		wireframe_buffer->fax_Quadrilateral(this->wireframe_quadrilaterals[i]);

	this->is_group = source.is_group;
}

SVSObject& SVSObject::operator=(const SVSObject &source)
{
	if (this == &source)
		return *this;
	
	for (unsigned int i = 0;i < source.quadrilaterals.size();i++)
		quadrilaterals.push_back(new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*source.quadrilaterals[i]));
	
	for (unsigned int i = 0;i < source.triangles.size();i++)
		triangles.push_back(new Zeni::Triangle<Zeni::Vertex3f_Color>(*source.triangles[i]));

	for (unsigned int i = 0;i < source.wireframe_quadrilaterals.size();i++)
		wireframe_quadrilaterals.push_back(new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*source.wireframe_quadrilaterals[i]));
	
	for (unsigned int i = 0;i < source.wireframe_triangles.size();i++)
		wireframe_triangles.push_back(new Zeni::Triangle<Zeni::Vertex3f_Color>(*source.wireframe_triangles[i]));
	
	this->name = source.name;
	
	this->center = source.center;
	this->scale = source.scale;
	this->rotation = source.rotation;
	
	this->transformation_matrix = source.transformation_matrix;
	
	this->buffer = new Zeni::Vertex_Buffer();
	this->wireframe_buffer = new Zeni::Vertex_Buffer();
	
	for (unsigned int i = 0;i < this->triangles.size();i++)
		buffer->fax_Triangle(this->triangles[i]);
	
	for (unsigned int i = 0;i < this->quadrilaterals.size();i++)
		buffer->fax_Quadrilateral(this->quadrilaterals[i]);

	for (unsigned int i = 0;i < this->wireframe_triangles.size();i++)
		wireframe_buffer->fax_Triangle(this->wireframe_triangles[i]);
	
	for (unsigned int i = 0;i < this->wireframe_quadrilaterals.size();i++)
		wireframe_buffer->fax_Quadrilateral(this->wireframe_quadrilaterals[i]);
	
	this->is_group = source.is_group;
	
	return *this;
}

SVSObject::~SVSObject()
{
	if (!is_group)
	{
		delete buffer;
		delete wireframe_buffer;

		for (std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>*>::iterator it = quadrilaterals.begin();it != quadrilaterals.end();it++)
			delete *it;

		for (std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>*>::iterator it = wireframe_quadrilaterals.begin();it != wireframe_quadrilaterals.end();it++)
			delete *it;

		for (std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>*>::iterator it = triangles.begin();it != triangles.end();it++)
			delete *it;

		for (std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>*>::iterator it = wireframe_triangles.begin();it != wireframe_triangles.end();it++)
			delete *it;
	}
	else
	{
		for (std::vector<SVSObject*>::iterator it = children.begin();it != children.end();it++)
			delete *it;
	}
}

void SVSObject::transform_position(Zeni::Point3f amount)
{	
	transformation_matrix = Zeni::Matrix4f::Translate(amount) * Zeni::Matrix4f::Rotate(rotation) * Zeni::Matrix4f::Scale(scale);
	center = amount;
}

void SVSObject::transform_scale(Zeni::Point3f amount)
{
	transformation_matrix = Zeni::Matrix4f::Translate(center) * Zeni::Matrix4f::Rotate(rotation) * Zeni::Matrix4f::Scale(amount);
	scale = amount;
}

void SVSObject::transform_rotation(Zeni::Quaternion amount)
{
	transformation_matrix = Zeni::Matrix4f::Translate(center) * Zeni::Matrix4f::Rotate(amount) * Zeni::Matrix4f::Scale(scale);
	rotation = amount;
}

void SVSObject::transform(Zeni::Matrix4f matrix, Zeni::Point3f pos, Zeni::Quaternion rot, Zeni::Point3f scale)
{
	transformation_matrix = matrix;
	this->center = pos;
	this->rotation = rot;
	this->scale = scale;
}

void SVSObject::render()
{
	Zeni::Video &vr = Zeni::get_Video();

	vr.select_world_matrix();
	vr.push_world_stack();

	vr.transform_scene(transformation_matrix);

	if (!is_group)
		buffer->render();

	for (std::vector<SVSObject*>::iterator it = children.begin();it != children.end();it++)
		(*it)->render();

	vr.pop_world_stack();
}

void SVSObject::render_wireframe()
{
	Zeni::Video &vr = Zeni::get_Video();

	vr.select_world_matrix();
	vr.push_world_stack();

	vr.transform_scene(transformation_matrix);

	if (!is_group)
		wireframe_buffer->render();

	for (std::vector<SVSObject*>::iterator it = children.begin();it != children.end();it++)
		(*it)->render_wireframe();

	vr.pop_world_stack();
}


std::vector<std::vector<int> > SVSObject::verts_for_faces(const std::vector<Zeni::Point3f> pts)
{
	std::vector<std::vector<int> > faces;
	
#ifdef _WIN32
	char* temp_folder = new char[MAX_PATH+1];
	GetTempPath(MAX_PATH+1, temp_folder);
#else
	std::string temp_folder = "/tmp/";
#endif
	
	std::string path = temp_folder;
	path += "qhull";
	
	std::ofstream to_output(path.c_str(), std::ofstream::out | std::ofstream::trunc);
	
	if (!to_output.is_open())
		throw Zeni::Error(("Unable to open qhull file: " + path).c_str());
	
	to_output << "3" << std::endl << pts.size() << std::endl;
	
	for (std::vector<Zeni::Point3f>::const_iterator it = pts.begin();it != pts.end();++it)
		to_output << it->x << " " << it->y << " " << it->z << std::endl;
	
	to_output.close();
	
	std::string command_line;
#ifdef _WIN32
	command_line = "qhull.exe i TI ";
#else
	command_line = "i TI ";
#endif
	command_line += path;
	command_line += " TO ";
	command_line += path;
	command_line += "2";
	
#ifdef _WIN32
	STARTUPINFO start_info; 
    PROCESS_INFORMATION process_info; 
    memset(&start_info, 0, sizeof(start_info)); 
    memset(&process_info, 0, sizeof(process_info)); 
    start_info.cb = sizeof(start_info);
	
	char* command_line_char = const_cast<char*>(command_line.c_str());
	
	if (!CreateProcess(NULL, command_line_char, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &start_info, &process_info))
	{
		std::cout << "Could not spawn qhull process!" << std::endl;
		throw Zeni::Error(("Unable spawn qhull at " + command_line).c_str());
	}

	DWORD exit_code = WaitForSingleObject(process_info.hProcess, INFINITE);
	exit_code;
#else
	std::string command = "qhull ";
	command += command_line;
	
	FILE *p = popen(command.c_str(), "w");
	int ret = pclose(p);
	if (ret != 0) {
		return faces;
	}
#endif
	std::string output_path = path + "2";
	
	char *end;
	std::ifstream output(output_path.c_str(), std::ifstream::in);
	
	if (!output.is_open())
		throw Zeni::Error(("Unable to open qhull file: " + output_path).c_str());
	
	std::string line;

	if (!getline(output, line))
		throw Zeni::Error("getline failed to get the first line of the file!");

	unsigned int nfacets = strtol(line.c_str(), &end, 10);
	if (*end != '\0' && *end != '\n') {
		throw Zeni::Error("File line doesn't contain null character");
	}
	
	while (getline(output, line))
	{
		std::vector<int> facet;

		std::istringstream iss(line, std::istringstream::in);

		std::string number;
		while (iss >> number)
		{
			int x = strtol(number.c_str(), &end, 10);
			if (*end != '\0') {
				break;
			}
			facet.push_back(x);
		}
		
		faces.push_back(facet);
	}
	assert (faces.size() == nfacets);

	if (faces.size() == 0)
		throw Zeni::Error("Zero faces for a non-group!");

	return faces;
}

bool SVSObject::addChild(SVSObject* object)
{
	if (!is_group)
		throw Zeni::Error("ERROR: Tried to add object to geo-object!");

	children.push_back(object);

	return true;
}
