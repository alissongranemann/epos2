source -notrace $::env(TOOLS_DIR)/log_utils.tcl

# Read script arguments
set bit_file [split [lindex $argv 0]]

command "connect_hw_server"
command "current_hw_target \[get_hw_targets\]"
command "open_hw_target"
command "current_hw_device \[lindex \[get_hw_devices\] 1\]"
command "open_hw"
command "set_property PROGRAM.FILE {$bit_file} \[lindex \[get_hw_devices\] 1\]"
command "program_hw_devices \[lindex \[get_hw_devices\] 1\]"
command "close_hw"
