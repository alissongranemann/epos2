solution rename XXNAMEXX_Top

directive set /XXNAMEXX_Top/tx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_out_stdreg_wait
directive set /XXNAMEXX_Top/rx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_in_wire_wait

directive set /XXNAMEXX_Top -EFFORT_LEVEL high
directive set /XXNAMEXX_Top/core -DESIGN_GOAL area

