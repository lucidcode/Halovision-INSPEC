/**< TD2XX_DeviceConfiguration */

__pD2XXDefaultConfiguration:
D2XX_Struct_start = .
Signature: 		.long 0xF7D1DCF6
ConfigVersion:		.word 0x0100 		/**< structure version 1.0 */
DelayAfterPHYEn: 	.word 0x0000 		/**< delay to be inserted whenever PHY is enabled */
VendorID: 		.word 0x0403
ProductID: 		.word 0x603A
BCDEnable:		.byte	0		/**< [0]=disable, 1=enable */
DFUCapable:		.byte	1		/**< [0]=disable, 1=enable */
SelfPowered:		.byte	1		/**< 0=disable, [1]=enable */
MaxPower:		.byte	0xFA		/**< 0xFA=500mA */
NumOfD2XXInterfaces:	.byte	7		/**< 7 (min=1..max=7) */
RMWKUPEnable: 		.byte	0		/**< [0]=disable, 1=enable */
MaxTransferSize: 	.word	1024, 512, 512, 512, 512, 512, 512
Strings:	/**< strings are preceded by length byte */
Strings_start = .
			.byte	0x04,'F','T','D','I'						/**< manufacturer string */
			.byte	0x0A,'F','T','9','3','0',' ','D','2','X','X' 			/**< product description */
			.byte	0x0F,'F','T','9','3','0','S','e','r','i','a','l','#','0','0','7 	/**< serial number */
			.byte	0x15,'D','F','U',' ','R','u','n','t','i','m','e',' ','I','n','t','e','r','f','a','c','e'  /* dfu description */
			.skip	(0x80-(. - Strings_start))
DfuDeviceInterfaceGUID:	/**< Default DeviceInterfaceGUID /*	{02D8AE6B-928B-4B55-8235-1B73E61B191C}*/
			.byte	'{'						/* { */
			.byte	0x30, 0x32, 0x44, 0x38, 0x41, 0x45, 0x36, 0x42		/* 02D8AE6B */
			.byte	'-'
			.byte	0x39, 0x32, 0x38, 0x42				/* 928B */
			.byte	'-'
			.byte	0x34, 0x42, 0x35, 0x35				/* 4B55 */
			.byte	'-'
			.byte	0x38, 0x32, 0x33, 0x35				/* 8235 */
			.byte	'-'
			.byte	0x31, 0x42, 0x37, 0x33, 0x45, 0x36, 0x31, 0x42, 0x31, 0x39, 0x31, 0x43	/* 1B73E61B191C */
			.byte	'}'
			.byte	0x00,0x00					/* double terminators */
XORchecksum: 		.word	0xAC2C						/* location for checksum */
D2XX_Struct_end = .
	.rept  (0x1000 - (. - D2XX_Struct_start))
	.byte	0xFF
	.endr



