/*
 * (C) Copyright 2006-2008
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _CLOCKS_TI814X_H_
#define _CLOCKS_TI814X_H_

#define OSC_0_FREQ	20

/* NOTE All PLLs must run at between 500 and 2000MHz */

/* Modena (ARM) PLL = 20/(1+1)*72 = 720MHz */
#define MODENA_N	0x1
/*#define MODENA_M	0x3C 600MHz*/
/*#define MODENA_M	0x1E 300MHz */
#define MODENA_M	0x48 /* 720MHz */
#define MODENA_M2	1
#define MODENA_CLKCTRL	0x1

/* DSP clock is 20 / (19+1) * 600 / 1 = 500MHz */
#define DSP_N		19
/*#define DSP_M		500  500 */
/*#define DSP_M		250  250 */
#define DSP_M		600  /* 600 */
#define DSP_M2		1
#define DSP_CLKCTRL	0x801

/* LPDDR clock is 20 / (19+1) * 666 / 4 ~= 166MHz */
#define LPDDR_N		19
#define LPDDR_M		666
#define LPDDR_M2		4
#define LPDDR_CLKCTRL	0x801

/* DDR2 clock is 20 / (19+1) * 800 / 2 = 400MHz */
#define DDR2_N		19
#define DDR2_M		800
#define DDR2_M2		2
#define DDR2_CLKCTRL	0x801

/* Audio clock is 250MHz - not currently used */
#define AUDIO_N		19
#define AUDIO_M		500
#define AUDIO_M2	2
#define AUDIO_CLKCTRL	0x801

/* Video0 clock starts at 516/21 = 24.571MHz - used to gen Audio 512Fs clock */
/* these value match what linux programs (minus the frac_m) for 48x512 */
#define VIDEO0_N	19
#define VIDEO0_M	516
#define VIDEO0_M2	21
#define VIDEO0_CLKCTRL	0x801

/* Video1 clock is 250MHz - used for RGMII */
#define VIDEO1_N	19
#define VIDEO1_M	500
#define VIDEO1_M2	2
#define VIDEO1_CLKCTRL	0x801

/* Layer 3 bus PLL */
#define L3_N		19
#define L3_M		(pg_val_ti814x(880, 800))
#define L3_M2		4
#define L3_CLKCTRL	0x801

/* DSS (HDVPSS) clock is 20 / (0+1) * 25 / 10 = 50MHz
 * (which is divded by two to get 25MHz for ethernet PHY*/
#define DSS_N		0
#define DSS_M		25
#define DSS_M2		10
#define DSS_CLKCTRL	0x801

/* IVA (HDVICP2) PLL */
#define IVA_N		19
#define IVA_M		(pg_val_ti814x(612, 612))
#define IVA_M2		2
#define IVA_CLKCTRL	0x801

/* ISS Imaging Subsystem PLL*/
#define ISS_N		19
#define ISS_M		800
#define ISS_M2		(pg_val_ti814x(2, 2))
#define ISS_CLKCTRL	0x801

/* USB PLL = 960MHz */
#define USB_N		19
#define USB_M		960
#define USB_M2		(pg_val_ti814x(1, 5))
#define USB_CLKCTRL	0x200a0801
#endif	/* endif _CLOCKS_TI814X_H_ */

