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

SVSViewerState::SVSViewerState(SVSSocket::socket_type type)
	: max_velo(SVSObject::global_scale * 5, SVSObject::global_scale * 5, SVSObject::global_scale * 5),
	max_time_step(1.0f / 20.0f),
	max_time_steps(10.0f)
{
	set_pausable(false); //Make sure we cannot pause the viewer because that would be bad.

	should_die = false;

	mu = SDL_CreateMutex();

	if (type == SVSSocket::STDIN)
		reader_socket = new SVSSocket(true);
	else if (type == SVSSocket::SOCKET)
		reader_socket = new SVSSocket();
	else
	{
		std::cout << "Invalid type: " << type << std::endl;
		exit(1);
	}

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

		bool reopened_pipe = false;

		SDL_mutexP(mu);
		if (scenes.size() > 0)
		{
#ifdef _WIN32
			reader_socket->reopen_pipe();
#endif

			reopened_pipe = true;
		}
		SDL_mutexV(mu);

		if (!reader_socket->listen())
			return false;

		if (reopened_pipe && scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0].clear_objects();
			SDL_mutexV(mu);
		}

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
			SDL_mutexV(mu);

			if (line.find_first_not_of("\t\n ") != std::string::npos && line.size() != 0)
			{
				SDL_mutexP(mu);
				reader_buffer.push_back(line);
				line = "";
				SDL_mutexV(mu);
			}
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
		{
			SDL_mutexP(mu);
			std::cout << "Invalid command: '" << reader_buffer[0] << "'" << std::endl;
			SDL_mutexV(mu);
		}

		reader_buffer.erase(reader_buffer.begin());
	}

	SDL_mutexV(mu);
}

bool SVSViewerState::parse_command(std::string command)
{
	std::istringstream iss(command);

	std::vector<std::string> parts;

	copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter<std::vector<std::string> >(parts));

	if (parts.size() == 1 && parts[0] == "clear")
		return true;

	if (parts.size() < 3)
		return false;

	std::string scene_name = parts[0];
	std::string command_char = parts[1];

	if (command_char.size() != 1)
		return false;

	int scene_number = -1;

	for (unsigned int i = 0;i < scenes.size();i++)
	{
		if (scenes[i].get_scene_name() == scene_name)
		{
			scene_number = i;
			break;
		}
	}

	if (scene_number == -1)
	{
		SVSScene new_scene(scene_name);
		scenes.push_back(new_scene);
		scene_number = scenes.size()-1;

		SDL_mutexP(mu);
		std::cout << "Warning: Created new scene: '" << scene_name << "'" << std::endl;
		SDL_mutexV(mu);
	}

	std::vector<std::string> subvector(parts.begin()+2, parts.end());

	if (command_char == "a")
	{
		if (!SVSParser::parse_add(subvector, scenes[scene_number]))
			return false;

		return true;
	}
	else if (command_char == "c")
	{
		if (!SVSParser::parse_change(subvector, scenes[scene_number]))
			return false;

		return true;
	}
	else if (command_char == "d")
	{
		if (!SVSParser::parse_delete(subvector, scenes[scene_number]))
			return false;

		return true;
	}
	else
		return false;
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

void SVSViewerState::draw_grid(float xstart, float ystart, int rows, int columns, float distance)
{
	glBegin(GL_LINES);
	for (int i = 0; i <= rows; i++) {
		glVertex2f(xstart, i * distance);
		glVertex2f(xstart + columns * distance, i * distance);
	}

	for (int i= 0; i <= columns; i++) {
		glVertex2f(i * distance, ystart);
		glVertex2f(i * distance, ystart + rows * distance);
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
		draw_grid(0.0f, 0.0f, 256, 256, SVSObject::global_scale);
	}

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);

		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0].render();
			SDL_mutexV(mu);
		}

		glDisable(GL_POLYGON_OFFSET_FILL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0].render_wireframe();
			SDL_mutexV(mu);
		}
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (scenes.size() > 0)
		{
			SDL_mutexP(mu);
			scenes[0].render();
			SDL_mutexV(mu);
		}
	}
}
