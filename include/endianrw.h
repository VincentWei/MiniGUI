/**
 * \file endianrw.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 * \brief This file includes functions for reading and writing data 
 *        from general sources, such as file, memory, etc., and also
 *        includes functions for reading and writing endian-specific 
 *        values.
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
 * $Id: endianrw.h 11349 2009-03-02 05:00:43Z weiym $
 * 
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *      The idea comes from LGPL'ed SDL by Sam Lantinga.
 */

#ifndef _MGUI_ENDIAN_RW_H
#define _MGUI_ENDIAN_RW_H


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/************************** General RW operations ****************************/

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup general_rw_fns General read/write operations
     *
     * MiniGUI's general read/write operation provides a general interface 
     * to read from and write to various data source, such as files, memory, 
     * and so on.
     *
     * @{
     */

#define RWAREA_TYPE_UNKNOWN 0
#define RWAREA_TYPE_STDIO   1
#define RWAREA_TYPE_MEM     2

/**
 * The read/write operation structure.
 */
typedef struct _MG_RWops {
    /**
     * Seek to \a offset relative to whence, one of stdio's whence values:\n
     *      SEEK_SET, SEEK_CUR, SEEK_END\n
     * Returns the final offset in the data source.
     */
    int (*seek)(struct _MG_RWops *context, int offset, int whence);

    /**
     * Read up to \a num objects each of size \a objsize from the data
     * source to the area pointed at by \a ptr.
     * Returns the number of objects read, or -1 if the read failed.
     */
    int (*read)(struct _MG_RWops *context, void *ptr, int objsize, int num);

    /**
     * Write exactly \a num objects each of size \a objsize from the area
     * pointed at by \a ptr to data source.
     * Returns \a num, or -1 if the write failed.
     */
    int (*write)(struct _MG_RWops *context, const void *ptr, int objsize, 
                    int num);

#ifdef _MGUSE_OWN_STDIO
    /* */
    int (*ungetc)(struct _MG_RWops *context, unsigned char c);
#endif

    /**
     * Close and free an allocated MG_RWops structure.
     */
    int (*close)(struct _MG_RWops *context);

    /**
     * Test the end-of-file indicator.
     */
    int (*eof)(struct _MG_RWops *context);

    /**
     * Indicates the type of data source.
     *  can be one of the following values:
     *  - RWAREA_TYPE_UNKNOWN\n
     *      A unknown (uninitialized) data source type.
     *  - RWAREA_TYPE_STDIO\n
     *      Stdio stream data source.
     *  - RWAREA_TYPE_MEM\n
     *      Memory data source.
     */
    Uint32 type;

    union {
        struct {
            int autoclose;
            FILE *fp;
        } stdio;
        struct {
            Uint8 *base;
            Uint8 *here;
            Uint8 *stop;
        } mem;
        struct {
            void *data1;
        } unknown;
    } hidden;
} MG_RWops;

/**
 * \fn MG_RWops* MGUI_RWFromFile(const char *file, const char *mode)
 * \brief Creates an MG_RWops object from a file.
 *
 * This function uses the mode specified by \a mode and opens the file \a file
 * by using stdio function \a fopen. If success, this function creates a 
 * MG_RWops object and returns it.
 *
 * \param file The file name.
 * \param mode The mode will be passed to \a fopen.
 * \return The pointer to created MG_RWops structure, NULL indicates error.
 *
 * \sa MG_RWops, MGUI_RWFromFP, MGUI_FreeRW, fopen(3)
 */
MG_EXPORT MG_RWops* MGUI_RWFromFile(const char *file, const char *mode);

/**
 * \fn MG_RWops* MGUI_RWFromFP(FILE *fp, int autoclose)
 * \brief Creates an MG_RWops object from an opened stdio FILE object.
 *
 * This function uses an opened stdio FILE object \a fp to create a 
 * MG_RWops object.
 *
 * \param fp The opened stdio FILE object.
 * \param autoclose Indicates whether to close the FILE object when \a 
 *        close method is called.
 * \return The pointer to created MG_RWops structure, NULL indicates error.
 *
 * \sa MG_RWops, MGUI_RWFromFile, MGUI_FreeRW
 */
MG_EXPORT MG_RWops* MGUI_RWFromFP(FILE *fp, int autoclose);

/**
 * \fn MG_RWops* MGUI_RWFromMem(void *mem, int size)
 * \brief Creates an MG_RWops object from a block of memory.
 *
 * This function creates an MG_RWops object from a block of memory pointed to 
 * by \a mem, which is \a size bytes long.
 *
 * \param mem The pointer to the memory block.
 * \param size The size of the memory block.
 *
 * \return The pointer to created MG_RWops structure, NULL indicates error.
 *
 * \sa MG_RWops, MGUI_FreeRW
 */
MG_EXPORT MG_RWops* MGUI_RWFromMem(void *mem, int size);

/**
 * \fn void MGUI_InitMemRW (MG_RWops* area, void *mem, int size)
 * \brief Initializes an MG_RWops object from a block of memory.
 *
 * This function initializes an MG_RWops object pointed to by \a area 
 * from a block of memory pointed to by \a mem, which is \a size bytes long.
 *
 * \param area The pointer to the MG_RWops object.
 * \param mem The pointer to the memory block.
 * \param size The size of the memory block.
 *
 * \return None.
 *
 * \sa MG_RWops, MGUI_FreeRW, MGUI_RWFromMem
 */
MG_EXPORT void MGUI_InitMemRW (MG_RWops* area, void *mem, int size);

/**
 * \fn MG_RWops* MGUI_AllocRW(void)
 * \brief Allocates an uninitialized MG_RWops object.
 *
 * This function allocates an uninitialized MG_RWops object. You can specify the 
 * fields of the structure, and implemente a customized MG_RWops object.
 *
 * \return The pointer to allocated MG_RWops structure, NULL indicates error.
 *
 * \sa MG_RWops
 */
MG_EXPORT MG_RWops* MGUI_AllocRW(void);

/**
 * \fn void MGUI_FreeRW(MG_RWops *area)
 * \brief Frees an MG_RWops object.
 *
 * This function frees the MG_RWops object pointed to by \a area.
 *
 * \param area The pointer to the MG_RWops object.
 *
 * \sa MGUI_RWFromFile, MGUI_RWFromFP, MGUI_RWFromMem
 */
MG_EXPORT void MGUI_FreeRW(MG_RWops *area);

/**
 * \def MGUI_RWseek(ctx, offset, whence)
 * \brief Seeks an MG_RWops object.
 *
 * This macro seeks to \a offset relative to \a whence.
 *
 * \param ctx The pointer to the MG_RWops object.
 * \param offset The offset relative to \a whence.
 * \param whence One of stdio's \a whence values:
 *
 *      - SEEK_SET\n
 *        the offset is relative to the start of the file.
 *      - SEEK_CUR\n
 *        the offset is relative to the current position indicator.
 *      - SEEK_END\n
 *        the offset is relative to the end of the file.
 *
 * \return The final offset in the data source.
 *
 * \sa MGUI_RWtell
 */
#define MGUI_RWseek(ctx, offset, whence)    (ctx)->seek(ctx, offset, whence)

/**
 * \def MGUI_RWtell(ctx)
 * \brief Obtains the current value of the position indicator for a data source.
 *
 * This macro obtains the current value of the position indicator for the 
 * data source pointed to by \a ctx.
 *
 * \param ctx The pointer to the MG_RWops object.
 *
 * \return The current value of the position indicator.
 *
 * \sa MGUI_RWseek
 */
#define MGUI_RWtell(ctx)                    (ctx)->seek(ctx, 0, SEEK_CUR)

/**
 * \def MGUI_RWread(ctx, ptr, size, n)
 * \brief Reads data blocks from a data source.
 *
 * This macro reads up to \a n objects each of size \a size from the data
 * source \a ctx to the area pointed to by \a ptr.
 *
 * \param ctx The pointer to the MG_RWops object.
 * \param ptr The buffer will save the data read.
 * \param size The size of each object.
 * \param n The number of objects to be read.
 * \return The number of objects read, or -1 if the read failed.
 *
 * \sa MGUI_RWwrite
 */
#define MGUI_RWread(ctx, ptr, size, n)      (ctx)->read(ctx, ptr, size, n)

/**
 * \def MGUI_RWwrite(ctx, ptr, size, n)
 * \brief Writes data blocks to a data source.
 *
 * This macro writes exactly \a n objects each of size \a size from the area
 * pointed to by \a ptr to the data source \a ctx.
 *
 * \param ctx The pointer to the MG_RWops object.
 * \param ptr The buffer contains the data to be written.
 * \param size The size of each object.
 * \param n The number of objects to be written.
 * \return The number written, or -1 if the write failed.
 *
 * \sa MGUI_RWread
 */
#define MGUI_RWwrite(ctx, ptr, size, n)     (ctx)->write(ctx, ptr, size, n)

/**
 * \def MGUI_RWclose(ctx)
 * \brief Closes an MG_RWops object.
 *
 * This macro close the MG_RWops object pointed to by \a ctx.
 *
 * \param ctx The pointer to the MG_RWops object.
 *
 * \return Upon successful completion 0 is returned, 
 *         otherwise non-zero on error.
 *
 * \sa MGUI_RWread
 */
#define MGUI_RWclose(ctx)                   (ctx)->close(ctx)

/**
 * \def MGUI_RWeof(ctx)
 * \brief Tests the end-of-file indicator for an data source.
 *
 * This macro tests the end-of-file indicator for the data source pointed to 
 * by \a ctx.
 *
 * \param ctx The pointer to the MG_RWops object.
 *
 * \return Non-zero if end-of-file indicator is set.
 *
 * \sa MGUI_RWtell
 */
#define MGUI_RWeof(ctx)                     (ctx)->eof(ctx)

/**
 * \fn int MGUI_RWgetc (MG_RWops* area)
 * \brief Reads the next character from an data source.
 *
 * This function reads the next character from the data source pointed to 
 * by \a area, and returns it as an \a unsigned char cast to an \a int, 
 * or \a EOF on end of file or error.
 *
 * \param area The pointer to the MG_RWops object.
 *
 * \return The character read, \a EOF indicates end-of-file or error.
 *
 * \sa MGUI_RWread
 */
MG_EXPORT int MGUI_RWgetc (MG_RWops* area);

    /** @} end of general_rw_fns */

/****************** Endian specific read/write interfaces *********************/

    /**
     * \defgroup endian_rw_fns Endian specific read/write interfaces
     *
     * The endian specific read/write functions read and write data
     * of the specified endianness, dynamically translating to 
     * the host machine endianness.
     *
     * e.g.: If you want to read a 16 bit value on big-endian machine from
     * an opened file containing little endian values, you would use:
     *
     * \code
     *  value = MGUI_ReadLE16(rp);
     * \endcode
     *
     * \sa general_rw_fns
     *
     * Example:
     *
     * \include endianness.c
     * @{
     */

/* The macros used to swap values */
/* Try to use superfast macros on systems that support them */
#ifdef linux
#include <endian.h>
#ifdef __arch__swab16
#define ArchSwap16  __arch__swab16
#endif
#ifdef __arch__swab32
#define ArchSwap32  __arch__swab32
#endif
#endif /* linux */

/* Use inline functions for compilers that support them, and static
   functions for those that do not.  Because these functions become
   static for compilers that do not support inline functions, this
   header should only be included in files that actually use them.
*/
#ifndef ArchSwap16
static inline Uint16 ArchSwap16(Uint16 D) {
        return((D<<8)|(D>>8));
}
#endif
#ifndef ArchSwap32
static inline Uint32 ArchSwap32(Uint32 D) {
        return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}
#endif
#ifdef MGUI_HAS_64BIT_TYPE
#ifndef ArchSwap64
static inline Uint64 ArchSwap64(Uint64 val) {
        Uint32 hi, lo;

        /* Separate into high and low 32-bit values and swap them */
        lo = (Uint32)(val&0xFFFFFFFF);
        val >>= 32;
        hi = (Uint32)(val&0xFFFFFFFF);
        val = ArchSwap32(lo);
        val <<= 32;
        val |= ArchSwap32(hi);
        return(val);
}
#endif
#else
#ifndef ArchSwap64
/* This is mainly to keep compilers from complaining in MGUI code.
   If there is no real 64-bit datatype, then compilers will complain about
   the fake 64-bit datatype that MGUI provides when it compiles user code.
*/
#define ArchSwap64(X)        (X)
#endif
#endif /* MGUI_HAS_64BIT_TYPE */

/* Byteswap item from the specified endianness to the native endianness */
#if MGUI_BYTEORDER == MGUI_LIL_ENDIAN
/** Swaps a 16-bit little endian integer to the native endianness. */
#define ArchSwapLE16(X)        (X)
/** Swaps a 32-bit little endian integer to the native endianness. */
#define ArchSwapLE32(X)        (X)
/** Swaps a 64-bit little endian integer to the native endianness. */
#define ArchSwapLE64(X)        (X)
/** Swaps a 16-bit big endian integer to the native endianness. */
#define ArchSwapBE16(X)        ArchSwap16(X)
/** Swaps a 32-bit big endian integer to the native endianness. */
#define ArchSwapBE32(X)        ArchSwap32(X)
/** Swaps a 64-bit big endian integer to the native endianness. */
#define ArchSwapBE64(X)        ArchSwap64(X)
#else
#define ArchSwapLE16(X)        ArchSwap16(X)
#define ArchSwapLE32(X)        ArchSwap32(X)
#define ArchSwapLE64(X)        ArchSwap64(X)
#define ArchSwapBE16(X)        (X)
#define ArchSwapBE32(X)        (X)
#define ArchSwapBE64(X)        (X)
#endif

/**
 * \fn Uint16 MGUI_ReadLE16(MG_RWops *src)
 * \brief Reads a 16-bit little endian integer from a MG_RWops object.
 *
 * This function reads a 16-bit little endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteLE16
 */
extern Uint16 MGUI_ReadLE16(MG_RWops *src);

/**
 * \fn Uint16 MGUI_ReadBE16(MG_RWops *src)
 * \brief Reads a 16-bit big endian integer from a MG_RWops object.
 *
 * This function reads a 16-bit big endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteBE16
 */
extern Uint16 MGUI_ReadBE16(MG_RWops *src);

/**
 * \fn Uint32 MGUI_ReadLE32(MG_RWops *src)
 * \brief Reads a 32-bit little endian integer from a MG_RWops object.
 *
 * This function reads a 32-bit little endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteLE32
 */
extern Uint32 MGUI_ReadLE32(MG_RWops *src);

/**
 * \fn Uint32 MGUI_ReadBE32(MG_RWops *src)
 * \brief Reads a 32-bit big endian integer from a MG_RWops object.
 *
 * This function reads a 32-bit big endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteBE32
 */
extern Uint32 MGUI_ReadBE32(MG_RWops *src);

/**
 * \fn Uint64 MGUI_ReadLE64(MG_RWops *src)
 * \brief Reads a 64-bit little endian integer from a MG_RWops object.
 *
 * This function reads a 64-bit little endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteLE64
 */
extern Uint64 MGUI_ReadLE64(MG_RWops *src);

/**
 * \fn Uint64 MGUI_ReadBE64(MG_RWops *src)
 * \brief Reads a 64-bit big endian integer from a MG_RWops object.
 *
 * This function reads a 64-bit big endian integer from the data source
 * pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the MG_RWops object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteBE64
 */
extern Uint64 MGUI_ReadBE64(MG_RWops *src);

/**
 * \fn int MGUI_WriteLE16(MG_RWops *src, Uint16 value)
 * \brief Writes an 16-bit integer of native format to a MG_RWops object 
 *        in littlen endianness.
 *
 * This function writes a 16-bit integer of native format to the data source
 * pointed to by \a src in littlen endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 16-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE16
 */
extern int MGUI_WriteLE16(MG_RWops *dst, Uint16 value);

/**
 * \fn int MGUI_WriteBE16(MG_RWops *src, Uint16 value)
 * \brief Writes an 16-bit integer of native format to a MG_RWops object 
 *        in big endianness.
 *
 * This function writes a 16-bit integer of native format to the data source
 * pointed to by \a src in big endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 16-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE16
 */
extern int MGUI_WriteBE16(MG_RWops *dst, Uint16 value);

/**
 * \fn int MGUI_WriteLE32(MG_RWops *src, Uint32 value)
 * \brief Writes an 32-bit integer of native format to a MG_RWops object 
 *        in littlen endianness.
 *
 * This function writes a 32-bit integer of native format to the data source
 * pointed to by \a src in littlen endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 32-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE32
 */
extern int MGUI_WriteLE32(MG_RWops *dst, Uint32 value);

/**
 * \fn int MGUI_WriteBE32(MG_RWops *src, Uint32 value)
 * \brief Writes an 32-bit integer of native format to a MG_RWops object 
 *        in big endianness.
 *
 * This function writes a 32-bit integer of native format to the data source
 * pointed to by \a src in big endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 32-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE32
 */
extern int MGUI_WriteBE32(MG_RWops *dst, Uint32 value);

/**
 * \fn int MGUI_WriteLE64(MG_RWops *src, Uint64 value)
 * \brief Writes an 64-bit integer of native format to a MG_RWops object 
 *        in littlen endianness.
 *
 * This function writes a 64-bit integer of native format to the data source
 * pointed to by \a src in littlen endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 64-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE64
 */
extern int MGUI_WriteLE64(MG_RWops *dst, Uint64 value);

/**
 * \fn int MGUI_WriteBE64(MG_RWops *src, Uint64 value)
 * \brief Writes an 64-bit integer of native format to a MG_RWops object 
 *        in big endianness.
 *
 * This function writes a 64-bit integer of native format to the data source
 * pointed to by \a src in big endiannes.
 *
 * \param src The pointer to the MG_RWops object.
 * \param value The 64-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE64
 */
extern int MGUI_WriteBE64(MG_RWops *dst, Uint64 value);

/**
 * \fn Uint16 MGUI_ReadLE16FP(FILE *src)
 * \brief Reads a 16-bit little endian integer from a stdio FILE object.
 *
 * This function reads a 16-bit little endian integer from the stdio 
 * FILE object pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the stdio FILE object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteLE16FP, MGUI_ReadLE16
 */
extern Uint16 MGUI_ReadLE16FP(FILE *src);

/**
 * \fn Uint32 MGUI_ReadLE32FP(FILE *src)
 * \brief Reads a 32-bit little endian integer from a stdio FILE object.
 *
 * This function reads a 32-bit little endian integer from the stdio 
 * FILE object pointed to by \a src, and return it in native format.
 *
 * \param src The pointer to the stdio FILE object.
 * \return The integer in native endianness.
 *
 * \sa MGUI_WriteLE32FP, MGUI_ReadLE32
 */
extern Uint32 MGUI_ReadLE32FP(FILE *src);

/**
 * \fn int MGUI_WriteLE16FP(FILE *dst, Uint16 value)
 * \brief Writes an 16-bit integer of native format to a stdio FILE object 
 *        in littlen endianness.
 *
 * This function writes a 16-bit integer of native format to the stdio 
 * FILE object pointed to by \a src in littlen endiannes.
 *
 * \param dst The pointer to the MG_RWops object.
 * \param value The 16-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE16FP, MGUI_WriteLE16
 */
extern int MGUI_WriteLE16FP(FILE *dst, Uint16 value);

/**
 * \fn int MGUI_WriteLE32FP(FILE *dst, Uint32 value)
 * \brief Writes an 32-bit integer of native format to a stdio FILE object 
 *        in littlen endianness.
 *
 * This function writes a 32-bit integer of native format to the stdio 
 * FILE object pointed to by \a src in littlen endiannes.
 *
 * \param dst The pointer to the MG_RWops object.
 * \param value The 32-bit integer in native endianness.
 * \return Returns 1 on success, else indicates an error.
 *
 * \sa MGUI_ReadLE32FP, MGUI_WriteLE32
 */
extern int MGUI_WriteLE32FP(FILE *dst, Uint32 value);

static inline Uint16 MGUI_ReadLE16Mem (const Uint8** data)
{
#if 1
    Uint16 h1, h2;

    h1 = *(*data); (*data)++;
    h2 = *(*data); (*data)++;
    return ((h2<<8)|h1);
#else
    Uint16 u;
    memcpy (&u, *data, sizeof (Uint16));
    u = ArchSwapLE16 (u);
    *data += sizeof (Uint16);
    return u;
#endif
}

static inline Uint32 MGUI_ReadLE32Mem (const Uint8** data)
{
#if 1
    Uint32 q1, q2, q3, q4;

    q1 = *(*data); (*data)++;
    q2 = *(*data); (*data)++;
    q3 = *(*data); (*data)++;
    q4 = *(*data); (*data)++;
    return ((q4<<24)|(q3<<16)|(q2<<8)|(q1));
#else
    Uint32 u;
    memcpy (&u, *data, sizeof (Uint32));
    u = ArchSwapLE32 (u);
    *data += sizeof (Uint32);
    return u;
#endif
}

static inline Uint16 MGUI_ReadBE16Mem (const Uint8** data)
{
#if 1
    Uint16 h1, h2;

    h1 = *(*data); (*data)++;
    h2 = *(*data); (*data)++;
    return ((h1<<8)|h2);
#else
    Uint16 u;
    memcpy (&u, *data, sizeof (Uint16));
    u = ArchSwapBE16 (u);
    *data += sizeof (Uint16);
    return u;
#endif
}

static inline Uint32 MGUI_ReadBE32Mem (const Uint8** data)
{
#if 1
    Uint32 q1, q2, q3, q4;

    q1 = *(*data); (*data)++;
    q2 = *(*data); (*data)++;
    q3 = *(*data); (*data)++;
    q4 = *(*data); (*data)++;
    return ((q1<<24)|(q2<<16)|(q3<<8)|(q4));
#else
    Uint32 u;
    memcpy (&u, *data, sizeof (Uint32));
    u = ArchSwapBE32 (u);
    *data += sizeof (Uint32);
    return u;
#endif
}

    /** @} end of endian_rw_fns */

    /** @} end of global_fns */

    /** @} end of fns */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _MGUI_ENDIAN_RW_H */

