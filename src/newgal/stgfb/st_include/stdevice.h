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
/*        MATERIALS, which is provided “AS IS”, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File   stdevice.h                                                            
 @brief  System-wide devices header file. This file defines the complete system 
         hardware & software configuration. It does this by including a 
         board-specific configuration file and a software specific configuration 
         file.                                                     
*/
#ifndef __STDEVICE_H
#define __STDEVICE_H

#if !defined(STAPIREF_COMPAT) && !defined(STAPIREF_INCLUDE_COMPAT)
/* STFAE - Reroute device definitions */
/* ---------------------------------- */
#if defined(cab5197)&&defined(ST_5197)             /* Cab 5197 board based on 5197              */
#include "cab5197_5197.h"
#endif
#if defined(sat5189)&&defined(ST_5197)             /* Sat 5189 board based on 5189              */
#include "sat5189_5197.h"
#endif
#if defined(sat7111)&&defined(ST_7111)             /* Sat 7111 board based on 7111              */
#include "sat7111_7111.h"
#endif
#if defined(hdk5289)&&defined(ST_5206)             /* HDK5289 board based on 5206               */
#include "hdk5289_5206.h"
#endif
#if defined(hdk7111)&&defined(ST_7111)             /* HDK7111 board based on 7111               */
#include "hdk7111_7111.h"
#endif
#if defined(mb411)&&defined(ST_7109)               /* MB411 Platform for STx7109                */
#include "mb411_7109.h"
#endif
#if defined(mb618)&&defined(ST_7111)               /* MB618 Platform for STx7111                */
#include "mb618_7111.h"
#endif
#if defined(mb628)&&defined(ST_7141)               /* MB628 Platform for STx7141                */
#include "mb628_7141.h"
#endif
#if defined(mb628e)&&defined(ST_7141)              /* MB628E Platform for STx7141               */
#include "mb628e_7141.h"
#endif
#if defined(fpb2)&&defined(ST_7141)                /* FPB2 Platform for STx7141                 */
#include "fpb2_7141.h"
#endif
#if defined(eud7141)&&defined(ST_7141)             /* EUD7141 Platform for STx7141              */
#include "eud7141_7141.h"
#endif
#if defined(mb671)&&defined(ST_7200)               /* MB671 Platform for STx7200                */
#include "mb671_7200.h"
#endif
#if defined(mb676)&&defined(ST_5197)               /* MB676 Platform for STx5197                */
#include "mb676_5197.h"
#endif
#if defined(mb680)&&defined(ST_7105)               /* MB680 Platform for STx7105                */
#include "mb680_7105.h"
#endif
#if defined(mb680)&&defined(ST_7106)               /* MB680 Platform for STx7106                */
#include "mb680_7106.h"
#endif
#if defined(mb837)&&defined(ST_7108)               /* MB837 Platform for STx7108                */
#include "mb837_7108.h"
#endif
#if defined(mb796)&&defined(ST_5206)               /* MB796 Platform for STx5206                */
#include "mb796_5206.h"
#endif
#if defined(mb704)&&defined(ST_5197)               /* MB704 Platform for STx5197                */
#include "mb704_5197.h"
#endif
#if defined(futarque_hybrid)&&defined(ST_7109)     /* Futarque hybrid Platform for STx7109      */
#include "futarque_hybrid_7109.h"
#endif
#if defined(cocoref_v2)&&defined(ST_7109)          /* Cocoref V2 Platform for STx7109           */
#include "cocoref_v2_7109.h"
#endif
#if defined(cocoref_gold)&&defined(ST_7109)        /* Cocoref GOLD Platform for STx7109         */
#include "cocoref_gold_7109.h"
#endif
#if defined(cocoref_gold_hnd)&&defined(ST_7109)    /* Cocoref GOLD HND Platform for STx7109     */
#include "cocoref_gold_hnd_7109.h"
#endif
#if defined(hmp)&&defined(ST_7109)                 /* HMP Platform for STx7109                  */
#include "hmp_7109.h"
#endif
#if defined(hmp7105)&&defined(ST_7105)             /* HMP7105 Platform for STx7105              */
#include "hmp7105_7105.h"
#endif
#if defined(dsg2500)&&defined(ST_7109)             /* DSG2500 Platform for STx7109              */
#include "dsg2500_7109.h"
#endif
#if defined(mb602)&&defined(ST_7109)               /* DSG2500 Platform for STx5202              */
#include "mb602_7109.h"
#endif
#if defined(sdk5202)&&defined(ST_7109)             /* SDK Platform for STx5202                  */
#include "sdk5202_7109.h"
#endif
#if defined(sdk7105)&&defined(ST_7105)             /* SDK Platform for STx7105                  */
#include "sdk7105_7105.h"
#endif
#if defined(iptv7105)&&defined(ST_7105)            /* IPTV Platform for STx7105                 */
#include "iptv7105_7105.h"
#endif
#if defined(dtc7167)&&defined(ST_7105)             /* DTC7167 Platform for STx7167              */
#include "dtc7167_7105.h"
#endif
#if defined(dtt7167)&&defined(ST_7105)             /* DTT7167 Platform for STx7167              */
#include "dtt7167_7105.h"
#endif
#if defined(cab7167)&&defined(ST_7105)             /* CAB7167 Platform for STx7167              */
#include "cab7167_7105.h"
#endif
#if defined(adt7167)&&defined(ST_7105)             /* ADT7167 Platform for STx7167              */
#include "adt7167_7105.h"
#endif
#if defined(dtt5267)&&defined(ST_7105)             /* DTT5267 Platform for STx5267              */
#include "dtt5267_7105.h"
#endif
#if defined(mtv7109)&&defined(ST_7109)             /* MTV7109 Evaluation Platform               */
#include "mtv7109_7109.h"
#endif
#if defined(idtv7109)&&defined(ST_7109)            /* IDTV7109 Evaluation Platform              */
#include "idtv7109_7109.h"
#endif
#if defined(hdref)&&defined(ST_7109)               /* HDREF Evaluation Platform                 */
#include "hdref_7109.h"
#endif
#if defined(custom001009)&&defined(ST_7109)        /* Custom001009 Platform for STx7109         */
#include "custom001009_7109.h"
#endif
#if defined(custom001011)&&defined(ST_7109)        /* Custom001011 Platform for STx7109         */
#include "custom001011_7109.h"
#endif
#if defined(custom001012)&&defined(ST_7109)        /* Custom001012 Platform for STx7109         */
#include "custom001012_7109.h"
#endif
#if defined(custom001013)&&defined(ST_7109)        /* Custom001013 Platform for STx7109         */
#include "custom001013_7109.h"
#endif
#if defined(custom001019)&&defined(ST_7109)        /* Custom001019 Platform for STx7109         */
#include "custom001019_7109.h"
#endif
#if defined(custom001022)&&defined(ST_7109)        /* Custom001022 Platform for STx7109         */
#include "custom001022_7109.h"
#endif
#if defined(custom001025)&&defined(ST_7109)        /* Custom001025 Platform for STx5202         */
#include "custom001025_7109.h"
#endif
#if defined(custom001026)&&defined(ST_7109)        /* Custom001026 Platform for STx5202         */
#include "custom001026_7109.h"
#endif
#if defined(custom001029)&&defined(ST_7109)        /* Custom001029 Platform for STx7109         */
#include "custom001029_7109.h"
#endif 
#if defined(custom001030)&&defined(ST_7111)        /* Custom001030 Platform for STx7111         */
#include "custom001030_7111.h"
#endif
#if defined(custom001031)&&defined(ST_7109)        /* Custom001031 Platform for STx7109         */
#include "custom001031_7109.h"
#endif 
#if defined(custom001033)&&defined(ST_7109)        /* Custom001033 Platform for STx7109         */
#include "custom001033_7109.h"
#endif
#if defined(custom001034)&&defined(ST_7141)        /* Custom001034 Platform for STx7141         */
#include "custom001034_7141.h"
#endif
#if defined(custom001035)&&defined(ST_7141)        /* Custom001035 Platform for STx7141         */
#include "custom001035_7141.h"
#endif 
#if defined(custom001036)&&defined(ST_7105)        /* Custom001036 Platform for STx7105         */
#include "custom001036_7105.h"
#endif
#if defined(custom001037)&&defined(ST_7109)        /* Custom001037 Platform for STx7109         */
#include "custom001037_7109.h"
#endif 
#if defined(custom002002)&&defined(ST_7109)        /* Custom002002 Platform for STx7109         */
#include "custom002002_7109.h"
#endif
#if defined(custom002005)&&defined(ST_7111)        /* Custom002005 Platform for STx7111         */
#include "custom002005_7111.h"
#endif
#if defined(custom002006)&&defined(ST_7105)        /* Custom002006 Platform for STx7105         */
#include "custom002006_7105.h"
#endif
#if defined(custom003004)&&defined(ST_7109)        /* Custom003004 Platform for STx7109         */
#include "custom003004_7109.h"
#endif
#if defined(custom003008)&&defined(ST_7111)        /* Custom003008 Platform for STx7111         */
#include "custom003008_7111.h"
#endif
#if defined(custom003009)&&defined(ST_7105)        /* Custom003009 Platform for STx7105         */
#include "custom003009_7105.h"
#endif
#if defined(custom003011)&&defined(ST_7105)        /* Custom003011 Platform for STx7105         */
#include "custom003011_7105.h"
#endif
#if defined(custom003012)&&defined(ST_7105)        /* Custom003012 Platform for STx7105         */
#include "custom003012_7105.h"
#endif
#if defined(custom004002)&&defined(ST_7109)        /* Custom004002 Platform for STx7109         */
#include "custom004002_7109.h"
#endif
#if defined(custom005001)&&defined(ST_7109)        /* Custom005001 Platform for STx7109         */
#include "custom005001_7109.h"
#endif
#if defined(custom006001)&&defined(ST_7109)        /* Custom006001 Platform for STx7109         */
#include "custom006001_7109.h"
#endif
#if defined(custom012002)&&defined(ST_7109)        /* Custom012002 Platform for STx7109         */
#include "custom012002_7109.h"
#endif
#if defined(custom015001)&&defined(ST_7200)        /* Custom015001 Platform for STx7200         */
#include "custom015001_7200.h"
#endif
#if defined(custom018001)&&defined(ST_7111)        /* Custom018001 Platform for STx7111         */
#include "custom018001_7111.h"
#endif
#if defined(custom019001)&&defined(ST_7200)        /* Custom019001 Platform for STx7200         */
#include "custom019001_7200.h"
#endif
#if defined(custom021001)&&defined(ST_7141)        /* Custom021001 Platform for STx7141         */
#include "custom021001_7141.h"
#endif

#else /*#if !defined(STAPIREF_COMPAT) && !defined(STAPIREF_INCLUDE_COMPAT)*/

/*
 * Select target evaluation platform.
 * At least one of these must be set at compile time. If the
 * user has not set the environment variables then the build
 * system will select mb231.
 */

/* Same board, different memory configuration */
#if defined(mb5518um)
	#define mb5518
#endif

/* Same board, different chip cuts and memory configuration */
#if defined(mb314_21) || defined(mb314_um) || defined(mb314_21_um)
    #define mb314
#endif

#if defined(mb231)        /* MB231 Evaluation Platform for STi5510 */
#include "mb231.h"
#elif defined(mb282)     /* MB282 Evaluation Platform for STi5512 */
#include "mb282.h"
#elif defined(mb282b)     /* MB282 Evaluation Platform for STi5512 */
#include "mb282b.h"
#elif defined(mb275) || defined(mb275_64) /* MB275 Evaluation Platform for STi5508 */
#include "mb275.h"
#elif defined(mb193) /* MB193 Evaluation Platform for ST20TP3 */
#include "mb193.h"
#elif defined(mb295) || defined(MB295)     /* MB295 Evaluation Platform for ST20TP3+7015 */
#include "mb295.h"
#elif defined(mb290)  /* MB290 Evaluation Platform for STi5514+7020 */
#include "mb290.h"
#elif defined(mb5518) /* MB5518 Evaluation Platform for STi5518 */
#include "mb5518.h"
#elif defined(mb317a) || defined(mb317b) /* MB317 Evaluation Platform for ST40GX1 */
#include "mb317.h"
#elif defined(mb314)  /* MB314 Evaluation Platform for STi5514 */
#include "mb314.h"
#elif defined(mediaref)
    #if defined(ST_5514)
        #include "mb314.h"
    #elif defined(ST_GX1) || defined(ST_NGX1)
        #include "mb317.h"
    #endif
#elif defined(mb361)  /* MB361 Evaluation Platform for STi5516/17 */
#include "mb361.h"
#elif defined(mb382)  /* MB382 Evaluation Platform for STi5517 */
#include "mb382.h"
#elif defined(mb376)  /* MB376 Evaluation Platform for STi5528 */
#include "mb376.h"
#elif defined(espresso)  /* Espresso Evaluation Platform for STi5528 */
#include "espresso.h"
#elif defined(mb390)  /* MB390 Evaluation Platform for STi5100/5101/5301 */
#include "mb390.h"
#elif defined(mb391)  /* MB391 Evaluation Platform for STi7710 */
#include "mb391.h"
#elif defined(mb394)  /* MB394 DTTi5516 Brick Board */
#include "mb394.h"
#elif defined(mb400)  /* MB400 Evaluation Platform for STi5105 */
#include "mb400.h"
#elif defined(mb385)  /* MB385-Champagne Evaluation Platform for STm5700 */
#include "mb385.h"
#elif defined(walkiry)/* Walkiry Evaluation Platform for STm5700 */
#include "walkiry.h"
#elif defined(mb411) || defined(cocoref_v2)  /* MB411 Evaluation Platform for STi7100/STx7109 */ /*cocoref_v2 for stapi_sdk*/
#include "mb411.h"
#elif defined(mb426)  /* MB426-Traviata Evaluation Platform for STm8010 */
#include "mb426.h"
#elif defined(mb421)  /* MB421 Evaluation Platform for STm8010 */
#include "mb421.h"
#elif defined(mb395)  /* MB395 Evaluation Platform for STi5100 */
#include "mb395.h"
#elif defined(maly3s) /* MALY3S Reference Platform for STi5105 */
#include "maly3s.h"
#elif defined(mb428)  /* MB428 Evaluation Platform for STx5525 */
#include "mb428.h"
#elif defined(mb457)  /* MB457 Evaluation Platform for STx5188 */
#include "mb457.h"
#elif defined(mb436)  /* MB436 Evaluation Platform for STx5107 */
#include "mb436.h"
#elif defined(DTT5107) /* DTT5107 Evaluation Platform for STx5107 */
#include "DTT5107.h"
#elif defined(CAB5107) /* CAB5107 Evaluation Platform for STx5107 */
#include "CAB5107.h"
#elif defined(SAT5107) /* SAT5107 Evaluation Platform for STx5107 */
#include "SAT5107.h"
#elif defined(mb519)  /* MB519 Evaluation Platform for STx7200 cut1 */
#include "mb519.h"
#elif defined(mb634)  /* MB634 Evaluation Platform for STx5162 */
#include "mb634.h"
#elif defined(mb618)  /* MB618 Evaluation Platform for STx7111 */
#include "mb618.h"
#elif defined(mb680)  /* MB680 Evaluation Platform for STx7105 */
#include "mb680.h"
#elif defined(mb796)  /* MB796 Evaluation Platform for STx5206 */
#include "mb796.h"
#elif defined(mb628)  /* MB628 Evaluation Platform for STx7141 */
#include "mb628.h"
#elif defined(mb671)  /* MB671 Evaluation Platform for STx7200 cut2 */
#include "mb671.h"
#elif defined(mb704)  /* MB704 Evaluation Platform for STx5197 */
#include "mb704.h"
#elif defined(mb676)  /* MB676 Evaluation Platform for STx5197/STx5189 */
#include "mb676.h"
#elif defined(mb837)  /* MB837 Evaluation Platform for STx7108 */
#include "mb837.h"
#elif defined(BRD_VALI_NACELLE)  /* Evaluation Platform for Nacelle */
#include "nacelle_brd.h"
#elif defined(BRD_VALI_SUNDIAL)  /* Evaluation Platform for Nacelle */
#include "sundial_brd.h"
#else
/*For GNBvd36334-->Request enhancement for user platform*/
/*If the user wants to use their own customised platform, they can add a file named "stdevice_user.h"
in the include directory. If the user specifies an unrecognised platform in DVD_PLATFORM &
does not put a stdevice_user.h file, then they will get a "#include file "stdevice_user.h" wouldn't open"
error during compilation*/
#include "stdevice_user.h" /*This file is to be added by the user for their customised platform*/
#endif


/* Select target processor device.
 * At least one of these must be set at compile time.
 */

#if defined(ST_5510)      /* STi5510 device */
#include "sti5510.h"
#elif defined(ST_5512)    /* STi5512 device */
#include "sti5512.h"
#elif defined(ST_5508)    /* STi5508 device */
#include "sti5508.h"
#elif defined(ST_5518)    /* STi5518 device */
#include "sti5518.h"
#elif defined(ST_5580)    /* STi5580 device */
#include "sti5580.h"
#elif defined(ST_TP3)     /* ST20TP3 device */
#include "st20tp3.h"
#elif defined(ST_GX1)     /* ST40GX1 device */
#include "st40gx1.h"
#elif defined(ST_NGX1)    /* ST40NGX1 device */
#include "st40ngx1.h"
#elif defined(ST_STB1)    /* ST40STB1 device */
#include "st40stb1.h"
#elif defined(ST_7750)    /* ST407750 device */
#include "st407750.h"
#elif defined(ST_5514)    /* STi5514 device */
#include "sti5514.h"
#elif defined(ST_5516)    /* STi5516 device */
#include "sti5516.h"
#elif defined(ST_5517)    /* STi5517 device */
#include "sti5517.h"
#elif defined(ST_5528)    /* STi5528 device */
#include "sti5528.h"
#elif defined(ST_5100)    /* STi5100 device */
#include "sti5100.h"
#elif defined(ST_5101)    /* STi5101 device */
#include "sti5101.h"
#elif defined(ST_7710)    /* STi7710 device */
#include "sti7710.h"
#elif defined(ST_5105)    /* STi5105 device */
#include "sti5105.h"
#elif defined(ST_5700)    /* STm5700 device */
#include "stm5700.h"
#elif defined(ST_7100)    /* STi7100 device */
#include "sti7100.h"
#elif defined(ST_8010)    /* STm8010 device */
#include "stm8010.h"
#elif defined(ST_5301)    /* STi5301 device */
#include "sti5301.h"
#elif defined(ST_5525)    /* STx5525 device */
#include "sti5525.h"
#elif defined(ST_7109)    /* STx7109 device */
#include "sti7109.h"
#elif defined(ST_5188)    /* STx5188 device */
#include "sti5188.h"
#elif defined(ST_5107)    /* STx5107 device */
#include "sti5107.h"
#elif defined(ST_7200)    /* STx7200 device */
#include "sti7200.h"
#elif defined(ST_5162)    /* STx5162 device */
#include "sti5162.h"
#elif defined(ST_7111)    /* STx7111 device */
#include "sti7111.h"
#elif defined(ST_7105)    /* STx7105 device */
#include "sti7105.h"
#elif defined(ST_7106)    /* STx7106 device */
#include "sti7106.h"
#elif defined(ST_7108)    /* STx7108 device */
#include "sti7108.h"
#elif defined(ST_5206)    /* STx5206 device */
#include "sti5206.h"
#elif defined(ST_7141)    /* STx7141 device */
#include "sti7141.h"
#elif defined(ST_5197)    /* STx5197 device */
#include "sti5197.h"
#elif defined(ST_VALI_NACELLE) /* VALI_NACELLE device */
#include "nacelle.h"
#elif defined(ST_VALI_SUNDIAL) /* VALI_SUNDIAL device */
#include "sundial.h"
#else
#error No target CPU selected by environment (variable DVD_FRONTEND)
#endif

/*
 * Select backend decoder device.
 * This is optional and may be omitted from the build.
 */
#if defined(ST_7015)      /* STi7015 device */
#include "sti7015.h"
#endif

#if defined(ST_7020)      /* STi7020 device */
#include "sti7020.h"
#endif

#if defined(ST_4629)      /* STi4629 device */
#include "sti4629.h"
#endif


/*
 * Software configuration
 */

#include "swconfig.h"

#endif /*#if !defined(STAPIREF_COMPAT) && !defined(STAPIREF_INCLUDE_COMPAT)*/
#endif /* __STDEVICE_H */

/* End of stdevice.h */

