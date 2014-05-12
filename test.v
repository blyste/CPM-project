module test (E0,E1,E2,S);

input E0,E1;

output S;

wire A;

nand NAND(S, E0, A);
nand NAND(A, E0, E1);

endmodule
