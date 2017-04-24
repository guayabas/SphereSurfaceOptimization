#pragma once
#include <GLFW/glfw3.h>
#include <FreeImage.h>
#include <cstdio>

void screenshoot(const char * fileName, unsigned width, unsigned height, bool useFrontBuffer = false)
{
	const char * location = "Screenshoots/%d.png";
	static unsigned id = 0;
	GLubyte * screenBuffer = new GLubyte[(width * height * 3)];
	(useFrontBuffer) ? glReadBuffer(GL_FRONT) : glReadBuffer(GL_BACK);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screenBuffer);
	FIBITMAP * image = FreeImage_ConvertFromRawBits(screenBuffer, width, height, (3 * width), 24, 0x0000FF, 0xFF0000, 0x00FF00, false);	
	char fileNameAsNumber[256];
#if defined WIN32 || defined _WIN32
	sprintf_s(fileNameAsNumber, location, id++);
#else
	sprintf(fileNameAsNumber, locationFormat, id++);
#endif
	FreeImage_Save(FIF_PNG, image, fileNameAsNumber, 0);
	FreeImage_Unload(image);
	delete[] screenBuffer;
}