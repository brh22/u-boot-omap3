
#include <asm/arch/ddr_defs_ti814x.h>

/* Values taken and adapted from "Micron MT47H32M16 32Mx16 DDR2 datasheet.
 * DDR2 is clocked at 400MHz
*/

#define CAS_LATENCY (6)
#define CLK_FREQ (400)
#define CLK_PERIOD (3)

#define ceil_div(x, y) (((x) + (y) - 1) / (y))
#define TIM_FIELD(param, field_lsb_pos) ((ceil_div(param, CLK_PERIOD)-1) << field_lsb_pos)

/* DDR PHY Control Register */
#define DYN_PWRDN_EN    (0x01 << 20)
#define IDLE_LOCAL_ODT  (0x03 << 12)   /* recomended value is 0x03 */
#define RD_LOCAL_ODT    (0x02 << 8) /* recomended value is 2 -> 50ohms */
#define PHY_READ_LATENCY (0xF & (CAS_LATENCY+1))

#define EMIF_PHY_CTRL   (DYN_PWRDN_EN | IDLE_LOCAL_ODT | \
                        RD_LOCAL_ODT | PHY_READ_LATENCY)

/* DDR Timing 1 Register */
#define T_RP (15)
#define T_RCD (15)
#define T_WR (15)
#define T_RAS (45)
#define T_RC (60)
#define T_RRD (10)
#define T_WTR (8)

#define EMIF_TIM1   (TIM_FIELD(T_RP, 25) | \
                    TIM_FIELD(T_RCD, 21) | \
                    TIM_FIELD(T_WR, 17) | \
                    TIM_FIELD(T_RAS, 12) | \
                    TIM_FIELD(T_RC, 6) | \
                    TIM_FIELD(T_RRD, 3) | \
                    TIM_FIELD(T_WTR, 0))

/* DDR Timing 2 Register */
#define T_XP (2)
#define T_ODT (0)
#define T_XSNR (T_RFC+10) /* tRFC(min)+10 */
#define T_XSRD (200*CLK_PERIOD)
#define T_RTP (8)   /* 7.5 ns */
#define T_CKE (3)

#define EMIF_TIM2   (TIM_FIELD(T_XP, 28) | \
                    TIM_FIELD(T_ODT, 25) | \
                    TIM_FIELD(T_XSNR, 16) | \
                    TIM_FIELD(T_XSRD, 6) | \
                    TIM_FIELD(T_RTP, 3) | \
                    TIM_FIELD(T_CKE, 0))

/* DDR Timing 3 Register */
#define T_PDLL_UL (5)	/* TRM says must be 5! */
#define T_CSTA (0)		/* Not applicable to our case: we only have one CS */
#define T_ZQCS (0)		/* Not applicable to DDR2 */
#define T_RFC (105)
#define T_RAS_MAX (15)	/* DM814x TRM says must be 0xF */

#define EMIF_TIM3   (TIM_FIELD(T_PDLL_UL, 28) | \
                    TIM_FIELD(T_CSTA, 24) | \
                    TIM_FIELD(T_ZQCS, 15) | \
                    TIM_FIELD(T_RFC, 4) | \
                    T_RAS_MAX)

/* DDR SDRAM Refresh Control Register */
#define PASR (6)			/* full Array self refresh */
#define REFRESH_RATE (CLK_FREQ*7)	/* CLK_FREQ*tREFI = 400*7.8125 */

#define EMIF_REF_CTRL	((PASR << 24) | (REFRESH_RATE))

/* DDR SDRAM Config 1 Register */
#define SDRAM_TYPE (2)		/* DDR2 */
#define IBANK_POS (1)		/* 4 interleaving banks per device CS, EBANK_POS = 0 */
#define DDR_TERM (1)		/* 75ohm */
#define DDQS (1)		/* Differential DQS */
#define SDRAM_DRIVE (1)		/* reduced drive strength */
#define BUS_WIDTH (0)		/* 0 = 32bits */
#define ROWSIZE (4)			/* 13bit rows -> 8K row size*/
#define IBANK (2)			/* 4 banks */
#define EBANK (0)			/* Use only CS0 */
#define PAGESIZE (2)		/* 10 columns -> 1K word page */

#define EMIF_SDRAM_CONFIG   ((SDRAM_TYPE << 29) | \
                            (IBANK_POS << 27) | \
                            (DDR_TERM << 24) | \
                            (DDQS << 23) | \
                            (SDRAM_DRIVE << 18) | \
                            (BUS_WIDTH << 14) | \
                            (CAS_LATENCY << 10) | \
                            (ROWSIZE << 7) | \
                            (IBANK << 4) | \
                            (EBANK << 3) | \
                            PAGESIZE)

/* DDR SDRAM Config 2 Register */
#define EBANK_POS (0)
#define EMIF_SDRAM_CONFIG2		(EBANK_POS << 27)

/* DDR SDRAM Output Impedance Calibration Config Register */
#define EMIF_SDRAM_ZQCR		0x50074BE1	/* SDRAM Output Impedance Calibration Configuration */

/* DMM LISA MAPPING: 1 contiguous 128MB section starting at 0x80000000, no interleaving */
#define ASI1230_DMM_LISA_MAP__0		((0x80 << 24) | (3 << 20) | (1 << 8))
#define ASI1230_DMM_LISA_MAP__1		0x0
#define ASI1230_DMM_LISA_MAP__2		0x0
#define ASI1230_DMM_LISA_MAP__3		0x0
