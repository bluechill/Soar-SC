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

const float SVSObject::global_scale = 25.0f;

SVSObject::SVSObject(std::string name, const std::vector<Zeni::Point3f> verts, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale, SVSObject* parent)
{	
	this->name = name;
	
	if (!parent && name != "world")
	{
		std::cout << "ERROR: Must inherit from a parent *always*!" << std::endl;
		exit(1);
	}
	
	if (parent && !parent->is_a_group())
	{
		std::cout << "ERROR: Tried to inherit from geo object not group!" << std::endl;
		exit(1);
	}

	position.x *= global_scale;
	position.y *= global_scale;
	position.z *= global_scale;
	
	if (verts.size() == 0)
	{
		is_group = true;
				
		this->center = position;
		this->parent = parent;
		
		this->scale = scale;
		this->rotation = rotation;
		
		Zeni::Matrix4f local_transformation_matrix = Zeni::Matrix4f::Translate(position) * Zeni::Matrix4f::Rotate(rotation) * Zeni::Matrix4f::Scale(scale);
		
		transformation_matrix = local_transformation_matrix;
		
		if (parent != NULL)
			transformation_matrix *= parent->get_transformation_matrix();			
		
		return;
	}
	
	this->buffer = new Zeni::Vertex_Buffer();
	
	std::vector<std::vector<int> > faces = verts_for_faces(verts);
	
	Zeni::Matrix4f local_transformation_matrix = Zeni::Matrix4f::Translate(position) * Zeni::Matrix4f::Rotate(rotation) * Zeni::Matrix4f::Scale(scale);
	
	std::cout << "Position: " << position.x << "," << position.y << "," << position.z << std::endl;
	std::cout << "Scale: " << scale.x << "," << scale.y << "," << scale.z << std::endl;
	
	std::cout << "Local Matrix: " << std::endl;
	std::cout << local_transformation_matrix[0][0] << " " << local_transformation_matrix[0][1] << " " << local_transformation_matrix[0][2] << " " << local_transformation_matrix[0][3] << std::endl;
	
	std::cout << local_transformation_matrix[1][0] << " " << local_transformation_matrix[1][1] << " " << local_transformation_matrix[1][2] << " " << local_transformation_matrix[1][3] << std::endl;
	
	std::cout << local_transformation_matrix[2][0] << " " << local_transformation_matrix[2][1] << " " << local_transformation_matrix[2][2] << " " << local_transformation_matrix[2][3] << std::endl;
	
	std::cout << local_transformation_matrix[3][0] << " " << local_transformation_matrix[3][1] << " " << local_transformation_matrix[3][2] << " " << local_transformation_matrix[3][3] << std::endl;
	
	transformation_matrix = local_transformation_matrix * parent->get_transformation_matrix();
	
	std::cout << "Final Matrix: " << std::endl;
	std::cout << transformation_matrix[0][0] << " " << transformation_matrix[0][1] << " " << transformation_matrix[0][2] << " " << transformation_matrix[0][3] << std::endl;
	
	std::cout << transformation_matrix[1][0] << " " << transformation_matrix[1][1] << " " << transformation_matrix[1][2] << " " << transformation_matrix[1][3] << std::endl;
	
	std::cout << transformation_matrix[2][0] << " " << transformation_matrix[2][1] << " " << transformation_matrix[2][2] << " " << transformation_matrix[2][3] << std::endl;
	
	std::cout << transformation_matrix[3][0] << " " << transformation_matrix[3][1] << " " << transformation_matrix[3][2] << " " << transformation_matrix[3][3] << std::endl;	
	
	int i = 0;
	
	for (std::vector<std::vector<int> >::iterator it = faces.begin();it != faces.end();++it)
	{
		++i;
		
		if (it->size() == 3)
		{
			//Triangle
			Zeni::Color color = Zeni::get_Colors()["magenta"];
			
			Zeni::Point3f position1 = verts.at(it->at(0)) + parent->get_center_position();
			Zeni::Point3f position2 = verts.at(it->at(1)) + parent->get_center_position();
			Zeni::Point3f position3 = verts.at(it->at(2)) + parent->get_center_position();
			
			position1 = transformation_matrix * position1;
			position2 = transformation_matrix * position2;
			position3 = transformation_matrix * position3;
						
			Zeni::Vertex3f_Color vert1(position1, color);
			Zeni::Vertex3f_Color vert2(position2, color);
			Zeni::Vertex3f_Color vert3(position3, color);
			
			Zeni::Triangle<Zeni::Vertex3f_Color> *triag = new Zeni::Triangle<Zeni::Vertex3f_Color>(vert1, vert2, vert3);
			triangles.push_back(triag);
			buffer->fax_Triangle(triag);
		}
		else if (it->size() == 4)
		{
			//Quad
			Zeni::Color color = Zeni::get_Colors()["magenta"];
			
			Zeni::Point3f position1 = verts.at(it->at(0)) + parent->get_center_position();
			Zeni::Point3f position2 = verts.at(it->at(1)) + parent->get_center_position();
			Zeni::Point3f position3 = verts.at(it->at(2)) + parent->get_center_position();
			Zeni::Point3f position4 = verts.at(it->at(3)) + parent->get_center_position();
			
			position1 = transformation_matrix * position1;
			position2 = transformation_matrix * position2;
			position3 = transformation_matrix * position3;
			position4 = transformation_matrix * position4;
			
			std::cout << "Corner (" << i << "): " << position1.x << "," << position1.y << "," << position1.z << std::endl;
			std::cout << "Corner (" << i << "): " << position2.x << "," << position2.y << "," << position2.z << std::endl;
			std::cout << "Corner (" << i << "): " << position3.x << "," << position3.y << "," << position3.z << std::endl;
			std::cout << "Corner (" << i << "): " << position4.x << "," << position4.y << "," << position4.z << std::endl;
			
			Zeni::Vertex3f_Color vert1(position1, color);
			Zeni::Vertex3f_Color vert2(position2, color);
			Zeni::Vertex3f_Color vert3(position3, color);
			Zeni::Vertex3f_Color vert4(position4, color);
						
			Zeni::Quadrilateral<Zeni::Vertex3f_Color> *quad = new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(vert1, vert2, vert3, vert4);
			quadrilaterals.push_back(quad);
			buffer->fax_Quadrilateral(quad);
		}
		else
		{
			throw Zeni::Error("Invalid Number of Vertices for faces");
			
			return;
		}
	}
	
	this->center = position;
	
	mu = SDL_CreateMutex();
}

SVSObject::SVSObject(const SVSObject& source)
{
	for (unsigned int i = 0;i < source.quadrilaterals.size();i++)
		quadrilaterals.push_back(new Zeni::Quadrilateral<Zeni::Vertex3f_Color>(*source.quadrilaterals[i]));
	
	for (unsigned int i = 0;i < source.triangles.size();i++)
		triangles.push_back(new Zeni::Triangle<Zeni::Vertex3f_Color>(*source.triangles[i]));
	
	this->name = source.name;
	this->parent = source.parent;
	
	this->center = source.center;
	this->scale = source.scale;
	this->rotation = source.rotation;
	
	this->transformation_matrix = source.transformation_matrix;
	
	this->buffer = new Zeni::Vertex_Buffer();
	
	for (unsigned int i = 0;i < this->triangles.size();i++)
		buffer->fax_Triangle(this->triangles[i]);
	
	for (unsigned int i = 0;i < this->quadrilaterals.size();i++)
		buffer->fax_Quadrilateral(this->quadrilaterals[i]);
	
	this->mu = SDL_CreateMutex();
	
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
	
	this->name = source.name;
	this->parent = source.parent;
	
	this->center = source.center;
	this->scale = source.scale;
	this->rotation = source.rotation;
	
	this->transformation_matrix = source.transformation_matrix;
	
	this->buffer = new Zeni::Vertex_Buffer();
	
	for (unsigned int i = 0;i < this->triangles.size();i++)
		buffer->fax_Triangle(this->triangles[i]);
	
	for (unsigned int i = 0;i < this->quadrilaterals.size();i++)
		buffer->fax_Quadrilateral(this->quadrilaterals[i]);
	
	this->mu = SDL_CreateMutex();
	
	this->is_group = source.is_group;
	
	return *this;
}

SVSObject::~SVSObject()
{
	if (!is_group)
		delete buffer;
	
	//SDL_DestroyMutex(mu);
}

void SVSObject::transform_position(Zeni::Point3f amount)
{	
	Zeni::Matrix4f matrix;
	matrix.Translate(amount);
	
	this->transform(matrix);
}

void SVSObject::transform_scale(Zeni::Point3f amount)
{
	Zeni::Matrix4f matrix;
	matrix.Scale(amount);
	
	this->transform(matrix);
}

void SVSObject::transform_rotation(Zeni::Quaternion amount)
{
	Zeni::Matrix4f matrix;
	matrix.Rotate(amount);
	
	this->transform(matrix);
}

void SVSObject::transform(Zeni::Matrix4f transformation_matrix)
{
	for (std::vector<Zeni::Triangle<Zeni::Vertex3f_Color>* >::iterator it = triangles.begin();it != triangles.end();++it)
	{
		(*it)->a.position = transformation_matrix * (*it)->a.position;
		(*it)->b.position = transformation_matrix * (*it)->b.position;
		(*it)->c.position = transformation_matrix * (*it)->c.position;
	}
	
	for (std::vector<Zeni::Quadrilateral<Zeni::Vertex3f_Color>* >::iterator it = quadrilaterals.begin();it != quadrilaterals.end();++it)
	{
		(*it)->a.position = transformation_matrix * (*it)->a.position;
		(*it)->b.position = transformation_matrix * (*it)->b.position;
		(*it)->c.position = transformation_matrix * (*it)->c.position;
		(*it)->d.position = transformation_matrix * (*it)->d.position;
	}
	
	this->center = transformation_matrix * this->center;
	
	SDL_mutexP(mu);
	
	delete buffer;
	
	buffer = new Zeni::Vertex_Buffer;
	
	for (unsigned int i = 0;i < triangles.size();i++)
		buffer->fax_Triangle(triangles[i]);
	
	for (unsigned int i = 0;i < quadrilaterals.size();i++)
		buffer->fax_Quadrilateral(quadrilaterals[i]);
	
	SDL_mutexV(mu);
}

void SVSObject::render()
{
	if (quadrilaterals.size() == 0 && triangles.size() == 0)
		return;
	
	SDL_mutexP(mu);
//	buffer->render();
	
	for (unsigned int i = 0;i < triangles.size();i++)
		Zeni::get_Video().render(*triangles[i]);
	
	for (unsigned int i = 0;i < quadrilaterals.size();i++)
	{
		Zeni::Video &vr = Zeni::get_Video();
		Zeni::Quadrilateral<Zeni::Vertex3f_Color>* quad_2 = quadrilaterals[i];
		Zeni::Quadrilateral<Zeni::Vertex3f_Color> quad = *quad_2;
		
		vr.render(quad);
	}
	
	SDL_mutexV(mu);
}

void SVSObject::render_wireframe()
{
	if (quadrilaterals.size() == 0 && triangles.size() == 0)
		return;
	
	SDL_mutexP(mu);
	//	buffer->render();
	
	for (unsigned int i = 0;i < triangles.size();i++)
	{
		triangles[i]->a.set_Color(Zeni::get_Colors()["white"]);
		triangles[i]->b.set_Color(Zeni::get_Colors()["white"]);
		triangles[i]->c.set_Color(Zeni::get_Colors()["white"]);
		
		Zeni::get_Video().render(*triangles[i]);
		
		triangles[i]->a.set_Color(Zeni::get_Colors()["magenta"]);
		triangles[i]->b.set_Color(Zeni::get_Colors()["magenta"]);
		triangles[i]->c.set_Color(Zeni::get_Colors()["magenta"]);
	}
	
	for (unsigned int i = 0;i < quadrilaterals.size();i++)
	{
		Zeni::Color original = quadrilaterals[i]->a.get_Color();
		quadrilaterals[i]->a.set_Color(Zeni::get_Colors()["white"]);
		quadrilaterals[i]->b.set_Color(Zeni::get_Colors()["white"]);
		quadrilaterals[i]->c.set_Color(Zeni::get_Colors()["white"]);
		quadrilaterals[i]->d.set_Color(Zeni::get_Colors()["white"]);
		
		Zeni::Video &vr = Zeni::get_Video();
		Zeni::Quadrilateral<Zeni::Vertex3f_Color>* quad_2 = quadrilaterals[i];
		Zeni::Quadrilateral<Zeni::Vertex3f_Color> quad = *quad_2;
		
		vr.render(quad);
		
		quadrilaterals[i]->a.set_Color(Zeni::get_Colors()["magenta"]);
		quadrilaterals[i]->b.set_Color(Zeni::get_Colors()["magenta"]);
		quadrilaterals[i]->c.set_Color(Zeni::get_Colors()["magenta"]);
		quadrilaterals[i]->d.set_Color(Zeni::get_Colors()["magenta"]);
	}
	
	SDL_mutexV(mu);
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
		return faces;
	
	to_output << "3" << std::endl << pts.size() << std::endl;
	
	for (std::vector<Zeni::Point3f>::const_iterator it = pts.begin();it != pts.end();++it)
		to_output << it->x << " " << it->y << " " << it->z << std::endl;
	
	to_output.close();
	
	std::string command_line;
#ifdef _WIN32
	command_line = "bin\\qhull.exe i TI ";
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
		return faces;
	}
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
		return faces;
	
	std::string line;
	getline(output, line);
	unsigned int nfacets = strtol(line.c_str(), &end, 10);
	if (*end != '\0') {
		return faces;
	}

	bool should_not_break = false;
	
	while (getline(output, line)) {
		const char *start = line.c_str();
		std::vector<int> facet;
		while (!should_not_break) {
			int x = strtol(start, &end, 10);
			if (end == start) {
				break;
			}
			facet.push_back(x);
			start = end;
		}
		faces.push_back(facet);
	}
	assert (faces.size() == nfacets);
	return faces;
}
