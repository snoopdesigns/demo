LDIR_WIN=lib
LIBS_WIN=-lopengl32 -lglfw3 -lfreeglut -lglew32
EXEC_FILE=run_main

main_win: src/main.c src/common/log.c src/common/gfx.c
	gcc -o $(EXEC_FILE) -L$(LDIR_WIN) src/main.c src/common/log.c src/common/gfx.c -I.src/include $(LIBS_WIN)