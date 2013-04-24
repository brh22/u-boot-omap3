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
#include <exports.h>
#include <asm/cache.h>
#include <asm/arch/cpu.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/clock.h>
#include <asm/arch/mem.h>
 #include <asm/arch/ddr_defs_ti814x.h>
/* It would be better to #include <asm/arch/mmc.h> rather than <asm/arch/mmc_host_def.h>,
 * but right now the latter causes a macro redefinition warning.
 */
#include <asm/arch/mmc_host_def.h>
#include <status_led.h>
#include <ns16550.h>
#include <spi_flash.h>

#include <mmc.h>
#include <ext2fs.h>

#ifdef CONFIG_DRIVER_TI_CPSW
#include <net.h>
#include <miiphy.h>
#include <netdev.h>
#endif /* CONFIG_DRIVER_TI_CPSW */

DECLARE_GLOBAL_DATA_PTR;

/* TODO: move these constants to a better place */
#define GPIO_CTRL       0x130
#define GPIO_OE         0x134
#define GPIO_DATAIN     0x138
#define GPIO_DATAOUT    0x13C
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194

#define NVMEMDATA_MAGIC (0xadb8e496)
struct nvmem_data {
	uint32_t magic;
	uint32_t checksum;
	uint32_t length;
	uint32_t version;

	uint32_t write_count;

	uint16_t model;
	char hw_rev1;
	unsigned char hw_rev2;
	uint32_t serial;

	uint8_t active_part_num;
	uint8_t avail_part_num[2];
	uint32_t boot_flags;

	uint8_t mac_addr_count;
	char mac_addr1[6];
	char mac_addr2[6];
} __attribute__ ((__packed__));

#ifndef CONFIG_SF_DEFAULT_MODE
# define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#endif

extern void config_asi1230_ddr2(void);
extern void config_asi1230_lpddr(void);
extern void enable_gpmc_cs_config(const u32 *gpmc_config,
		struct gpmc_cs *cs, u32 base, u32 size);

static u32 board_hw_id = 0;

/* Implement board specific LED API. LED1..4 are attached to GPIO0[1..4]
 * and LED5..8 to GPIO0[15..18]
 */
#define LED_MASK 	0x0007801E

void __led_toggle(led_id_t mask)
{
	mask &= LED_MASK;
	u32 o_reg_val = __raw_readl(GPIO0_BASE + GPIO_DATAOUT) ^ mask;
	__raw_writel(o_reg_val, GPIO0_BASE + GPIO_DATAOUT);
}

void __led_init(led_id_t mask, int state)
{
	/* Make sure we do not try to set GPIO OE bits that do not belong to LEDs
	   then set the output enable register and proceed to set the state of the LED.
	 */
	mask &= LED_MASK;
	u32 oe_reg_val = __raw_readl(GPIO0_BASE + GPIO_OE) & (~mask);
	__raw_writel(oe_reg_val, GPIO0_BASE + GPIO_OE);
	__led_set(mask, state);
}

void __led_set(led_id_t mask, int state)
{
	u32 o_reg_val = mask & LED_MASK;
	/* LEDs are lit when the corresponding GPIO is driven low */
	if (state != STATUS_LED_OFF){
		__raw_writel(o_reg_val, GPIO0_BASE + GPIO_CLEARDATAOUT);
	}
	else
		__raw_writel(o_reg_val, GPIO0_BASE + GPIO_SETDATAOUT);
}

int is_factory_boot_enabled(void)
{
	/* Read GP1[4] to see if we should boot in eng mode */
	int factory_boot = !(__raw_readl(GPIO1_BASE + GPIO_DATAIN) & (1 << 4));
	if (factory_boot)
		return 1;
	return 0;
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

/* asi1230_hw_id() must be called after GPIOs have been init-ed */
u32 asi1230_hw_id(void)
{
	/* Read back HW ID from GP1[0..2] */
	return __raw_readl(GPIO1_BASE + GPIO_DATAIN) & 0x7;
}

void asi1230_peripheral_reset( int state )
{
	/* peripheral reset (RESETA-) is on GP0[6] and is active low */
	/* set OE low, making the pin an output */
	u32 oe_reg_val = __raw_readl(GPIO0_BASE + GPIO_OE) & (~(1<<6));
	__raw_writel(oe_reg_val, GPIO0_BASE + GPIO_OE);
	if (0 == state)
		__raw_writel((1<<6), GPIO0_BASE + GPIO_CLEARDATAOUT);
	else
		__raw_writel((1<<6), GPIO0_BASE + GPIO_SETDATAOUT);
}

/*  */

void cmd_macro_config(u32 ddr_phy, u32 inv_clk_out,
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

void data_macro_config(u32 macro_num, u32 emif, u32 rd_dqs_cs0,
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
static void video1_pll_config(void);
static void sata_pll_config(void);
static void modena_pll_config(void);
static void l3_pll_config(void);
static void dsp_pll_config(void);
static void iss_pll_config(void);
static void iva_pll_config(void);
static void usb_pll_config(void);
static void dss_pll_config(void);
#endif

static void unlock_pll_control_mmr(void);
/*
 * spinning delay to use before udelay works
 */
inline void asi1230_delay(unsigned long loops)
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

#define ASI2416_MODULE_BASE 0x01000000	/* from sec 2.12.1 of DM814x DS */
static u32 gpmc_asi2416_module[GPMC_MAX_REG] = {
	0x00001000,	/* CONFIG1 - 16bit, async NOR like. non-mux addr */
	0x000F0F00,	/* CONFIG2 - CSONTIME=0, CSRDOFFTIME, CSWROFFTIME = 150ns*/
	0x00000000,	/* CONFIG3 - ADV times all 0*/
	0x0C000F00,	/* CONFIG4 - OEONTIME, WEONTIME=0ns, OEOFFTIME=150,WEOFFTIME=120ns */
	0x000E1010,	/* CONFIG5 - RDCYCLETIME=160ns, WRCYCLETIME=160ns, RDACCESSTIME = 140ns*/
	0x0E0004C4, 	/* CONFIG6 - WRACCESSTIME=140ns, CYCLE2CYCLEDELAY=40ns, BUSTURNAROUNDTIME=40ns*/
	0x00000000	/* CONFIG7 - start with CS disabled */
};

u32 asi1230_dram_size(void)
{
	/* ID 0x01 has 64Mbytes */
	if (board_hw_id == 0x1)
		return 0x4000000;

	/* ID 0x00 has 128Mbytes */
	if (board_hw_id == 0x0)
		return 0x8000000;

	/* Default to 64Mbytes for unknown IDs */
	return 0x4000000;
}

/*
 * Basic board specific setup
 */
int board_init(void)
{
	u32 *gpmc_config;
	u32 regVal=0;

	show_boot_progress(BOOT_PROGRESS_PASTRELOC);

	board_hw_id = asi1230_hw_id();

#ifdef CONFIG_TI814X_MIN_CONFIG
	/* set RESETA- low and then high again */
	asi1230_peripheral_reset(0);
	asi1230_delay(0xFFFF);	/* measured 200us delay with a 600MHz ARM A8 clock */
	asi1230_peripheral_reset(1);
#endif

	/* setup RMII_REFCLK (CPTS_RFTCLK in TRM) to be sourced from
	 * video1_pll so we can use video0 for audio clocks */
	__raw_writel(CPTS_RFT_CLK_VIDEO1_PLL_OUT, RMII_REFCLK_SRC);

	/* set CLKOUT_MUX register so that:
	 * CLKOUT0 is SYSCLK (used to output Video 0 PLL for audio clock)
	 * CLKOUT1 is ISS PLL out (used to generate 25MHz for the ethernet PHY)
	*/
	__raw_writel(0x00030000, CLKOUT_MUX);

	/* Source CLKOUT0 from SYSCLK-VIDEO0 PLL
	 *
	 * Note TRM (SPRUGZ8A 14 November 2011 – Revised February 2012)
	 * description of CLKOUTDIV field (2.9.3.1 CM_CLKOUT_CTRL, page 535)
	 * is incorrect - ratio is (CLKOUTDIV+1). Ranges between 1..8
	 *
	 * Set to divide by 1
	 */
	__raw_writel(CM_CLKOUT_CTL_ENABLE + CM_CLKOUT_CTL_DIV(0) +
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

	/* setup GPMC, CS0..4 for 8 bit async, non-multiplexed access, for ASI2416 modules*/
	gpmc_init();
	gpmc_config = gpmc_asi2416_module;
	enable_gpmc_cs_config(gpmc_config, &gpmc_cfg->cs[0],
			ASI2416_MODULE_BASE, GPMC_SIZE_16M);
	enable_gpmc_cs_config(gpmc_config, &gpmc_cfg->cs[1],
			ASI2416_MODULE_BASE+0x01000000, GPMC_SIZE_16M);
	enable_gpmc_cs_config(gpmc_config, &gpmc_cfg->cs[2],
			ASI2416_MODULE_BASE+0x02000000, GPMC_SIZE_16M);
	enable_gpmc_cs_config(gpmc_config, &gpmc_cfg->cs[3],
			ASI2416_MODULE_BASE+0x03000000, GPMC_SIZE_16M);
	enable_gpmc_cs_config(gpmc_config, &gpmc_cfg->cs[4],
			ASI2416_MODULE_BASE+0x04000000, GPMC_SIZE_16M);

	#if 0
	/* test loop to see if we see anything on the GPMC */
	while(1)
	{
		/* 16bit accesses */
		__raw_writew(0, ASI2416_MODULE_BASE );	/* CS0 */
		__raw_writew(1, ASI2416_MODULE_BASE );
		__raw_readw( ASI2416_MODULE_BASE );
		__raw_writew(0, ASI2416_MODULE_BASE+0x01000000 );	/* CS1 */
	}
	#endif
	return 0;
}

/*
 * sets uboots idea of sdram size
 */
int dram_init(void)
{
	/* Fill up board info */
	gd->bd->bi_dram[0].start = PHYS_DRAM_1;
	gd->bd->bi_dram[0].size = asi1230_dram_size();
	return 0;
}

void show_boot_progress(int status)
{

	if (status == BOOT_PROGRESS_HELLOWORLD) {
		/* Status LED API cannot be used before DRAM is initialized
		   *and* we are past relocation so we call our backend directly
		 */
		__led_init(STATUS_LED_BIT, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT1, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT2, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT3, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT4, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT5, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT6, STATUS_LED_ON);
		__led_init(STATUS_LED_BIT7, STATUS_LED_ON);
		return;
	}

	if (status == BOOT_PROGRESS_PASTDRAMINIT) {
		/* Status LED API cannot be used before DRAM is initialized
		   *and* we are past relocation so we call our backend directly
		 */
		__led_set(STATUS_LED_BIT,  STATUS_LED_OFF);
		__led_set(STATUS_LED_BIT1, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT2, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT3, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT4, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT5, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT6, STATUS_LED_ON);
		__led_set(STATUS_LED_BIT7, STATUS_LED_ON);
		return;
	}

	if (status == BOOT_PROGRESS_PASTRELOC) {
#ifdef CONFIG_TI814X_MIN_CONFIG
		/* Turn on LED1 and turn off LED0 to indicate we are past relocation */
		status_led_set(0, STATUS_LED_ON);
		status_led_set(1, STATUS_LED_OFF);
		status_led_set(2, STATUS_LED_ON);
		status_led_set(3, STATUS_LED_ON);
#else
		/* Indicate we are running 2nd stage U-Boot */
		status_led_set(0, STATUS_LED_ON);
		status_led_set(1, STATUS_LED_ON);
		status_led_set(2, STATUS_LED_OFF);
		status_led_set(3, STATUS_LED_ON);
#endif
		return;
	}

	if (status < 0)
		status_led_set(0, STATUS_LED_OFF);
	else
		status_led_set(0, STATUS_LED_ON);
}

int load_ext2_file(char *addr, char *iface_name, int dev, int part, char *filename)
{
	int err = -1;
	int part_length;
	disk_partition_t info;
	block_dev_desc_t *dev_desc = NULL;

	struct mmc *mmc = find_mmc_device(dev);
	if (!mmc)
		goto exit_err;

	mmc_init(mmc);

	dev_desc = get_dev(iface_name, dev);
	if (dev_desc==NULL) {
		printf ("** Block device %s %d not supported\n", iface_name, dev);
		goto exit_err;
	}

	if (get_partition_info(dev_desc, part, &info)) {
		printf ("** Bad partition %d **\n", part);
		goto exit_err;
	}

	if (strncmp((char *)info.type, BOOT_PART_TYPE, sizeof(info.type)) != 0) {
		printf ("** Invalid partition type \"%.32s\""
			" (expect \"" BOOT_PART_TYPE "\")\n",
			info.type);
		goto exit_err;
	}

	printf ("Loading file \"%s\" "
		"from %s device %d:%d (%.32s)\n",
		filename, iface_name, dev, part, info.name);

	part_length = ext2fs_set_blk_dev(dev_desc, part);
	if (part_length == 0) {
		printf ("** Bad partition - %s %d:%d **\n", iface_name, dev, part);
		goto exit_err;
	}

	if (!ext2fs_mount(part_length)) {
		printf ("** Bad ext2 partition or disk - %s %d:%d **\n",
			iface_name, dev, part);
		goto exit_err;
	}

	int filelen = ext2fs_open(filename);
	if (filelen < 0) {
		printf("** File not found %s\n", filename);
		goto exit_err;
	}

	if (ext2fs_read(addr, filelen) != filelen) {
		printf("** Unable to read \"%s\" from %s %d:%d **\n",
			filename, iface_name, dev, part);
		goto exit_err;
	}

	err = 0;

exit_err:
	ext2fs_close();
	return err;
}

int setenv_for_engboot(int part_num)
{
	run_command("setenv bootcmd 'mmc rescan 0; "
		"ext2load mmc 0:${bpart_num} 0x80800000 /boot/u-boot.sd; "
		"go 0x80800000'", 0);
	return 0;
}

int available_mem_mb(void)
{
	return gd->bd->bi_dram[0].size / 0x100000;
}

int set_memmap(void)
{
	char val[64];
	char *endp = NULL;
	unsigned int phy_start = gd->bd->bi_dram[0].start;
	unsigned int phy_end = phy_start + gd->bd->bi_dram[0].size;
	unsigned int resm_size = 0;
	unsigned int resm_start = phy_start;
	unsigned int total_phy = available_mem_mb() << 20;
	char *resm_size_str = getenv("reserved_mem_size");
	char *resm_start_str = getenv("reserved_mem_start");

	if (resm_size_str) {
		resm_size = ustrtoul(resm_size_str, &endp, 0);
	}

	if (!resm_size)
		return 0; /* nothing to do, return */

	if (resm_start_str) {
		resm_start += ustrtoul(resm_start_str, &endp, 0);
	}

	if (resm_start == phy_start) {
		sprintf(val, "mem=0x%x@0x%x", total_phy - resm_size,
			resm_start + resm_size);
	} else if (resm_start + resm_size == phy_start + phy_end) {
		sprintf(val, "mem=0x%x", total_phy - resm_size);
	} else {
		sprintf(val, "mem=0x%x mem=0x%x@0x%x",
			resm_start - phy_start,
			total_phy - (resm_start - phy_start) - resm_size,
			resm_start + resm_size);
	}
	return setenv("memmap", val);
}

int setenv_for_normalboot(int part_num)
{
	char *addr = (char *)0x80800000;
	int err = load_ext2_file(addr, "mmc", 0, part_num, "/boot/boot.scr");
	if (!err) {
		source((ulong)addr, NULL);
		set_memmap();
#ifdef CONFIG_TI814X_MIN_CONFIG
		run_command("setenv bootargs ${bootargs} ${memmap}", 0);
#else
		run_command("setenv bootargs ${bootargs} ${memmap} S", 0);
#endif /* CONFIG_TI814X_MIN_CONFIG */
	}
	return -1;
}

int board_late_init(void)
{
	int factory_boot;
	char part_num_str[4];

#if defined(CONFIG_TI81XX_PCIE_BOOT) && defined(CONFIG_TI814X_MIN_CONFIG)
	extern int pcie_init(void);
	printf("\nSetting up for pcie boot...\n");
	pcie_init();
	return 0;
#endif

	factory_boot = is_factory_boot_enabled();
	if (factory_boot) {
		setenv("factory_boot", "1");
	} else {
		setenv("factory_boot", "0");
	}

	/* Read boot partition from SPI flash */
	struct spi_flash *flash;
	struct nvmem_data nv_data = {0};
	flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED,
		CONFIG_SF_DEFAULT_MODE);
	if (flash)
		spi_flash_read(flash, CONFIG_ASI1230_NVMEM_DATA_OFS,
			sizeof(struct nvmem_data), &nv_data);
	if (nv_data.magic != NVMEMDATA_MAGIC)
		nv_data.active_part_num = CONFIG_ASI1230_FACTORY_PARTNUM;
	if (nv_data.active_part_num != nv_data.avail_part_num[0] &&
		nv_data.active_part_num != nv_data.avail_part_num[1])
		nv_data.active_part_num = CONFIG_ASI1230_FACTORY_PARTNUM;
	if (nv_data.active_part_num < 2 || nv_data.active_part_num > 10)
		nv_data.active_part_num = CONFIG_ASI1230_FACTORY_PARTNUM;
	if (factory_boot)
		nv_data.active_part_num = CONFIG_ASI1230_FACTORY_PARTNUM;
	sprintf(part_num_str, "%d", nv_data.active_part_num);
	setenv("bpart_num", part_num_str);

#ifdef CONFIG_TI814X_MIN_CONFIG
	/* If eng mode is enabled do not execute preboot but rather bootdelay/console */
	if (is_eng_mode_enabled()) {
		setenv("eng_mode", "1");
		printf("Booting in engineering mode\n");
		printf("Active boot part num: %d\n", nv_data.active_part_num);
		setenv_for_engboot(nv_data.active_part_num);
	} else {
#ifndef DEBUG
		if (!factory_boot)
			gd->flags |= GD_FLG_SILENT;
		setenv("bootdelay", "0");
#endif /* DEBUG */
		setenv("eng_mode", "0");
		setenv_for_normalboot(nv_data.active_part_num);
	}
#else
	setenv_for_normalboot(nv_data.active_part_num);
#endif /* CONFIG_TI814X_MIN_CONFIG */

	return 0;
}

#ifdef CONFIG_SETUP_PLL
static void audio_pll_config()
{
	pll_config(AUDIO_PLL_BASE, AUDIO_N, AUDIO_M, AUDIO_M2, AUDIO_CLKCTRL);
}

static void video0_pll_config()
{
	pll_config(VIDEO0_PLL_BASE, VIDEO0_N, VIDEO0_M, VIDEO0_M2, VIDEO0_CLKCTRL);
}

static void video1_pll_config()
{
	pll_config(VIDEO1_PLL_BASE, VIDEO1_N, VIDEO1_M, VIDEO1_M2, VIDEO1_CLKCTRL);
}

static void sata_pll_config()
{
	__raw_writel(0xC12C003C, SATA_PLLCFG1);
	__raw_writel(0x004008E0, SATA_PLLCFG3);
	asi1230_delay(0xFFFF);

	__raw_writel(0x80000004, SATA_PLLCFG0);
	asi1230_delay(0xFFFF);

	/* Enable PLL LDO */
	__raw_writel(0x80000014, SATA_PLLCFG0);
	asi1230_delay(0xFFFF);

	/* Enable DIG LDO, ENBGSC_REF, PLL LDO */
	__raw_writel(0x80000016, SATA_PLLCFG0);
	asi1230_delay(0xFFFF);

	__raw_writel(0xC0000017, SATA_PLLCFG0);
	asi1230_delay(0xFFFF);

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

static void dss_pll_config()
{
	pll_config(DSS_PLL_BASE, DSS_N, DSS_M, DSS_M2, DSS_CLKCTRL);
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

	/* GPIO1 */
	__raw_writel(0x102, CM_ALWON_GPIO_1_CLKCTRL);
	while (__raw_readl(CM_ALWON_GPIO_1_CLKCTRL) != 0x102) ;
	/* GATINGRATIO 0x3: Functional clock is interface clock divided by 8 */
	__raw_writel((0x3 << 1), GPIO1_BASE + GPIO_CTRL);

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
#define CLKOUT_MUX_SOURCE_ISS_DPLL 4
#define CLKOUT_MUX_SOURCE_L3_DPLL 5
#define CLKOUT_MUX_SOURCE_OSC0 6
#define CLKOUT_MUX_INIT ((CLKOUT_MUX_SOURCE_OSC0 << 16) +\
			(CLKOUT_MUX_SOURCE_PRCM_SYSCLK_OUT << 0))

	/* is this code even executed ????? - SGT does not think so */
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
 * board specific muxing of pins
 */

void set_muxconf(u32 * reg_addr, u8 conf_val)
{
	u32 reg_val = __raw_readl(*reg_addr);
	/* bits 6:4 of val goes to bits [18:16] of register
	 * bit 19 of register is unchanged
	 */
	reg_val &= ~0x000300FF;	/* keep bit 19, clear 17-16, 7-0 */
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
void s_init(u32 in_external_dram)
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
	{
		/* Enable the control module */
		__raw_writel(0x2, CM_ALWON_CONTROL_CLKCTRL);

#ifdef CONFIG_SETUP_PLL
		/* Setup the various plls */
		audio_pll_config();
		video0_pll_config();
		video1_pll_config();
		sata_pll_config();
		modena_pll_config();
		l3_pll_config();
		dsp_pll_config();
		iva_pll_config();
		iss_pll_config();
		usb_pll_config();

		/* used to gen 25MHz on CLKOUT1 for ethernet PHY */
		dss_pll_config();

		/*  With clk freqs setup to desired values,
		 *  enable the required peripherals
		 */
		per_clocks_enable();
#endif
	}
	/* Do the required pin-muxing before modules are setup */
	set_muxconf_regs();

	/* Indicate to the outside world that we are alive */
	show_boot_progress(BOOT_PROGRESS_HELLOWORLD);

#ifdef CONFIG_SETUP_PLL
	/* Read back HW ID after peripheral and pinmux setup */
	{
		u32 hwid = asi1230_hw_id();
		/* Init DDR PLL depending on the HW ID */
		if (hwid == 0x01) {
			/* ID = 1 (the only rev with LPDDR) */
			pll_config(DDR_PLL_BASE, LPDDR_N, LPDDR_M, LPDDR_M2,
				LPDDR_CLKCTRL);
			config_asi1230_lpddr();
		} else {
			/* Assume anything else uses DDR2 */
			pll_config(DDR_PLL_BASE, DDR2_N, DDR2_M, DDR2_M2,
				DDR2_CLKCTRL);
			config_asi1230_ddr2();
		}
	}
#endif

	/* external dram is initialized */
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
	val |= PHY_BMCR_AUTON;
	if (miiphy_write(name, addr, PHY_BMCR, val) != 0) {
		printf("failed to write bmcr\n");
		return;
	}
	miiphy_read(name, addr, PHY_BMCR, &val);

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
	.bd_ram_ofs = 0x2000,
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
