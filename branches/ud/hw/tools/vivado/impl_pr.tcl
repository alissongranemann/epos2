source -notrace $::env(TOOLS_DIR)/log_utils.tcl

# Import environment variables
set log_dir $::env(LOG_DIR)
set route_dir $::env(ROUTE_DIR)
set synth_dir $::env(SYNTH_DIR)
set part $::env(DEVICE)$::env(PACKAGE)$::env(SPEED)

# Read script arguments
set top [split [lindex $argv 0]]
set recfg_comps [split [lindex $argv 1]]

file mkdir $route_dir/$top

foreach recfg_comp $recfg_comps {
    # Clear out the reconfigurable partition, implement the configuration with
    # the static design loaded in memory, lock down all placement and route
    command "open_checkpoint $route_dir/static/static.dcp -part $part"
    command "update_design -cell $top -black_box"
    command "lock_design -level routing"
    command "read_checkpoint -cell $top $synth_dir/$recfg_comp.dcp"

    command "file mkdir $log_dir/$top/${recfg_comp}"

    # Optimize, place and route
    command "opt_design" "${log_dir}/${top}/${recfg_comp}/opt.log"
    command "place_design" "${log_dir}/${top}/${recfg_comp}/place.log"
    command "route_design" "${log_dir}/${top}/${recfg_comp}/route.log"

    # Generate reports
    command "report_utilization -file $log_dir/${top}/${recfg_comp}/utilization.log"
    command "report_timing_summary -file $log_dir/${top}/${recfg_comp}/timing_summary.log"
    command "report_drc -file $log_dir/${top}/${recfg_comp}/drc.log"

    # Checkpoint to the routed design
    command "write_checkpoint -force $route_dir/${top}/${recfg_comp}.dcp"
}

close_project
