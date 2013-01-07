/*---------------------------------------------------------------------------*
  Project:  Dolphin GX library
  File:     GXFifo.h

  Copyright 1998-2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: GXFifo.h,v $
  Revision 1.5  03/10/2006 07:29:36  mitu
  changed include file from <dolphin/-.h> to <revolution/-.h>.

  Revision 1.4  02/09/2006 12:30:03  hirose
  Added const specifier to GXSet* APIs. Added GXGetFifoWrap().

  Revision 1.3  02/06/2006 05:01:12  hirose
  API updates. Deleted obsolete portion.

  Revision 1.2  2006/02/04 11:56:46  hashida
  (none)

  Revision 1.1.1.1  2005/12/29 06:53:28  hiratsu
  Initial import.

  Revision 1.1.1.1  2005/05/12 02:41:07  yasuh-to
  transitioned from the Dolphin source tree

    
    13     02/07/24 9:06 Hirose
    Resolved version conflicts.
    
    13    02/04/09 15:15 Hirose
    const type specifier support. (worked by hiratsu@IRD)
    
    12    4/10/01 6:47p Carl
    Added GXSaveGPFifo.
    
    11    11/09/00 3:52p Tian
    Added write gather pipe redirection
    
    10    11/01/00 11:13a Hirose
    flag fix EPPC -> !EMU
    
    9     6/05/00 3:46p Carl
    Removed GetFifoTop; added GetFifoLimits.
    
    8     5/30/00 4:37p Tian
    Added overflow count API
    
    7     5/23/00 6:45p Carl
    Added get prototypes.
    
    6     4/26/00 4:53p Carl
    CallBacks are now Callbacks.  Set callback routines now return old
    callbacks.  Added gets for fifo objects.
    
    5     3/15/00 1:30p Tian
    Added fifo wrap bit to GXGetFifoStatus
    
    4     3/13/00 4:16p Tian
    Added breakpoint callback
    
    3     3/13/00 3:05p Tian
    Added GXGetGPStatus, updated GXGetFifoStatus.
    
    2     2/17/00 5:52p Tian
    Added GXSetCurrentGXThread and GXGetCurrentGXThread
    
    12    10/26/99 6:14p Tian
    Cleanup, added min fifo size and size of hi water mark buffer.
    
    11    10/25/99 11:02a Tian
    made __GXWritePI available in all EPPC builds
    
    10    10/22/99 3:23p Tian
    Added __GXWritePI to capture all PI commands for CSIM
    
    9     10/04/99 11:10a Tian
    Updated GXGetFifoStatus prototype
    
    8     9/23/99 3:26p Tian
    New GXFifo API
    
    7     9/08/99 5:15p Alligator
    
    5     8/02/99 4:17p Shiki
    Included <revolution/os/OSAlarm.h>.

    4     99/07/30 6:37p Shiki
    Added EPPC specific functions.

    3     99/07/30 5:13p Shiki
    Uncommented. Fixed minor errors.

    2     7/27/99 1:07p Alligator
    updated api

    1     7/14/99 4:20p Alligator
    split gx.h into individual header files for each major section of API
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __GXFIFO_H__
#define __GXFIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <revolution/types.h>
#include <revolution/gx/GXEnum.h>


// Minimum size for any FIFO is 64k
// The hi watermark is automatically set to fifosize - 16k,
// large enough for a full locked cache DMA.
#define GX_FIFO_MINSIZE             (64*1024)
#define GX_FIFO_HI_WATERMARK_BUFFER (16*1024)

// Placeholder structure for GXFifoObj.  Actual structure members are private.
#define GX_FIFO_OBJ_SIZE            128

typedef struct{
    u8      pad[GX_FIFO_OBJ_SIZE];
} GXFifoObj;

typedef void (*GXBreakPtCallback)(void);
    

/*---------------------------------------------------------------------------*
  Init FIFO APIs
 *---------------------------------------------------------------------------*/
void GXInitFifoBase(
    GXFifoObj*  fifo,
    void*       base,
    u32         size );

void GXInitFifoPtrs(
    GXFifoObj*  fifo,
    void*       readPtr,
    void*       writePtr );

void GXInitFifoLimits(
    GXFifoObj*  fifo,
    u32         hiWaterMark,
    u32         loWaterMark );


/*---------------------------------------------------------------------------*
  Attaching FIFOs to CPU/GP
 *---------------------------------------------------------------------------*/
void GXSetCPUFifo   ( const GXFifoObj* fifo );
void GXSetGPFifo    ( const GXFifoObj* fifo );


/*---------------------------------------------------------------------------*
  Get APIs
 *---------------------------------------------------------------------------*/
void GXGetGPStatus(
    GXBool* overhi,
    GXBool* underlow,
    GXBool* readIdle,
    GXBool* cmdIdle,
    GXBool* brkpt);

GXBool  GXGetCPUFifo( GXFifoObj* fifo );
GXBool  GXGetGPFifo ( GXFifoObj* fifo );

void*   GXGetFifoBase( const GXFifoObj* fifo );
void    GXGetFifoPtrs( const GXFifoObj* fifo, void** readPtr, void** writePtr );
u32     GXGetFifoCount( const GXFifoObj* fifo );
u32     GXGetFifoSize( const GXFifoObj* fifo );
void    GXGetFifoLimits( const GXFifoObj* fifo, u32* hi, u32* lo );
GXBool  GXGetFifoWrap( const GXFifoObj* fifo );

GXBool  GXIsCPUGPFifoLinked( void );


/*---------------------------------------------------------------------------*
  Breakpoint APIs
 *---------------------------------------------------------------------------*/
GXBreakPtCallback GXSetBreakPtCallback( GXBreakPtCallback cb );

void GXEnableBreakPt( void* breakPtr );
void GXDisableBreakPt( void );


/*---------------------------------------------------------------------------*
  Redirecting write gather pipe
 *---------------------------------------------------------------------------*/
volatile void*   GXRedirectWriteGatherPipe   ( void * ptr );
void             GXRestoreWriteGatherPipe    ( void );


/*---------------------------------------------------------------------------*
  GX thread that will be interrupted by FIFO handlers in immediate mode
 *---------------------------------------------------------------------------*/
#include <revolution/os/OSThread.h>
OSThread*   GXSetCurrentGXThread ( void );
OSThread*   GXGetCurrentGXThread ( void );



/*---------------------------------------------------------------------------*
  Overflow counter API
 *---------------------------------------------------------------------------*/
u32 GXGetOverflowCount      ( void );
u32 GXResetOverflowCount    ( void );


#ifdef __cplusplus
}
#endif

#endif //__GXFIFO_H__
