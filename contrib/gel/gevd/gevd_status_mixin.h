#ifndef gevd_status_mixin_h
#define gevd_status_mixin_h

//=========================== Basics ====================================
//
// .NAME    gevd_status_mixin
// .HEADER  gel Package
// .LIBRARY gevd
// .INCLUDE gevd/gevd_status_mixin.h
// .FILE    gevd_status_mixin.cxx
//
// .SECTION Description
//      This mixin adds a status word and methods to control the status
// word.  The interpretation of the status word depends on the context
// of the usage.  However, in all cases a status of zero is defined as
// good.
//
// Modified by:      Brian DeCleene
// Date:             June 17, 1991
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

   inline void SetStatusGood()          { status = 0; }
   inline void SetStatusBad(int c =-1)  { status = c; }

   gevd_status_mixin() { ClearStatus(); }

 public:
   inline StatusCode Stat()          const { return status; }     // XXX
   inline bool       StatusGood()    const { return status == 0; }
   inline bool       StatusBad()     const { return status != 0; }
   inline StatusCode GetStatusCode() const { return status; }
};

#endif // gevd_status_mixin_h
