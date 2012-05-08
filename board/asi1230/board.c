/*
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/cache.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/clock.h>
#include <asm/arch/mem.h>
/* It would be better to #include <asm/arch/mmc.h> rather than <asm/arch/mmc_host_def.h>,
 * but right now the latter causes a macro redefinition warning.
 */
#include <asm/arch/mmc_host_def.h>
#include <status_led.h>
#include <ns16550.h>

#ifdef CONFIG_DRIVER_TI_CPSW
#include <net.h>
#include <miiphy.h>
#include <netdev.h>
#endif /* CONFIG_DRIVER_TI_CPSW */

#include "ddr_defs.h"

DECLARE_GLOBAL_DATA_PTR;

/* TODO: move these constants to a better place */
#define GPIO_CTRL       0x130
#define GPIO_OE         0x134
#define GPIO_DATAIN     0x138
#define GPIO_DATAOUT    0x13C
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194

/* Implement board specific LED API. LEDs are attached to GPIO0 pins from 0 to 3 */
void __led_toggle(led_id_t mask)
{
	mask &= 0x1E;
	u32 o_reg_val = __raw_readl(GPIO0_BASE + GPIO_DATAOUT) ^ mask;
	__raw_writel(o_reg_val, GPIO0_BASE + GPIO_DATAOUT);
}

void __led_init(led_id_t mask, int state)
{
	/* Make sure we do not try to set GPIO OE bits that do not belong to LEDs
	   then set the output enable register and proceed to set the state of the LED.
	 */
	mask &= 0x1E;
	u32 oe_reg_val = __raw_readl(GPIO0_BASE + GPIO_OE) & (~mask);
	__raw_writel(oe_reg_val, GPIO0_BASE + GPIO_OE);
	__led_set(mask, state);
}

void __led_set(led_id_t mask, int state)
{
	u32 o_reg_val = mask & 0x1E;
	/* LEDs are lit when the corresponding GPIO is driven low */
	if (state != STATUS_LED_OFF)
		__raw_writel(o_reg_val, GPIO0_BASE + GPIO_CLEARDATAOUT);
	else
		__raw_writel(o_reg_val, GPIO0_BASE + GPIO_SETDATAOUT);
}

int is_eng_mode_enabled(void)
{
	/* Read UBOOTBY- GP0[5] to see if we should boot in eng mode */
	int uboot_bypass = !(__raw_readl(GPIO0_BASE + GPIO_DATAIN) & (1 << 5));
	if (uboot_bypass)
		return 1;

	/* Read UART's 'break' indicator bit */
	NS16550_t com_port = (NS16550_t) CONFIG_SYS_NS16550_COM1;
	uboot_bypass = readb(&com_port->lsr) & UART_LSR_BI;
	if (uboot_bypass)
		return 1;

	return 0;
}

/*  */

static void cmd_macro_config(u32 ddr_phy, u32 inv_clk_out,
			     u32 ctrl_slave_ratio_cs0, u32 cmd_dll_lock_diff)
{
	u32 ddr_phy_base = (DDR_PHY0 == ddr_phy) ?
	    DDR0_PHY_BASE_ADDR : DDR1_PHY_BASE_ADDR;

	__raw_writel(inv_clk_out, ddr_phy_base + CMD1_REG_PHY_INVERT_CLKOUT_0);
	__raw_writel(inv_clk_out, ddr_phy_base + CMD0_REG_PHY_INVERT_CLKOUT_0);
	__raw_writel(inv_clk_out, ddr_phy_base + CMD2_REG_PHY_INVERT_CLKOUT_0);

	__raw_writel(((ctrl_slave_ratio_cs0 << 10) | ctrl_slave_ratio_cs0),
		     ddr_phy_base + CMD0_REG_PHY_CTRL_SLAVE_RATIO_0);
	__raw_writel(((ctrl_slave_ratio_cs0 << 10) | ctrl_slave_ratio_cs0),
		     ddr_phy_base + CMD1_REG_PHY_CTRL_SLAVE_RATIO_0);
	__raw_writel(((ctrl_slave_ratio_cs0 << 10) | ctrl_slave_ratio_cs0),
		     ddr_phy_base + CMD2_REG_PHY_CTRL_SLAVE_RATIO_0);

	__raw_writel(cmd_dll_lock_diff,
		     ddr_phy_base + CMD0_REG_PHY_DLL_LOCK_DIFF_0);
	__raw_writel(cmd_dll_lock_diff,
		     ddr_phy_base + CMD1_REG_PHY_DLL_LOCK_DIFF_0);
	__raw_writel(cmd_dll_lock_diff,
		     ddr_phy_base + CMD2_REG_PHY_DLL_LOCK_DIFF_0);
}

static void data_macro_config(u32 macro_num, u32 emif, u32 rd_dqs_cs0,
			      u32 wr_dqs_cs0, u32 fifo_we_cs0, u32 wr_data_cs0)
{
	/* 0xA4 is size of each data macro mmr region.
	 * phy1 is at offset 0x400 from phy0
	 */
	u32 base = (macro_num * 0xA4) + (emif * 0x400);

	__raw_writel(((rd_dqs_cs0 << 10) | rd_dqs_cs0),
		     (DATA0_REG_PHY0_RD_DQS_SLAVE_RATIO_0 + base));
	__raw_writel(((wr_dqs_cs0 << 10) | wr_dqs_cs0),
		     (DATA0_REG_PHY0_WR_DQS_SLAVE_RATIO_0 + base));
	__raw_writel(((PHY_WRLVL_INIT_CS1_DEFINE << 10) |
		      PHY_WRLVL_INIT_CS0_DEFINE),
		     (DATA0_REG_PHY0_WRLVL_INIT_RATIO_0 + base));
	__raw_writel(((PHY_GATELVL_INIT_CS1_DEFINE << 10) |
		      PHY_GATELVL_INIT_CS0_DEFINE),
		     (DATA0_REG_PHY0_GATELVL_INIT_RATIO_0 + base));
	__raw_writel(((fifo_we_cs0 << 10) | fifo_we_cs0),
		     (DATA0_REG_PHY0_FIFO_WE_SLAVE_RATIO_0 + base));
	__raw_writel(((wr_data_cs0 << 10) | wr_data_cs0),
		     (DATA0_REG_PHY0_WR_DATA_SLAVE_RATIO_0 + base));
	__raw_writel(PHY_DLL_LOCK_DIFF_DEFINE,
		     (DATA0_REG_PHY0_DLL_LOCK_DIFF_0 + base));
}

#ifdef CONFIG_SETUP_PLL
static void pll_config(u32, u32, u32, u32, u32);
#if 0
static void pcie_pll_config(void);
#endif
static void audio_pll_config(void);
static void video0_pll_config(void);
static void sata_pll_config(void);
static void modena_pll_config(void);
static void l3_pll_config(void);
static void ddr_pll_config(void);
static void dsp_pll_config(void);
static void iss_pll_config(void);
static void iva_pll_config(void);
static void usb_pll_config(void);
#endif

static void unlock_pll_control_mmr(void);
/*
 * spinning delay to use before udelay works
 */
static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}

#define CPTS_RFT_CLK_VIDEO0_PLL_OUT (0 << 1)
#define CPTS_RFT_CLK_VIDEO1_PLL_OUT (1 << 1)
#define CPTS_RFT_CLK_AUDIO_PLL_OUT (2 << 1)
#define CPTS_RFT_CLK_VIDEO_PLL_CLK2 (3 << 1)
#define CPTS_RFT_CLK_L3_PLL_OUT (4 << 1)

#define CM_CLKOUT_CTL_SOURCE_DEVOSC 0
#define CM_CLKOUT_CTL_SOURCE_USB_PLL 1
#define CM_CLKOUT_CTL_SOURCE_VIDEO0_PLL 2
#define CM_CLKOUT_CTL_SOURCE_AUDIO_PLL 3
#define CM_CLKOUT_CTL_DIV(ratio) (ratio << 3)
#define CM_CLKOUT_CTL_ENABLE (1 << 7)

/*
 * Basic board specific setup
 */
int board_init(void)
{
	u32 regVal;

	/* setup RMII_REFCLK to be sourced from audio_pll */
	/* __raw_writel(CPTS_RFT_CLK_AUDIO_PLL_OUT, RMII_REFCLK_SRC); */

	/* setup RMII_REFCLK to be sourced from video0_pll (default) */
	__raw_writel(CPTS_RFT_CLK_VIDEO0_PLL_OUT, RMII_REFCLK_SRC);

	/* Note TRM (SPRUGZ8A 14 November 2011 – Revised February 2012)
	 * description of CLKOUTDIV field (2.9.3.1 CM_CLKOUT_CTRL, page 535)
	 * is incorrect - ratio is (CLKOUTDIV+1)
	 */
	__raw_writel(CM_CLKOUT_CTL_ENABLE + CM_CLKOUT_CTL_DIV(7) +
		     CM_CLKOUT_CTL_SOURCE_VIDEO0_PLL, CM_CLKOUT_CTL);

	/* program GMII_SEL register for RGMII mode and
	 * disable internal TX clock skew
	 */
	__raw_writel(0x33a, GMII_SEL);

	/* Get Timer and UART out of reset */

	/* UART softreset */
	regVal = __raw_readl(UART_SYSCFG);
	regVal |= 0x2;
	__raw_writel(regVal, UART_SYSCFG);
	while ((__raw_readl(UART_SYSSTS) & 0x1) != 0x1) ;

	/* Disable smart idle */
	regVal = __raw_readl(UART_SYSCFG);
	regVal |= (1 << 3);
	__raw_writel(regVal, UART_SYSCFG);

	/* mach type passed to kernel */
	gd->bd->bi_arch_number = MACH_TYPE_ASI1230;

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_DRAM_1 + 0x100;
	gpmc_init();

	return 0;
}

/*
 * sets uboots idea of sdram size
 */
int dram_init(void)
{
	/* Fill up board info */
	gd->bd->bi_dram[0].start = PHYS_DRAM_1;
	gd->bd->bi_dram[0].size = PHYS_DRAM_1_SIZE;
	return 0;
}

void show_boot_progress(int status)
{

	if (status == BOOT_PROGRESS_HELLOWORLD) {
		/* Status LED API cannot be used before DRAM is initialized,
		   so we call out backend directly
		 */
		__led_init(STATUS_LED_BIT, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT1, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT2, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT3, STATUS_LED_ON);
		return;
	}

	if (status == BOOT_PROGRESS_PASTDRAMINIT) {
		status_led_set(0, STATUS_LED_ON);
		status_led_set(1, STATUS_LED_OFF);
		status_led_set(2, STATUS_LED_OFF);
		status_led_set(3, STATUS_LED_OFF);
		return;
	}

	if (status == BOOT_PROGRESS_PASTRELOC) {
#ifdef CONFIG_TI814X_MIN_CONFIG
		/* Turn on LED1 and turn off LED0 to indicate we are past relocation */
		status_led_set(0, STATUS_LED_OFF);
		status_led_set(1, STATUS_LED_ON);
		status_led_set(2, STATUS_LED_OFF);
		status_led_set(3, STATUS_LED_OFF);
#else
		/* Indicate we are running 2nd stage U-Boot */
		status_led_set(0, STATUS_LED_OFF);
		status_led_set(1, STATUS_LED_OFF);
		status_led_set(2, STATUS_LED_ON);
		status_led_set(3, STATUS_LED_OFF);
#endif
		return;
	}

	if (status < 0)
		status_led_set(0, STATUS_LED_OFF);
	else
		status_led_set(0, STATUS_LED_ON);
}

int misc_init_r(void)
{
	show_boot_progress(BOOT_PROGRESS_PASTRELOC);

#if defined(CONFIG_TI81XX_PCIE_BOOT) && defined(CONFIG_TI814X_MIN_CONFIG)
	extern int pcie_init(void);
	printf("\nSetting up for pcie boot...\n");
	pcie_init();
	return 0;
#endif

#ifdef CONFIG_TI814X_MIN_CONFIG
	/* If eng mode is enabled do not execute preboot but rather bootdelay/console */
	if (is_eng_mode_enabled()) {
		setenv("preboot", "\0");
		printf("Booting in engineering mode\n");
	}
#ifndef DEBUG
	else {
		gd->flags |= GD_FLG_SILENT;
	}
#endif /* DEBUG */
#endif /* CONFIG_TI814X_MIN_CONFIG */

	return 0;
}

void config_asi1230_mddr(void)
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
	cmd_macro_config(DDR_PHY0, DDR3_PHY_INVERT_CLKOUT_OFF,
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
	__raw_writel(mDDR_EMIF_READ_LATENCY, EMIF4_0_DDR_PHY_CTRL_1);
	__raw_writel(mDDR_EMIF_READ_LATENCY, EMIF4_0_DDR_PHY_CTRL_1_SHADOW);
	__raw_writel(mDDR_EMIF_TIM1, EMIF4_0_SDRAM_TIM_1);
	__raw_writel(mDDR_EMIF_TIM1, EMIF4_0_SDRAM_TIM_1_SHADOW);
	__raw_writel(mDDR_EMIF_TIM2, EMIF4_0_SDRAM_TIM_2);
	__raw_writel(mDDR_EMIF_TIM2, EMIF4_0_SDRAM_TIM_2_SHADOW);
	__raw_writel(mDDR_EMIF_TIM3, EMIF4_0_SDRAM_TIM_3);
	__raw_writel(mDDR_EMIF_TIM3, EMIF4_0_SDRAM_TIM_3_SHADOW);
	__raw_writel(mDDR_EMIF_SDRAM_CONFIG, EMIF4_0_SDRAM_CONFIG);
	__raw_writel(mDDR_EMIF_SDRAM_CONFIG2, EMIF4_0_SDRAM_CONFIG2);

	__raw_writel(mDDR_EMIF_REF_CTRL | DDR_EMIF_REF_TRIGGER,
		     EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(mDDR_EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);
	__raw_writel(mDDR_EMIF_SDRAM_ZQCR, EMIF4_0_SDRAM_ZQCR);
	__raw_writel(mDDR_EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(mDDR_EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);

	__raw_writel(mDDR_EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(mDDR_EMIF_REF_CTRL, EMIF4_0_SDRAM_REF_CTRL_SHADOW);

	/* The first write/read results in garbage. Is there something wrong with
	   the dram controller's setup?

	   In the meantime we write, read and then wait a little before moving on.
	 */
	*((ulong *) PHYS_DRAM_1) = 0xFFFFFFFF;
	ulong tmp = *((ulong *) PHYS_DRAM_1);
	tmp = 0;
	delay(0xFFF);
}

#ifdef CONFIG_SETUP_PLL
static void audio_pll_config()
{
	pll_config(AUDIO_PLL_BASE, AUDIO_N, AUDIO_M, AUDIO_M2, AUDIO_CLKCTRL);
}

static void video0_pll_config()
{
	pll_config(VIDEO0_PLL_BASE, AUDIO_N, AUDIO_M, AUDIO_M2, AUDIO_CLKCTRL);
}

static void sata_pll_config()
{
	__raw_writel(0xC12C003C, SATA_PLLCFG1);
	__raw_writel(0x004008E0, SATA_PLLCFG3);
	delay(0xFFFF);

	__raw_writel(0x80000004, SATA_PLLCFG0);
	delay(0xFFFF);

	/* Enable PLL LDO */
	__raw_writel(0x80000014, SATA_PLLCFG0);
	delay(0xFFFF);

	/* Enable DIG LDO, ENBGSC_REF, PLL LDO */
	__raw_writel(0x80000016, SATA_PLLCFG0);
	delay(0xFFFF);

	__raw_writel(0xC0000017, SATA_PLLCFG0);
	delay(0xFFFF);

	/* wait for ADPLL lock */
	while (((__raw_readl(SATA_PLLSTATUS) & 0x01) == 0x0)) ;

}

static void usb_pll_config()
{
	pll_config(USB_PLL_BASE, USB_N, USB_M, USB_M2, USB_CLKCTRL);
}

static void modena_pll_config()
{
	pll_config(MODENA_PLL_BASE,
		   MODENA_N, MODENA_M, MODENA_M2, MODENA_CLKCTRL);
}

static void l3_pll_config()
{
	pll_config(L3_PLL_BASE, L3_N, L3_M, L3_M2, L3_CLKCTRL);
}

static void ddr_pll_config()
{
	pll_config(DDR_PLL_BASE, DDR_N, DDR_M, DDR_M2, DDR_CLKCTRL);
}

static void dsp_pll_config()
{
	pll_config(DSP_PLL_BASE, DSP_N, DSP_M, DSP_M2, DSP_CLKCTRL);
}

static void iss_pll_config()
{
	pll_config(ISS_PLL_BASE, ISS_N, ISS_M, ISS_M2, ISS_CLKCTRL);
}

static void iva_pll_config()
{
	pll_config(IVA_PLL_BASE, IVA_N, IVA_M, IVA_M2, IVA_CLKCTRL);
}

/*
 * configure individual ADPLLJ
 */
static void pll_config(u32 base, u32 n, u32 m, u32 m2, u32 clkctrl_val)
{
	u32 m2nval, mn2val, read_clkctrl = 0;

	m2nval = (m2 << 16) | n;
	mn2val = m;

	/* by-pass pll */
	read_clkctrl = __raw_readl(base + ADPLLJ_CLKCTRL);
	__raw_writel((read_clkctrl | 0x00800000), (base + ADPLLJ_CLKCTRL));
	while ((__raw_readl(base + ADPLLJ_STATUS) & 0x101) != 0x101) ;
	read_clkctrl = __raw_readl(base + ADPLLJ_CLKCTRL);
	__raw_writel((read_clkctrl & 0xfffffffe), (base + ADPLLJ_CLKCTRL));

	/*
	 * ref_clk = 20/(n + 1);
	 * clkout_dco = ref_clk * m;
	 * clk_out = clkout_dco/m2;
	 */

	__raw_writel(m2nval, (base + ADPLLJ_M2NDIV));
	__raw_writel(mn2val, (base + ADPLLJ_MN2DIV));

	/* Load M2, N2 dividers of ADPLL */
	__raw_writel(0x1, (base + ADPLLJ_TENABLEDIV));
	__raw_writel(0x0, (base + ADPLLJ_TENABLEDIV));

	/* Loda M, N dividers of ADPLL */
	__raw_writel(0x1, (base + ADPLLJ_TENABLE));
	__raw_writel(0x0, (base + ADPLLJ_TENABLE));

	read_clkctrl = __raw_readl(base + ADPLLJ_CLKCTRL);

	if (MODENA_PLL_BASE == base)
		/* Clear IDLE bit, making PLL active */
		__raw_writel((read_clkctrl & 0xff7fffff) | clkctrl_val,
			     base + ADPLLJ_CLKCTRL);
	else
		/* Clear IDLE and SELFREQDCO bits 
		 * typical clkctrl_val 0x801 sets HS2 DCO range and TINITZ
		 */
		__raw_writel((read_clkctrl & 0xff7fe3ff) | clkctrl_val,
			     base + ADPLLJ_CLKCTRL);
	/* Wait for phase and freq lock */
	while ((__raw_readl(base + ADPLLJ_STATUS) & 0x600) != 0x600) ;

}
#endif

/*
 * Enable the clks & power for perifs (TIMER1, UART0,...)
 */
void per_clocks_enable(void)
{
	u32 temp;

	__raw_writel(0x2, CM_ALWON_L3_SLOW_CLKSTCTRL);

	/* TODO: No module level enable as in ti8148 ??? */
#if 0
	/* TIMER 1 */
	__raw_writel(0x2, CM_ALWON_TIMER_1_CLKCTRL);
#endif
	/* Selects OSC0 (20MHz) for DMTIMER1 */
	temp = __raw_readl(DMTIMER_CLKSRC);
	temp &= ~(0x7 << 3);
	temp |= (0x4 << 3);
	__raw_writel(temp, DMTIMER_CLKSRC);

#if 0
	while (((__raw_readl(CM_ALWON_L3_SLOW_CLKSTCTRL) & (0x80000 << 1)) >>
		(19 + 1)) != 1) ;
	while (((__raw_readl(CM_ALWON_TIMER_1_CLKCTRL) & 0x30000) >> 16) != 0) ;
#endif
	__raw_writel(0x2, (DM_TIMER1_BASE + 0x54));
	while (__raw_readl(DM_TIMER1_BASE + 0x10) & 1) ;

	__raw_writel(0x1, (DM_TIMER1_BASE + 0x38));

	/* UARTs */
	__raw_writel(0x2, CM_ALWON_UART_0_CLKCTRL);
	while (__raw_readl(CM_ALWON_UART_0_CLKCTRL) != 0x2) ;

	__raw_writel(0x2, CM_ALWON_UART_1_CLKCTRL);
	while (__raw_readl(CM_ALWON_UART_1_CLKCTRL) != 0x2) ;

	__raw_writel(0x2, CM_ALWON_UART_2_CLKCTRL);
	while (__raw_readl(CM_ALWON_UART_2_CLKCTRL) != 0x2) ;

	while ((__raw_readl(CM_ALWON_L3_SLOW_CLKSTCTRL) & 0x2100) != 0x2100) ;

	/* GPIO0 */
	__raw_writel(0x102, CM_ALWON_GPIO_0_CLKCTRL);
	while (__raw_readl(CM_ALWON_GPIO_0_CLKCTRL) != 0x102) ;
	/* GATINGRATIO 0x3: Functional clock is interface clock divided by 8 */
	__raw_writel((0x3 << 1), GPIO0_BASE + GPIO_CTRL);

	/* SPI */
	__raw_writel(0x2, CM_ALWON_SPI_CLKCTRL);
	while (__raw_readl(CM_ALWON_SPI_CLKCTRL) != 0x2) ;

	/* I2C0 and I2C2 */
	__raw_writel(0x2, CM_ALWON_I2C_0_CLKCTRL);
	while (__raw_readl(CM_ALWON_I2C_0_CLKCTRL) != 0x2) ;
	/* Ethernet */
	__raw_writel(0x2, CM_ETHERNET_CLKSTCTRL);
	__raw_writel(0x2, CM_ALWON_ETHERNET_0_CLKCTRL);
	while ((__raw_readl(CM_ALWON_ETHERNET_0_CLKCTRL) & 0x30000) != 0) ;
	__raw_writel(0x2, CM_ALWON_ETHERNET_1_CLKCTRL);
	/* HSMMC */
	__raw_writel(0x2, CM_ALWON_HSMMC_CLKCTRL);
	while (__raw_readl(CM_ALWON_HSMMC_CLKCTRL) != 0x2) ;

	/*
	 * McASP0 and McASP2
	 * select mcasp2 clk from sys_clk_22 (OSC 0)
	 * so that audio clk (sys_clk_20) can be used for RMII
	 * ToDo :
	 * This can be removed once kernel exports set_parent()
	 */
	__raw_writel(0x2, CM_AUDIOCLK_MCASP0_CLKSEL);
	while (__raw_readl(CM_AUDIOCLK_MCASP0_CLKSEL) != 0x2) ;
	__raw_writel(0x2, CM_AUDIOCLK_MCASP2_CLKSEL);
	while (__raw_readl(CM_AUDIOCLK_MCASP2_CLKSEL) != 0x2) ;

	/* CLKOUT 1,0 MUX setup (TRMp522 */
#define CLKOUT_MUX_SOURCE_PRCM_SYSCLK_OUT 0
#define CLKOUT_MUX_SOURCE_L3_DPLL 5
#define CLKOUT_MUX_SOURCE_OSC0 6
#define CLKOUT_MUX_INIT ((CLKOUT_MUX_SOURCE_OSC0 << 16) +\
			(CLKOUT_MUX_SOURCE_PRCM_SYSCLK_OUT << 0))

	__raw_writel(CLKOUT_MUX_INIT, CLKOUT_MUX);
	while (__raw_readl(CLKOUT_MUX) != CLKOUT_MUX_INIT) ;

	/* WDT */
	/* For WDT to be functional, it needs to be first stopped by writing
	 * the pattern 0xAAAA followed by 0x5555 in the WDT start/stop register.
	 * After that a write-once register in Control module needs to be
	 * configured to unfreeze the timer.
	 * Note: It is important to stop the watchdog before unfreezing it
	 */
	__raw_writel(0xAAAA, WDT_WSPR);
	while (__raw_readl(WDT_WWPS) != 0x0) ;
	__raw_writel(0x5555, WDT_WSPR);
	while (__raw_readl(WDT_WWPS) != 0x0) ;

	/* Unfreeze WDT */
	__raw_writel(0x13, WDT_UNFREEZE);
}

/*
 * inits clocks for PRCM as defined in clocks.h
 */
void prcm_init(u32 in_ddr)
{
	/* Enable the control module */
	__raw_writel(0x2, CM_ALWON_CONTROL_CLKCTRL);

#ifdef CONFIG_SETUP_PLL
	/* Setup the various plls */
	audio_pll_config();
	video0_pll_config();
	sata_pll_config();
	modena_pll_config();
	l3_pll_config();
	ddr_pll_config();
	dsp_pll_config();
	iva_pll_config();
	iss_pll_config();

	usb_pll_config();

	/*  With clk freqs setup to desired values,
	 *  enable the required peripherals
	 */
	per_clocks_enable();
#endif
}

/*
 * board specific muxing of pins
 */

void set_muxconf(u32 * reg_addr, u8 conf_val)
{
	u32 reg_val = __raw_readl(*reg_addr);
	/* bits 6:4 of val goes to bits [18:16] of register
	 * bit 19 of register is unchanged
	 */
	reg_val &= ~0x0003000F;	/* keep bit 19, clear 17-16, 7-0 */
	reg_val |= 0x00040000;	/* set bit 18 */
	reg_val |= (conf_val << 12) & 0x00030000;

	/* low nibble of val encodes pin mode selection */
	conf_val &= 0x0F;
	reg_val |= conf_val ? (1 << (conf_val - 1)) : 0;

	__raw_writel(reg_val, *reg_addr);
	*reg_addr += 4;
}

void set_muxconf_regs(void)
{
	u32 pin_ctrl_addr = PIN_CTRL_BASE;

	/* Generate the pinmux setup as code with macro expansion instead of storing an array because 
	 * set_muxconf_regs() is executed before relocation and the compiler can generate references
	 * to data sections when initializing local variables on the stack.
	 */
#define MUX_VAL(offset, value) set_muxconf(&pin_ctrl_addr, value);
#define MUX_NOP(pin) pin_ctrl_addr += 4;	/* Skip reserved pin */
#include "mux.h"

	/* MMC/SD pull-down enable */
	__raw_writel(0x000C0040, 0x48140928);
}

void unlock_pll_control_mmr()
{
	/* ??? */
	__raw_writel(0x1EDA4C3D, 0x481C5040);
	__raw_writel(0x2FF1AC2B, 0x48140060);
	__raw_writel(0xF757FDC0, 0x48140064);
	__raw_writel(0xE2BC3A6D, 0x48140068);
	__raw_writel(0x1EBF131D, 0x4814006c);
	__raw_writel(0x6F361E05, 0x48140070);

}

/*
 * early system init of muxing and clocks.
 */
void s_init(u32 in_ddr)
{
	/* TODO: Revisit enabling of I/D-cache in 1st stage */
#if 0
	icache_enable();
	dcache_enable();
#endif

	/*
	 * Disable Write Allocate on miss to avoid starvation of other masters
	 * (than A8).
	 *
	 * Ref DM814x Erratum: TODO
	 */
	l2_disable_wa();

	/* Can be removed as A8 comes up with L2 enabled */
	l2_cache_enable();
	unlock_pll_control_mmr();
	/* Setup the PLLs and the clocks for the peripherals */
	prcm_init(in_ddr);
	/* Do the required pin-muxing before modules are setup
	   (move this back to the beginning of board_init() if it is too early).
	 */
	set_muxconf_regs();

	/* Indicate to the outside world that we are alive */
	show_boot_progress(BOOT_PROGRESS_HELLOWORLD);

	if (!in_ddr)
		config_asi1230_mddr();	/* Do DDR settings */

	/* mDDR is initialized */
	show_boot_progress(BOOT_PROGRESS_PASTDRAMINIT);
}

/*
 * Reset the board
 */
void reset_cpu(ulong addr)
{
	addr = __raw_readl(PRM_DEVICE_RSTCTRL);
	addr &= ~BIT(1);
	addr |= BIT(1);
	__raw_writel(addr, PRM_DEVICE_RSTCTRL);
}

#ifdef CONFIG_DRIVER_TI_CPSW

#define PHY_VSC8601_ID 0x00070421
#define PHY_VSC8601_EXCTRL1_REG 0x17
#define PHY_VSC8601_LEDCTRL_REG 0x1B
#define PHY_VSC8601_RXCLKSKEW 0x100

/* TODO : Check for the board specific PHY */
static void phy_init(char *name, int addr)
{
	unsigned short val;
	unsigned int cntr = 0;
	unsigned int phy_id = 0;

	miiphy_reset(name, addr);

	udelay(100000);

	miiphy_read(name, addr, PHY_PHYIDR1, &val);
	phy_id = (val << 16) & 0xffff0000;
	miiphy_read(name, addr, PHY_PHYIDR2, &val);
	phy_id |= val & 0x0000ffff;

	if (phy_id == PHY_VSC8601_ID) {
		/* Enable RGMII RX/TX clock skew */
		miiphy_read(name, addr, PHY_VSC8601_EXCTRL1_REG, &val);
		val |= PHY_VSC8601_RXCLKSKEW;
		miiphy_write(name, addr, PHY_VSC8601_EXCTRL1_REG, val);
		miiphy_read(name, addr, PHY_VSC8601_EXCTRL1_REG, &val);

		/* Blink RJ-45 activity LED at a 10Hz rate */
		miiphy_read(name, addr, PHY_VSC8601_LEDCTRL_REG, &val);
		val |= 0x06;
		miiphy_write(name, addr, PHY_VSC8601_LEDCTRL_REG, val);
		miiphy_read(name, addr, PHY_VSC8601_LEDCTRL_REG, &val);
	}

	/* Enable Autonegotiation */
	if (miiphy_read(name, addr, PHY_BMCR, &val) != 0) {
		printf("failed to read bmcr\n");
		return;
	}
	val |= PHY_BMCR_DPLX | PHY_BMCR_AUTON | PHY_BMCR_100_MBPS;
	if (miiphy_write(name, addr, PHY_BMCR, val) != 0) {
		printf("failed to write bmcr\n");
		return;
	}
	miiphy_read(name, addr, PHY_BMCR, &val);

	/* Setup GIG advertisement */
	miiphy_read(name, addr, PHY_1000BTCR, &val);
	val |= PHY_1000BTCR_1000FD;
	val &= ~PHY_1000BTCR_1000HD;
	miiphy_write(name, addr, PHY_1000BTCR, val);
	miiphy_read(name, addr, PHY_1000BTCR, &val);

	/* Setup general advertisement */
	if (miiphy_read(name, addr, PHY_ANAR, &val) != 0) {
		printf("failed to read anar\n");
		return;
	}
	val |= (PHY_ANLPAR_10 | PHY_ANLPAR_10FD | PHY_ANLPAR_TX |
		PHY_ANLPAR_TXFD);
	if (miiphy_write(name, addr, PHY_ANAR, val) != 0) {
		printf("failed to write anar\n");
		return;
	}
	miiphy_read(name, addr, PHY_ANAR, &val);

	/* Restart auto negotiation */
	miiphy_read(name, addr, PHY_BMCR, &val);
	val |= PHY_BMCR_RST_NEG;
	miiphy_write(name, addr, PHY_BMCR, val);

	/*check AutoNegotiate complete - it can take upto 3 secs */
	do {
		udelay(40000);
		cntr++;

		if (!miiphy_read(name, addr, PHY_BMSR, &val)) {
			if (val & PHY_BMSR_AUTN_COMP)
				break;
		}
	} while (cntr < 250);

	if (!miiphy_read(name, addr, PHY_BMSR, &val)) {
		if (!(val & PHY_BMSR_AUTN_COMP))
			printf("Auto negotitation failed\n");
	}
}

static void cpsw_control(int enabled)
{
	/* nothing for now */
	/* TODO : VTP was here before */
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
	 .slave_reg_ofs = 0x50,
	 .sliver_reg_ofs = 0x700,
	 .phy_id = 1,
	 },
	{
	 .slave_reg_ofs = 0x90,
	 .sliver_reg_ofs = 0x740,
	 .phy_id = 0,
	 },
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base = TI814X_CPSW_MDIO_BASE,
	.cpsw_base = TI814X_CPSW_BASE,
	.mdio_div = 0xff,
	.channels = 8,
	.cpdma_reg_ofs = 0x100,
	.cpdma_sram_ofs = 0x200,
	.slaves = 1,
	.slave_data = cpsw_slaves,
	.ale_reg_ofs = 0x600,
	.ale_entries = 1024,
	.host_port_reg_ofs = 0x28,
	.hw_stats_reg_ofs = 0x400,
	.mac_control = (1 << 5) /* MIIEN      */ ,
	.control = cpsw_control,
	.phy_init = phy_init,
	.host_port_num = 0,
};

extern void cpsw_eth_set_mac_addr(const u_int8_t * addr);

int board_eth_init(bd_t * bis)
{
	u_int8_t mac_addr[6];
	u_int32_t mac_hi, mac_lo;

	if (!eth_getenv_enetaddr("ethaddr", mac_addr)) {
		char mac_addr_env[20];

		printf("<ethaddr> not set. Reading from E-fuse\n");
		/* try reading mac address from efuse */
		mac_lo = __raw_readl(MAC_ID0_LO);
		mac_hi = __raw_readl(MAC_ID0_HI);
		mac_addr[0] = mac_hi & 0xFF;
		mac_addr[1] = (mac_hi & 0xFF00) >> 8;
		mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
		mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
		mac_addr[4] = mac_lo & 0xFF;
		mac_addr[5] = (mac_lo & 0xFF00) >> 8;
		/* set the ethaddr variable with MACID detected */
		sprintf(mac_addr_env, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac_addr[0], mac_addr[1], mac_addr[2],
			mac_addr[3], mac_addr[4], mac_addr[5]);
		setenv("ethaddr", mac_addr_env);
	}

	if (is_valid_ether_addr(mac_addr)) {
		printf("Detected MACID:%x:%x:%x:%x:%x:%x\n", mac_addr[0],
		       mac_addr[1], mac_addr[2], mac_addr[3],
		       mac_addr[4], mac_addr[5]);
		cpsw_eth_set_mac_addr(mac_addr);
	} else {
		printf("Caution:using static MACID!! Set <ethaddr> variable\n");
	}

	if (PG1_0 != get_cpu_rev()) {
		cpsw_slaves[0].phy_id = 0;
		cpsw_slaves[1].phy_id = 1;
	}

	return cpsw_register(&cpsw_data);
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t * bis)
{
	omap_mmc_init(0);
	return 0;
}
#endif
