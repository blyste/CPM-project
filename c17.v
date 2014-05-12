// Verilog
// c17
// Ninputs 5
// Noutputs 2
// NtotalGates 6
// NAND2 6

module c17 (E0,E2,E1,E3,E4,S0,S1);

input E0,E2,E1,E3,E4;

output S0,S1;

wire A,B,C,D;

nand NAND2_1 (A, E0, E1);
nand NAND2_2 (B, E1, E3);
nand NAND2_3 (C, E2, B);
nand NAND2_4 (D, B, E4);
nand NAND2_5 (S0, A, C);
nand NAND2_6 (S1, C, D);

endmodule
