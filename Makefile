LIBS  = -lm -lSOIL -lglfw -framework OpenGL -framework CoreFoundation -lassimp -lportaudio
INCLUDE = -I. -Ithird-party/include 
CFLAGS = -Wall --std=c11

SRC=$(wildcard *.c)
PREFABS=$(wildcard prefabs/*.c)
SRC_THIRD_PARTY=$(wildcard third-party/src/*.c)

game: $(SRC) $(SRC_THIRD_PARTY) $(PREFABS)
	gcc -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIBS)

test:
	./game

clean:
	rm game
