//////////////////////////////////////////////////////////////////////////////
//
//  Copyright \251 Mentor Graphics Corporation, 1996-2004, All Rights Reserved.
//                       UNPUBLISHED, LICENSED SOFTWARE.
//            CONFIDENTIAL AND PROPRIETARY INFORMATION WHICH IS THE
//          PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS.
//
//////////////////////////////////////////////////////////////////////////////
module dualport_ram_be ( data_in, addr, re, we, data_out, clk, a_rst, s_rst, en);

parameter ram_id          = 1;
parameter words           = 'd256;
parameter width           = 8;
parameter addr_width      = 8;
parameter [0:0] a_reset_active  = 0;
parameter [0:0] s_reset_active  = 1;
parameter [0:0] enable_active   = 1;
parameter [0:0] re_active   = 1;
parameter [0:0] we_active   = 1;
parameter num_byte_enables = 1;
parameter clock_edge = 1;
parameter num_input_registers         = 2;
parameter num_output_registers        = 1;
parameter no_of_dualport_readwrite_port  = 2;

localparam byte_width = width / num_byte_enables;

  input [(width*no_of_dualport_readwrite_port)-1:0] data_in;
  input [(addr_width*no_of_dualport_readwrite_port)-1:0] addr;
  input [(no_of_dualport_readwrite_port*num_byte_enables)-1:0] re;
  input [(no_of_dualport_readwrite_port*num_byte_enables)-1:0] we;
  output [(width*no_of_dualport_readwrite_port)-1:0] data_out;
  input clk;
  input a_rst;
  input s_rst;
  input en;

  reg  [width-1:0] mem [words-1:0];
  // pragma attribute mem block_ram 1

  wire [num_byte_enables-1:0] rea;
  wire [num_byte_enables-1:0] reb;
  wire [num_byte_enables-1:0] wea;
  wire [num_byte_enables-1:0] web;
  wire [width-1:0] data_ina;
  wire [width-1:0] data_inb;
  reg [width-1:0] data_outa;
  reg [width-1:0] data_outb;

  wire [addr_width-1:0] addra;
  wire [addr_width-1:0] addrb;

  reg [addr_width-1:0] addra_reg;
  reg [addr_width-1:0] addrb_reg;

  reg [num_byte_enables-1:0] rea_reg;
  reg [num_byte_enables-1:0] reb_reg;

// synopsys translate_off
  integer count;
  initial
  begin
    for (count = 0; count < words; count = count + 1) 
      mem[count] = 0;
  end
// synopsys translate_on

  assign addra = addr[(2*addr_width)-1:addr_width];
  assign addrb = addr[addr_width-1:0];

  assign rea = re[((1*num_byte_enables)+num_byte_enables)-1:1*num_byte_enables];
  assign reb = re[((0*num_byte_enables)+num_byte_enables)-1:0*num_byte_enables];
  assign wea = we[((1*num_byte_enables)+num_byte_enables)-1:1*num_byte_enables];
  assign web = we[((0*num_byte_enables)+num_byte_enables)-1:0*num_byte_enables];

  assign data_ina = data_in[(2*width)-1:width];
  assign data_inb = data_in[width-1:0];

  reg [width-1:0] data_ina_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [width-1:0] data_inb_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [addr_width-1:0] addra_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [addr_width-1:0] addrb_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [num_byte_enables-1:0] rea_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [num_byte_enables-1:0] reb_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [num_byte_enables-1:0] wea_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];
  reg [num_byte_enables-1:0] web_inreg [((num_input_registers > 1) ? num_input_registers - 2 : 0): 0];

  genvar i;
  generate
  begin : pipe
    if (num_input_registers > 1)
    begin
      for( i=num_input_registers-2; i >= 1; i = i - 1)
      begin : SHIFT_MSBs // Verilog 2000 syntax requires that GENERATE_LOOP_STATEMENT be a named block
                         // Modelsim seems to allow it though it should be a bug
        wire [width-1:0] data_ina_i, data_inb_i;
        wire [addr_width-1:0] addra_i, addrb_i;
        wire [num_byte_enables-1:0] rea_i, reb_i;
        wire [num_byte_enables-1:0] wea_i, web_i;
  
        mgc_generic_reg #(width,      clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_data_ina_inreg (data_ina_inreg[i-1], clk, en, a_rst, s_rst, data_ina_i);
        mgc_generic_reg #(width,      clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_data_inb_inreg (data_inb_inreg[i-1], clk, en, a_rst, s_rst, data_inb_i);
        mgc_generic_reg #(addr_width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_addra_inreg    (addra_inreg[i-1],    clk, en, a_rst, s_rst, addra_i);
        mgc_generic_reg #(addr_width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_addrb_inreg    (addrb_inreg[i-1],    clk, en, a_rst, s_rst, addrb_i);
        mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_rea_inreg      (rea_inreg[i-1],      clk, en, a_rst, s_rst, rea_i);
        mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_reb_inreg      (reb_inreg[i-1],      clk, en, a_rst, s_rst, reb_i);
        mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_wea_inreg      (wea_inreg[i-1],      clk, en, a_rst, s_rst, wea_i);
        mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b0) i_web_inreg      (web_inreg[i-1],      clk, en, a_rst, s_rst, web_i);
  
        always@(data_ina_i)
          data_ina_inreg[i] = data_ina_i;
        always@(data_inb_i)
          data_inb_inreg[i] = data_inb_i;
  
        always@(addra_i)
          addra_inreg[i] = addra_i;
        always@(addrb_i)
          addrb_inreg[i] = addrb_i;
  
        always@(rea_i)
          rea_inreg[i] = rea_i;
        always@(reb_i)
          reb_inreg[i] = reb_i;
        always@(wea_i)
          wea_inreg[i] = wea_i;
        always@(web_i)
          web_inreg[i] = web_i;
      end // end for loop

      // SHIFT for the first stage
      wire [width-1:0] data_ina_init, data_inb_init;
      wire [addr_width-1:0] addra_init, addrb_init;
      wire [num_byte_enables-1:0] rea_init, reb_init;
      wire [num_byte_enables-1:0] wea_init, web_init;
    
      mgc_generic_reg #(width,      clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_ina_inreg_i (data_ina, clk, en, a_rst, s_rst, data_ina_init);
      mgc_generic_reg #(width,      clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_inb_inreg_i (data_inb, clk, en, a_rst, s_rst, data_inb_init);
      mgc_generic_reg #(addr_width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_addra_inreg_i    (addra,    clk, en, a_rst, s_rst, addra_init);
      mgc_generic_reg #(addr_width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_addrb_inreg_i    (addrb,    clk, en, a_rst, s_rst, addrb_init);
      mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_rea_inreg_i      (rea,      clk, en, a_rst, s_rst, rea_init);
      mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_reb_inreg_i      (reb,      clk, en, a_rst, s_rst, reb_init);
      mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_wea_inreg_i      (wea,      clk, en, a_rst, s_rst, wea_init);
      mgc_generic_reg #(num_byte_enables, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_web_inreg_i      (web,      clk, en, a_rst, s_rst, web_init);
    
      always@(data_ina_init)
        data_ina_inreg[0] = data_ina_init;
      always@(data_inb_init)
        data_inb_inreg[0] = data_inb_init;
    
      always@(addra_init)
        addra_inreg[0] = addra_init;
      always@(addrb_init)
        addrb_inreg[0] = addrb_init;
    
      always@(rea_init)
        rea_inreg[0] = rea_init;
      always@(reb_init)
        reb_inreg[0] = reb_init;

      always@(wea_init)
        wea_inreg[0] = wea_init;
      always@(web_init)
        web_inreg[0] = web_init;
    end
  end
  endgenerate

  wire [width-1:0]      data_ina_f = (num_input_registers > 1) ? data_ina_inreg[num_input_registers-2] : data_ina;
  wire [width-1:0]      data_inb_f = (num_input_registers > 1) ? data_inb_inreg[num_input_registers-2] : data_inb;
  wire [addr_width-1:0] addra_f    = (num_input_registers > 1) ? addra_inreg[num_input_registers-2]    : addra;
  wire [addr_width-1:0] addrb_f    = (num_input_registers > 1) ? addrb_inreg[num_input_registers-2]    : addrb;
  wire [num_byte_enables-1:0]            rea_f      = (num_input_registers > 1) ? rea_inreg[num_input_registers-2]      : rea;
  wire [num_byte_enables-1:0]            reb_f      = (num_input_registers > 1) ? reb_inreg[num_input_registers-2]      : reb;
  wire [num_byte_enables-1:0]            wea_f      = (num_input_registers > 1) ? wea_inreg[num_input_registers-2]      : wea;
  wire [num_byte_enables-1:0]            web_f      = (num_input_registers > 1) ? web_inreg[num_input_registers-2]      : web;

  reg addra_NotDontCare;
  reg addrb_NotDontCare;
  integer j, k, l, m, n, o, p, q;

  generate
    if (clock_edge == 1'b1)
      begin: POSEDGE_BLK
        always @(posedge clk)
        begin
          if ( en == enable_active )
          begin
            addra_NotDontCare = 1'b1;
            begin: blka_X
              for (j = 0; j < addr_width; j = j + 1)
              begin
                if ((addra_f[j] === 1'bX) || (addra_f[j] === 1'bZ))
                begin
                  addra_NotDontCare = 1'b0;
                  for (k = 0; k < num_byte_enables; k = k + 1)
                  begin
                    if (wea_f[k] == we_active) begin
                      $display($time, " Error: Dont-care addresses for pipelined dualport memory write on port A");
                      $finish;
                    end
                  end
                  disable blka_X;
                end
              end
            end
                                                                                        
            addra_reg <= addra_f;
            rea_reg   <= rea_f;

            if (addra_NotDontCare)
            begin
              if ((addra_f < words) && (addra_f >= 0)) begin
                for (j = 0; j < num_byte_enables; j = j + 1)
                begin
                  if (wea_f[j] == we_active) begin
                    // mem[addra_f] <= data_ina_f;
                    mem[addra_f][((j*byte_width)+byte_width)-1-:byte_width] <= data_ina_f[((j*byte_width)+byte_width)-1-:byte_width];
                  end
                end
              end else begin
                for (j = 0; j < num_byte_enables; j = j + 1)
                begin
                  if (wea_f[j] == we_active) begin
                    $display($time, " Error: Out-of-bound pipelined dualport memory-write on port A");
                    $finish;
                  end
                end
              end
            end
          end
        end
        
        always @(posedge clk)
        begin
          if ( en == enable_active )
          begin
            addrb_NotDontCare = 1'b1;
            begin: blkb_X
              for (l = 0; l < addr_width; l = l + 1)
              begin
                if ((addrb_f[l] === 1'bX) || (addrb_f[l] === 1'bZ))
                begin
                  addrb_NotDontCare = 1'b0;
                  for (m = 0; m < num_byte_enables; m = m + 1)
                  begin
                    if (web_f[m] == we_active) begin
                      $display($time, " Error: Dont-care addresses for pipelined dualport memory write on port B");
                      $finish;
                    end
                  end
                  disable blkb_X;
                end
              end
            end
                                                                                        
            addrb_reg <= addrb_f;
            reb_reg   <= reb_f;

            if (addrb_NotDontCare)
            begin
              if ((addrb_f < words) && (addrb_f >= 0)) begin
                for (l = 0; l < num_byte_enables; l = l + 1)
                begin
                  if (web_f[l] == we_active) begin
                    // mem[addrb_f] <= data_inb_f;
                    mem[addrb_f][((l*byte_width)+byte_width)-1-:byte_width] <= data_inb_f[((l*byte_width)+byte_width)-1-:byte_width];
                  end
                end
              end else begin
                for (l = 0; l < num_byte_enables; l = l + 1)
                begin
                  if (web_f[l] == we_active) begin
                    $display($time, " Error: Out-of-bound pipelined dualport memory-write on port B");
                    $finish;
                  end
                end
              end
            end
          end
        end
      end
    else
      begin: NEGEDGE_BLK
        always @(negedge clk)
        begin
          if ( en == enable_active )
          begin
            addra_NotDontCare = 1'b1;
            begin: blka_X
              for (n = 0; n < addr_width; n = n + 1)
              begin
                if ((addra_f[n] === 1'bX) || (addra_f[n] === 1'bZ))
                begin
                  addra_NotDontCare = 1'b0;
                  for (o = 0; o < num_byte_enables; o = o + 1)
                  begin
                    if (wea_f[o] == we_active) begin
                      $display($time, " Error: Dont-care addresses for pipelined dualport memory write on port A");
                      $finish;
                    end
                  end
                  disable blka_X;
                end
              end
            end
                                                                                        
            addra_reg <= addra_f;
            rea_reg   <= rea_f;

            if (addra_NotDontCare)
            begin
              if ((addra_f < words) && (addra_f >= 0)) begin
                for (n = 0; n < num_byte_enables; n = n + 1)
                begin
                  if (wea_f[n] == we_active) begin
                    // mem[addra_f] <= data_ina_f;
                    mem[addra_f][((n*byte_width)+byte_width)-1-:byte_width] <= data_ina_f[((n*byte_width)+byte_width)-1-:byte_width];
                  end
                end
              end else begin
                for (n = 0; n < num_byte_enables; n = n + 1)
                begin
                  if (wea_f[n] == we_active) begin
                    $display($time, " Error: Out-of-bound pipelined dualport memory-write on port A");
                    $finish;
                  end
                end
              end
            end
          end
        end
        
        always @(negedge clk)
        begin
          if ( en == enable_active )
          begin
            addrb_NotDontCare = 1'b1;
            begin: blkb_X
              for (p = 0; p < addr_width; p = p + 1)
              begin
                if ((addrb_f[p] === 1'bX) || (addrb_f[p] === 1'bZ))
                begin
                  addrb_NotDontCare = 1'b0;
                  for (q = 0; q < num_byte_enables; q = q + 1)
                  begin
                    if (web_f[q] == we_active) begin
                      $display($time, " Error: Dont-care addresses for pipelined dualport memory write on port B");
                      $finish;
                    end
                  end
                  disable blkb_X;
                end
              end
            end
                                                                                        
            addrb_reg <= addrb_f;
            reb_reg   <= reb_f;

            if (addrb_NotDontCare)
            begin
              if ((addrb_f < words) && (addrb_f >= 0)) begin
                for (p = 0; p < num_byte_enables; p = p + 1)
                begin
                  if (web_f[p] == we_active) begin
                    // mem[addrb_f] <= data_inb_f;
                    mem[addrb_f][((p*byte_width)+byte_width)-1-:byte_width] <= data_inb_f[((p*byte_width)+byte_width)-1-:byte_width];
                  end
                end
              end else begin
                for (p = 0; p < num_byte_enables; p = p + 1)
                begin
                  if (web_f[p] == we_active) begin
                    $display($time, " Error: Out-of-bound pipelined dualport memory-write on port B");
                    $finish;
                  end
                end
              end
            end
          end
        end
      end
  endgenerate

  integer r;
  always@(
          mem[addra_reg]
          or addra_NotDontCare
          or addra_reg
          or rea_reg
          )
  begin
    if (addra_NotDontCare)
      if ((addra_reg < words) && (addra_reg >= 0))
        for (r = 0; r < num_byte_enables; r = r + 1)
        begin
	  if ( rea_reg[r] == re_active )
            data_outa[((r*byte_width) + byte_width) - 1 -: byte_width] <= mem[addra_reg][((r*byte_width) + byte_width) - 1 -: byte_width];
          else
            data_outa[((r*byte_width) + byte_width) - 1 -: byte_width] <= {(byte_width){1'bX}};
        end
      else
        data_outa <= {width{1'bX}};
    else
      data_outa <= {width{1'bX}};
  end


  integer s;
  always@(
          mem[addrb_reg]
          or addrb_NotDontCare
          or addrb_reg
          or reb_reg
          )
  begin
    if (addrb_NotDontCare)
      if ((addrb_reg < words) && (addrb_reg >= 0))
        for (s = 0; s < num_byte_enables; s = s + 1)
        begin
	  if ( reb_reg[s] == re_active )
            data_outb[((s*byte_width) + byte_width) - 1 -: byte_width] <= mem[addrb_reg][((s*byte_width) + byte_width) - 1 -: byte_width];
          else
            data_outb[((s*byte_width) + byte_width) - 1 -: byte_width] <= {(byte_width){1'bX}};
        end
      else
        data_outb <= {width{1'bX}};
    else
      data_outb <= {width{1'bX}};
  end

  reg [width-1:0] data_outa_outreg [((num_output_registers > 0) ? num_output_registers - 1 : 0): 0];
  reg [width-1:0] data_outb_outreg [((num_output_registers > 0) ? num_output_registers - 1 : 0): 0];

  genvar t;
  generate
  begin : outblk
    if (num_output_registers > 0)
    begin
      for( t = num_output_registers-1; t >= 1; t = t - 1)
      begin : SHIFT_MSBs // Verilog 2000 syntax requires that GENERATE_LOOP_STATEMENT be a named block
                         // Modelsim seems to allow it though it should be a bug
        wire [width-1:0] data_outa_i, data_outb_i;
        mgc_generic_reg #(width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_outa_outreg (data_outa_outreg[t-1], clk, en, a_rst, s_rst, data_outa_i);
        mgc_generic_reg #(width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_outb_outreg (data_outb_outreg[t-1], clk, en, a_rst, s_rst, data_outb_i);
  
        always@(data_outa_i)
          data_outa_outreg[t] = data_outa_i;
        always@(data_outb_i)
          data_outb_outreg[t] = data_outb_i;
      end // end for loop

      // SHIFT for the first stage
      wire [width-1:0] data_outa_init, data_outb_init;
      mgc_generic_reg #(width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_outa_outreg_i (data_outa, clk, en, a_rst, s_rst, data_outa_init);
      mgc_generic_reg #(width, clock_edge, enable_active, a_reset_active, s_reset_active, 1'b0, 1'b0, 1'b1) i_data_outb_outreg_i (data_outb, clk, en, a_rst, s_rst, data_outb_init);

      always@(data_outa_init)
        data_outa_outreg[0] = data_outa_init;
      always@(data_outb_init)
        data_outb_outreg[0] = data_outb_init;
    end
  end
  endgenerate

  assign data_out = (num_output_registers > 0) ? {data_outa_outreg[num_output_registers-1], data_outb_outreg[num_output_registers-1]} : {data_outa,data_outb};

endmodule
