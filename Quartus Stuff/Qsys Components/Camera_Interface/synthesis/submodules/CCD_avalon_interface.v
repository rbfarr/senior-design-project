module CCD_avalon_interface(iclk, iswitches, ikeys, iogpio, ovga_rst, omem_rst, 
							ored, ogreen, oblue, odval, oy_cont, oframe_cont, orclk, opixclk);
//module CCD_avalon_interface(iclk, irst, CCD_PIXCLK, idata, ifval, ilval, iexposure_dec_p,
//         izoom_mode_sw, istart, iend, iexposure_adj, i2c_sclk, i2c_sdat,/
//			oycont, oframe, odval, ored, ogreen, oblue, oRST_0, oRST_1, oRST_2);
	
	//=========================================================================
	// Input Declarations
	//=========================================================================
	input			iclk;
	input	[9:0]	iswitches;
	input	[3:0]	ikeys;
	
	//=========================================================================
	// Inout Declarations
	//=========================================================================
	inout	[35:0]	iogpio;
	
	//=========================================================================
	// Output Declarations
	//=========================================================================
	output			ovga_rst;
	output			omem_rst;
	output	[11:0]	ored;
	output	[11:0]	ogreen;
	output	[11:0]	oblue;
	output			odval;
	output	[15:0]	oy_cont;
	output	[31:0]	oframe_cont;
	output	[1:0]	orclk;
	output			opixclk;

	//=========================================================================
	// Register Declarations
	//=========================================================================
	reg		[11:0]	rCCD_DATA;
	reg				rCCD_LVAL;
	reg				rCCD_FVAL;
	reg		[1:0]	rClk;
	
	//=========================================================================
	// Wire Declarations
	//=========================================================================
	wire	[11:0]	CCD_DATA;
	wire			CCD_SDAT;
	wire			CCD_SCLK;
	wire			CCD_FLASH;
	wire			CCD_FVAL;
	wire			CCD_LVAL;
	wire			CCD_PIXCLK;
	wire			CCD_MCLK;				//	CCD Master Clock
	
	wire	[11:0]	mCCD_DATA;
	wire			mCCD_DVAL;
	wire			mCCD_DVAL_d;
	wire	[15:0]	X_Cont;
	wire	[15:0]	Y_Cont;
	wire	[9:0]	X_ADDR;
	wire	[31:0]	Frame_Cont;
	wire			DLY_RST_0;
	wire			DLY_RST_1;
	wire			DLY_RST_2;
	
	//=============================================================================
	// Structural assignments
	//=============================================================================
	assign	CCD_DATA[0]	=	iogpio[13];
	assign	CCD_DATA[1]	=	iogpio[12];
	assign	CCD_DATA[2]	=	iogpio[11];
	assign	CCD_DATA[3]	=	iogpio[10];
	assign	CCD_DATA[4]	=	iogpio[9];
	assign	CCD_DATA[5]	=	iogpio[8];
	assign	CCD_DATA[6]	=	iogpio[7];
	assign	CCD_DATA[7]	=	iogpio[6];
	assign	CCD_DATA[8]	=	iogpio[5];
	assign	CCD_DATA[9]	=	iogpio[4];
	assign	CCD_DATA[10]=	iogpio[3];
	assign	CCD_DATA[11]=	iogpio[1];
	assign	CCD_MCLK	=	rClk[0];
	assign	iogpio[16]	=	CCD_MCLK;
	assign	CCD_FVAL	   =	iogpio[22];
	assign	CCD_LVAL	   =	iogpio[21];
	assign	CCD_PIXCLK	=	iogpio[0];
	assign	opixclk		=	CCD_PIXCLK;
	assign	iogpio[19]	=	1'b1;  // tRIGGER
	assign	iogpio[17]	=	DLY_RST_1;
	
	assign	orclk		=	rClk;
	assign	oy_cont		=	Y_Cont;
	assign	oframe_cont	=	Frame_Cont;
	assign	ovga_rst	=	DLY_RST_2;
	assign	omem_rst	=	DLY_RST_0;
	
	//=============================================================================
	// Internal data modification
	//=============================================================================
	always @ (posedge iclk)	rClk <= rClk + 1;
	
	always @ (posedge CCD_PIXCLK)
	begin
		rCCD_DATA	<=	CCD_DATA;
		rCCD_LVAL	<=	CCD_LVAL;
		rCCD_FVAL	<=	CCD_FVAL;
	end
	
	//=============================================================================
	// Instance Declarations
	//=============================================================================
	
	Reset_Delay		rst	(	
							.iCLK  (iclk),
							.iRST  (ikeys[0]),
							.oRST_0(DLY_RST_0),
							.oRST_1(DLY_RST_1),
							.oRST_2(DLY_RST_2)
						);
	
	CCD_Capture		ccd	(	
							.oDATA      (mCCD_DATA),
							.oDVAL      (mCCD_DVAL),
							.oX_Cont    (X_Cont),
							.oY_Cont    (Y_Cont),
							.oFrame_Cont(Frame_Cont),
							.iDATA      (rCCD_DATA),
							.iFVAL      (rCCD_FVAL),
							.iLVAL      (rCCD_LVAL),
							.iSTART     (!ikeys[3]),
							.iEND       (!ikeys[2]),
							.iCLK       (CCD_PIXCLK),
							.iRST       (DLY_RST_2)
						);
	
	RAW2RGB			rgb	(	
						    .iCLK   (CCD_PIXCLK),
							.iRST   (DLY_RST_1),
							.iDATA  (mCCD_DATA),
							.iDVAL  (mCCD_DVAL),
							.oRed   (ored),
							.oGreen (ogreen),
							.oBlue  (oblue),
							.oDVAL  (odval),
							.iX_Cont(X_Cont),
							.iY_Cont(Y_Cont)
						);
						
	I2C_CCD_Config 	i2c	(	//	Host Side
							.iCLK				(iclk),
							.iRST_N         	(DLY_RST_2),
							.iZOOM_MODE_SW  	(iswitches[8]),
							.iEXPOSURE_ADJ  	(ikeys[1]),
							.iEXPOSURE_DEC_p	(iswitches[0]),
							//	I2C Side
							.I2C_SCLK		  	(iogpio[24]),
							.I2C_SDAT		  	(iogpio[23])
							   );
						 
endmodule