all: lab7 lab7a

lab7: lab7.cpp
	g++ lab7.cpp -Wall -lX11 -lGL -lGLU -lm ./libggfonts.a -olab7 \
	    -D USE_OPENAL_SOUND \
	    /usr/lib/x86_64-linux-gnu/libopenal.so\
	    /usr/lib/x86_64-linux-gnu/libalut.so
lab7a: lab7a.cpp
	g++ lab7a.cpp -Wall -lX11 -lGL -lGLU -lm ./libggfonts.a -olab7a \
	    -D USE_OPENAL_SOUND \
	    /usr/lib/x86_64-linux-gnu/libopenal.so\
	    /usr/lib/x86_64-linux-gnu/libalut.so

clean:
	rm -f lab7 lab7a

