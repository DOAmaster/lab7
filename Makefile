all: lab7 lab7a

lab7: lab7.cpp
	g++ lab7.cpp -Wall -lX11 -lGL -lGLU -lm ./libggfonts.a -olab7

lab7a: lab7a.cpp
	g++ lab7a.cpp -Wall -lX11 -lGL -lGLU -lm ./libggfonts.a -olab7a

clean:
	rm -f lab7 lab7a

