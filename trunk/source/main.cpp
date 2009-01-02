/***
 * hesperus: main.cpp
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#include <cstdio>
#include <cstdlib>

#include <SDL.h>
#include <source/ogl/WrappedGL.h>
#include <gl/glu.h>

#include <source/gui/ExplicitLayout.h>
#include <source/gui/Picture.h>
#include <source/gui/Screen.h>
#include <source/io/FileUtil.h>
using namespace hesp;

void quit(int code)
{
	SDL_Quit();
	exit(code);
}

void handle_key_down(SDL_keysym *keysym)
{
	switch(keysym->sym)
	{
		case SDLK_ESCAPE:
			quit(0);
			break;
		default:
			break;
	}
}

void process_events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				handle_key_down(&event.key.keysym);
				break;
			case SDL_QUIT:
				quit(0);
				break;
		}
	}
}

int main( int argc, char* argv[] )
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) quit(EXIT_FAILURE);

	SDL_WM_SetCaption("The Scarlet Pimpernel", NULL);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int width = 1024;
	int height = 768;
	int bpp = 32;
	int flags = SDL_OPENGL;// | SDL_FULLSCREEN;

	if(SDL_SetVideoMode(width, height, bpp, flags) == 0) quit(EXIT_FAILURE);

	// Setup the screen.
	Screen& screen = Screen::instance();
	screen.layout().add(new Picture("resources/title.bmp"), Extents(width/4, 0, width*3/4, width/8));
	Container<ExplicitLayout> *cont = new Container<ExplicitLayout>;
	cont->layout().add(new Picture("resources/title.bmp"), Extents(500, 0, 700, 50));
	screen.layout().add(cont, Extents(100, 100, 200, 200));
	screen.fit(Extents(0, 0, 1024, 768));

#if 1
	try
	{
		// Test level loading.
		Level_Ptr unlitLevel = FileUtil::load_level_file("resources/simple.bsu");
		Level_Ptr litLevel = FileUtil::load_level_file("resources/simple.bsp");
	}
	catch(Exception& e) {}
#endif

	for(;;)
	{
		process_events();
		screen.render();
	}

	return 0;
}
