all:
	rm -f program.exe src/*.o
	gcc -Iinclude src/camera.c src/main.c src/model.c src/scene.c src/skybox.c src/texture.c -o program.exe -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lm

clean:
	rm -f program.exe src/*.o