/**
 * \file fixedmath.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/12
 * 
 * \brief This file includes fixed point and three-dimension math routines.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: fixedmath.h 12154 2009-10-15 07:21:34Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *      Fixed-point math routins come from Allegro (a gift software)
 *      by Shawn Hargreaves and others.
 */

#ifndef _MGUI_MGHAVE_FIXED_MATH_H
#define _MGUI_MGHAVE_FIXED_MATH_H


#include <errno.h>
#include <math.h>
#include <stdlib.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MGHAVE_FIXED_MATH

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup fixed_math_fns Fixed point math functions
     * 
     * You know that the float point mathematics routines are very
     * expensive. If you do not want precision mathematics result, 
     * you can use fixed point. MiniGUI uses a double word (32-bit)
     * integer to represent a fixed point ranged from -32767.0 to 
     * 32767.0, and defines some fixed point mathematics routines for 
     * your application. Some GDI functions need fixed point 
     * math routines, like \a Arc.
     *
     * Example 1:
     * 
     * \include fixed_point.c
     *
     * Example 2:
     * 
     * \include fixedpoint.c
     * @{
     */

/**
 * \fn fixed fixsqrt (fixed x)
 * \brief Returns the non-negative square root of a fixed point value.
 *
 * This function returns the non-negative square root of \a x.
 * It fails and sets errno to EDOM, if \a x is negative.
 *
 * \sa fixhypot
 */
MG_EXPORT fixed fixsqrt (fixed x);

/**
 * \fn fixed fixhypot (fixed x, fixed y)
 * \brief Returns the Euclidean distance from the origin.
 * 
 * The function returns the \a sqrt(x*x+y*y). This is the length of 
 * the hypotenuse of a right-angle triangle with sides of length \a x and \a y, 
 * or the distance of the point \a (x,y) from the origin.
 *
 * \sa fixsqrt
 */
MG_EXPORT fixed fixhypot (fixed x, fixed y);

/**
 * \fn fixed fixatan (fixed x)
 * \brief Calculates the arc tangent of a fixed point value.
 *
 * This function calculates the arc tangent of \a x; that is the value 
 * whose tangent is \a x.
 *
 * \return Returns the arc tangent in radians and the value is 
 *         mathematically defined to be between -PI/2 and PI/2 (inclusive).
 *
 * \sa fixatan2
 */
MG_EXPORT fixed fixatan (fixed x);

/**
 * \fn fixed fixatan2 (fixed y, fixed x)
 * \brief Calculates the arc tangent of two fixed point variables.
 *
 *  This function calculates the arc tangent of the two variables \a x and \a y.
 *  It is similar to calculating the arc tangent of \a y / \a x, except that 
 *  the signs of both arguments are used to determine the quadrant of the 
 *  result.
 *
 * \return Returns the result in radians, which is between -PI and PI 
 *         (inclusive).
 *
 * \sa fixatan
 */
MG_EXPORT fixed fixatan2 (fixed y, fixed x);

extern MG_EXPORT fixed _cos_tbl[];
extern MG_EXPORT fixed _tan_tbl[];
extern MG_EXPORT fixed _acos_tbl[];

/************************** inline fixed point math functions *****************/
/* ftofix and fixtof are used in generic C versions of fixmul and fixdiv */

/**
 * \fn fixed ftofix (double x)
 * \brief Converts a float point value to a fixed point value.
 *
 * This function converts the specified float point value \a x to 
 * a fixed point value.
 *
 * \note The float point should be ranged from -32767.0 to 32767.0.
 * If it runs out of the range, this function sets \a errno to \a ERANGE.
 *
 * \sa fixtof
 */
static inline fixed ftofix (double x)
{ 
   if (x > 32767.0) {
      errno = ERANGE;
      return 0x7FFFFFFF;
   }

   if (x < -32767.0) {
      errno = ERANGE;
      return -0x7FFFFFFF;
   }

   return (long)(x * 65536.0 + (x < 0 ? -0.5 : 0.5)); 
}

/**
 * \fn double fixtof (fixed x)
 * \brief Converts a fixed point value to a float point value.
 *
 * This function converts the specified fixed point value \a x to 
 * a float point value.
 *
 * \sa ftofix
 */
static inline double fixtof (fixed x)
{ 
   return (double)x / 65536.0; 
}


/**
 * \fn fixed fixadd (fixed x, fixed y)
 * \brief Returns the sum of two fixed point values.
 *
 * This function adds two fixed point values \a x and \a y, and
 * returns the sum.
 *
 * \param x x,y: Two addends.
 * \param y x,y: Two addends.
 * \return The sum. If the result runs out of range of fixed point, 
 *         this function sets \a errno to \a ERANGE.
 *
 * \sa fixsub
 */
static inline fixed fixadd (fixed x, fixed y)
{
   fixed result = x + y;

   if (result >= 0) {
      if ((x < 0) && (y < 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFF;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y > 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFF;
      }
      else
	 return result;
   }
}

/**
 * \fn fixed fixsub (fixed x, fixed y)
 * \brief Subtract a fixed point value from another.
 *
 * This function subtracts the fixed point values \a y from the fixed 
 * point value \a x, and returns the difference.
 *
 * \param x The minuend.
 * \param y The subtrahend.
 *
 * \return The difference. If the result runs out of range of fixed point, 
 *         this function sets \a errno to \a ERANGE.
 *
 * \sa fixadd
 */
static inline fixed fixsub (fixed x, fixed y)
{
   fixed result = x - y;

   if (result >= 0) {
      if ((x < 0) && (y > 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFF;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y < 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFF;
      }
      else
	 return result;
   }
}

/**
 * \fn fixed fixmul (fixed x, fixed y)
 * \brief Returns the product of two fixed point values.
 * 
 * This function returns the product of two fixed point values \a x and \a y.
 *
 * \param x The faciend.
 * \param y The multiplicato.
 * \return The prodcut. If the result runs out of range of fixed point, 
 *         this function sets \a errno to \a ERANGE.
 * 
 * \sa fixdiv
 */
MG_EXPORT fixed fixmul (fixed x, fixed y);

/**
 * \fn fixed fixdiv (fixed x, fixed y)
 * \brief Returns the quotient of two fixed point values.
 * 
 * This function returns the quotient of two fixed point values \a x and \a y.
 *
 * \param x The dividend.
 * \param y The divisor.
 * \return The quotient. If the result runs out of range of fixed point, 
 *         this function sets \a errno to \a ERANGE.
 * 
 * \sa fixmul
 */
MG_EXPORT fixed fixdiv (fixed x, fixed y);

/**
 * \fn int fixceil (fixed x)
 * \brief Rounds a fixed point value to the nearest integer.
 *
 * This function rounds the fixed point value \a x to the nearest integer
 * and returns it.
 *
 * \return The rounded integer value.
 */

static inline int fixceil (fixed x)
{
   if (x > 0x7FFF0000) {
      errno = ERANGE;
      return 0x7FFF;
   }

   x += 0xFFFF;
   if (x > 0)
       return (x >> 16);
   else
       return ~((~x) >> 16);
}

/**
 * \fn fixed itofix (int x)
 * \brief Converts an integer to a fixed point value.
 *
 * This function converts the integer \a x to a fixed point value.
 *
 * \sa fixtoi
 */
static inline fixed itofix (int x)
{ 
   return x << 16;
}

/**
 * \fn int fixtoi (fixed x)
 * \brief Converts an fixed point value to an integer.
 *
 * This function converts the fixed point \a x to an integer.
 *
 * \sa itofix
 */
static inline int fixtoi (fixed x)
{ 
   return (x >> 16) + ((x & 0x8000) >> 15);
}

/**
 * \fn fixed fixcos (fixed x)
 * \brief Returns the cosine of a fixed point.
 *
 * This function returns the cosine of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa fixacos
 */
static inline fixed fixcos (fixed x)
{
   return _cos_tbl[((x + 0x4000) >> 15) & 0x1FF];
}

/**
 * \fn fixed fixsin (fixed x)
 * \brief Returns the sine of a fixed point.
 *
 * This function returns the sine of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa fixasin
 */
static inline fixed fixsin (fixed x)
{ 
   return _cos_tbl[((x - 0x400000 + 0x4000) >> 15) & 0x1FF];
}

/**
 * \fn fixed fixtan (fixed x)
 * \brief Returns the tangent of a fixed point.
 *
 * This function returns the tangent of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa fixcos, fixsin
 */
static inline fixed fixtan (fixed x)
{ 
   return _tan_tbl[((x + 0x4000) >> 15) & 0xFF];
}

/**
 * \fn fixed fixacos (fixed x)
 * \brief Calculates and returns the arc cosine of a fixed point.
 *
 * This function calculates the arc cosine of the fixed point \a x; 
 * that is the value whose cosine is \a x. If \a x falls outside
 * the range -1 to 1, this function fails and \a errno is set to EDOM.
 *
 * \return Returns the arc cosine in radians and the value is mathematically 
 *         defined to be between 0 and PI (inclusive).
 *
 * \sa fixcos
 */
static inline fixed fixacos (fixed x)
{
   if ((x < -65536) || (x > 65536)) {
      errno = EDOM;
      return 0;
   }

   return _acos_tbl[(x+65536+127)>>8];
}

/**
 * \fn fixed fixasin (fixed x)
 * \brief Calculates and returns the arc sine of a fixed point.
 *
 * This function calculates the arc sine of the fixed point \a x; 
 * that is the value whose sine is \a x. If \a x falls outside
 * the range -1 to 1, this function fails and \a errno is set to EDOM.
 *
 * \return Returns the arc sine in radians and the value is mathematically 
 *         defined to be between -PI/2 and PI/2 (inclusive).
 *
 * \sa fixsin
 */
static inline fixed fixasin (fixed x)
{ 
   if ((x < -65536) || (x > 65536)) {
      errno = EDOM;
      return 0;
   }

   return 0x00400000 - _acos_tbl[(x+65536+127)>>8];
}

    /** @} end of fixed_math_fns */


    /** @} end of global_fns */

    /** @} end of fns */

#endif /* _MGHAVE_FIXED_MATH */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _MGUI_MGHAVE_FIXED_MATH_H */

