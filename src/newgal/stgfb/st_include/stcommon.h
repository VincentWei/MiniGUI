/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided ?AS IS?, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File   stcommon.h                                                            
 @brief                                                                         
                                                                               
 Interface to revision reporting and clock info functions                                                                             
                                                                               
*/

#ifndef _STCOMMON_H_
#define _STCOMMON_H_

/* C++ support */
/* ----------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
/* -------- */

#include "stddefs.h"

#ifdef ST_OSLINUX
#include "linuxcommon.h"
#endif


/* Clock infos structure for 5197 */
/* ------------------------------ */
#if defined(ST_5197)
typedef struct ST_ClockInfo_s
{
 U32 plla_clk;      /* PLL A clock                 */
 U32 pllb_clk;      /* PLL B clock                 */
 U32 st40cpu_clk;   /* ST40 cpu clock              */
 U32 st40per_clk;   /* ST40 peripheral clock       */
 U32 st40tick_clk;  /* ST40 timer clock            */
 U32 lmi_clk;       /* LMI sys clock               */
 U32 blitter_clk;   /* Blitter clock               */
 U32 sys_clk;       /* System clock                */
 U32 ic_clk;        /* Interconnect bus clock      */
 U32 com_clk;       /* Communication clock         */
 U32 fdma_clk;      /* FDMA clock                  */
 U32 av_clk;        /* Audio/Video clock           */
 U32 lcmpeg2_clk;   /* MPEG2 decoder clock         */
 U32 audio_clk;     /* Audio clock                 */
 U32 spare_clk;     /* Spare clock                 */
 U32 eth_clk;       /* Ethernet clock              */
 U32 fsa_0_clk;     /* Frequency synth A-0 clock   */
 U32 fsa_1_clk;     /* Frequency synth A-1 clock   */
 U32 fsa_2_clk;     /* Frequency synth A-2 clock   */
 U32 fsa_3_clk;     /* Frequency synth A-3 clock   */
 U32 fsb_0_clk;     /* Frequency synth B-0 clock   */
 U32 fsb_1_clk;     /* Frequency synth B-1 clock   */
 U32 fsb_2_clk;     /* Frequency synth B-2 clock   */
 U32 fsb_3_clk;     /* Frequency synth B-3 clock   */
 U32 fs_spare_clk;  /* FS Spare clock              */
 U32 pcm_clk;       /* PCM clock                   */
 U32 spdif_clk;     /* SPDIF clock                 */
 U32 dss_clk;       /* DSS clock                   */
 U32 pix_clk;       /* Pixel clock                 */
 U32 fdma_fs_clk;   /* FS FDMA clock               */
 U32 aux_clk;       /* Auxiliary clock             */
 U32 usb_clk;       /* USB clock                   */
} ST_ClockInfo_t;
#endif

/* Clock infos structure for 7109 & 7100*/
/* ------------------------------ */
#if defined(ST_7109) || defined(ST_7100)
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 struct 
  {
   U32 pll0_clk;     /* PLL0 clock                  */
   U32 pll1_clk;     /* PLL1 clock                  */
   U32 st40cpu_clk;  /* ST40 cpu clock              */
   U32 st40bus_clk;  /* ST40 bus clock              */
   U32 st40per_clk;  /* ST40 peripheral clock       */
   U32 st40tick_clk; /* ST40 timer clock            */
   U32 fdma_clk;     /* FDMA clock                  */
   U32 lcmpeg2_clk;  /* MPEG2 decoder clock         */
   U32 lmisys_clk;   /* LMI sys clock               */
   U32 lmivid_clk;   /* LMI vid clock               */
   U32 st231vid_clk; /* Lx vid clock                */
   U32 st231aud_clk; /* Lx aud clock                */
   U32 ich_clk;      /* High interconnect bus clock */
   U32 icl_clk;      /* Low interconnect bus clock  */
   U32 com_clk;      /* Com clock                   */
   U32 emi_clk;      /* Emi clock                   */
  } ckga;
 /* Clock gen B */
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 hdmidll_clk;  /* HDMI serializer clock       */
   U32 hdmibch_clk;  /* HDMI bch clock              */
   U32 hdmitmds_clk; /* HDMI tmds clock             */
   U32 hdmipix_clk;  /* HDMI pixel clock            */
   U32 hdpix_clk;    /* HD pixel clock              */
   U32 hddisp_clk;   /* HD display clock            */
   U32 c656_clk;     /* DVO pixel clock             */
   U32 gdp2_clk;     /* GDP2 pixel clock            */
   U32 sdpix_clk;    /* SD pixel clock              */
   U32 sddisp_clk;   /* SD display clock            */
   U32 dvp_clk;      /* Emi clock                   */
   U32 pipe_clk;     /* Video pipeline clock        */
   U32 h264pp_clk;   /* H264 preprocessor clock     */
   U32 ic_clk;       /* Interconnect clock          */
   U32 rtc_clk;      /* Real time clock             */
   U32 lpc_clk;      /* Low power  clock            */
  } ckgb;
 /* Clock gen C */
 struct 
  {
   U32 pcm_0_clk;    /* PCM 0 clock                 */
   U32 pcm_1_clk;    /* PCM 1 clock                 */
   U32 spdif_clk;    /* Spdif clock                 */
  } ckgc;
} ST_ClockInfo_t;
#endif

/* Clock infos structure for 7111 & 7105*/
/* ------------------------------ */
#if defined(ST_7111) || defined (ST_7105)
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 struct 
  {
   U32 lmi_clk;         /* LMI PLL clock               */
   U32 pll0_clk;        /* PLL0 clock                  */
   U32 pll1_clk;        /* PLL1 clock                  */
   U32 st40cpu_clk;     /* ST40 cpu clock              */
   U32 st40tick_clk;    /* ST40 timer clock            */
   U32 com_clk;         /* Peripherals clock           */
   U32 fdma0_clk;       /* FDMA0 clock                 */
   U32 fdma1_clk;       /* FDMA1 clock                 */
   U32 lxaud_clk;       /* Lx aud cpu clock            */
   U32 lxdmu_clk;       /* Lx dmu cpu clock            */
   U32 bdisp_clk;       /* Blitter display clock       */
   U32 disp_clk;        /* Display clock               */
   U32 ts_clk;          /* Transport bus clock         */
   U32 vdp_clk;         /* Video display pipe clock    */
   U32 blit_clk;        /* Blitter clock               */
   U32 eth_clk;         /* Ethernet clock              */
   U32 pci_clk;         /* PCI clock                   */
   U32 emi_clk;         /* Emi clock                   */
  } ckga;
 /* Clock gen B */
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 hdmidll_clk;  /* HDMI serializer clock       */
   U32 hdmibch_clk;  /* HDMI bch clock              */
   U32 hdmitmds_clk; /* HDMI tmds clock             */
   U32 hdmipix_clk;  /* HDMI pixel clock            */
   U32 hdpix_clk;    /* HD pixel clock              */
   U32 hddisp_clk;   /* HD display clock            */
   U32 c656_clk;     /* DVO pixel clock             */
   U32 gdp3_clk;     /* GDP3 pixel clock            */
   U32 sdpix_clk;    /* SD pixel clock              */
   U32 sddisp_clk;   /* SD display clock            */
   U32 dvp_clk;      /* Emi clock                   */
   U32 pipe_clk;     /* Video pipeline clock        */
   U32 h264pp_clk;   /* H264 preprocessor clock     */
   U32 ic_clk;       /* Interconnect clock          */
   U32 rtc_clk;      /* Real time clock             */
   U32 lpc_clk;      /* Low power  clock            */
  } ckgb;
} ST_ClockInfo_t;
#endif

/* Clock infos structure for 7141 */
/* ------------------------------ */
#if defined(ST_7141)
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 struct 
  {
   U32 lmi_clk;         /* LMI PLL clock               */
   U32 pll0_clk;        /* PLL0 clock                  */
   U32 pll1_clk;        /* PLL1 clock                  */
   U32 st40cpu_clk;     /* ST40 cpu clock              */
   U32 st40tick_clk;    /* ST40 timer clock            */
   U32 com_clk;         /* Peripherals clock           */
   U32 fdma0_clk;       /* FDMA0 clock                 */
   U32 fdma1_clk;       /* FDMA1 clock                 */
   U32 fdma2_clk;       /* FDMA2 clock                 */
   U32 lxaud_clk;       /* Lx aud cpu clock            */
   U32 lxdmu_clk;       /* Lx dmu cpu clock            */
   U32 bdisp_clk;       /* Blitter display clock       */
   U32 disp_clk;        /* Display clock               */
   U32 ts_clk;          /* Transport bus clock         */
   U32 vdp_clk;         /* Video display pipe clock    */
   U32 blit_clk;        /* Blitter clock               */
   U32 eth_clk;         /* Ethernet clock              */
   U32 pci_clk;         /* PCI clock                   */
   U32 emi_clk;         /* Emi clock                   */
  } ckga;
 /* Clock gen B */
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 hdmidll_clk;  /* HDMI serializer clock       */
   U32 hdmibch_clk;  /* HDMI bch clock              */
   U32 hdmitmds_clk; /* HDMI tmds clock             */
   U32 hdmipix_clk;  /* HDMI pixel clock            */
   U32 hdpix_clk;    /* HD pixel clock              */
   U32 hddisp_clk;   /* HD display clock            */
   U32 c656_clk;     /* DVO pixel clock             */
   U32 gdp3_clk;     /* GDP3 pixel clock            */
   U32 sdpix_clk;    /* SD pixel clock              */
   U32 sddisp_clk;   /* SD display clock            */
   U32 dvp_clk;      /* Emi clock                   */
   U32 pipe_clk;     /* Video pipeline clock        */
   U32 h264pp_clk;   /* H264 preprocessor clock     */
   U32 ic_clk;       /* Interconnect clock          */
   U32 rtc_clk;      /* Real time clock             */
   U32 lpc_clk;      /* Low power  clock            */
  } ckgb;
} ST_ClockInfo_t;
#endif

/* Clock infos structure for 7200 */
/* ------------------------------ */
#if defined(ST_7200)
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 struct 
  {
   U32 lmi_clk;         /* LMI PLL clock               */
   U32 pll0_clk;        /* PLL0 clock                  */
   U32 pll1_clk;        /* PLL1 clock                  */
   U32 pll2_clk;        /* PLL2 clock                  */
   U32 st40cpu_clk;     /* ST40 cpu clock              */
   U32 st40bus_clk;     /* ST40 bus clock              */
   U32 st40per_clk;     /* ST40 peripheral clock       */
   U32 st40tick_clk;    /* ST40 timer clock            */
   U32 fdma0_clk;       /* FDMA0 clock                 */
   U32 fdma1_clk;       /* FDMA1 clock                 */
   U32 lxaud0_clk;      /* Lx aud0 cpu clock           */
   U32 lxaud1_clk;      /* Lx aud1 cpu clock           */
   U32 lxdmu0_clk;      /* Lx dmu0 cpu clock           */
   U32 lxdmu1_clk;      /* Lx dmu1 cpu clock           */
   U32 ic_266_clk;      /* High interconnect bus clock */
   U32 bdisp_266_clk;   /* Video clock                 */
   U32 lxdmu0_266_clk;  /* Lx dmu0 interconnect clock  */
   U32 lxdmu1_266_clk;  /* Lx dmu1 interconnect clock  */
   U32 disp_266_clk;    /* Video clock                 */
   U32 ic_reg_clk;      /* Low interconnect bus clock  */
   U32 bdisp_200_clk;   /* Blitter display clock       */
   U32 compo_200_clk;   /* Compositor clock            */
   U32 disp_200_clk;    /* Video clock                 */
   U32 vdp_200_clk;     /* Video display pipe clock    */
   U32 fdma_200_clk;    /* High FDMA clock             */
   U32 emi_clk;         /* Emi clock                   */
   U32 eth_clk;         /* Ethernet clock              */
   U32 com_clk;         /* Peripherals clock           */
  } ckga;
 /* Clock gen B */
 struct 
  {
   U32 pll0_clk;        /* PLL0 clock                  */
   U32 bdisp_266_clk;   /* Blitter display 266 clock   */
   U32 lxdmu0_266_clk;  /* Lx demu0 266 clock          */
   U32 lxdmu1_266_clk;  /* Lx demu1 266 clock          */
   U32 ic_reg_clk;      /* Register interconnect clock */
   U32 compo_200_clk;   /* Compositor clock            */
   U32 disp_200_clk;    /* Video clock                 */
   U32 vdp_200_clk;     /* Video display pipe clock    */
   U32 emi_clk;         /* Emi clock                   */
   U32 eth_clk;         /* Ethernet clock              */
   U32 com_clk;         /* Peripherals clock           */
   U32 fs0_1_clk;       /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;       /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;       /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;       /* Frequency synth0 clock 4    */
   U32 fs1_1_clk;       /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;       /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;       /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;       /* Frequency synth4 clock 4    */
   U32 fs2_1_clk;       /* Frequency synth1 clock 1    */
   U32 fs2_2_clk;       /* Frequency synth2 clock 2    */
   U32 fs2_3_clk;       /* Frequency synth3 clock 3    */
   U32 fs2_4_clk;       /* Frequency synth4 clock 4    */
   U32 hdmi_pll_clk;    /* HDMI clock before PLL       */
   U32 hdmi_phy_clk;    /* HDMI PHY clock after PLL    */
   U32 hd0pix_clk;      /* HD0 pixel clock             */
   U32 hd1pix_clk;      /* HD1 pixel clock             */
   U32 pipe_clk;        /* Video pipeline clock        */
   U32 lxdmu0_pipe_clk; /* Video pipeline clock        */
   U32 lxdmu1_pipe_clk; /* Video pipeline clock        */
   U32 disp_pipe_clk;   /* Video pipeline clock        */
   U32 vdp_pipe_clk;    /* Video pipeline clock        */
   U32 usb_clk;         /* USB clock                   */
   U32 sd0_pix_clk;     /* SD0 pixel clock             */
   U32 sd0_tsg_clk;     /* SD0 transport stream clock  */
   U32 periph_27_clk;   /* Periph 27MHz clock          */
   U32 sd1_pix_clk;     /* SD1 pixel clock             */
   U32 sd1_tsg_clk;     /* SD1  transport stream clock */
   U32 fs_216_clk;      /* 216 clock                   */
   U32 frc2_clk;        /* FRC2 clock                  */
   U32 ic_177_clk;      /* Interconnect clock 177MHz   */
   U32 dss_clk;         /* DSS clock                   */
   U32 ic_166_clk;      /* Interconnect clock 166MHz   */
   U32 rtc_clk;         /* Real time clock             */
   U32 lpc_clk;         /* Low power  clock            */
  } ckgb;
 /* Clock gen C */
 struct 
  {
   U32 fs0_1_clk;       /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;       /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;       /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;       /* Frequency synth0 clock 4    */
   U32 fs1_1_clk;       /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;       /* Frequency synth1 clock 2    */
   U32 fs1_3_clk;       /* Frequency synth1 clock 3    */
   U32 fs1_4_clk;       /* Frequency synth1 clock 4    */
   U32 pcm_0_clk;       /* PCM 0 clock                 */
   U32 pcm_1_clk;       /* PCM 1 clock                 */
   U32 pcm_2_clk;       /* PCM 2 clock                 */
   U32 pcm_3_clk;       /* PCM 3 clock                 */
   U32 spdif_clk;       /* Spdif clock                 */
   U32 spdif_hdmi_clk;  /* HDMI spdif clock            */
 } ckgc;
 /* Clockgen Local HD & SD TV output */
 struct
 {
   U32 main_ref_clk;     /* HDTVOUT main local clock   */
   U32 main_pix_clk;     /* HDTVOUT main pixel clock   */
   U32 fdvo_clk;         /* HDTVOUT Flex DVO clock     */
   U32 hdmi_bch_clk;     /* HDTVOUT HDMI BCH clock     */
   U32 hdmi_tmds_clk;    /* HDTVOUT TMDS clock         */ 
   U32 hdmi_tmds_phy_clk;/* TMDS PHY clock             */
   U32 obelix_main_clk;  /* Obelix main clock          */
   U32 obelix_aux_clk;   /* Obelix aux clock           */
   U32 obelix_gdp_clk;   /* Obelix gdp clock           */
   U32 obelix_vdp_clk;   /* Obelix video display pipe  */ 
   U32 obelix_cap_clk;   /* Obelix capture port clock  */
   U32 aux_ref_clk;      /* SDTVOUT aux local clock    */
   U32 aux_pix_clk;      /* SDTVOUT aux pixel clock    */
   U32 denc_clk;         /* SDTVOUT denc clock         */
   U32 ch34_clk;         /* SDTVOUT channel 3/4 clock  */
  } loctvout;
 /* Clockgen Remote SD TV output */
 struct 
 {
   U32 ref_clk;          /* SDTVOUT remote clock        */
   U32 pix_clk;          /* SDTVOUT pixel clock         */
   U32 asterix_clk;      /* Asterix main clock          */
   U32 fdvo_clk;         /* SDTVOUT Flex DVO clock      */
   U32 denc_clk;         /* SDTVOUT denc clock          */
 } remtvout;
} ST_ClockInfo_t;
#endif


/* Clock infos structure for 7106*/
/* ------------------------------ */
#if defined(ST_7106) 
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 struct 
  {
   U32 pll0hs_clk;        /* PLL0 HS clock              */
   U32 pll0ls_clk;        /* PLL0 LS clock              */
   U32 pll1_clk;          /* PLL1  clock                */
   U32 st40cpu_clk;       /* ST40 cpu clock             */
   U32 st40tick_clk;      /* ST40 timer clock           */
   U32 com_clk;           /* Peripherals clock          */
   U32 fdma0_clk;         /* FDMA0 clock                */
   U32 fdma1_clk;         /* FDMA1 clock                */
   U32 lxaud_clk;         /* Lx aud cpu clock           */
   U32 lxdmu_clk;         /* Lx dmu cpu clock           */
   U32 bdisp_clk;         /* Blitter display clock      */
   U32 disp_clk;          /* Display clock              */
   U32 ts_clk;            /* Transport bus clock        */
   U32 compo_clk;         /* Compositor clock           */
   U32 vdp_clk;           /* Video display pipe clock   */
   U32 blit_clk;          /* Blitter clock              */
   U32 eth_clk;           /* Ethernet clock             */
   U32 eth1_clk;          /* Ethernet1 clock            */
   U32 pci_clk;           /* PCI clock                  */
   U32 emi_clk;           /* Emi clock                  */
   U32 ic_clk;            /* Interconnect clock         */
   U32 mes_clk;           /*CLKA_IC_IF_200              */
 } ckga;
  /* Clock gen B */
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 fs_spare_clk; /* FS0 Spare clock             */
   U32 hdmitmds_clk; /* HDMI tmds clock             */
   U32 c656_1_clk;   /* DVO1 pixel clock            */
   U32 hdpix_clk;    /* HD pixel clock              */
   U32 hddisp_clk;   /* HD display clock            */
   U32 sddisp_clk;   /* SD display clock            */
   U32 c656_clk;     /* DVO pixel clock             */
   U32 gdp3_clk;     /* GDP3 pixel clock            */
   U32 sdpix_clk;    /* SD pixel clock              */
   U32 dvp_clk;      /* Emi clock                   */
   U32 lpc_clk;      /* Low power  clock            */
  } ckgb;
 /* CLOCK GEN C */
 struct
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
  }ckgc;
 /* CLOCK GEN D */
 struct
  {
   U32 lmi_clk;         /* LMI PLL clock            */
  }ckgd;

} ST_ClockInfo_t;
#endif

/* Clock infos structure for 5206*/
/* ------------------------------ */
#if defined(ST_5206) 
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
  struct 
  {
   U32 pll0hs_clk;      /* PLL0 HS clock                */
   U32 pll0ls_clk;      /* PLL0 LS clock                */
   U32 pll1_clk;        /* PLL1  clock                  */
   U32 st40cpu_clk;     /* ST40 cpu clock               */
   U32 st40tick_clk;    /* ST40 timer clock             */
   U32 com_clk;         /* Peripherals clock            */
   U32 fdma0_clk;       /* FDMA0 clock                  */
   U32 fdma1_clk;       /* FDMA1 clock                  */
   U32 lxaud_clk;       /* Lx aud cpu clock             */
   U32 lxdmu_clk;       /* Lx dmu cpu clock             */
   U32 bdisp_clk;       /* Blitter display clock        */
   U32 disp_clk;        /* Display clock                */
   U32 ts_clk;          /* Transport bus clock          */
   U32 vdp_clk;         /* Video display pipe clock     */
   U32 blit_clk;        /* Blitter clock                */
   U32 eth_clk;         /* Ethernet clock               */
   U32 pci_clk;         /* PCI clock                    */
   U32 emi_clk;         /* Emi clock                    */
   U32 compo_clk;       /* CLKA_IC_COMPO_200            */
   U32 ic_clk;          /* Interconnect clock           */
   U32 mes_clk;         /* MES clock                    */
  } ckga;
 
  /* Clock gen B */  
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 fs_spare_clk; /* FS0 Spare clock             */
   U32 sdpix_clk;    /* SD pixel clock              */ 
   U32 hdpix_clk;    /* HD pixel clock              */ 
   U32 lpc_clk;      /* Low power  clock            */
  } ckgb;

 /* CLOCK GEN C */
 struct
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
  }ckgc;
 /* CLOCK GEN D */
 struct
  {
   U32 lmi_clk;      /* LMI PLL clock CLKD_LMI2X*/
  }ckgd;

} ST_ClockInfo_t;
#endif

/* Clock infos structure for 7108*/
/* ------------------------------ */
#if defined(ST_7108) 
typedef struct ST_ClockInfo_s
{
 /* Clock gen A */
 
 struct 
  {
   U32 pll0hs_l_clk;      /* PLL0 HS Left clock         */
   U32 pll0ls_l_clk;      /* PLL0 LS Left clock         */
   U32 pll1_l_clk;        /* PLL1  Left clock           */
   U32 st40cpu_clk;       /* ST40 Host cpu clock        */
   U32 st40cpu_1_clk;     /* ST40 RT cpu clock          */
   U32 st40tick_clk;      /* ST40 timer clock           */
   U32 com_clk;           /* Peripherals clock          */
   U32 lxaud_clk;         /* Lx aud cpu clock           */
   U32 lxdmu_clk;         /* Lx dmu cpu clock           */
   U32 eth1_clk;          /* Ethernet1 clock            */
   U32 pci_clk;           /* PCI clock                  */
   U32 emi_clk;           /* Emi clock                  */
   U32 pll0hs_r_clk;      /* PLL0 HS Right clock        */
   U32 pll0ls_r_clk;      /* PLL0 LS Right clock        */
   U32 pll1_r_clk;        /* PLL1  Right clock          */
   U32 fdma0_clk;         /* FDMA0 clock                */
   U32 fdma1_clk;         /* FDMA1 clock                */
   U32 fdma2_clk;         /* FDMA1 clock                */
   U32 eth_clk;           /* Ethernet clock             */
   U32 bdisp_clk;         /* Blitter display clock      */
   U32 ts_clk;            /* Transport bus clock        */
   U32 compo_clk;         /* Compositor clock           */
   U32 vdp_clk;           /* Video display pipe clock   */
   U32 blit_clk;          /* Blitter clock              */
  
 } ckga;
  /* Clock gen B */
 struct 
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
   U32 fs1_1_clk;    /* Frequency synth1 clock 1    */
   U32 fs1_2_clk;    /* Frequency synth2 clock 2    */
   U32 fs1_3_clk;    /* Frequency synth3 clock 3    */
   U32 fs1_4_clk;    /* Frequency synth4 clock 4    */
   U32 hdpix_vc_clk; /* HD pixel clock to video clock controller */
   U32 sdpix_vc_clk; /* SD pixel clock to video clock controller */
   U32 hdpix_clk;    /* HD pixel clock              */
   U32 hddisp_clk;   /* HD display clock            */
   U32 sddisp_clk;   /* SD display clock            */
   U32 c656_clk;     /* DVO pixel clock             */
   U32 gdp1_clk;     /* GDP1 pixel clock            */
   U32 gdp2_clk;     /* GDP2 pixel clock            */   
   U32 sdpix_clk;    /* SD pixel clock              */
   U32 lpc_clk;      /* Low power  clock            */
   U32 usb_clk;      /*USB clock                    */
   U32 ccsc_clk;     /* CCSC clock                  */
   U32 dss_clk;      /* samrtcard clock             */
  } ckgb;
 /* CLOCK GEN C */
 struct
  {
   U32 fs0_1_clk;    /* Frequency synth0 clock 1    */
   U32 fs0_2_clk;    /* Frequency synth0 clock 2    */
   U32 fs0_3_clk;    /* Frequency synth0 clock 3    */
   U32 fs0_4_clk;    /* Frequency synth0 clock 4    */
  }ckgc;
 /* CLOCK GEN D */
 struct
  {
   U32 lmi_clk;      /* LMI PLL clock               */
  }ckgd;

} ST_ClockInfo_t;
#endif

/* Legacy Clock info structure common across all SOCs*/
typedef struct ST_LegacyClockInfo_s
{
    U32     STBus;
    U32     CommsBlock;
    U32     VideoMem;
    U32     AudioDSP;
    U32     EMI;
    U32     Flash;
    U32     SDRAM;
    U32     PCM;
    U32     HPTimer;
    U32     LPTimer;
    U32     ST40;
    U32     ST40Per;
    U32     PCI;
    U32     Aux;
    
} ST_LegacyClockInfo_t;


/* Prototypes */
/* ---------- */
U32  ST_ConvRevToNum(ST_Revision_t *ptRevision_p);
U32  ST_GetCutRevision(void);
U32  ST_GetClockSpeed(void);
U32  ST_GetClocksPerSecond(void);
U32  ST_GetClocksPerSecondHigh(void);
U32  ST_GetClocksPerSecondLow(void);
U32  ST_GetMajorRevision(ST_Revision_t *ptRevision_p);
U32  ST_GetMinorRevision(ST_Revision_t *ptRevision_p);
U32  ST_GetPatchRevision(ST_Revision_t *ptRevision_p);
#if !defined (ST_7109) && !defined (ST_5197) && !defined(ST_7100)  && !defined(ST_7108) 
//U32            ST_GetIpVersion(ST_HWIPtype_t ipname);
#endif
U32            ST_GetSocRevision(void);
BOOL          ST_AreStringsEqual(const char *DeviceName1_p,const char *DeviceName2_p);
ST_ErrorCode_t ST_GetClockInfo(ST_ClockInfo_t *ClockInfo_p);
ST_ErrorCode_t ST_GetLegacyClockInfo (ST_LegacyClockInfo_t *ClockInfo_p);
ST_Revision_t STCOMMON_GetRevision(void);
ST_ErrorCode_t STCOMMON_Open(void);
ST_ErrorCode_t STCOMMON_Close(void);


/* C++ support */
/* ----------- */
#ifdef __cplusplus
}
#endif

#endif

