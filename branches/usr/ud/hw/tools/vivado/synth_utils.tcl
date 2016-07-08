source -notrace $::env(TOOLS_DIR)/log_utils.tcl

proc read_srcs { srcs log_dir } {
    # Filter source files by extension
    set vhdl_srcs [lsearch -all -inline $srcs *.vhd]
    set verilog_srcs [lsearch -all -inline $srcs *.v]
    set xco_srcs [lsearch -all -inline $srcs *.xco]
    set xdc_srcs [lsearch -all -inline $srcs *.xdc]
    set tcl_srcs [lsearch -all -inline $srcs *.tcl]

    # Create bd designs
    if {[llength $tcl_srcs] > 0} {
        command "source {$tcl_srcs}"
        command "generate_target all \[get_files .srcs/sources_1/bd/ps/ps.bd\]" "$log_dir/gen_target.log"
    }

    # Read source files
    if {[llength $vhdl_srcs] > 0} {
        command "read_vhdl {$vhdl_srcs}"
    }

    if {[llength $verilog_srcs] > 0} {
        command "read_verilog {$verilog_srcs}"
    }

    if {[llength $xco_srcs] > 0} {
        command "import_ip {$xco_srcs}"

        set ips ""

        foreach ip $xco_srcs {
            lappend ips [file tail [file rootname $ip]]
        }

        command "upgrade_ip \[get_ips $ips\]" "$log_dir/upgrade_ips.log"
        command "synth_ip \[get_ips $ips\]" "$log_dir/synth_ips.log"
    }

    if {[llength $xdc_srcs] > 0} {
        command "read_xdc {$xdc_srcs}"
    }
}
