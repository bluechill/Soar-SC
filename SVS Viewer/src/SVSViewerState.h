//
//  SVSViewerState.h
//  Application
//
//  Created by Alex Turner on 6/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSViewerState_h
#define Application_SVSViewerState_h

#include "zenilib.h"

#include "SVSSocket.h"
#include "SVSScene.h"

#include <string>
#include <vector>

class SVSViewerState : public Zeni::Gamestate_Base
{
	friend int thread_runner(void* viewer);
public:
	SVSViewerState(SVSSocket::socket_type type);
	~SVSViewerState();
	
	bool reader_function();
	
	bool parse_command(std::string command);
	
	//zenilib specific functions
	void perform_logic();
	void render();
	
	void on_key(const SDL_KeyboardEvent &event);
	
	void on_mouse_motion(const SDL_MouseMotionEvent &event);
	void on_mouse_button(const SDL_MouseButtonEvent &event);
	
	void on_push();
	void on_pop();

	bool button1_down;

	SDL_mutex *mu;
	
private:
	std::vector<std::string> reader_buffer;
	
	SVSSocket *reader_socket;
	
	SDL_Thread* reader_thread;
	
	std::vector<SVSScene> scenes;
	
	Zeni::Camera camera;
	
	bool should_die;
	
	struct Movement
	{
		bool left;
		bool right;
		bool up;
		bool down;
		
		bool plus;
		bool minus;
	} movement;
	
	Zeni::Time last_iteration_time;
	float time_passed;
	float max_time_step;
	float max_time_steps;
	Zeni::Vector3f max_velo;
	
	void draw_grid(float xstart, float ystart, int rows, int columns, float distance);
	
	bool grid;
	bool wireframe;
	
	bool mouse_grabbed;

	Zeni::Point3f backup_speed;
};

#endif
