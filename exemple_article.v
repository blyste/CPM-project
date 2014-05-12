// Verilog
// c17
// Ninputs 5
// Noutputs 2
// NtotalGates 6
// NAND2 6

module c17 (I1, I2, I3, I4, I5, K);

input I1, I2, I3, I4, I5;

output K;

wire A, B, C, D, E, F, G, H, I, J;

and caca(A, I1, I2);
and caca(B, I1, I2);
and caca(C, I3, I4);
and caca(D, I3, I4);
and caca(E, B, C);
and caca(F, E, A);
and caca(G, E, D);
and caca(H, D, I5);
and caca(I, G, F);

// I F H ===> K
and caca(J, I, H);
and caca(K, J, F);

endmodule
