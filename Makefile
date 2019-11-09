FLAGS	:= -std=c++17 -ffast-math -Wall -Wextra -pedantic -O3
LIBS	:= -lX11

all:
	g++ -o rast main.cpp -O3 -march=native $(FLAGS) $(LIBS)

debug:
	g++ -o rast main.cpp -ggdb $(FLAGS) $(LIBS)

clean:
	rm -f rast

