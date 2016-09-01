LDIR_WIN=lib
UNAME := $(shell uname)
ifeq ($(UNAME), CYGWIN_NT-6.1)
	LIBS=-lopengl32 -lglfw3 -lfreeglut -lglew32
endif
ifeq ($(UNAME), Linux)
	LIBS=-lGL -lglut -lglfw -lGLEW
endif
EXEC_FILE=run_main

all: clean main 

main: src/main.cpp src/common/log.cpp src/common/gfx.cpp src/common/utils.cpp src/common/shader.cpp
	g++ -g -o $(EXEC_FILE) -L$(LDIR_WIN) src/main.cpp src/common/log.cpp src/common/gfx.cpp src/common/utils.cpp src/common/shader.cpp -I.src/include $(LIBS)
	
clean: 
	rm -f $(EXEC_FILE)*
