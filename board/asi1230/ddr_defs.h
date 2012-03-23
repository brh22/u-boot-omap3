
#include <asm/arch/ddr_defs_ti814x.h>

/* Values taken and adapted from "IS43LR32160B: 4M x 32Bits x 4Banks Mobile
 * DDR SDRAM" datasheet. mDDR is clocked at 166MHz
*/

#define CAS_LATENCY (3)
#define mDDR_CLK_FREQ (166)
#define mDDR_CLK_PERIOD (6)

#define DYN_PWRDN_EN (0x01 << 20)
#define IDLE_LOCAL_ODT_0x03 (0x03 << 12)
#define RD_LOCAL_ODT_0x02 (0x02 << 8)
#define PHY_READ_LATENCY (0xF & (CAS_LATENCY+1))

#define mDDR_EMIF_READ_LATENCY	(DYN_PWRDN_EN | IDLE_LOCAL_ODT_0x03 | RD_LOCAL_ODT_0x02 | PHY_READ_LATENCY)

#define T_RP (18)
#define T_RCD (18)
#define T_WR (12)
#define T_RAS (42)
#define T_RC (60)
#define T_RRD (12)
#define T_WTR (12)

#define ceil_div(x, y) (((x) + (y) - 1) / (y))
#define TIM_FIELD(param, field_lsb_pos) ((ceil_div(param, mDDR_CLK_PERIOD)-1) << field_lsb_pos)

#define mDDR_EMIF_TIM1  TIM_FIELD(T_RP, 25) | \
                        TIM_FIELD(T_RCD, 21) | \
                        TIM_FIELD(T_WR, 17) | \
                        TIM_FIELD(T_RAS, 12) | \
                        TIM_FIELD(T_RC, 6) | \
                        TIM_FIELD(T_RRD, 3) | \
                        TIM_FIELD(T_WTR, 0)

#define T_XP (25)
#define T_ODT (0) /* ODT does nto apply to mDDR */
#define T_XSNR (200) /* (?) Using tXSR from datasheet */
#define T_XSRD (200) /* Same as tXSR ? */
#define T_RTP (mDDR_CLK_PERIOD) /* (?) Let's start with 1 clock period (not on datasheet) */
#define T_CKE (200+mDDR_CLK_PERIOD) /* (?) Datasheet has drawing but no mention of the value in AC params */

#define mDDR_EMIF_TIM2			TIM_FIELD(T_XP, 28) | TIM_FIELD(T_ODT, 25) | TIM_FIELD(T_XSNR, 16) |\
                                TIM_FIELD(T_XSRD, 6) | TIM_FIELD(T_RTP, 3) | TIM_FIELD(T_CKE, 0)

#define T_PDLL_UL (0) /* (?) no DLL on mDDR */
#define T_CSTA (0) /* (?) Not applicable to our case: we only have one CS */
#define T_ZQCS (0) /* (?) Not applicable to mDDR */
#define T_RFC (110)
#define T_RAS_MAX (12) /* (?) tRASmax / tREFI = 100/7.8 = 12 */


#define mDDR_EMIF_TIM3			TIM_FIELD(T_PDLL_UL, 28) | TIM_FIELD(T_CSTA, 24) | \
                                TIM_FIELD(T_ZQCS, 15) | TIM_FIELD(T_RFC, 4) | T_RAS_MAX

#define PASR (6) /* 1/16th Array self refresh */
#define REFRESH_RATE (1294) /* (?) mDDR_CLK_FREQ*tREFI = 166*7.8 = 1294.8 */
#define mDDR_EMIF_REF_CTRL	(PASR << 24) | (REFRESH_RATE)

#define SDRAM_TYPE (1) /* mDDR */
#define IBANK_POS (0) /* Try the high performance mode first, see EBANK_POS too */
#define SDRAM_DRIVE (3) /* lowest (1/8) drive strength */

#define BUS_WIDTH (0) /* 0 = 32bits */
#define ROWSIZE (4) /* 13bits rows */
#define IBANK (2) /* 4 banks */
#define EBANK (0) /* Use only CS0 */ 
#define PAGESIZE (1) /* 9 columns -> 512 word page */
	
#define mDDR_EMIF_SDRAM_CONFIG		(SDRAM_TYPE << 29) | (IBANK_POS << 27) | (SDRAM_DRIVE << 18) |\
                                    (BUS_WIDTH << 14)  | (CAS_LATENCY << 10) | (ROWSIZE << 7) |\
                                    (IBANK << 4) | (EBANK << 3) | PAGESIZE

#define EBANK_POS (0)
#define mDDR_EMIF_SDRAM_CONFIG2		(EBANK_POS << 27)

#define mDDR_EMIF_SDRAM_ZQCR		0x50074BE1 /* SDRAM Output Impedance Calibration Configuration */

/* ASI1230 DMM LISA MAPPING: 1 contiguous 64MB section starting at 0x80000000, no interleaving */
#define ASI1230_DMM_LISA_MAP__0		((0x80 << 24) | (2 << 20) | (1 << 8))
#define ASI1230_DMM_LISA_MAP__1		0x0
#define ASI1230_DMM_LISA_MAP__2		0x0
#define ASI1230_DMM_LISA_MAP__3		0x0
