//
//  SVSScene.cpp
//  Application
//
//  Created by Alex Turner on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSScene.h"

SVSScene::SVSScene(std::string name)
{
	scene_name = name;
	const float scale = SVSObject::global_scale / 4; //Specific to starcraft (walkable tile vs build tile)
	this->add_object("world", "", std::vector<Zeni::Point3f>(), Zeni::Point3f(), Zeni::Quaternion(), Zeni::Point3f(scale,scale,scale));
}

SVSScene::SVSScene(const SVSScene &source)
{
	this->scene_name = source.scene_name;
	
	this->objects.clear();
	
	for (unsigned int i = 0;i < source.objects.size();i++)
		this->objects.push_back(new SVSObject(*source.objects[i]));
}

SVSScene& SVSScene::operator=(const SVSScene& source)
{
	if (this == &source)
		return *this;
	
	this->scene_name = source.scene_name;
	
	this->objects.clear();
	
	for (unsigned int i = 0;i < source.objects.size();i++)
		this->objects.push_back(new SVSObject(*source.objects[i]));
	
	return *this;
}

SVSScene::~SVSScene()
{
	for (unsigned int i = 0;i < objects.size();i++)
		delete objects[i];
}

bool SVSScene::add_object(std::string name, std::string parent, std::vector<Zeni::Point3f> vertices, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale)
{
	for (unsigned int i = 0;i < objects.size();i++)
	{
		if (objects[i]->get_name() == name)
		{
			std::cout << "Object already exists with name: '" << name << "'" << std::endl;
			return false;
		}
	}
	
	SVSObject* parent_object = get_object_by_name(parent);
	
	SVSObject* object = new SVSObject(name, vertices, position, rotation, scale, parent_object);
	objects.push_back(object);
	
	return true;
}

bool SVSScene::update_object(std::string name, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale)
{
	SVSObject* object = get_object_by_name(name);
	
	Zeni::Matrix4f transformation;
	transformation.Rotate(rotation);
	transformation.Scale(scale);
	transformation.Translate(position);
	
	object->transform(transformation);
	
	return true;
}

bool SVSScene::delete_object(std::string name)
{
	bool deleted = false;
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();)
	{
		if ((*it)->get_name() == name)
		{
			delete (*it);
			it = objects.erase(it);
			
			deleted = true;
			
			break;
		}
		else
			++it;
	}
	
	return deleted;
}

SVSObject* SVSScene::get_object_by_name(std::string name)
{
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();++it)
	{
		if ((*it)->get_name() == name)
			return (*it);
	}
	
	return NULL;
}

std::string SVSScene::get_scene_name()
{
	return scene_name;
}

void SVSScene::render()
{
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();++it)
		(*it)->render();
}

void SVSScene::render_wireframe()
{
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();++it)
		(*it)->render_wireframe();
}
