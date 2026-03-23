CC ?= C:/msys64/ucrt64/bin/gcc.exe
SDL2_PREFIX ?= C:/msys64/ucrt64

TARGET = program.exe
SRC = src/camera.c src/main.c src/model.c src/scene.c src/skybox.c src/texture.c

CFLAGS += -Iinclude -I$(SDL2_PREFIX)/include/SDL2
LDFLAGS += -L$(SDL2_PREFIX)/lib
LDLIBS += -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lm

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) src/*.o