#ifndef __PLATFORM_H__
#define __PLATFORM_H__
//-------- Microsoft Windows OS
#if defined _WIN64 || defined _WIN32
//---- ignore annoying warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define PLATFORM_WINDOWS
//----
#include <windows.h>
#include <gl/glut.h>
#define _USE_MATH_DEFINES
#else 
//-------- Mac OS
#include <GLUT/glut.h>
#define PLATFORM_MACOS
#endif
//-------------------------
#endif
