#!/bin/smake -f
# makefile for interactive CG programing workshop
# 2008 T.OHSHIMA

# 使い方：
# makeコマンドから参照されます．
# make TARGET=プログラム名（拡張子省略）
# 例）make TARGET=sample1

CC	= g++
OPTION	= -O2 -Wall

# use WIN for Cygwin
# OS	= LINUX
OS	= WIN

ifeq ($(OS), LINUX) 
LFLAG	= -L/usr/X11R6/lib -lm -lX11 -lGL -lGLU -lglut -lXext -lXmu -lXi
endif
ifeq ($(OS), WIN)
LFLAG	= -lm -lglut32 -lglu32 -lopengl32
endif

default: $(TARGET)

.cpp.o:
	$(CC) -c $<

.c.o:
	$(CC) -c $<

$(TARGET): $(TARGET).o
	$(CC) -o $@ $@.o $(OPTION) $(LFLAG)

ifeq ($(OS),LINUX)
clean:
	@rm -f *.o
endif
ifeq ($(OS), WIN)
clean:
	@rm -f *.o *.exe
endif

cleanall: clean
	@rm -f *~ *.*~ *.*.*~
