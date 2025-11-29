// PLATFORM INDEPENDENT SETTINGS
//-------- Microsoft Windows OS
#if defined _WIN64 || defined _WIN32
//---- ignore annoying warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define PLATFORM_WINDOWS
//----
#include <windows.h>
#include "glut-3.7.6-bin/GL/glut.h"
#define _USE_MATH_DEFINES
#else 
//-------- Mac OS
#include <GLUT/glut.h>
#define PLATFORM_MACOS
#endif
