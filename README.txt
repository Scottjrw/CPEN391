CPEN391 Group 5

File Structure


-- Qsys --

# Main Qsys System
cpen391_group5_qsys.qsys -- Qsys File

ip/ -- Qsys subsystems
ip/presets/ -- Presets for some Qsys modules

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
rbf_soc_system.cof -- Settings for Convert Programming Files to generate a soc_system.rbf needed by the HPS

# Provided files (Needed for synthesis)
DE1_SoC.sdc 
DE1_SoC_Quartus_Settings_File.qsf

# SOF (Included for convenience)
output_files/cpen391_group5.sof -- Last Synthesis (Should be same one used in Demo)

-- Software --
software/ -- All software here

software/cpen391_group5/ -- Main Project
software/cpen391_group5/libs/ -- Libraries we built

software/cpen391_group5_bsp/ -- Main Project BSP

software/kernel/ -- Development of our Linux kernel driver(s) see the readme in the directory for more info

software/software-linux/ -- ARM Software Folder

software/wifi_scripts/ -- Lua sources for the Wifi Module

software/<other files> -- Ignore these, just some configurations for vim

