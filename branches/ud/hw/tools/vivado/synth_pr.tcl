source -notrace $::env(TOOLS_DIR)/log_utils.tcl
source -notrace $::env(TOOLS_DIR)/synth_utils.tcl

# Import environment variables
set base_dir $::env(BASE_DIR)
set log_dir $::env(LOG_DIR)
set synth_dir $::env(SYNTH_DIR)
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)

# Read script arguments
set srcs [split [lindex $argv 0]]
set top [split [lindex $argv 1]]
set generics [split [lindex $argv 2]]

# Create project in memory
create_project -in_memory -part $part

set_property target_language $::env(TARGET_LANGUAGE) [current_project]
set_property generic $generics [current_fileset]

read_srcs $srcs $log_dir

file mkdir $log_dir/$top

# Run OOC synthesis
command "synth_design -mode out_of_context -top $top" "$log_dir/$top/synth.log"

# Generate reports
command "report_utilization -file $log_dir/$top/utilization.log"

# Write checkpoint
command "write_checkpoint -force $synth_dir/$top.dcp"

close_project
