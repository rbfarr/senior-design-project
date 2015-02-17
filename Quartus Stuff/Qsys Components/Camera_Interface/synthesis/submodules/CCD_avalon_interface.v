module CCD_avalon_interface(iclk, irst, CCD_PIXCLK, idata, ifval, ilval, iexposure_dec_p,
         izoom_mode_sw, istart, iend, iexposure_adj, i2c_sclk, i2c_sdat,
			oycont, oframe, odval, ored, ogreen, oblue, oRST_0, oRST_1, oRST_2);

	// signals for connecting to the avalon fabric
	input iclk;
	input irst;

	// input conduit
	input CCD_PIXCLK;
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
	output i2c_sdat;
	output oRST_0;
	output oRST_1;
	output oRST_2;
	output [15:0] oycont;
	output [31:0] oframe;
	output odval;
	output [11:0] ored;
	output [11:0] ogreen;
	output [11:0] oblue;
	
	// internal connections
	wire mdval;
	wire [11:0] mData;
	wire [15:0] mX_Cont;
	wire [15:0] mY_Cont;
	wire mRST_0;
	wire mRST_1;
	wire mRST_2;
	
	assign oycont = mY_Cont;
	
	// Actual CCD Data Interface
	CCD_Capture Cap     (.ICLk(CCD_PIXCLK), .IENd(iend), .IFVal(ifval), .ILVal(ilval),
					     .IRst(irst), .IStart(istart), .IData(idata), .oDval(mdval),
					     .oDATA(mData), .oX_Cont(mX_Cont), .oY_Cont(oycont),
						 .oFrame_Cont(oframe)
					     );
						 
	// CCD I2C Configuration Interface
	I2C_CCD_Config I2C  (.iCLK(iclk), .iEXPOSURE_ADJ(iexposure_adj),
					     .iEXPOSURE_DEC_p(iexposure_dec_p), .iRST_N(mRST_2),
					     .iZOOM_MODE_SW(izoom_mode_sw), .I2C_SCLK(i2c_sclk),
					     .I2C_SDAT(i2c_sdat)
					     );
					   
	// Raw CCD data -> RGB Converter
	RAW2RGB Raw2Rgb     (.iCLK(CCD_PIXCLK), .iDVAL(mdval), .iRST(mRST_1),
						 .iDATA(mData), .oDVAL(odval), .oRed(ored),
						 .oGreen(ogreen), .oBlue(oblue)
						 );
						 
	Reset_Delay RstDly  (.iCLK(iclk), .iRST(irst), .oRST_0(mRST_0),
						 .oRST_1(mRST_1), .oRST_2(mRST_2)
						 );
						 
	// Tidy up reset outputs
	assign oRST_0 = mRST_0;
	assign oRST_1 = mRST_1;
	assign oRST_2 = mRST_2;

endmodule