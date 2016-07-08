source -notrace $::env(TOOLS_DIR)/log_utils.tcl

# Import environment variables
set build_dir $::env(BUILD_DIR)
set log_dir $::env(LOG_DIR)
set route_dir $::env(ROUTE_DIR)
set synth_dir $::env(SYNTH_DIR)
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)

# Read script arguments
set xdcs [split [lindex $argv 0]]
set recfg_parts [split [lindex $argv 1]]
set synth_recfg_comp [split [lindex $argv 2]]

command "open_checkpoint $synth_dir/static.dcp -part $part"

# Create the reconfigurable partition area on the fabric and load the default
# logic on it. Vivado doesn't allow the implementation of a design with a black
# box on it.
command "read_xdc {$xdcs}"
foreach recfg_part $recfg_parts {
    command "read_checkpoint -cell $recfg_part $synth_recfg_comp"
}

# Generate ltx file for debug probes
command "write_debug_probes -quiet $build_dir/debug_probes.ltx"

# Optimize, place and route
command "opt_design" "$log_dir/static/opt.log"
command "place_design" "$log_dir/static/place.log"
command "route_design" "$log_dir/static/route.log"

# Generate reports
command "report_utilization -file $log_dir/static/utilization.log"
command "report_timing_summary -file $log_dir/static/timing_summary.log"
command "report_drc -file $log_dir/static/drc.log"
command "write_xdc -force -no_fixed_only -file $build_dir/static.xdc"

file mkdir $route_dir/static

# Checkpoint to static routed design
command "write_checkpoint -force $route_dir/static/static.dcp"

close_project
