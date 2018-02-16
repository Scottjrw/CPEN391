CPEN391 Group 5

File Structure

-- Qsys --

# Main Qsys System
cpen391_group5_qsys.qsys -- Qsys File
cpen391_group5_qsys/ -- Qsys Generation Directory

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
test_pixel_cluster_qsys/ -- Qsys Generation Directory


-- Quartus --
cpen391_group5.qpf
cpen391_group5.qsf

