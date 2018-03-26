INCFLAGS := -Ilibs -Ilibs/UI -I../shared_libs
CPPFLAGS := -Wall -std=c++14 -O3 $(INCFLAGS)

SRC_FILES := main.cpp
SRC_FILES += libs/SimpleGraphics.cpp
SRC_FILES += libs/touch.cpp
SRC_FILES += libs/TermiosUtil.cpp
SRC_FILES += libs/UI/UI.cpp
SRC_FILES += libs/UI/cursor.cpp
SRC_FILES += libs/UI/screen.cpp
SRC_FILES += libs/wifi.cpp
SRC_FILES += libs/video.cpp
#SRC_FILES += ../shared_libs/

OBJ_DIR := obj/software_linux

OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

prog: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: prog
	sudo ./$<

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ_FILES)
	$(RM) prog
	
