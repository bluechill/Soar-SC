//
//  SVSViewerState.cpp
//  Application
//
//  Created by Alex Turner on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSViewerState.h"

#include <string>
#include <sstream>
#include <iterator>

#include "SVSParser.h"

int thread_runner(void* viewer)
{
	return reinterpret_cast<SVSViewerState*>(viewer)->reader_function();
}

SVSViewerState::SVSViewerState(unsigned short port)
	: max_velo(SVSObject::global_scale * 5, SVSObject::global_scale * 5, SVSObject::global_scale * 5),
	max_time_step(1.0f / 20.0f),
	max_time_steps(10.0f)
{
	set_pausable(false); //Make sure we cannot pause the viewer because that would be bad.

	should_die = false;

	mu = SDL_CreateMutex();

	reader_socket = new SVSSocket(port);

	reader_thread = SDL_CreateThread(thread_runner, this);

	movement.left = false;
	movement.right = false;
	movement.up= false;
	movement.down = false;
	movement.plus = false;
	movement.minus = false;

	camera.far_clip = 15000.0f;
	camera.near_clip = 1.0f;

	grid = true;
	wireframe = true;

	button1_down = false;
}

SVSViewerState::~SVSViewerState()
{
	should_die = true;

	SDL_WaitThread(reader_thread, NULL);
	SDL_DestroyMutex(mu);
}

bool SVSViewerState::reader_function()
{
	std::string line;

	while (!should_die)
	{
		if (!reader_socket->wait_for_connection())
			return false;

		SDL_mutexP(mu);
		std::cout << "Got a connection" << std::endl;
		SDL_mutexV(mu);
		
		while (!should_die)
		{
			reader_socket->recieve_line(line);
			if (reader_socket->client_disconnected())
				break;

			SDL_mutexP(mu);
			std::cout << "Recieved: '" << line << "'" << std::endl;
			
			if (line.find_first_not_of("\t\n ") != std::string::npos && line.size() != 0)
			{
				reader_buffer.push_back(line);
				line = "";
			}
			SDL_mutexV(mu);
		}

		SDL_mutexP(mu);
		std::cout << "Client (SVS) disconnected" << std::endl;
		SDL_mutexV(mu);

	}

	return true;
}

void SVSViewerState::perform_logic()
{	
	//Handle movement first

	const Zeni::Time temp_time = Zeni::get_Timer().get_time();
	time_passed += temp_time.get_seconds_since(last_iteration_time);
	last_iteration_time = temp_time;

	if (time_passed / max_time_step > max_time_steps)
		time_passed = max_time_step * max_time_steps;

	while (time_passed > max_time_step)
	{
		time_passed -= max_time_step;
		camera.move_left_xy(max_velo.i * max_time_step *
			(movement.left - movement.right));
		camera.move_forward_xy(max_velo.j * max_time_step *
			(movement.up - movement.down));
		camera.position.z += max_velo.k * max_time_step *
			(movement.plus - movement.minus);
	}

	camera.move_left_xy(max_velo.i * time_passed *
		(movement.left - movement.right));
	camera.move_forward_xy(max_velo.j * time_passed *
		(movement.up - movement.down));
	camera.position.z += max_velo.k * time_passed *
		(movement.plus - movement.minus);

	time_passed = 0.0f;

	//Then everythign else

	SDL_mutexP(mu);

	if (reader_buffer.size() > 0)
	{
		if (!parse_command(reader_buffer[0]))
			std::cout << "Invalid command: '" << reader_buffer[0] << "'" << std::endl;

		reader_buffer.erase(reader_buffer.begin());
	}

	SDL_mutexV(mu);
}

bool SVSViewerState::parse_command(std::string command)
{
	using namespace std;

	std::istringstream iss(command);

	std::vector<std::string> parts;

	copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter<std::vector<std::string> >(parts));

	//Ripped from the actual SVS Viewer
	string scene_pat;
	string geom_pat;
	vector<string> args;
	
	if (parts.size() == 0)
		return true;
	else if (parts[0] == "save")
		return true; //Ignore save commands
	else if (parts[0] == "layer")
		return true; /*proc_layer_cmd(fields + 1);*/ //Ignore layer commands
	else if (parts[0] == "draw")
	{
		return true; //Ignore draw commands
	}
	else
	{
		scene_pat = parts[0];
		if (parts.size() > 1)
		{
			geom_pat = parts[1];
			args.reserve(parts.size()-1);
			args.insert(args.end(), parts.begin()+1, parts.end());
		}
		else
			geom_pat = "";
	}
	
	if (scene_pat == "")
		return true;
	else if (scene_pat[0] == '+' && (scene_pat.size() > 3 || scene_pat[2] != '1'))
		return true; //Only care about the first scene since the agent doesn't use anything but that.
	else if (scene_pat[0] == '-' && (scene_pat.size() > 3 || scene_pat[2] != '1'))
		return true; //Only care about the first scene since the agent doesn't use anything but that.
	else if (scene_pat[0] == 'S' && (scene_pat.size() > 2 || scene_pat[1] != '1'))
		return true;

	if (scene_pat[0] == '-')
	{
		if (scenes.size() > 0)
			scenes.erase(scenes.begin());

		return true;
	}
	else if (scene_pat[0] == '+')
	{
		if (scenes.size() == 0)
			scenes.push_back(new SVSScene(scene_pat.c_str()+1));
	}
	
	if (geom_pat == "")
		return true;
	
	return process_geometry_command(scenes[0], args);
}

bool SVSViewerState::process_geometry_command(SVSScene* scene, std::vector<std::string> parts)
{
	std::vector<Zeni::Point3f> vertices;
	Zeni::Point3f position;
	Zeni::Quaternion rotation;
	Zeni::Point3f scale(1,1,1);
	
	if (parts.size() < 1) // Must have at least a name
		return false;
	
	std::string name = parts[0].c_str()+1;
	std::string parent_name;

	bool is_add = false;
	bool is_change = false;
	bool is_delete = false;

	if (parts[0][0] == '+')
		is_add = true;
	else if (parts[0][0] == '-')
		is_delete = true;
	else
		return false;
	
	if (is_add && scene->find_object(parts[0].c_str()+1) != nullptr)
		is_change = true;

	for (unsigned int i = 1;i < parts.size();i++)
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
				if (is_change)
					break;

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

	if (is_change)
	{
		if (!scene->update_object(name, position, rotation, scale))
			return false;
		
		return true;
	}
	else if (is_add)
	{
		if (!scene->add_object(name, parent_name, vertices, position, rotation, scale))
			return false;
		
		return true;
	}
	else if (is_delete)
	{
		if (!scene->delete_object(name))
			return false;

		return true;
	}
	else
		return false;
}

bool SVSViewerState::parse_verts(std::vector<std::string> &parts, std::vector<Zeni::Point3f> &verts)
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

bool SVSViewerState::parse_vector3(std::vector<std::string> &parts, Zeni::Point3f &vert)
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

void SVSViewerState::on_push() {
	/*Zeni::get_Window().mouse_hide(true);
	Zeni::get_Window().mouse_grab(true);*/

	set_pausable(false);

	//mouse_grabbed = true;
}

void SVSViewerState::on_pop() {
	/*Zeni::get_Window().mouse_grab(false);
	Zeni::get_Window().mouse_hide(false);

	mouse_grabbed = false;*/
}

void SVSViewerState::on_key(const SDL_KeyboardEvent &event)
{
	switch (event.keysym.sym)
	{
	case SDLK_w:
		{
			movement.up = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_s:
		{
			movement.down = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_a:
		{
			movement.left = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_d:
		{
			movement.right = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_e:
	case SDLK_EQUALS:
	case SDLK_PLUS:
		{
			movement.plus = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_q:
	case SDLK_MINUS:
		{
			movement.minus = event.type == SDL_KEYDOWN;
			break;
		}
	case SDLK_t:
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (wireframe)
					wireframe = false;
				else
					wireframe = true;
			}

			break;
		}
	case SDLK_g:
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (grid)
					grid = false;
				else
					grid = true;
			}

			break;
		}
	case SDLK_ESCAPE:
		{
			exit(1);
			break;
		}
	case SDLK_PERIOD:
		{
			if (event.type != SDL_KEYDOWN)
				break;

			if (max_velo.x >= SVSObject::global_scale * 10)
				break;

			max_velo.x += SVSObject::global_scale;
			max_velo.y += SVSObject::global_scale;
			max_velo.z += SVSObject::global_scale;

			break;
		}
	case SDLK_COMMA:
		{
			if (event.type != SDL_KEYDOWN)
				break;

			if (max_velo.x <= SVSObject::global_scale)
				break;

			max_velo.x -= SVSObject::global_scale;
			max_velo.y -= SVSObject::global_scale;
			max_velo.z -= SVSObject::global_scale;

			break;
		}
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
	case SDLK_SLASH:
		{
			if (max_velo.x >= SVSObject::global_scale * 20)
			{
				max_velo = backup_speed;
			}
			else
			{
				backup_speed = max_velo;

				max_velo.x = SVSObject::global_scale * 20;
				max_velo.y = SVSObject::global_scale * 20;
				max_velo.z = SVSObject::global_scale * 20;
			}
			break;
		}
	case SDLK_SPACE:
		{
			if (max_velo.x <= SVSObject::global_scale / 10.0f)
			{
				max_velo = backup_speed;
			}
			else
			{
				backup_speed = max_velo;

				max_velo.x = SVSObject::global_scale / 10.0f;
				max_velo.y = SVSObject::global_scale / 10.0f;
				max_velo.z = SVSObject::global_scale / 10.0f;
			}
			break;
		}
		/*case SDLK_u:
		{
		if (mouse_grabbed)
		{
		Zeni::get_Window().mouse_grab(false);
		Zeni::get_Window().mouse_hide(false);
		}
		else
		{
		Zeni::get_Window().mouse_grab(true);
		Zeni::get_Window().mouse_hide(true);
		}
		break;
		}*/

	default:
		Gamestate_Base::on_key(event);
	}
}

void SVSViewerState::on_mouse_button(const SDL_MouseButtonEvent &event)
{
	if (event.button == SDL_BUTTON_LEFT)
		button1_down = event.type == SDL_MOUSEBUTTONDOWN;
}

void SVSViewerState::on_mouse_motion(const SDL_MouseMotionEvent &event)
{
	if (!button1_down)
		return;

	camera.turn_left_xy(-event.xrel / 100.0f);

	// Back up a couple vectors
	const Zeni::Quaternion prev_orientation = camera.orientation;
	const Zeni::Vector3f prev_up = camera.get_up();

	camera.adjust_pitch(event.yrel / 100.0f);

	/* Restore the backup vectors if flipped over
	* (if the up vector is pointing down)
	*
	* Note that we want to be sure not to freeze a player 
	* who is already flipped (for whatever reason).
	*/
	if(camera.get_up().k < 0.0f && prev_up.k >= 0.0f)
		camera.orientation = prev_orientation;
}

void SVSViewerState::draw_grid(float xstart, float ystart, int rows, int columns, float distance_x, float distance_y)
{
	glBegin(GL_LINES);
	for (int i = 0; i <= rows; i++) {
		glVertex2f(xstart, i * distance_y);
		glVertex2f(xstart + columns * distance_x, i * distance_y);
	}

	for (int i= 0; i <= columns; i++) {
		glVertex2f(i * distance_x, ystart);
		glVertex2f(i * distance_x, ystart + rows * distance_y);
	}
	glEnd();
}

void SVSViewerState::render()
{	
	//TODO: add multiple scenes functionality
	Zeni::get_Video().set_3d(camera);
	//	Zeni::get_Video().set_backface_culling(false);
	Zeni::get_Video().set_clear_Color(Zeni::get_Colors()["black"]);

	if (grid)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		draw_grid(0.0f, 0.0f, 256/4, 256/4, SVSObject::global_scale * 4, SVSObject::global_scale * 4);
	}

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);

		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0]->render();
			SDL_mutexV(mu);
		}

		glDisable(GL_POLYGON_OFFSET_FILL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0]->render_wireframe();
			SDL_mutexV(mu);
		}
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0]->render();
			SDL_mutexV(mu);
		}
	}
}
