#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H
//-------- Microsoft Windows OS
#if defined _WIN64 || defined _WIN32
//---- ignore annoying warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define PLATFORM_WINDOWS
//----
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "gl/glut.h"
#define _USE_MATH_DEFINES
#include <math.h>
#else 
//-------- Mac OS
//#define OSX_AVAILABLE_STARTING(...)
//#define OSX_AVAILABLE_BUT_DEPRECATED_MSG(...)
//#include <stdlib.h>
//#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cstdio>
#include <cstdlib>
#include <GLUT/glut.h>
#include <unistd.h>
#include <cmath>
#define PLATFORM_MACOS
void Sleep(int msec) { usleep( msec ); }
#endif
//#include "myShape.h"
//-------------------------
#endif
