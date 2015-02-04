proc command { command  {log ""}} {
    set cfh [open "$::env(COMMAND_LOG)" a]

    puts $cfh $command
    flush $cfh

    # Ignore newline or comments
    if {[string match "\n" $command] || [string match "#*" $command]} {
        return 0
    }

    puts "COMMAND: $command"

    set command_name [lindex [split $command] 0]

    if {[llength $log] > 0} {
        if {[catch "$command > $log"]} {
            parse_log $log
            regexp {(\.*.*)(\..*)} $log matched log_name log_type

            puts "Writing checkpoint ${log_name}_error.dcp for debug."
            command "write_checkpoint -force ${log_name}_error.dcp"

            error "ERROR: $command_name command failed.\n\t$command\nSee log file $log for more details."
        }

        parse_log $log
    } else {
        if {[catch $command]} {
            error "ERROR: $command_name command failed.\n\t$command\n"
        }
    }

    close $cfh
}

proc parse_log { log } {
    set crfh [open "$::env(CRITICAL_LOG)" a]

    if {[file exists $log]} {
        set lfh [open $log r]
        set log_data [read $lfh]
        close $lfh

        set log_lines [split $log_data "\n" ]

        puts $crfh "#Parsing $log:"

        foreach line $log_lines {
            if {[string match "CRITICAL WARNING*" $line]} {
                puts $crfh "\t$line"
                puts $line
            }

            if {[string match "ERROR:*" $line]} {
                puts $line
            }
        }
    } else {
        puts $crfh "ERROR: Could not find $log."
    }

    puts $crfh "\n"
    flush $crfh
    close $crfh
}

