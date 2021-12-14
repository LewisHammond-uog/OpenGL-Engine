#include "RenderingProject.h"

#define DEFAULT_SCREENWIDTH 1920
#define DEFAULT_SCREENHEIGHT 1080

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif //!STB_IMAGE_IMPLEMENTATION


// main that controls the creation/destruction of an application
int WinMain(int argc, char* argv[])
{

	// explicitly control the creation of our application
	RenderingProject* app = new RenderingProject();
	app->run("RenderingProject Project", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}