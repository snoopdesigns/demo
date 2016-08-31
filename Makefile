LDIR_WIN=lib
LIBS_WIN=-lopengl32 -lglfw3 -lfreeglut -lglew32
EXEC_FILE=run_main

all: clean main_win 

main_win: src/main.c src/common/log.c src/common/gfx.c src/common/utils.c src/common/shader.c
	gcc -o $(EXEC_FILE) -L$(LDIR_WIN) src/main.c src/common/log.c src/common/gfx.c src/common/utils.c src/common/shader.c -I.src/include $(LIBS_WIN)
	
clean: 
	rm -f $(EXEC_FILE)*