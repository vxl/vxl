// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#ifndef gevd_status_mixin_h
#define gevd_status_mixin_h

//=========================== Basics ====================================
//
// .NAME	gevd_status_mixin
// .HEADER GeneralUtility Package
// .LIBRARY Basics
// .INCLUDE	Basics/gevd_status_mixin.h
// .FILE	gevd_status_mixin.h
// .FILE	gevd_status_mixin.C
//
// .SECTION Description
//      This mixin adds a status word and methods to contol the status
// word.  The interpretation of the status word depends on the context
// of the usage.  However, in all cases a status of zero is defined as
// good.
//
// Modified by:      Brian DeCleene
// Date:	     June 17, 1991
// Description:      Combined M&DSO status and CR&D status into a 
//                   single object.
//
//
//======================================================================

/* ------------------------------------ */
/*       DEFINE A STATUS TYPE           */
typedef int StatusCode;


class gevd_status_mixin
{
private:
   int status;

public:
   inline void ClearStatus()            { status = 0;   }
   inline void ClearStatus(int x)       { status &= ~x; }
   inline void SetStatus(int x = 0)     { status |= x;  }

   inline void SetStatusGood()		{ status = 0; }
   inline void SetStatusBad(int c =-1)	{ status = c; }
      
   gevd_status_mixin() { ClearStatus(); }

public:
   inline StatusCode Stat()          const { return status; }     // XXX
   inline bool       StatusGood()    const { return status == 0; }
   inline bool       StatusBad()     const { return status != 0; }
   inline StatusCode GetStatusCode() const { return status; }
};

#endif // gevd_status_mixin_h
