/***
 * hesperus: Game.cpp
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#include "Game.h"

#include <iostream>

#include <gl/glew.h>
#include <source/ogl/WrappedGL.h>
#include <gl/glu.h>

#include <ASXEngine.h>

#include <source/exceptions/Exception.h>
#include <source/gui/Screen.h>
#include <source/io/util/DirectoryFinder.h>
#include "GameState_Menu.h"
namespace bf = boost::filesystem;

namespace hesp {

//#################### CONSTRUCTORS ####################
Game::Game()
try
:	m_mouseMotionSinceStateChange(false)
{
	// Read in the configuration options.
	ASXEngine configEngine;
	bf::path scriptsDir = determine_scripts_directory();
	if(!configEngine.load_and_build_script((scriptsDir / "config.as").file_string(), "config"))
	{
		configEngine.output_messages(std::cout);
		quit_with_error("Could not load configuration options - check the config.as script for problems");
	}
	ASXModule_Ptr configModule = configEngine.get_module("config");
	int width						= configModule->get_global_variable<int>("width");
	int height						= configModule->get_global_variable<int>("height");
	bool fullScreen					= configModule->get_global_variable<bool>("fullScreen");
	const std::string& levelName	= configModule->get_global_variable<std::string>("levelName");

	// Set up the window.
	if(SDL_Init(SDL_INIT_VIDEO) < 0) quit(EXIT_FAILURE);

	SDL_WM_SetCaption("The Scarlet Pimpernel", NULL);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int bpp = 32;
	int flags = SDL_OPENGL;
	if(fullScreen) flags |= SDL_FULLSCREEN;

	if(SDL_SetVideoMode(width, height, bpp, flags) == 0) quit(EXIT_FAILURE);

	Screen& screen = Screen::instance();
	screen.set_dimensions(Extents(0, 0, width, height));
	screen.fit();

	if(glewInit() != GLEW_OK) quit_with_error("GLEW could not be initialised");
	if(!glewGetExtension("GL_ARB_multitexture")) quit_with_error("Multitexturing not supported");

	// Set the initial game state.
	bf::path levelsDir = determine_levels_directory();
	std::string relativeLevelPath = levelName + "/" + levelName + ".bsp";
	m_state.reset(new GameState_Menu("Main", (levelsDir / relativeLevelPath).file_string(), m_soundSystem));
	m_state->enter();
}
catch(Exception& e) { quit_with_error(e.cause()); }

//#################### PUBLIC METHODS ####################
void Game::run()
{
	const Screen& screen = Screen::instance();

	Uint32 lastDraw = SDL_GetTicks();
	for(;;)
	{
		if(m_state->quit_requested()) quit(0);

		process_events();

		Uint32 frameTime = SDL_GetTicks();
		Uint32 timeElapsed = frameTime - lastDraw;
		if(timeElapsed >= 16)	// aim for 62.5 frames per second
		{
#if 0
			std::cout << "Render " << timeElapsed << std::endl;
#endif

			screen.render();
			lastDraw = frameTime;

			// Note:	We clamp the elapsed time to 50ms to prevent things moving
			//			too far between one frame and the next.
			GameState_Ptr newState = m_state->update(std::min(timeElapsed, Uint32(50)), m_input);
			m_input.set_mouse_motion(0, 0);

			if(newState)
			{
				m_state->leave();
				m_state = newState;
				m_state->enter();
				lastDraw = SDL_GetTicks();
				m_mouseMotionSinceStateChange = false;
				continue;
			}
		}
	}
}

//#################### PRIVATE METHODS ####################
void Game::handle_key_down(const SDL_keysym& keysym)
{
	m_input.press_key(keysym.sym);
}

void Game::handle_key_up(const SDL_keysym& keysym)
{
	m_input.release_key(keysym.sym);
}

void Game::handle_mousebutton_down(const SDL_MouseButtonEvent& e)
{
	switch(e.button)
	{
		case SDL_BUTTON_LEFT:
			m_input.press_mouse_button(MOUSE_BUTTON_LEFT, e.x, e.y);
			break;
		case SDL_BUTTON_MIDDLE:
			m_input.press_mouse_button(MOUSE_BUTTON_MIDDLE, e.x, e.y);
			break;
		case SDL_BUTTON_RIGHT:
			m_input.press_mouse_button(MOUSE_BUTTON_RIGHT, e.x, e.y);
			break;
		default:
			break;
	}
}

void Game::handle_mousebutton_up(const SDL_MouseButtonEvent& e)
{
	switch(e.button)
	{
		case SDL_BUTTON_LEFT:
			m_input.release_mouse_button(MOUSE_BUTTON_LEFT);
			break;
		case SDL_BUTTON_MIDDLE:
			m_input.release_mouse_button(MOUSE_BUTTON_MIDDLE);
			break;
		case SDL_BUTTON_RIGHT:
			m_input.release_mouse_button(MOUSE_BUTTON_RIGHT);
			break;
		default:
			break;
	}
}

void Game::process_events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				handle_key_down(event.key.keysym);
				break;
			case SDL_KEYUP:
				handle_key_up(event.key.keysym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				handle_mousebutton_down(event.button);
				break;
			case SDL_MOUSEBUTTONUP:
				handle_mousebutton_up(event.button);
				break;
			case SDL_MOUSEMOTION:
				m_input.set_mouse_position(event.motion.x, event.motion.y);
				if(m_mouseMotionSinceStateChange) m_input.set_mouse_motion(event.motion.xrel, event.motion.yrel);
				else m_mouseMotionSinceStateChange = true;
				break;
			case SDL_QUIT:
				quit(0);
				break;
			default:
				break;
		}
	}
}

void Game::quit(int code)
{
	SDL_Quit();
	exit(code);
}

void Game::quit_with_error(const std::string& error)
{
	SDL_Quit();
	std::cout << "Error: " << error << std::endl;
	exit(EXIT_FAILURE);
}

}
