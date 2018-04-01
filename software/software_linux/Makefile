INCFLAGS := -Ilibs -Ilibs/UI -Ishared_libs -I. -Ilibs/GestureRecognition
DEPFLAGS := -MMD
CPPFLAGS := -g -Wall -Werror -std=c++14 -O3 $(INCFLAGS) $(DEPFLAGS)

#SRC_FILES := main.cpp
SRC_FILES := testmain.cpp
SRC_FILES += libs/SimpleGraphics.cpp
SRC_FILES += libs/GestureRecognition/GeometricRecognizer.cpp
SRC_FILES += libs/touch.cpp
SRC_FILES += libs/TermiosUtil.cpp
SRC_FILES += libs/UI/UI.cpp
SRC_FILES += libs/UI/cursor.cpp
SRC_FILES += libs/UI/screen.cpp
SRC_FILES += libs/wifi.cpp
SRC_FILES += libs/video.cpp
SRC_FILES += libs/NIOS_Processor.cpp
#SRC_FILES += ../shared_libs/

OBJ_DIR := obj/software_linux

OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.d,$(SRC_FILES))

prog: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: prog
	sudo ./$<

nios_init: nios/nios_init.o
	g++ -std=c++14 -Wall -o nios_init $^

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ_FILES)
	$(RM) prog
	
include $(wildcard $(DEP_FILES))
