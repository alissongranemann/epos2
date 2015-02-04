// ----------------------------------------------------------------------
//  HLS HDL:        Verilog Netlister
//  HLS Version:    2011a.41 Production Release
//  HLS Date:       Thu Apr  7 20:18:15 PDT 2011
// 
//  Generated by:   tiago@spectrum
//  Generated date: Tue Aug  6 19:06:51 2013
// ----------------------------------------------------------------------

// 
module Dummy_Caller_Nodemgc_rom_24_89_15 (addr, data_out
);
  input [6:0]addr ;
  output [14:0]data_out ;

  parameter rom_id = 24;
  parameter size = 89;
  parameter width = 15;

  reg [15-1:0] mem [89-1:0];
  // pragma attribute mem MEM_INIT_BLOCK rom_init_blk
  always@(addr) begin: rom_init_blk
    mem[0] = 15'b000000000000111;
    mem[1] = 15'b000000000001000;
    mem[2] = 15'b000000000001001;
    mem[3] = 15'b000000000001010;
    mem[4] = 15'b000000000001011;
    mem[5] = 15'b000000000001100;
    mem[6] = 15'b000000000001101;
    mem[7] = 15'b000000000001110;
    mem[8] = 15'b000000000010000;
    mem[9] = 15'b000000000010001;
    mem[10] = 15'b000000000010011;
    mem[11] = 15'b000000000010101;
    mem[12] = 15'b000000000010111;
    mem[13] = 15'b000000000011001;
    mem[14] = 15'b000000000011100;
    mem[15] = 15'b000000000011111;
    mem[16] = 15'b000000000100010;
    mem[17] = 15'b000000000100101;
    mem[18] = 15'b000000000101001;
    mem[19] = 15'b000000000101101;
    mem[20] = 15'b000000000110010;
    mem[21] = 15'b000000000110111;
    mem[22] = 15'b000000000111100;
    mem[23] = 15'b000000001000010;
    mem[24] = 15'b000000001001001;
    mem[25] = 15'b000000001010000;
    mem[26] = 15'b000000001011000;
    mem[27] = 15'b000000001100001;
    mem[28] = 15'b000000001101011;
    mem[29] = 15'b000000001110110;
    mem[30] = 15'b000000010000010;
    mem[31] = 15'b000000010001111;
    mem[32] = 15'b000000010011101;
    mem[33] = 15'b000000010101101;
    mem[34] = 15'b000000010111110;
    mem[35] = 15'b000000011010001;
    mem[36] = 15'b000000011100110;
    mem[37] = 15'b000000011111101;
    mem[38] = 15'b000000100010111;
    mem[39] = 15'b000000100110011;
    mem[40] = 15'b000000101010001;
    mem[41] = 15'b000000101110011;
    mem[42] = 15'b000000110011000;
    mem[43] = 15'b000000111000001;
    mem[44] = 15'b000000111101110;
    mem[45] = 15'b000001000100000;
    mem[46] = 15'b000001001010110;
    mem[47] = 15'b000001010010010;
    mem[48] = 15'b000001011010100;
    mem[49] = 15'b000001100011100;
    mem[50] = 15'b000001101101100;
    mem[51] = 15'b000001111000011;
    mem[52] = 15'b000010000100100;
    mem[53] = 15'b000010010001110;
    mem[54] = 15'b000010100000010;
    mem[55] = 15'b000010110000011;
    mem[56] = 15'b000011000010000;
    mem[57] = 15'b000011010101011;
    mem[58] = 15'b000011101010110;
    mem[59] = 15'b000100000010010;
    mem[60] = 15'b000100011100000;
    mem[61] = 15'b000100111000011;
    mem[62] = 15'b000101010111101;
    mem[63] = 15'b000101111010000;
    mem[64] = 15'b000110011111111;
    mem[65] = 15'b000111001001100;
    mem[66] = 15'b000111110111010;
    mem[67] = 15'b001000101001100;
    mem[68] = 15'b001001100000111;
    mem[69] = 15'b001010011101110;
    mem[70] = 15'b001011100000110;
    mem[71] = 15'b001100101010100;
    mem[72] = 15'b001101111011100;
    mem[73] = 15'b001111010100101;
    mem[74] = 15'b010000110110110;
    mem[75] = 15'b010010100010101;
    mem[76] = 15'b010100011001010;
    mem[77] = 15'b010110011011111;
    mem[78] = 15'b011000101011011;
    mem[79] = 15'b011011001001011;
    mem[80] = 15'b011101110111001;
    mem[81] = 15'b100000110110010;
    mem[82] = 15'b100100001000100;
    mem[83] = 15'b100111101111110;
    mem[84] = 15'b101011101110001;
    mem[85] = 15'b110000000101111;
    mem[86] = 15'b110100111001110;
    mem[87] = 15'b111010001100010;
    mem[88] = 15'b111111111111111;
  end

  reg [15-1:0] data_out_t;
  reg [6:0] addr_mx;
  always@(addr)
  begin
    if (addr >= 0 && addr < 89)
      addr_mx = addr;
    else
      addr_mx = {1'b0, addr[5:0]};
    data_out_t = mem[addr_mx];
  end
  assign data_out = data_out_t;

endmodule


