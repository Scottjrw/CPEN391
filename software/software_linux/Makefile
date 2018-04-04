INCFLAGS := -Ilibs -Ilibs/UI -Ishared_libs -I. -Ilibs/GestureRecognition
DEPFLAGS := -MMD
CPPFLAGS := -g -Wall -Werror -std=c++14 -O1 $(INCFLAGS) $(DEPFLAGS)

#SRC_FILES := main.cpp
SRC_FILES := testRecognizer.cpp
#SRC_FILES += libs/bluetooth.cpp
#SRC_FILES += libs/NIOS_Processor.cpp
#SRC_FILES += libs/NIOS_Processor_Init.cpp
SRC_FILES += libs/SimpleGraphics.cpp
SRC_FILES += libs/SimpleGraphicsFonts.cpp
SRC_FILES += libs/TermiosUtil.cpp
SRC_FILES += libs/touch.cpp
#SRC_FILES += libs/video.cpp
#SRC_FILES += libs/WandControl.cpp
#SRC_FILES += libs/wifi.cpp

SRC_FILES += libs/UI/cursor.cpp
SRC_FILES += libs/UI/screen.cpp
SRC_FILES += libs/UI/UI.cpp
#SRC_FILES += libs/UI/LoginPanel.cpp

SRC_FILES += libs/GestureRecognition/GeometricRecognizer.cpp

OBJ_DIR := obj/software_linux

OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.d,$(SRC_FILES))

prog: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: prog
	sudo ./$<

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

dat:
	$(MAKE) -C ../cpen391_group5 dat
	cp -v ../cpen391_group5/mem_init/hdl_sim/sdram.dat nios.dat

clean:
	$(RM) $(OBJ_FILES)
	$(RM) prog

rebuild: clean
	rm -r obj

	
include $(wildcard $(DEP_FILES))
