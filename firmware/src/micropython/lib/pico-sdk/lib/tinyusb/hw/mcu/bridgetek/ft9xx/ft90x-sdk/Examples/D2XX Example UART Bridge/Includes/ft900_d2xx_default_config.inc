/**< TD2XX_DeviceConfiguration */

__pD2XXDefaultConfiguration:
D2XX_Struct_start = .
Signature: 		.long 0xF7D1DCF6
ConfigVersion:		.word 0x0100 		/**< structure version 1.0 */
DelayAfterPHYEn: 	.word 0x0000 		/**< delay to be inserted whenever PHY is enabled */
VendorID: 		.word 0x0403
ProductID: 		.word 0x6033
BCDEnable:		.byte	0		/**< [0]=disable, 1=enable */
DFUCapable:		.byte	1		/**< [0]=disable, 1=enable */
SelfPowered:		.byte	1		/**< 0=disable, [1]=enable */
MaxPower:		.byte	0xFA		/**< 0xFA=500mA */
NumOfD2XXInterfaces:	.byte	3		/**< 3 (min=1..max=3) */
RMWKUPEnable: 		.byte	0		/**< [0]=disable, 1=enable */
MaxTransferSize: 	.word	0x0400, 0x0400, 0x0400, 0, 0, 0, 0
Strings:	/**< strings are preceded by length byte */
Strings_start = .
			.byte	0x04,'F','T','D','I'						/**< manufacturer string */
			.byte	0x0A,'F','T','9','0','0',' ','D','2','X','X' 			/**< product description */
			.byte	0x0E,'F','T','9','0','0','S','e','r','i','a','l','#','0','3' 	/**< Serial number. Maximum should be limited  to 14 characters. */
			.byte	0x15,'D','F','U',' ','R','u','n','t','i','m','e',' ','I','n','t','e','r','f','a','c','e'  /* dfu description */
			.skip	(0x80-(. - Strings_start))
DfuDeviceInterfaceGUID:	/**< Default DeviceInterfaceGUID {47C2B3D7-CF33-4423-86FA-2480C412FB5D} */
			.byte	'{'						/* { */
			.byte	0x34, 0x37, 0x43, 0x32, 0x42, 0x33, 0x44, 0x37		/* 47C2B3D7 */
			.byte	'-'
			.byte	0x43, 0x46, 0x33, 0x33				/* CF33 */
			.byte	'-'
			.byte	0x34, 0x34, 0x32, 0x33				/* 4423 */
			.byte	'-'
			.byte	0x38, 0x36, 0x46, 0x41				/* 86FA */
			.byte	'-'
			.byte	0x32, 0x34, 0x38, 0x30, 0x43, 0x34, 0x31, 0x32, 0x46, 0x42, 0x35, 0x44	/* 2480C412FB5D */
			.byte	'}'
			.byte	0x00,0x00					/* double terminators */
XORchecksum: 		.word	0xBA78						/* location for checksum */
D2XX_Struct_end = .
	.rept  (0x1000 - (. - D2XX_Struct_start))
	.byte	0xFF
	.endr



