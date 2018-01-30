/* Quartus II 64-Bit Version 15.0.0 Build 145 04/22/2015 Patches 0.01we SJ Web Edition */
JedecChain;
	FileRevision(JESD32A);
	DefaultMfr(6E);

	P ActionCode(Ign)
		Device PartName(SOCVHPS) MfrSpec(OpMask(0));
	P ActionCode(Cfg)
		Device PartName(5CSEMA5F31) Path("/home/guoj/Documents/Year-3/CPEN-391/repo/output_files/") File("cpen391_group5.sof") MfrSpec(OpMask(1));

ChainEnd;

AlteraBegin;
	ChainType(JTAG);
AlteraEnd;
