CXXFLAGS := -std=c++14 -I..
OBJ := main.o GeometricRecognizer.o
PROGRAM := gesture

default: $(OBJ)
	c++ $(CXXFLAGS) $(OBJ) -o $(PROGRAM)

clean:
	$(RM) $(OBJ) $(PROGRAM)
