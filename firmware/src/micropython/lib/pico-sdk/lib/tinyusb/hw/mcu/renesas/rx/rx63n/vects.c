/************************************************************************/
/*    File Version: V1.00                                               */
/*    File Version: V1.00 [Hardware Manual Revision : 1.00]             */
/*    Date Generated: 19/08/2013                                        */
/************************************************************************/

#include "interrupt_handlers.h"

typedef void (*fp) (void);
extern void PowerON_Reset (void);
extern void stack (void);

#define FVECT_SECT          __attribute__ ((section (".fvectors")))

const void *HardwareVectors[] FVECT_SECT  = {
//;0xffffff80  MDES  Endian Select Register
#ifdef __RX_LITTLE_ENDIAN__ 
(fp)0xffffffff,
#endif
#ifdef __RX_BIG_ENDIAN__ 
(fp)0xfffffff8,
#endif
//;0xffffff84  Reserved
    (fp)0,
//;0xffffff88  OFS1
    (fp)0xffffffff,
//;0xffffff8C  OFS0
    (fp)0xffffffff,
//;0xffffff90  Reserved
    (fp)0,
//;0xffffff94  Reserved
    (fp)0,
//;0xffffff98  Reserved
    (fp)0,
//;0xffffff9C  Reserved
    (fp)0,
//;0xffffffA0  Reserved
    (fp)0xFFFFFFFF,
//;0xffffffA4  Reserved
    (fp)0xFFFFFFFF,
//;0xffffffA8  Reserved
    (fp)0xFFFFFFFF,
//;0xffffffAC  Reserved
    (fp)0xFFFFFFFF,
//;0xffffffB0  Reserved
    (fp)0,
//;0xffffffB4  Reserved
    (fp)0,
//;0xffffffB8  Reserved
    (fp)0,
//;0xffffffBC  Reserved
    (fp)0,
//;0xffffffC0  Reserved
    (fp)0,
//;0xffffffC4  Reserved
    (fp)0,
//;0xffffffC8  Reserved
    (fp)0,
//;0xffffffCC  Reserved
    (fp)0,
//;0xffffffd0  Exception(Supervisor Instruction)
    INT_Excep_SuperVisorInst,
//;0xffffffd4  Exception(Access Instruction)
    INT_Excep_AccessInst,
//;0xffffffd8  Reserved
    (fp)0,
//;0xffffffdc  Exception(Undefined Instruction)
    INT_Excep_UndefinedInst,
//;0xffffffe0  Reserved
    (fp)0,
//;0xffffffe4  Exception(Floating Point)
    INT_Excep_FloatingPoint,
//;0xffffffe8  Reserved
    (fp)0,
//;0xffffffec  Reserved
    (fp)0,
//;0xfffffff0  Reserved
    (fp)0,
//;0xfffffff4  Reserved
    (fp)0,
//;0xfffffff8  NMI
    INT_NonMaskableInterrupt,
//;0xfffffffc  RESET
//;<<VECTOR DATA START (POWER ON RESET)>>
//;Power On Reset PC
    PowerON_Reset                                                                                                                 
//;<<VECTOR DATA END (POWER ON RESET)>>
};


#define RVECT_SECT          __attribute__ ((section (".rvectors")))

const fp RelocatableVectors[] RVECT_SECT  = {
//;0x0000  Reserved
    (fp)INT_Excep_BRK,
//;0x0004  Reserved
    (fp)0,
//;0x0008  Reserved
    (fp)0,
//;0x000C  Reserved
    (fp)0,
//;0x0010  Reserved
    (fp)0,
//;0x0014  Reserved
    (fp)0,
//;0x0018  Reserved
    (fp)0,
//;0x001C  Reserved
    (fp)0,
//;0x0020  Reserved
    (fp)0,
//;0x0024  Reserved
    (fp)0,
//;0x0028  Reserved
    (fp)0,
//;0x002C  Reserved
    (fp)0,
//;0x0030  Reserved
    (fp)0,
//;0x0034  Reserved
    (fp)0,
//;0x0038  Reserved
    (fp)0,
//;0x003C  Reserved
    (fp)0,
//;0x0040  BUSERR
    (fp)INT_Excep_BSC_BUSERR,
//;0x0044  Reserved
    (fp)0,
//;0x0048  Reserved
    (fp)0,
//;0x004C  Reserved
    (fp)0,
//;0x0050  Reserved
    (fp)0,
//;0x0054 FCU_FCUERR
    (fp)INT_Excep_FCU_FIFERR,
//;0x0058  Reserved
    (fp)0,
//;0x005C  FCU_FRDYI
    (fp)INT_Excep_FCU_FRDYI,  
//;0x0060  Reserved
    (fp)0,
//;0x0064  Reserved
    (fp)0,
//;0x0068  Reserved
    (fp)0,
//;0x006C  ICU_SWINT                                                    
    (fp)INT_Excep_ICU_SWINT,                
//;0x0070  CMT0_CMI0                                    
    (fp)INT_Excep_CMT0_CMI0,
//;0x0074  CMT1_CMI1
    (fp)INT_Excep_CMT1_CMI1,
//;0x0078  CMT2_CMI2
    (fp)INT_Excep_CMT2_CMI2,
//;0x007C  CMT3_CMI3
    (fp)INT_Excep_CMT3_CMI3,
//;0x0080  ETHER_EINT                
    (fp)INT_Excep_ETHER_EINT,    
//;0x0084  USB0_D0FIFO0                
    (fp)INT_Excep_USB0_D0FIFO0,    
//;0x0088  USB0_D1FIFO0
    (fp)INT_Excep_USB0_D1FIFO0,
//;0x008C  USB0_USBI0
    (fp)INT_Excep_USB0_USBI0,
//;0x0090 USB1_D0FIFO1
    (fp)INT_Excep_USB1_D0FIFO1,    
//;0x0094  USB1_D1FIFO1
    (fp)INT_Excep_USB1_D1FIFO1,
//;0x0098  USB1_USBI1
    (fp)INT_Excep_USB1_USBI1,    
//;0x009C  RSPI0 SPRI0
    (fp)INT_Excep_RSPI0_SPRI0,
//;0x00A0  RSPI0_SPTI0
    (fp)INT_Excep_RSPI0_SPTI0,
//;0x00A4  RSPI0_SPII0
    (fp)INT_Excep_RSPI0_SPII0,
//;0x00A8  RSPI1_SPRI1
    (fp)INT_Excep_RSPI1_SPRI1,
//;0x00AC  RSPI1_SPTI1
    (fp)INT_Excep_RSPI1_SPTI1,    
//;0x00B0  RSPI1_SPII1
    (fp)INT_Excep_RSPI1_SPII1,
//;0x00B4  RSPI2_SPRI2
    (fp)INT_Excep_RSPI2_SPRI2, 
//;0x00B8  RSPI2_SPTI2
    (fp)INT_Excep_RSPI2_SPTI2,    
//;0x00BC  RSPI2_SPII2
    (fp)INT_Excep_RSPI2_SPII2,
//;0x00C0  CAN0_RXF0
    (fp)INT_Excep_CAN0_RXF0,
//;0x00C4  CAN0_TXF0
    (fp)INT_Excep_CAN0_TXF0,    
//;0x00C8  CAN0_RXM0
    (fp)INT_Excep_CAN0_RXM0,
//;0x00CC  CAN0_TXM0
    (fp)INT_Excep_CAN0_TXM0,
//;0x00D0  CAN1_RXF1
    (fp)INT_Excep_CAN1_RXF1,
//;0x00D4  CAN1_TXF1
    (fp)INT_Excep_CAN1_TXF1,    
//;0x00D8  CAN1_RXM1
    (fp)INT_Excep_CAN1_RXM1,
//;0x00DC  CAN1_TXM1
    (fp)INT_Excep_CAN1_TXM1,
//;0x00E0  CAN2_RXF2
    (fp)INT_Excep_CAN2_RXF2,
//;0x00E4  CAN2_TXF2
    (fp)INT_Excep_CAN2_TXF2,    
//;0x00E8  CAN2_RXM2
    (fp)INT_Excep_CAN2_RXM2,
//;0x00EC  CAN2_TXM2
    (fp)INT_Excep_CAN2_TXM2,
//;0x0F0  Reserved
    (fp)0,    
//;0x0F4  Reserved
    (fp)0,
//;0x0F8   RTC_CUP    
    (fp)INT_Excep_RTC_CUP,
//;0x00FC  Reserved
    (fp)0,
//;0x0100  ICU IRQ0
    (fp)INT_Excep_ICU_IRQ0,
//;0x0104 ICU IRQ1
    (fp)INT_Excep_ICU_IRQ1,
//;0x0108 ICU IRQ2
    (fp)INT_Excep_ICU_IRQ2,
//;0x010C ICU IRQ3
    (fp)INT_Excep_ICU_IRQ3,
//;0x0110 ICU IRQ4
    (fp)INT_Excep_ICU_IRQ4,
//;0x0114 ICU IRQ5
    (fp)INT_Excep_ICU_IRQ5,
//;0x0118 ICU IRQ6
    (fp)INT_Excep_ICU_IRQ6,
//;0x011C ICU IRQ7
    (fp)INT_Excep_ICU_IRQ7,
//;0x0120 ICU IRQ8
    (fp)INT_Excep_ICU_IRQ8,
//;0x0124 ICU IRQ9
    (fp)INT_Excep_ICU_IRQ9,
//;0x0128 ICU IRQ10
    (fp)INT_Excep_ICU_IRQ10,
//;0x012C ICU IRQ11
    (fp)INT_Excep_ICU_IRQ11,
//;0x0130 ICU IRQ12
    (fp)INT_Excep_ICU_IRQ12,
//;0x0134 ICU IRQ13
    (fp)INT_Excep_ICU_IRQ13,
//;0x0138 ICU IRQ14
    (fp)INT_Excep_ICU_IRQ14,
//;0x013C ICU IRQ15
    (fp)INT_Excep_ICU_IRQ15,
//;0x0140  Reserved
    (fp)0,
//;0x0144  Reserved
    (fp)0,
//;0x0148  Reserved
    (fp)0,
//;0x014C  Reserved
    (fp)0,
//;0x0150  Reserved
    (fp)0,
//;0x0154  Reserved
    (fp)0,
//;0x0158  Reserved
    (fp)0,
//;0x015C  Reserved
    (fp)0,
//;0x0160  Reserved
    (fp)0,
//;0x0164  Reserved
    (fp)0,
//;0x0168  USB USBR0
    (fp)INT_Excep_USB_USBR0,
//;0x016C  USB USBR1
    (fp)INT_Excep_USB_USBR1,
//;0x0170  RTC_ALM
    (fp)INT_Excep_RTC_ALM,        
//;0x0174  RTC_PRD
    (fp)INT_Excep_RTC_PRD,
//;0x0178  Reserved
    (fp)0,
//;0x017C  Reserved
    (fp)0,
//;0x0180  Reserved
    (fp)0,
//;0x0184  Reserved
    (fp)0,
//;0x0188  AD_ADI0
    (fp)INT_Excep_AD_ADI0,
//;0x018C  Reserved
    (fp)0,
//;0x0190  Reserved
    (fp)0,
//;0x0194  Reserved
    (fp)0,
//;0x0198  S12AD_S12ADI0
    (fp)INT_Excep_S12AD_S12ADI0,
//;0x019C  Reserved
    (fp)0,                                        
//;0x01A0  Reserved
    (fp)0,
//;0x01A4  Reserved
    (fp)0,    
//;0x01A8  ICU GROUP0
    (fp)INT_Excep_ICU_GROUP0,
//;0x01AC ICU GROUP1
    (fp)INT_Excep_ICU_GROUP1,
//;0x01B0  ICU GROUP2
    (fp)INT_Excep_ICU_GROUP2,    
//;0x01B4  ICU GROUP3
    (fp)INT_Excep_ICU_GROUP3,                        
//;0x01B8  ICU GROUP4
    (fp)INT_Excep_ICU_GROUP4,
//;0x01BC  ICU GROUP5
    (fp)INT_Excep_ICU_GROUP5,
//;0x01C0  ICU GROUP6
    (fp)INT_Excep_ICU_GROUP6,
//;0x01C4  Reserved
    (fp)0,
//;0x1C8    ICU GROUP12
    (fp)INT_Excep_ICU_GROUP12,
//;0x01CC  Reserved
    (fp)0,
//;0x01D0  Reserved
    (fp)0,
//;0x01D4  Reserved
    (fp)0,
//;0x01D8  Reserved
    (fp)0,
//;0x01DC  Reserved
    (fp)0,    
//;0x01E0  Reserved
    (fp)0,
//;0x01E4  Reserved
    (fp)0,
//;0x01E8 SCI2_SCIX0
     (fp)INT_Excep_SCI12_SCIX0,
//;0x01EC SCI2_SCIX1
     (fp)INT_Excep_SCI12_SCIX1,
//;0x01F0 SCI2_SCIX2
     (fp)INT_Excep_SCI12_SCIX2,  
//;0x01F4 SCI2_SCIX3
     (fp)INT_Excep_SCI12_SCIX3,
//;0x01F8  TPU0 TGI0A
    (fp)INT_Excep_TPU0_TGI0A,
//;0x01FC  TPU0 TGI0B
    (fp)INT_Excep_TPU0_TGI0B,
//;0x0200 TPU0 TGI0C
    (fp)INT_Excep_TPU0_TGI0C,
//;0x0204 TPU0 TGI0D
    (fp)INT_Excep_TPU0_TGI0D,
//;0x0208 TPU1 TGI1A
    (fp)INT_Excep_TPU1_TGI1A,
//;0x020C TPU1 TGI1B
    (fp)INT_Excep_TPU1_TGI1B,
//;0x0210 TPU2 TGI2A
    (fp)INT_Excep_TPU2_TGI2A,
//;0x0214 TPU2 TGI2B
    (fp)INT_Excep_TPU2_TGI2B,
//;0x0218 TPU3 TGI3A
    (fp)INT_Excep_TPU3_TGI3A,
//;0x021C TPU3 TGI3B
    (fp)INT_Excep_TPU3_TGI3B,
//;0x0220 TPU3 TGI3C
    (fp)INT_Excep_TPU3_TGI3C,
//;0x0224 TPU3 TGI3D
    (fp)INT_Excep_TPU3_TGI3D,
//;0x0228 TPU4 TGI4A
    (fp)INT_Excep_TPU4_TGI4A,
//;0x022C TPU4 TGI4B
    (fp)INT_Excep_TPU4_TGI4B,
//;0x0230 TPU5 TGI5A
    (fp)INT_Excep_TPU5_TGI5A,
//;0x0234 TPU5 TGI5B
    (fp)INT_Excep_TPU5_TGI5B,
//;0x0238 TPU6 TGI6A
    (fp)INT_Excep_TPU6_TGI6A,
//;0x0238 MTU0 TGIA0
//  (fp)INT_Excep_MTU0_TGIA0,
//;0x023C TPU6 TGI6B
    (fp)INT_Excep_TPU6_TGI6B,
//;0x023C MTU0 TGIB0
//  (fp)INT_Excep_MTU0_TGIB0,
//;0x0240 TPU6 TGI6C
    (fp)INT_Excep_TPU6_TGI6C,
//;0x0240 MTU0 TGIC0
//  (fp)INT_Excep_MTU0_TGIC0,
//;0x0244 TPU6 TGI6D
    (fp)INT_Excep_TPU6_TGI6D,
//;0x0244 MTU0 TGID0
//  (fp)INT_Excep_MTU0_TGID0,
//;0x0248 MTU0 TGIE0
    (fp)INT_Excep_MTU0_TGIE0,
//;0x024C MTU0 TGIF0
    (fp)INT_Excep_MTU0_TGIF0,
//;0x0250 TPU7 TGI7A
    (fp)INT_Excep_TPU7_TGI7A,
//;0x0250 MTU1 TGIA1
//  (fp)INT_Excep_MTU1_TGIA1,
//;0x0254 TPU7 TGI7B
    (fp)INT_Excep_TPU7_TGI7B,
//;0x0254 MTU1 TGIB1
//  (fp)INT_Excep_MTU1_TGIB1,
//;0x0258 TPU8 TGI8A
    (fp)INT_Excep_TPU8_TGI8A,
//;0x0258 MTU2 TGIA2
//  (fp)INT_Excep_MTU2_TGIA2,
//;0x025C TPU8 TGI8B
    (fp)INT_Excep_TPU8_TGI8B,
//;0x025C MTU2 TGIB2
//  (fp)INT_Excep_MTU2_TGIB2,
//;0x0260 TPU9 TGI9A
    (fp)INT_Excep_TPU9_TGI9A,
//;0x0260 MTU3 TGIA3
//  (fp)INT_Excep_MTU3_TGIA3,
//;0x0264 TPU9 TGI9B
    (fp)INT_Excep_TPU9_TGI9B,
//;0x0264 MTU3 TGIB3
//  (fp)INT_Excep_MTU3_TGIB3,
//;0x0268 TPU9 TGI9C
    (fp)INT_Excep_TPU9_TGI9C,
// ;0x0268MTU3 TGIC3
//  (fp)INT_Excep_MTU3_TGIC3,
//;0x026C TPU9 TGI9D
    (fp)INT_Excep_TPU9_TGI9D,
//;0x026C MTU3 TGID3
//  (fp)INT_Excep_MTU3_TGID3,
//;0x0270 TPU10 TGI10A
    (fp)INT_Excep_TPU10_TGI10A,
//;0x0270 MTU4 TGIA4
//  (fp)INT_Excep_MTU4_TGIA4,
//;0x0274 TPU10 TGI10B
    (fp)INT_Excep_TPU10_TGI10B,
//;0x0274 MTU4 TGIB4
//  (fp)INT_Excep_MTU4_TGIB4,
//;0x0278 MTU4 TGIC4
    (fp)INT_Excep_MTU4_TGIC4,
//;0x027C MTU4 TGID4
    (fp)INT_Excep_MTU4_TGID4,

//;0x0280 MTU4 TCIV4
    (fp)INT_Excep_MTU4_TCIV4,
//;0x0284 MTU5 TGIU5
    (fp)INT_Excep_MTU5_TGIU5,
//;0x0288 MTU5 TGIV5
    (fp)INT_Excep_MTU5_TGIV5,
//;0x028C MTU5 TGIW5
    (fp)INT_Excep_MTU5_TGIW5,
//;0x0290 TPU11 TGI11A
    (fp)INT_Excep_TPU11_TGI11A,
//;0x0294 TPU11 TGI11B
    (fp)INT_Excep_TPU11_TGI11B,
//;0x0298 POE OEI1
    (fp)INT_Excep_POE_OEI1,
//;0x029C POE OEI2
    (fp)INT_Excep_POE_OEI2,
//;0x02A0  Reserved
    (fp)0,
//;0x02A4  Reserved
    (fp)0,    
//;0x02A8 TMR0_CMIA0
    (fp)INT_Excep_TMR0_CMIA0,
//;0x02AC TMR0_CMIB0
    (fp)INT_Excep_TMR0_CMIB0,
//;0x02B0 TMR0_OVI0
    (fp)INT_Excep_TMR0_OVI0,
//;0x02B4 TMR1_CMIA1
    (fp)INT_Excep_TMR1_CMIA1,
//;0x02B8 TMR1_CMIB1
    (fp)INT_Excep_TMR1_CMIB1,
//;0x02BC TMR1_OVI1
    (fp)INT_Excep_TMR1_OVI1,
//;0x02C0 TMR2_CMIA2
    (fp)INT_Excep_TMR2_CMIA2,
//;0x02C4 TMR2_CMIB2
    (fp)INT_Excep_TMR2_CMIB2,
//;0x02C8 TMR2_OVI2
    (fp)INT_Excep_TMR2_OVI2,
//;0x02CC TMR3_CMIA3
    (fp)INT_Excep_TMR3_CMIA3,
//;0x02D0 TMR3_CMIB3
    (fp)INT_Excep_TMR3_CMIB3,
//;0x02D4 TMR3_OVI3
    (fp)INT_Excep_TMR3_OVI3,
//;0x02D8 RIIC0 EEI0
    (fp)INT_Excep_RIIC0_EEI0,
//;0x02DC RIIC0 RXI0
    (fp)INT_Excep_RIIC0_RXI0,
//;0x02E0 RIIC0 TXI0
    (fp)INT_Excep_RIIC0_TXI0,
//;0x02E4 RIIC0 TEI0
    (fp)INT_Excep_RIIC0_TEI0,
//;0x02E8 RIIC1 EEI1
    (fp)INT_Excep_RIIC1_EEI1,
//;0x02EC RIIC1 RXI1
    (fp)INT_Excep_RIIC1_RXI1,
//;0x02F0 RIIC1 TXI1
    (fp)INT_Excep_RIIC1_TXI1,
//;0x02F4 RIIC1 TEI1
    (fp)INT_Excep_RIIC1_TEI1,
//;0x02F8 RIIC2 EEI2
    (fp)INT_Excep_RIIC2_EEI2,
//;0x02FC RIIC2 RXI2
    (fp)INT_Excep_RIIC2_RXI2,
//;0x0300 RIIC2 TXI2
    (fp)INT_Excep_RIIC2_TXI2,
//;0x0304 RIIC2 TEI2
    (fp)INT_Excep_RIIC2_TEI2,
//;0x0308 RIIC3 EEI3
    (fp)INT_Excep_RIIC3_EEI3,
//;0x030C RIIC3 RXI3
    (fp)INT_Excep_RIIC3_RXI3,
//;0x0310 RIIC3 TXI3
    (fp)INT_Excep_RIIC3_TXI3,
//;0x0314 RIIC3 TEI3
    (fp)INT_Excep_RIIC3_TEI3,
//;0x0318 DMAC DMAC0I
    (fp)INT_Excep_DMAC_DMAC0I,
//;0x031C DMAC DMAC1I
    (fp)INT_Excep_DMAC_DMAC1I,
//;0x0320 DMAC DMAC2I
    (fp)INT_Excep_DMAC_DMAC2I,
//;0x0324 DMAC DMAC3I
    (fp)INT_Excep_DMAC_DMAC3I,
//;0x0328 EXDMAC EXDMAC0I
    (fp)INT_Excep_EXDMAC_EXDMAC0I,
//;0x032C EXDMAC EXDMAC1I
    (fp)INT_Excep_EXDMAC_EXDMAC1I,
//;0x0330  Reserved
    (fp)0,
//;0x0334  Reserved
    (fp)0,
//;0x0338  DEU DEU0
    (fp)INT_Excep_DEU_DEU0,
//;0x033C  DEU DEU1
    (fp)INT_Excep_DEU_DEU1,
//;0x0340  PDC PCDFI
    (fp)INT_Excep_PDC_PCDFI,
//;0x0344  PDC PCFEI
    (fp)INT_Excep_PDC_PCFEI,
//;0x0348  PDC PCERI
    (fp)INT_Excep_PDC_PCERI,
//;0x034C  Reserved
    (fp)0,
//;0x0350  Reserved
    (fp)0,
//;0x0354  Reserved
    (fp)0,
//;0x0358 SCI0_RXI0
    (fp)INT_Excep_SCI0_RXI0,
//;0x035C SCI0_TXI0
    (fp)INT_Excep_SCI0_TXI0,
//;0x0360 SCI0_TEI0
    (fp)INT_Excep_SCI0_TEI0,
//;0x0364 SCI1_RXI1
    (fp)INT_Excep_SCI1_RXI1,
//;0x0368 SCI1_TXI1
    (fp)INT_Excep_SCI1_TXI1,
//;0x036C SCI1_TEI1
    (fp)INT_Excep_SCI1_TEI1,
//;0x0370 SCI2_RXI2
    (fp)INT_Excep_SCI2_RXI2,
//;0x0374 SCI2_TXI2
    (fp)INT_Excep_SCI2_TXI2,
//;0x0378 SCI2_TEI2
    (fp)INT_Excep_SCI2_TEI2,
//;0x037C SCI3_RXI3
    (fp)INT_Excep_SCI3_RXI3,
//;0x0380 SCI3_TXI3
    (fp)INT_Excep_SCI3_TXI3,
//;0x0384 SCI3_TEI3
    (fp)INT_Excep_SCI3_TEI3,
//;0x0388 SCI4_RXI4
    (fp)INT_Excep_SCI4_RXI4,
//;0x038C SCI4_TXI4
    (fp)INT_Excep_SCI4_TXI4,
//;0x0390 SCI4_TEI4
    (fp)INT_Excep_SCI4_TEI4,
//;0x0394 SCI5_RXI5
    (fp)INT_Excep_SCI5_RXI5,
//;0x0398 SCI5_TXI5
    (fp)INT_Excep_SCI5_TXI5,
//;0x039C SCI5_TEI5
    (fp)INT_Excep_SCI5_TEI5,
//;0x03A0 SCI6_RXI6
    (fp)INT_Excep_SCI6_RXI6,
//;0x03A4 SCI6_TXI6
    (fp)INT_Excep_SCI6_TXI6,
//;0x03A8 SCI6_TEI6
    (fp)INT_Excep_SCI6_TEI6,
//;0x03AC SCI7_RXI7
    (fp)INT_Excep_SCI7_RXI7,
//;0x03B0 SCI7_TXI7
    (fp)INT_Excep_SCI7_TXI7,
//;0x03B4 SCI7_TEI7
    (fp)INT_Excep_SCI7_TEI7,
//;0x03B8 SCI8_RXI8
    (fp)INT_Excep_SCI8_RXI8,
//;0x03BC SCI8_TXI8
    (fp)INT_Excep_SCI8_TXI8,
//;0x03C0 SCI8_TEI8
    (fp)INT_Excep_SCI8_TEI8,
//;0x03C4 SCI9_RXI9
    (fp)INT_Excep_SCI9_RXI9,
//;0x03C8 SCI9_TXI9
    (fp)INT_Excep_SCI9_TXI9,
//;0x03CC SCI9_TEI9
    (fp)INT_Excep_SCI9_TEI9,
//;0x03D0 SCI10_RXI10
    (fp)INT_Excep_SCI10_RXI10,
//;0x03D4 SCI10_TXI10
    (fp)INT_Excep_SCI10_TXI10,
//;0x03D8 SCI10_TEI10
    (fp)INT_Excep_SCI10_TEI10,
//;0x03DC SCI11_RXI11
    (fp)INT_Excep_SCI11_RXI11,
//;0x03E0 SCI11_TXI11
    (fp)INT_Excep_SCI11_TXI11,
//;0x03E4 SCI11_TEI11
    (fp)INT_Excep_SCI11_TEI11,
//;0x03E8 SCI12_RXI12
    (fp)INT_Excep_SCI12_RXI12,
//;0x03EC SCI12_TXI12
    (fp)INT_Excep_SCI12_TXI12,
//;0x03F0 SCI12_TEI12
    (fp)INT_Excep_SCI12_TEI12,
//;0x03F4 IEB IEBINT
    (fp)INT_Excep_IEB_IEBINT,
//;0x03F8  Reserved
    (fp)0,
//;0x03FC  Reserved
    (fp)0,
};
