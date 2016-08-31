LDIR_WIN=lib
LIBS_WIN=-lopengl32 -lglfw3 -lfreeglut -lglew32
EXEC_FILE=run_main

all: clean main_win 

main_win: src/main.cpp src/common/log.cpp src/common/gfx.cpp src/common/utils.cpp src/common/shader.cpp
	gcc -o $(EXEC_FILE) -L$(LDIR_WIN) src/main.cpp src/common/log.cpp src/common/gfx.cpp src/common/utils.cpp src/common/shader.cpp -I.src/include $(LIBS_WIN)
	
clean: 
	rm -f $(EXEC_FILE)*