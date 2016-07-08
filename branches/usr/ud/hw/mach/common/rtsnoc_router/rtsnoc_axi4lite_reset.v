module rtsnoc_axi4lite_reset(
    axi_rst_i,
    noc_reset_o
);

    
    input wire axi_rst_i; 
    output wire noc_reset_o;
    
    
    assign noc_reset_o = ~axi_rst_i;
    
endmodule
