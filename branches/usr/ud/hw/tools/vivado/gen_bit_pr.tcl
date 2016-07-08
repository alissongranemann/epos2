source -notrace $::env(TOOLS_DIR)/log_utils.tcl

# Import environment variables
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)
set bit_dir $::env(BIT_DIR)

# Read script arguments
set route_dcps [split [lindex $argv 0]]

foreach route_dcp $route_dcps {
    set pr [file tail [file rootname $route_dcp]]
    set top [file tail [file dirname $route_dcp]]

    command "file mkdir $bit_dir/$top"

    command "open_checkpoint $route_dcp -part $part"
    command "write_bitstream -force -file $bit_dir/$top/$pr"

    # Remove unused generated bitstreams
    # TODO: Find a better way of doing it!
    command "file rename -force $bit_dir/$top/$pr\_$top\_partial.bit $bit_dir/$pr.bit"
    command "file delete -force {*} [glob $bit_dir/$top/$pr*]"
    command "file rename -force $bit_dir/$pr.bit $bit_dir/$top/$pr.bit"
}

close_project
