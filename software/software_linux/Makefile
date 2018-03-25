CFLAGS = -std=c++11
CXXFLAGS = -std=c++11
default: main.o  SimpleGraphics.o UI.o touch.o wifi.o TermiosUtil.o screen.o cursor.o
	g++ -std=c++11  main.o SimpleGraphics.o UI.o touch.o wifi.o TermiosUtil.o screen.o cursor.o -o main

clean:
	$(RM) main.o SimpleGraphics.o UI.o touch.o wifi.o TermiosUtil.o screen.o cursor.o
	
