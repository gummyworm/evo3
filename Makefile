LIBS  = -lm -lSOIL -lglew -lglfw -framework OpenGL -framework CoreFoundation -lassimp
INCLUDE = -I. -Ithird-party/include 
CFLAGS = -Wall --std=c11

SRC=$(wildcard *.c)
PREFABS=$(wildcard prefabs/*.c)
SRC_THIRD_PARTY=$(wildcard third-party/src/*.c)

game: $(SRC) $(SRC_THIRD_PARTY) $(PREFABS)
	gcc -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIBS)

clean:
	rm game
