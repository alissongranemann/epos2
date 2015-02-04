solution rename toplevel

set toplevel [regexp -inline ".*_Node" [directive get -DESIGN_HIERARCHY]]

directive set /$toplevel/tx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_out_stdreg_wait
directive set /$toplevel/rx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_in_wire_wait

directive set /$toplevel -EFFORT_LEVEL high
directive set /$toplevel/core -DESIGN_GOAL area

