CPEN391 Group 5

File Structure


-- Qsys --

# Main Qsys System
cpen391_group5_qsys.qsys -- Qsys File

# Hardware Accelerated Vision
pixel_cluster/ -- Verilog source for hardware vision algorithm
pixel_cluster/sim/ -- ModelSim simulations for pixel_cluster
pixel_cluster_hw.tcl -- Qsys custom component script
pixel_cluster_hw.tcl~

# Hardware Accelerated Graphics
signal_control.sv -- Verilog source file for hardware graphics
graphics_hw.tcl -- Qsys custom Component script
graphics_hw.tcl~

# Test System (Not very interesting)
test_pixel_cluster.qsys -- Qsys File

# HPS Test System (Also not very interesting)
testhps.qsys

-- Quartus --
cpen391_group5.qpf -- Project File
cpen391_group5.qsf -- Settings File
cpen391_group5.sv -- Top Module Verilog Source

# Provided files (Needed for synthesis)
DE1_SoC.sdc 
DE1_SoC_Quartus_Settings_File.qsf

# SOF (Included for convenience)
output_files/cpen391_group5_time_limited.sof -- Last Synthesis (Should be same one used in Demo)

-- Software --
software/ -- All software here

software/cpen391_group5/ -- Main Project
software/cpen391_group5/libs/ -- Libraries we built

software/cpen391_group5_bsp/ -- Main Project BSP

software/kernel/ -- Development of our Linux kernel driver(s) see the readme in the directory for more info

software/wifi_scripts/ -- Lua sources for the Wifi Module

software/<other files> -- Ignore these, just some configurations for vim


-- Software In Depth --
# software/cpen391_group5
main.cpp -- Definition of the main function

vision_updated.hpp/cpp -- New Gesture Recognizer Class

<other files> -- Makefile and eclipse config

# software/cpen391_group5/libs
cursor.hpp/cpp -- Cursor class, used to follow user touches or the red dots

Fonts.c -- Exercise 1.7 fonts

PixelCluster.hpp/cpp -- Class for managing the HW accelerated vision

screen.hpp/cpp -- Screen class for grouping various graphical objects together

SimpleGraphics.hpp/cpp -- Low level graphics commands, interfaces with HW Graphics module

touch.hpp/cpp -- touchscreen interface class

touch_uart.hpp -- touchscreen uart protocol definitions

UI.hpp/cpp -- Generic high level graphical classes

video.hpp/cpp -- Camera settings library

wifi.hpp/cpp -- Wifi module library

