
#include <config.h>
#include <asm/arch/cpu.h>

#include "ddr2.h"

extern void asi1230_delay(unsigned long loops);
extern void cmd_macro_config(u32 ddr_phy, u32 inv_clk_out,
	u32 ctrl_slave_ratio_cs0, u32 cmd_dll_lock_diff);
extern void data_macro_config(u32 macro_num, u32 emif, u32 rd_dqs_cs0,
	u32 wr_dqs_cs0, u32 fifo_we_cs0, u32 wr_data_cs0);

void config_asi1230_ddr2(void)
{
	int macro, emif;

	/*Enable the Power Domain Transition of L3 Fast Domain Peripheral */
	__raw_writel(0x2, CM_DEFAULT_FW_CLKCTRL);
	/*Enable the Power Domain Transition of L3 Fast Domain Peripheral */
	__raw_writel(0x2, CM_DEFAULT_L3_FAST_CLKSTCTRL);
	__raw_writel(0x2, CM_DEFAULT_EMIF_0_CLKCTRL);	/*Enable EMIF0 Clock */
	/*__raw_writel(0x2, CM_DEFAULT_EMIF_1_CLKCTRL); Enable EMIF1 Clock*/
	__raw_writel(0x2, CM_DEFAULT_DMM_CLKCTRL);

	/*Poll for L3_FAST_GCLK  & DDR_GCLK  are active */
	while ((__raw_readl(CM_DEFAULT_L3_FAST_CLKSTCTRL) & 0x300) != 0x300) ;
	/*Poll for Module is functional */
	while ((__raw_readl(CM_DEFAULT_EMIF_0_CLKCTRL)) != 0x2) ;
	/* while ((__raw_readl(CM_DEFAULT_EMIF_1_CLKCTRL)) != 0x2); no EMIF1 */
	while ((__raw_readl(CM_DEFAULT_DMM_CLKCTRL)) != 0x2) ;

	/* No need to enable the memory clock inversion feature,
	 * see 6.3.5.1 of TMS320DM814x TRM
	 */
	cmd_macro_config(DDR_PHY0, PHY_INVERT_CLKOUT_DEFINE,
			 DDR2_PHY_CTRL_SLAVE_RATIO_CS0_DEFINE,
			 PHY_CMD0_DLL_LOCK_DIFF_DEFINE);

	/* Init only PHY0 */
	emif = 0;
	for (macro = 0; macro <= DATA_MACRO_3; macro++) {
		data_macro_config(macro, emif,
				  DDR2_PHY_RD_DQS_CS0_DEFINE,
				  DDR2_PHY_WR_DQS_CS0_DEFINE,
				  DDR2_PHY_RD_DQS_GATE_CS0_DEFINE,
				  DDR2_PHY_WR_DATA_CS0_DEFINE);
	}

	/* DDR IO CTRL config */
	__raw_writel(DDR0_IO_CTRL_DEFINE, DDR0_IO_CTRL);

	__raw_writel(__raw_readl(VTP0_CTRL_REG) | 0x00000040, VTP0_CTRL_REG);

	/* Write 0 to CLRZ bit */
	__raw_writel(__raw_readl(VTP0_CTRL_REG) & 0xfffffffe, VTP0_CTRL_REG);

	/* Write 1 to CLRZ bit */
	__raw_writel(__raw_readl(VTP0_CTRL_REG) | 0x00000001, VTP0_CTRL_REG);

	/* Read VTP control registers & check READY bits */
	while ((__raw_readl(VTP0_CTRL_REG) & 0x00000020) != 0x20) ;

	/* Setup DMM */
	__raw_writel(ASI1230_DMM_LISA_MAP__0, DMM_LISA_MAP__0);
	__raw_writel(ASI1230_DMM_LISA_MAP__1, DMM_LISA_MAP__1);
	__raw_writel(ASI1230_DMM_LISA_MAP__2, DMM_LISA_MAP__2);
	__raw_writel(ASI1230_DMM_LISA_MAP__3, DMM_LISA_MAP__3);

	while (__raw_readl(DMM_LISA_MAP__0) != ASI1230_DMM_LISA_MAP__0) ;
	while (__raw_readl(DMM_LISA_MAP__1) != ASI1230_DMM_LISA_MAP__1) ;
	while (__raw_readl(DMM_LISA_MAP__2) != ASI1230_DMM_LISA_MAP__2) ;
	while (__raw_readl(DMM_LISA_MAP__3) != ASI1230_DMM_LISA_MAP__3) ;

	__raw_writel(0x80000000, DMM_PAT_BASE_ADDR);

	/*Program EMIF0 CFG Registers */
	__raw_writel(EMIF_PHY_CTRL, EMIF4_0_DDR_PHY_CTRL_1);
	__raw_writel(EMIF_PHY_CTRL, EMIF4_0_DDR_PHY_CTRL_1_SHADOW);
	__raw_writel(EMIF_TIM1, EMIF4_0_SDRAM_TIM_1);
	__raw_writel(EMIF_TIM1, EMIF4_0_SDRAM_TIM_1_SHADOW);
	__raw_writel(EMIF_TIM2, EMIF4_0_SDRAM_TIM_2);
	__raw_writel(EMIF_TIM2, EMIF4_0_SDRAM_TIM_2_SHADOW);
	__raw_writel(EMIF_TIM3, EMIF4_0_SDRAM_TIM_3);
	__raw_writel(EMIF_TIM3, EMIF4_0_SDRAM_TIM_3_SHADOW);
	__raw_writel(EMIF_SDRAM_CONFIG, EMIF4_0_SDRAM_CONFIG);
	__raw_writel(EMIF_SDRAM_CONFIG2, EMIF4_0_SDRAM_CONFIG2);

	__raw_writel(EMIF_REF_CTRL | DDR_EMIF_REF_TRIGGER, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);
	__raw_writel(EMIF_SDRAM_ZQCR, EMIF4_0_SDRAM_ZQCR);
	__raw_writel(EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);

	__raw_writel(EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);

	/* The first write/read results in garbage. Is there something wrong with
	   the dram controller's setup?

	   In the meantime we write, read and then wait a little before moving on.
	 */
	*((ulong *) PHYS_DRAM_1) = 0xFFFFFFFF;
	ulong tmp = *((ulong *) PHYS_DRAM_1);
	tmp = 0;
	asi1230_delay(0xFFF);
}
