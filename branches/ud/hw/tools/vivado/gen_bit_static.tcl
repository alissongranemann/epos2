source -notrace $::env(TOOLS_DIR)/log_utils.tcl

# Import environment variables
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)
set bit_dir $::env(BIT_DIR)

# Read script arguments
set route_dcp [split [lindex $argv 0]]

set pr [file tail [file rootname $route_dcp]]
set top [file tail [file dirname $route_dcp]]

command "file mkdir $bit_dir/$top"

command "open_checkpoint $route_dcp -part $part"
command "write_bitstream -force -file $bit_dir/$top/$pr"

close_project
