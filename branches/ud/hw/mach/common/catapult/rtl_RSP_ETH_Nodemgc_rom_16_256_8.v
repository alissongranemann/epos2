// ----------------------------------------------------------------------
//  HLS HDL:        Verilog Netlister
//  HLS Version:    2011a.41 Production Release
//  HLS Date:       Thu Apr  7 20:18:15 PDT 2011
// 
//  Generated by:   tiago@spectrum
//  Generated date: Thu Aug  8 13:59:28 2013
// ----------------------------------------------------------------------

// 
module RSP_ETH_Nodemgc_rom_16_256_8 (addr, data_out
);
  input [7:0]addr ;
  output [7:0]data_out ;

  parameter rom_id = 16;
  parameter size = 256;
  parameter width = 8;

  reg [8-1:0] mem [256-1:0];
  // pragma attribute mem MEM_INIT_BLOCK rom_init_blk
  always@(addr) begin: rom_init_blk
    mem[0] = 8'b10001101;
    mem[1] = 8'b00000001;
    mem[2] = 8'b00000010;
    mem[3] = 8'b00000100;
    mem[4] = 8'b00001000;
    mem[5] = 8'b00010000;
    mem[6] = 8'b00100000;
    mem[7] = 8'b01000000;
    mem[8] = 8'b10000000;
    mem[9] = 8'b00011011;
    mem[10] = 8'b00110110;
    mem[11] = 8'b01101100;
    mem[12] = 8'b11011000;
    mem[13] = 8'b10101011;
    mem[14] = 8'b01001101;
    mem[15] = 8'b10011010;
    mem[16] = 8'b00101111;
    mem[17] = 8'b01011110;
    mem[18] = 8'b10111100;
    mem[19] = 8'b01100011;
    mem[20] = 8'b11000110;
    mem[21] = 8'b10010111;
    mem[22] = 8'b00110101;
    mem[23] = 8'b01101010;
    mem[24] = 8'b11010100;
    mem[25] = 8'b10110011;
    mem[26] = 8'b01111101;
    mem[27] = 8'b11111010;
    mem[28] = 8'b11101111;
    mem[29] = 8'b11000101;
    mem[30] = 8'b10010001;
    mem[31] = 8'b00111001;
    mem[32] = 8'b01110010;
    mem[33] = 8'b11100100;
    mem[34] = 8'b11010011;
    mem[35] = 8'b10111101;
    mem[36] = 8'b01100001;
    mem[37] = 8'b11000010;
    mem[38] = 8'b10011111;
    mem[39] = 8'b00100101;
    mem[40] = 8'b01001010;
    mem[41] = 8'b10010100;
    mem[42] = 8'b00110011;
    mem[43] = 8'b01100110;
    mem[44] = 8'b11001100;
    mem[45] = 8'b10000011;
    mem[46] = 8'b00011101;
    mem[47] = 8'b00111010;
    mem[48] = 8'b01110100;
    mem[49] = 8'b11101000;
    mem[50] = 8'b11001011;
    mem[51] = 8'b10001101;
    mem[52] = 8'b00000001;
    mem[53] = 8'b00000010;
    mem[54] = 8'b00000100;
    mem[55] = 8'b00001000;
    mem[56] = 8'b00010000;
    mem[57] = 8'b00100000;
    mem[58] = 8'b01000000;
    mem[59] = 8'b10000000;
    mem[60] = 8'b00011011;
    mem[61] = 8'b00110110;
    mem[62] = 8'b01101100;
    mem[63] = 8'b11011000;
    mem[64] = 8'b10101011;
    mem[65] = 8'b01001101;
    mem[66] = 8'b10011010;
    mem[67] = 8'b00101111;
    mem[68] = 8'b01011110;
    mem[69] = 8'b10111100;
    mem[70] = 8'b01100011;
    mem[71] = 8'b11000110;
    mem[72] = 8'b10010111;
    mem[73] = 8'b00110101;
    mem[74] = 8'b01101010;
    mem[75] = 8'b11010100;
    mem[76] = 8'b10110011;
    mem[77] = 8'b01111101;
    mem[78] = 8'b11111010;
    mem[79] = 8'b11101111;
    mem[80] = 8'b11000101;
    mem[81] = 8'b10010001;
    mem[82] = 8'b00111001;
    mem[83] = 8'b01110010;
    mem[84] = 8'b11100100;
    mem[85] = 8'b11010011;
    mem[86] = 8'b10111101;
    mem[87] = 8'b01100001;
    mem[88] = 8'b11000010;
    mem[89] = 8'b10011111;
    mem[90] = 8'b00100101;
    mem[91] = 8'b01001010;
    mem[92] = 8'b10010100;
    mem[93] = 8'b00110011;
    mem[94] = 8'b01100110;
    mem[95] = 8'b11001100;
    mem[96] = 8'b10000011;
    mem[97] = 8'b00011101;
    mem[98] = 8'b00111010;
    mem[99] = 8'b01110100;
    mem[100] = 8'b11101000;
    mem[101] = 8'b11001011;
    mem[102] = 8'b10001101;
    mem[103] = 8'b00000001;
    mem[104] = 8'b00000010;
    mem[105] = 8'b00000100;
    mem[106] = 8'b00001000;
    mem[107] = 8'b00010000;
    mem[108] = 8'b00100000;
    mem[109] = 8'b01000000;
    mem[110] = 8'b10000000;
    mem[111] = 8'b00011011;
    mem[112] = 8'b00110110;
    mem[113] = 8'b01101100;
    mem[114] = 8'b11011000;
    mem[115] = 8'b10101011;
    mem[116] = 8'b01001101;
    mem[117] = 8'b10011010;
    mem[118] = 8'b00101111;
    mem[119] = 8'b01011110;
    mem[120] = 8'b10111100;
    mem[121] = 8'b01100011;
    mem[122] = 8'b11000110;
    mem[123] = 8'b10010111;
    mem[124] = 8'b00110101;
    mem[125] = 8'b01101010;
    mem[126] = 8'b11010100;
    mem[127] = 8'b10110011;
    mem[128] = 8'b01111101;
    mem[129] = 8'b11111010;
    mem[130] = 8'b11101111;
    mem[131] = 8'b11000101;
    mem[132] = 8'b10010001;
    mem[133] = 8'b00111001;
    mem[134] = 8'b01110010;
    mem[135] = 8'b11100100;
    mem[136] = 8'b11010011;
    mem[137] = 8'b10111101;
    mem[138] = 8'b01100001;
    mem[139] = 8'b11000010;
    mem[140] = 8'b10011111;
    mem[141] = 8'b00100101;
    mem[142] = 8'b01001010;
    mem[143] = 8'b10010100;
    mem[144] = 8'b00110011;
    mem[145] = 8'b01100110;
    mem[146] = 8'b11001100;
    mem[147] = 8'b10000011;
    mem[148] = 8'b00011101;
    mem[149] = 8'b00111010;
    mem[150] = 8'b01110100;
    mem[151] = 8'b11101000;
    mem[152] = 8'b11001011;
    mem[153] = 8'b10001101;
    mem[154] = 8'b00000001;
    mem[155] = 8'b00000010;
    mem[156] = 8'b00000100;
    mem[157] = 8'b00001000;
    mem[158] = 8'b00010000;
    mem[159] = 8'b00100000;
    mem[160] = 8'b01000000;
    mem[161] = 8'b10000000;
    mem[162] = 8'b00011011;
    mem[163] = 8'b00110110;
    mem[164] = 8'b01101100;
    mem[165] = 8'b11011000;
    mem[166] = 8'b10101011;
    mem[167] = 8'b01001101;
    mem[168] = 8'b10011010;
    mem[169] = 8'b00101111;
    mem[170] = 8'b01011110;
    mem[171] = 8'b10111100;
    mem[172] = 8'b01100011;
    mem[173] = 8'b11000110;
    mem[174] = 8'b10010111;
    mem[175] = 8'b00110101;
    mem[176] = 8'b01101010;
    mem[177] = 8'b11010100;
    mem[178] = 8'b10110011;
    mem[179] = 8'b01111101;
    mem[180] = 8'b11111010;
    mem[181] = 8'b11101111;
    mem[182] = 8'b11000101;
    mem[183] = 8'b10010001;
    mem[184] = 8'b00111001;
    mem[185] = 8'b01110010;
    mem[186] = 8'b11100100;
    mem[187] = 8'b11010011;
    mem[188] = 8'b10111101;
    mem[189] = 8'b01100001;
    mem[190] = 8'b11000010;
    mem[191] = 8'b10011111;
    mem[192] = 8'b00100101;
    mem[193] = 8'b01001010;
    mem[194] = 8'b10010100;
    mem[195] = 8'b00110011;
    mem[196] = 8'b01100110;
    mem[197] = 8'b11001100;
    mem[198] = 8'b10000011;
    mem[199] = 8'b00011101;
    mem[200] = 8'b00111010;
    mem[201] = 8'b01110100;
    mem[202] = 8'b11101000;
    mem[203] = 8'b11001011;
    mem[204] = 8'b10001101;
    mem[205] = 8'b00000001;
    mem[206] = 8'b00000010;
    mem[207] = 8'b00000100;
    mem[208] = 8'b00001000;
    mem[209] = 8'b00010000;
    mem[210] = 8'b00100000;
    mem[211] = 8'b01000000;
    mem[212] = 8'b10000000;
    mem[213] = 8'b00011011;
    mem[214] = 8'b00110110;
    mem[215] = 8'b01101100;
    mem[216] = 8'b11011000;
    mem[217] = 8'b10101011;
    mem[218] = 8'b01001101;
    mem[219] = 8'b10011010;
    mem[220] = 8'b00101111;
    mem[221] = 8'b01011110;
    mem[222] = 8'b10111100;
    mem[223] = 8'b01100011;
    mem[224] = 8'b11000110;
    mem[225] = 8'b10010111;
    mem[226] = 8'b00110101;
    mem[227] = 8'b01101010;
    mem[228] = 8'b11010100;
    mem[229] = 8'b10110011;
    mem[230] = 8'b01111101;
    mem[231] = 8'b11111010;
    mem[232] = 8'b11101111;
    mem[233] = 8'b11000101;
    mem[234] = 8'b10010001;
    mem[235] = 8'b00111001;
    mem[236] = 8'b01110010;
    mem[237] = 8'b11100100;
    mem[238] = 8'b11010011;
    mem[239] = 8'b10111101;
    mem[240] = 8'b01100001;
    mem[241] = 8'b11000010;
    mem[242] = 8'b10011111;
    mem[243] = 8'b00100101;
    mem[244] = 8'b01001010;
    mem[245] = 8'b10010100;
    mem[246] = 8'b00110011;
    mem[247] = 8'b01100110;
    mem[248] = 8'b11001100;
    mem[249] = 8'b10000011;
    mem[250] = 8'b00011101;
    mem[251] = 8'b00111010;
    mem[252] = 8'b01110100;
    mem[253] = 8'b11101000;
    mem[254] = 8'b11001011;
    mem[255] = 8'b10001101;
  end

  reg [8-1:0] data_out_t;
  always@(addr)
  begin
    data_out_t = mem[addr];
  end
  assign data_out = data_out_t;

endmodule


