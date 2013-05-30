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
	const float scale = SVSObject::global_scale;
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

void SVSScene::clear_objects()
{

	objects.clear();

	float scale = SVSObject::global_scale;
	add_object("world", "", std::vector<Zeni::Point3f>(), Zeni::Point3f(), Zeni::Quaternion(), Zeni::Point3f(scale,scale,scale)); 
}

SVSObject* SVSScene::find_object_in_objects(std::vector<SVSObject*> &objects, std::string name)
{
	for (unsigned int i = 0;i < objects.size();i++)
	{
		if (objects[i]->get_name() == name)
			return objects[i];

		if (objects[i]->is_a_group())
		{
			std::vector<SVSObject*> &children = objects[i]->getChildren();

			SVSObject* temp = find_object_in_objects(children, name);
			if (temp != nullptr)
				return temp;
		}
	}

	return nullptr;
}

bool SVSScene::add_object(std::string name, std::string parent, std::vector<Zeni::Point3f> vertices, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale)
{
	if (parent != "")
	{
		if (find_object_in_objects(objects, name))
			throw Zeni::Error(("ERROR: Object already exists with the name '" + name + "'").c_str());

		SVSObject* svs_parent = find_object_in_objects(objects, parent);

		if (!svs_parent)
			throw Zeni::Error(("ERROR: Could not find parent: '" + parent + "'").c_str());

		SVSObject* object = new SVSObject(name, vertices, position, rotation, scale);
		svs_parent->addChild(object);
	}
	else
	{
		SVSObject* object = new SVSObject(name, vertices, position, rotation, scale);
		objects.push_back(object);
	}
	return true;
}

bool SVSScene::update_object(std::string name, Zeni::Point3f position, Zeni::Quaternion rotation, Zeni::Point3f scale)
{
	SVSObject* object = get_object_by_name(name);

	if (object == NULL)
		return false;

	Zeni::Matrix4f transformation = Zeni::Matrix4f::Translate(position) * Zeni::Matrix4f::Rotate(rotation) * Zeni::Matrix4f::Scale(scale);

	object->transform(transformation, position, rotation, scale);

	return true;
}

bool SVSScene::delete_object_recursive(std::string name, std::vector<SVSObject*> &objects)
{
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();it++)
	{
		if ((*it)->get_name() == name)
		{
			objects.erase(it);
			return true;
		}
		else
		{
			std::vector<SVSObject*> &children = (*it)->getChildren();

			if (delete_object_recursive(name, children))
				return true;
		}
	}

	return false;
}

bool SVSScene::delete_object(std::string name)
{
	return delete_object_recursive(name, objects);
}

SVSObject* SVSScene::get_object_in_vector(std::string name, std::vector<SVSObject*> &objects)
{
	for (std::vector<SVSObject*>::iterator it = objects.begin();it != objects.end();++it)
	{
		if ((*it)->get_name() == name)
			return (*it);

		std::vector<SVSObject*> &children = (*it)->getChildren();
		SVSObject* object = get_object_in_vector(name, children);
		if (object != NULL)
			return object;
	}

	return NULL;
}

SVSObject* SVSScene::get_object_by_name(std::string name)
{
	return get_object_in_vector(name, objects);
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
