.PHONY=build run

nui: build run

build: src/*.c src/*.h res/*/*
	gcc -g src/*.c -lSDL2 -lSDL2_image -lm -lGL -lGLEW -o nui

run:
	./nui
