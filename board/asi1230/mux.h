/*
* (C) Copyright 2011
* Texas Instruments, <www.ti.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/

#ifndef _MUX_H_
#define _MUX_H_

/*
 * DISABLED - Disabled
 * FCN1 - Mux Fcn 1
 * FCN2 - Mux Fcn 2
 * FCN3 - Mux Fcn 3
 * FCN4 - Mux Fcn 4
 * FCN5 - Mux Fcn 5
 * FCN6 - Mux Fcn 6
 * FCN7 - Mux Fcn 7
 * FCN8 - Mux Fcn 8
 * IPD - Internal pull-down
 * IPU - Internal pull-up
 * DIS - Internal pull disabled
 */

#define DISABLED 0
#define FCN1 1
#define FCN2 2
#define FCN3 3
#define FCN4 4
#define FCN5 5
#define FCN6 6
#define FCN7 7
#define FCN8 8
#define IPD (0 << 4)
#define DIS (1 << 4)
#define IPU (1 << 5)

/*
 * To get the physical address the offset has
 * to be added to TI814X_CTRL_BASE
 */

#define PINCNTL1                                  0x0800
#define PINCNTL2                                  0x0804
#define PINCNTL3                                  0x0808
#define PINCNTL4                                  0x080C
#define PINCNTL5                                  0x0810
#define PINCNTL6                                  0x0814
#define PINCNTL7                                  0x0818
#define PINCNTL8                                  0x081C
#define PINCNTL9                                  0x0820
#define PINCNTL10                                 0x0824
#define PINCNTL11                                 0x0828
#define PINCNTL12                                 0x082C
#define PINCNTL13                                 0x0830
#define PINCNTL14                                 0x0834
#define PINCNTL15                                 0x0838
#define PINCNTL16                                 0x083C
#define PINCNTL17                                 0x0840
#define PINCNTL18                                 0x0844
#define PINCNTL19                                 0x0848
#define PINCNTL20                                 0x084C
#define PINCNTL21                                 0x0850
#define PINCNTL22                                 0x0854
#define PINCNTL23                                 0x0858
#define PINCNTL24                                 0x085C
#define PINCNTL25                                 0x0860
#define PINCNTL26                                 0x0864
#define PINCNTL27                                 0x0868
#define PINCNTL28                                 0x086C
#define PINCNTL29                                 0x0870
#define PINCNTL30                                 0x0874
#define PINCNTL31                                 0x0878
#define PINCNTL32                                 0x087C
#define PINCNTL33                                 0x0880
#define PINCNTL34                                 0x0884
#define PINCNTL35                                 0x0888
#define PINCNTL36                                 0x088C
#define PINCNTL37                                 0x0890
#define PINCNTL38                                 0x0894
#define PINCNTL39                                 0x0898
#define PINCNTL40                                 0x089C
#define PINCNTL41                                 0x08A0
#define PINCNTL42                                 0x08A4
#define PINCNTL43                                 0x08A8
#define PINCNTL44                                 0x08AC
#define PINCNTL45                                 0x08B0
#define PINCNTL46                                 0x08B4
#define PINCNTL47                                 0x08B8
#define PINCNTL48                                 0x08BC
#define PINCNTL49                                 0x08C0
#define PINCNTL50                                 0x08C4
#define PINCNTL51                                 0x08C8
#define PINCNTL52                                 0x08CC
#define PINCNTL53                                 0x08D0
#define PINCNTL54                                 0x08D4
#define PINCNTL55                                 0x08D8
#define PINCNTL56                                 0x08DC
#define PINCNTL57                                 0x08E0
#define PINCNTL58                                 0x08E4
#define PINCNTL59                                 0x08E8
#define PINCNTL60                                 0x08EC
#define PINCNTL61                                 0x08F0
#define PINCNTL62                                 0x08F4
#define PINCNTL63                                 0x08F8
#define PINCNTL64                                 0x08FC
#define PINCNTL65                                 0x0900
#define PINCNTL68                                 0x090C
#define PINCNTL69                                 0x0910
#define PINCNTL70                                 0x0914
#define PINCNTL71                                 0x0918
#define PINCNTL72                                 0x091C
#define PINCNTL73                                 0x0920
#define PINCNTL74                                 0x0924
#define PINCNTL75                                 0x0928
#define PINCNTL76                                 0x092C
#define PINCNTL77                                 0x0930
#define PINCNTL78                                 0x0934
#define PINCNTL79                                 0x0938
#define PINCNTL80                                 0x093C
#define PINCNTL81                                 0x0940
#define PINCNTL82                                 0x0944
#define PINCNTL83                                 0x0948
#define PINCNTL84                                 0x094C
#define PINCNTL85                                 0x0950
#define PINCNTL86                                 0x0954
#define PINCNTL87                                 0x0958
#define PINCNTL88                                 0x095C
#define PINCNTL89                                 0x0960
#define PINCNTL90                                 0x0964
#define PINCNTL91                                 0x0968
#define PINCNTL92                                 0x096C
#define PINCNTL93                                 0x0970
#define PINCNTL94                                 0x0974
#define PINCNTL95                                 0x0978
#define PINCNTL96                                 0x097C
#define PINCNTL97                                 0x0980
#define PINCNTL98                                 0x0984
#define PINCNTL99                                 0x0988
#define PINCNTL100                                0x098C
#define PINCNTL101                                0x0990
#define PINCNTL102                                0x0994
#define PINCNTL103                                0x0998
#define PINCNTL104                                0x099C
#define PINCNTL105                                0x09A0
#define PINCNTL106                                0x09A4
#define PINCNTL107                                0x09A8
#define PINCNTL108                                0x09AC
#define PINCNTL109                                0x09B0
#define PINCNTL110                                0x09B4
#define PINCNTL111                                0x09B8
#define PINCNTL112                                0x09BC
#define PINCNTL113                                0x09C0
#define PINCNTL114                                0x09C4
#define PINCNTL115                                0x09C8
#define PINCNTL116                                0x09CC
#define PINCNTL117                                0x09D0
#define PINCNTL118                                0x09D4
#define PINCNTL119                                0x09D8
#define PINCNTL120                                0x09DC
#define PINCNTL121                                0x09E0
#define PINCNTL122                                0x09E4
#define PINCNTL123                                0x09E8
#define PINCNTL124                                0x09EC
#define PINCNTL125                                0x09F0
#define PINCNTL126                                0x09F4
#define PINCNTL127                                0x09F8
#define PINCNTL128                                0x09FC
#define PINCNTL129                                0x0A00
#define PINCNTL130                                0x0A04
#define PINCNTL131                                0x0A08
#define PINCNTL132                                0x0A0C
#define PINCNTL133                                0x0A10
#define PINCNTL134                                0x0A14
#define PINCNTL135                                0x0A18
#define PINCNTL136                                0x0A1C
#define PINCNTL137                                0x0A20
#define PINCNTL138                                0x0A24
#define PINCNTL139                                0x0A28
#define PINCNTL140                                0x0A2C
#define PINCNTL141                                0x0A30
#define PINCNTL142                                0x0A34
#define PINCNTL143                                0x0A38
#define PINCNTL144                                0x0A3C
#define PINCNTL145                                0x0A40
#define PINCNTL146                                0x0A44
#define PINCNTL147                                0x0A48
#define PINCNTL148                                0x0A4C
#define PINCNTL149                                0x0A50
#define PINCNTL150                                0x0A54
#define PINCNTL151                                0x0A58
#define PINCNTL152                                0x0A5C
#define PINCNTL153                                0x0A60
#define PINCNTL154                                0x0A64
#define PINCNTL155                                0x0A68
#define PINCNTL156                                0x0A6C
#define PINCNTL157                                0x0A70
#define PINCNTL158                                0x0A74
#define PINCNTL159                                0x0A78
#define PINCNTL160                                0x0A7C
#define PINCNTL161                                0x0A80
#define PINCNTL162                                0x0A84
#define PINCNTL163                                0x0A88
#define PINCNTL164                                0x0A8C
#define PINCNTL165                                0x0A90
#define PINCNTL166                                0x0A94
#define PINCNTL167                                0x0A98
#define PINCNTL168                                0x0A9C
#define PINCNTL169                                0x0AA0
#define PINCNTL170                                0x0AA4
#define PINCNTL171                                0x0AA8
#define PINCNTL172                                0x0AAC
#define PINCNTL173                                0x0AB0
#define PINCNTL174                                0x0AB4
#define PINCNTL175                                0x0AB8
#define PINCNTL176                                0x0ABC
#define PINCNTL177                                0x0AC0
#define PINCNTL178                                0x0AC4
#define PINCNTL179                                0x0AC8
#define PINCNTL180                                0x0ACC
#define PINCNTL181                                0x0AD0
#define PINCNTL182                                0x0AD4
#define PINCNTL183                                0x0AD8
#define PINCNTL184                                0x0ADC
#define PINCNTL185                                0x0AE0
#define PINCNTL186                                0x0AE4
#define PINCNTL187                                0x0AE8
#define PINCNTL188                                0x0AEC
#define PINCNTL189                                0x0AF0
#define PINCNTL190                                0x0AF4
#define PINCNTL191                                0x0AF8
#define PINCNTL192                                0x0AFC
#define PINCNTL193                                0x0B00
#define PINCNTL194                                0x0B04
#define PINCNTL195                                0x0B08
#define PINCNTL196                                0x0B0C
#define PINCNTL197                                0x0B10
#define PINCNTL198                                0x0B14
#define PINCNTL199                                0x0B18
#define PINCNTL200                                0x0B1C
#define PINCNTL201                                0x0B20
#define PINCNTL202                                0x0B24
#define PINCNTL203                                0x0B28
#define PINCNTL204                                0x0B2C
#define PINCNTL205                                0x0B30
#define PINCNTL206                                0x0B34
#define PINCNTL207                                0x0B38
#define PINCNTL208                                0x0B3C
#define PINCNTL209                                0x0B40
#define PINCNTL210                                0x0B44
#define PINCNTL211                                0x0B48
#define PINCNTL212                                0x0B4C
#define PINCNTL213                                0x0B50
#define PINCNTL214                                0x0B54
#define PINCNTL215                                0x0B58
#define PINCNTL216                                0x0B5C
#define PINCNTL217                                0x0B60
#define PINCNTL218                                0x0B64
#define PINCNTL219                                0x0B68
#define PINCNTL220                                0x0B6C
#define PINCNTL221                                0x0B70
#define PINCNTL222                                0x0B74
#define PINCNTL223                                0x0B78
#define PINCNTL224                                0x0B7C
#define PINCNTL225                                0x0B80
#define PINCNTL226                                0x0B84
#define PINCNTL227                                0x0B88
#define PINCNTL228                                0x0B8C
#define PINCNTL229                                0x0B90
#define PINCNTL230                                0x0B94
#define PINCNTL231                                0x0B98
#define PINCNTL232                                0x0B9C
#define PINCNTL233                                0x0BA0
#define PINCNTL234                                0x0BA4
#define PINCNTL235                                0x0BA8
#define PINCNTL236                                0x0BAC
#define PINCNTL237                                0x0BB0
#define PINCNTL238                                0x0BB4
#define PINCNTL239                                0x0BB8
#define PINCNTL240                                0x0BBC
#define PINCNTL241                                0x0BC0
#define PINCNTL242                                0x0BC4
#define PINCNTL243                                0x0BC8
#define PINCNTL244                                0x0BCC
#define PINCNTL245                                0x0BD0
#define PINCNTL246                                0x0BD4
#define PINCNTL247                                0x0BD8
#define PINCNTL248                                0x0BDC
#define PINCNTL249                                0x0BE0
#define PINCNTL250                                0x0BE4
#define PINCNTL251                                0x0BE8
#define PINCNTL252                                0x0BEC
#define PINCNTL253                                0x0BF0
#define PINCNTL254                                0x0BF4
#define PINCNTL255                                0x0BF8
#define PINCNTL256                                0x0BFC
#define PINCNTL257                                0x0C00
#define PINCNTL258                                0x0C04
#define PINCNTL259                                0x0C08
#define PINCNTL260                                0x0C0C
#define PINCNTL261                                0x0C10
#define PINCNTL262                                0x0C14
#define PINCNTL263                                0x0C18
#define PINCNTL264                                0x0C1C
#define PINCNTL270                                0x0C34

    MUX_VAL(PINCNTL1, (IPU | FCN1))	/* SD1_CLK */
    MUX_VAL(PINCNTL2, (IPU | FCN1))	/* SD1_CMD_MUX0 */
    MUX_VAL(PINCNTL3, (IPU | FCN1))	/* SD1_DAT[0] */
    MUX_VAL(PINCNTL4, (IPU | FCN1))	/* SD1_DAT[1]_SDIRQ */
    MUX_VAL(PINCNTL5, (IPU | FCN1))	/* SD1_DAT[2]_SDRW */
    MUX_VAL(PINCNTL6, (IPU | FCN1))	/* SD1_DAT[3] */
    MUX_VAL(PINCNTL7, (IPU | FCN2))	/* SPI[1]_SCS[1] */
    MUX_VAL(PINCNTL8, (IPU | FCN8))	/* GP0[1] */
    MUX_VAL(PINCNTL9, (IPU | FCN8))	/* GP0[2] */
    MUX_VAL(PINCNTL10, (IPU | FCN8))	/* GP0[3] */
    MUX_VAL(PINCNTL11, (IPU | FCN8))	/* GP0[4] */
    MUX_VAL(PINCNTL12, (IPU | FCN8))	/* GP0[5] */
    MUX_VAL(PINCNTL13, (IPU | FCN8))	/* GP0[6] */
    MUX_VAL(PINCNTL14, (IPD | FCN3))	/* MCA[0]_AHCLKX */
    MUX_VAL(PINCNTL15, (IPD | FCN1))	/* AUD_CLKIN1 */
    MUX_VAL(PINCNTL16, (IPD | FCN1))	/* AUD_CLKIN2 */
    MUX_VAL(PINCNTL17, (IPD | FCN1))	/* MCA[0]_ACLKX */
    MUX_VAL(PINCNTL18, (IPD | FCN1))	/* MCA[0]_AFSX */
    MUX_VAL(PINCNTL19, (IPD | FCN1))	/* MCA[0]_ACLKR */
    MUX_VAL(PINCNTL20, (IPD | FCN1))	/* MCA[0]_AFSR */
    MUX_VAL(PINCNTL21, (IPD | FCN1))	/* MCA[0]_AXR[0] */
    MUX_VAL(PINCNTL22, (IPU | FCN1))	/* MCA[0]_AXR[1] */
    MUX_VAL(PINCNTL23, (IPU | FCN1))	/* MCA[0]_AXR[2] */
    MUX_VAL(PINCNTL24, (IPD | FCN1))	/* MCA[0]_AXR[3] */
    MUX_VAL(PINCNTL25, (IPD | FCN1))	/* MCA[0]_AXR[4] */
    MUX_VAL(PINCNTL26, (IPD | FCN1))	/* MCA[0]_AXR[5] */
    MUX_VAL(PINCNTL27, (IPD | FCN1))	/* MCA[0]_AXR[6] */
    MUX_VAL(PINCNTL28, (IPD | FCN1))	/* MCA[0]_AXR[7]_MUX0 */
    MUX_VAL(PINCNTL29, (IPD | FCN1))	/* MCA[0]_AXR[8]_MUX0 */
    MUX_VAL(PINCNTL30, (IPD | FCN1))	/* MCA[0]_AXR[9]_MUX0 */
    MUX_VAL(PINCNTL31, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL32, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL33, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL34, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL35, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL36, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL37, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL38, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL39, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL40, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL41, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL42, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL43, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL44, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL45, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL46, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL47, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL48, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL49, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL50, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL51, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL52, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL53, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL54, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL55, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL56, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL57, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL58, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL59, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL60, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL61, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL62, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL63, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL64, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL65, (IPU | DISABLED))	/* safe_mode */
    MUX_NOP(66)
    MUX_NOP(67)
    MUX_VAL(PINCNTL68, (IPU | FCN8))	/* GP1[0] */
    MUX_VAL(PINCNTL69, (IPU | FCN8))	/* GP1[1] */
    MUX_VAL(PINCNTL70, (IPU | FCN1))	/* UART0_RXD */
    MUX_VAL(PINCNTL71, (IPU | FCN1))	/* UART0_TXD */
    MUX_VAL(PINCNTL72, (IPU | FCN1))	/* UART0_CTS */
    MUX_VAL(PINCNTL73, (IPU | FCN1))	/* UART0_RTS */
    MUX_VAL(PINCNTL74, (IPU | FCN8))	/* GP1[2] */
    MUX_VAL(PINCNTL75, (IPU | FCN8))	/* GP1[3] */
    MUX_VAL(PINCNTL76, (IPU | FCN8))	/* GP1[4] */
    MUX_VAL(PINCNTL77, (IPU | FCN1))	/* UART0_RIN */
    MUX_VAL(PINCNTL78, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL79, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL80, (IPU | FCN1))	/* SPI[0]_SCS[1] */
    MUX_VAL(PINCNTL81, (IPU | FCN1))	/* SPI[0]_SCS[0] */
    MUX_VAL(PINCNTL82, (IPD | FCN1))	/* SPI[0]_SCLK */
    MUX_VAL(PINCNTL83, (DIS | FCN1))	/* SPI[0]_D[1] */
    MUX_VAL(PINCNTL84, (DIS | FCN1))	/* SPI[0]_D[0] */
    MUX_VAL(PINCNTL85, (IPU | FCN1))	/* SPI[1]_SCS[0] */
    MUX_VAL(PINCNTL86, (IPU | FCN1))	/* SPI[1]_SCLK */
    MUX_VAL(PINCNTL87, (IPU | FCN1))	/* SPI[1]_D[1] */
    MUX_VAL(PINCNTL88, (IPU | FCN1))	/* SPI[1]_D[0] */
    MUX_VAL(PINCNTL89, (DIS | FCN1))	/* GPMC_D[0] */
    MUX_VAL(PINCNTL90, (DIS | FCN1))	/* GPMC_D[1] */
    MUX_VAL(PINCNTL91, (DIS | FCN1))	/* GPMC_D[2] */
    MUX_VAL(PINCNTL92, (DIS | FCN1))	/* GPMC_D[3] */
    MUX_VAL(PINCNTL93, (DIS | FCN1))	/* GPMC_D[4] */
    MUX_VAL(PINCNTL94, (DIS | FCN1))	/* GPMC_D[5] */
    MUX_VAL(PINCNTL95, (DIS | FCN1))	/* GPMC_D[6] */
    MUX_VAL(PINCNTL96, (DIS | FCN1))	/* GPMC_D[7] */
    MUX_VAL(PINCNTL97, (DIS | FCN1))	/* GPMC_D[8] */
    MUX_VAL(PINCNTL98, (DIS | FCN1))	/* GPMC_D[9] */
    MUX_VAL(PINCNTL99, (DIS | FCN1))	/* GPMC_D[10] */
    MUX_VAL(PINCNTL100, (DIS | FCN1))	/* GPMC_D[11] */
    MUX_VAL(PINCNTL101, (DIS | FCN1))	/* GPMC_D[12] */
    MUX_VAL(PINCNTL102, (DIS | FCN1))	/* GPMC_D[13] */
    MUX_VAL(PINCNTL103, (DIS | FCN1))	/* GPMC_D[14] */
    MUX_VAL(PINCNTL104, (DIS | FCN1))	/* GPMC_D[15] */
    MUX_VAL(PINCNTL105, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL106, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL107, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL108, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL109, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL110, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL111, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL112, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL113, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL114, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL115, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL116, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL117, (IPD | FCN2 )) 	/* GPMC_A[1]_MUX1 */\
    MUX_VAL(PINCNTL118, (IPD | FCN2 )) 	/* GPMC_A[2]_MUX1 */\
    MUX_VAL(PINCNTL119, (IPD | FCN2 )) 	/* GPMC_A[3]_MUX1 */\
    MUX_VAL(PINCNTL120, (IPD | FCN2 )) 	/* GPMC_A[4]_MUX1 */\
    MUX_VAL(PINCNTL121, (DISABLED )) 	/* safe_mode */\
    MUX_VAL(PINCNTL122, (IPU | FCN1 )) 	/* GPMC_CS[0] */\
    MUX_VAL(PINCNTL123, (IPU | FCN1 )) 	/* GPMC_CS[1] */\
    MUX_VAL(PINCNTL124, (IPU | FCN1 )) 	/* GPMC_CS[2] */\
    MUX_VAL(PINCNTL125, (IPU | FCN1 )) 	/* GPMC_CS[3] */\
    MUX_VAL(PINCNTL126, (IPU | FCN1 )) 	/* GPMC_CS[4] */\
    MUX_VAL(PINCNTL127, (IPU | DISABLED )) 	/* safe_mode */\
    MUX_VAL(PINCNTL128, (IPU | DISABLED )) 	/* safe_mode */\
    MUX_VAL(PINCNTL129, (IPU | FCN1 )) 	/* GPMC_OE_RE */\
    MUX_VAL(PINCNTL130, (IPU | FCN1 )) 	/* GPMC_WE */\
    MUX_VAL(PINCNTL131, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL132, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL133, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL134, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL135, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL136, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL137, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL138, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL139, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL140, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL141, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL142, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL143, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL144, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL145, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL146, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL147, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL148, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL149, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL150, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL151, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL152, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL153, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL154, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL155, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL156, (IPU | FCN8))	/* GP0[10]_MUX0 */
    MUX_VAL(PINCNTL157, (IPD | FCN8))	/* GP0[11]_MUX0 */
    MUX_VAL(PINCNTL158, (IPU | FCN8))	/* GP0[12]_MUX0 */
    MUX_VAL(PINCNTL159, (IPU | FCN8))	/* GP0[13]_MUX0 */
    MUX_VAL(PINCNTL160, (IPD | FCN8))	/* GP0[14]_MUX0 */
    MUX_VAL(PINCNTL161, (IPD | FCN8))	/* GP0[15]_MUX0 */
    MUX_VAL(PINCNTL162, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL163, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL164, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL165, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL166, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL167, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL168, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL169, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL170, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL171, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL172, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL173, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL174, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL175, (IPD | FCN1))	/* VOUT[0]_FLD_MUX1 */
    MUX_VAL(PINCNTL176, (IPD | FCN1))	/* VOUT[0]_CLK */
    MUX_VAL(PINCNTL177, (IPD | FCN1))	/* VOUT[0]_HSYNC */
    MUX_VAL(PINCNTL178, (IPD | FCN1))	/* VOUT[0]_VSYNC */
    MUX_VAL(PINCNTL179, (IPD | FCN1))	/* VOUT[0]_AVID */
    MUX_VAL(PINCNTL180, (IPD | FCN1))	/* VOUT[0]_B_CB_C[2] */
    MUX_VAL(PINCNTL181, (IPD | FCN1))	/* VOUT[0]_B_CB_C[3] */
    MUX_VAL(PINCNTL182, (IPD | FCN1))	/* VOUT[0]_B_CB_C[4] */
    MUX_VAL(PINCNTL183, (IPD | FCN1))	/* VOUT[0]_B_CB_C[5] */
    MUX_VAL(PINCNTL184, (IPD | FCN1))	/* VOUT[0]_B_CB_C[6] */
    MUX_VAL(PINCNTL185, (IPD | FCN1))	/* VOUT[0]_B_CB_C[7] */
    MUX_VAL(PINCNTL186, (IPD | FCN1))	/* VOUT[0]_B_CB_C[8] */
    MUX_VAL(PINCNTL187, (IPD | FCN1))	/* VOUT[0]_B_CB_C[9] */
    MUX_VAL(PINCNTL188, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[2] */
    MUX_VAL(PINCNTL189, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[3] */
    MUX_VAL(PINCNTL190, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[4] */
    MUX_VAL(PINCNTL191, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[5] */
    MUX_VAL(PINCNTL192, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[6] */
    MUX_VAL(PINCNTL193, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[7] */
    MUX_VAL(PINCNTL194, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[8] */
    MUX_VAL(PINCNTL195, (IPD | FCN1))	/* VOUT[0]_G_Y_YC[9] */
    MUX_VAL(PINCNTL196, (IPD | FCN1))	/* VOUT[0]_R_CR[2] */
    MUX_VAL(PINCNTL197, (IPD | FCN1))	/* VOUT[0]_R_CR[3] */
    MUX_VAL(PINCNTL198, (IPD | FCN1))	/* VOUT[0]_R_CR[4] */
    MUX_VAL(PINCNTL199, (IPD | FCN1))	/* VOUT[0]_R_CR[5] */
    MUX_VAL(PINCNTL200, (IPD | FCN1))	/* VOUT[0]_R_CR[6] */
    MUX_VAL(PINCNTL201, (IPD | FCN1))	/* VOUT[0]_R_CR[7] */
    MUX_VAL(PINCNTL202, (IPD | FCN1))	/* VOUT[0]_R_CR[8] */
    MUX_VAL(PINCNTL203, (IPD | FCN1))	/* VOUT[0]_R_CR[9] */
    MUX_VAL(PINCNTL204, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL205, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL206, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL207, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL208, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL209, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL210, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL211, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL212, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL213, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL214, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL215, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL216, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL217, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL218, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL219, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL220, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL221, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL222, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL223, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL224, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL225, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL226, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL227, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL228, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL229, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL230, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL231, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL232, (IPD | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL233, (IPU | FCN1))	/* MDCLK */
    MUX_VAL(PINCNTL234, (IPU | FCN1))	/* MDIO */
    MUX_VAL(PINCNTL235, (IPD | FCN1))	/* EMAC[0]_MTCLK/EMAC[0]_RGRXC */
    MUX_VAL(PINCNTL236, (IPD | FCN1))	/* EMAC[0]_MCOL/EMAC[0]_RGRXCTL */
    MUX_VAL(PINCNTL237, (IPD | FCN1))	/* EMAC[0]_MCRS/EMAC[0]_RGRXD[2] */
    MUX_VAL(PINCNTL238, (IPD | FCN1))	/* EMAC[0]_MRXER/EMAC[0]_RGTXCTL */
    MUX_VAL(PINCNTL239, (IPD | FCN1))	/* EMAC[0]_MRCLK/EMAC[0]_RGTXC */
    MUX_VAL(PINCNTL240, (IPD | FCN1))	/* EMAC[0]_MRXD[0]/EMAC[0]_RGTXD[0] */
    MUX_VAL(PINCNTL241, (IPD | FCN1))	/* EMAC[0]_MRXD[1]/EMAC[0]_RGRXD[0] */
    MUX_VAL(PINCNTL242, (IPD | FCN1))	/* EMAC[0]_MRXD[2]/EMAC[0]_RGRXD[1] */
    MUX_VAL(PINCNTL243, (IPD | FCN1))	/* EMAC[0]_MRXD[3]/EMAC[1]_RGRXCTL */
    MUX_VAL(PINCNTL244, (IPD | FCN1))	/* EMAC[0]_MRXD[4]/EMAC[0]_RGRXD[3] */
    MUX_VAL(PINCNTL245, (IPD | FCN1))	/* EMAC[0]_MRXD[5]/EMAC[0]_RGTXD[3] */
    MUX_VAL(PINCNTL246, (IPD | FCN1))	/* EMAC[0]_MRXD[6]/EMAC[0]_RGTXD[2] */
    MUX_VAL(PINCNTL247, (IPD | FCN1))	/* EMAC[0]_MRXD[7]/EMAC[0]_RGTXD[1] */
    MUX_VAL(PINCNTL248, (IPD | FCN1))	/* EMAC[0]_MRXDV/EMAC[1]_RGRXD[1] */
    MUX_VAL(PINCNTL249, (IPD | FCN1))	/* EMAC[0]_GMTCLK/EMAC[1]_RGRXC */
    MUX_VAL(PINCNTL250, (IPD | FCN1))	/* EMAC[0]_MTXD[0]/EMAC[1]_RGRXD[3] */
    MUX_VAL(PINCNTL251, (IPD | FCN1))	/* EMAC[0]_MTXD[1]/EMAC[1]_RGTXD[1] */
    MUX_VAL(PINCNTL252, (IPD | FCN1))	/* EMAC[0]_MTXD[2]/EMAC[1]_RGTXCTL */
    MUX_VAL(PINCNTL253, (IPD | FCN1))	/* EMAC[0]_MTXD[3]/EMAC[1]_RGTXD[0] */
    MUX_VAL(PINCNTL254, (IPD | FCN1))	/* EMAC[0]_MTXD[4]/EMAC[1]_RGTXD[2] */
    MUX_VAL(PINCNTL255, (IPD | FCN1))	/* EMAC[0]_MTXD[5]/EMAC[1]_RGTXC */
    MUX_VAL(PINCNTL256, (IPD | FCN1))	/* EMAC[0]_MTXD[6]/EMAC[1]_RGRXD[0] */
    MUX_VAL(PINCNTL257, (IPD | FCN1))	/* EMAC[0]_MTXD[7]/EMAC[1]_RGTXD[3] */
    MUX_VAL(PINCNTL258, (IPD | FCN1))	/* EMAC[0]_MTXEN/EMAC[1]_RGRXD[2] */
    MUX_VAL(PINCNTL259, (DIS | FCN3))	/* CLKOUT0 pin J7 */
    MUX_VAL(PINCNTL260, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL261, (IPU | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL262, (DIS | DISABLED))	/* safe_mode */
    MUX_VAL(PINCNTL263, (IPU | FCN1))	/* I2C[0]_SCL */
    MUX_VAL(PINCNTL264, (IPU | FCN1))	/* I2C[0]_SDA */
    MUX_NOP(265)
    MUX_NOP(266)
    MUX_NOP(267)
    MUX_NOP(268)
    MUX_NOP(269)
    MUX_VAL(PINCNTL270, (IPD | DISABLED)) /* safe_mode */
#endif
