LIBS  = -lm -lSOIL -lglfw -framework OpenGL -framework CoreFoundation -lassimp -lportaudio
INCLUDE = -I. -Ithird-party/include
CFLAGS = -Wall --std=c11 -DDEBUG

SRC=$(filter-out main.c, $(wildcard *.c))
TESTS=$(wildcard tests/*.c)
PREFABS=$(wildcard prefabs/*.c)
SRC_THIRD_PARTY=$(wildcard third-party/src/*.c)

game: $(SRC) main.c $(SRC_THIRD_PARTY) $(PREFABS)
	gcc -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIBS)

unit: $(TESTS) $(SRC) $(SRC_THIRD_PARTY) $(PREFABS)
	gcc -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIBS)

test:
	./game

clean:
	rm game
