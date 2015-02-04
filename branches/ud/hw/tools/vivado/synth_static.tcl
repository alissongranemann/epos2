source -notrace $::env(TOOLS_DIR)/log_utils.tcl
source -notrace $::env(TOOLS_DIR)/synth_utils.tcl

# Import environment variables
set log_dir $::env(LOG_DIR)
set synth_dir $::env(SYNTH_DIR)
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)

# Read script arguments
set srcs [split [lindex $argv 0]]
set top [split [lindex $argv 1]]

# Create project in memory
create_project -in_memory -part $part

set_property target_language $::env(TARGET_LANGUAGE) [current_project]

read_srcs $srcs $log_dir

file mkdir $log_dir/static

# Run synthesis
command "synth_design -top $top" "$log_dir/static/synth.log"

# Write checkpoint
command "write_checkpoint -force $synth_dir/static.dcp"

close_project
