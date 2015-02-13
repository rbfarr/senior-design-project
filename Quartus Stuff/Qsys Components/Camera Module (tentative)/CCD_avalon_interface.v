module CCD_avalon_interface(iclk, irst, idata, ifval, ilval, iexposure_dec_p,
         izoom_mode_sw, istart, iend, iexposure_adj, i2c_sclk, i2c_sdat,
			oycont, oframe, odval, ored, ogreen, oblue);

	// signals for connecting to the avalon fabric
	input iclk;
	input irst;

	// input conduit
	input [11:0] idata;
	input ifval;
	input ilval;
	input istart;
	input iend;
	input iexposure_dec_p;
	input iexposure_adj;
	input izoom_mode_sw;

	// output conduit
	output i2c_sclk;
	output i2c_sdata
	output [15:0] oycont;
	output [31:0] oframe;
	output odval;
	output [11:0] ored;
	output [11:0] ogreen;
	output [11:0] oblue;
	
	// internal connections
	wire mdval;
	wire [15:0] mX_Cont;
	
	CCD_Capture Cap(.ICLk(iclk), .IENd(iend), .IFVal(ifval), .ILVal(ilval),
						 .IRst(irst), .IStart(istart), .IData(idata), .oDval(mdval), .oX_Cont(mX_Cont)
						 );

endmodule