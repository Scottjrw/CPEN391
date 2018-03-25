INCFLAGS := -Ilibs -Ilibs/UI
CPPFLAGS := -Wall -std=c++14 -O3 $(INCFLAGS)

SRC_FILES := main.cpp
SRC_FILES += libs/cursor.cpp
SRC_FILES += libs/screen.cpp
SRC_FILES += libs/SimpleGraphics.cpp
SRC_FILES += libs/touch.cpp
SRC_FILES += libs/TermiosUtil.cpp
SRC_FILES += libs/UI.cpp
SRC_FILES += libs/wifi.cpp

OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

main: $(OBJ_FILES)

run: main
	sudo ./main

$(OBJ_DIR)/%.o: %.cpp
   $(C++) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)
	$(RM) main
	
