//*****************************************************************************
// File name: vidl_mpegcodec_sptr.h
// Description: A smart pointer on a vidl_mpegcodec
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2002/07/17| l.e.galup                |Creation
//*****************************************************************************
#ifndef VIDL_MPEGCODEC_REF_H
#define VIDL_MPEGCODEC_REF_H

//
// typedef for class vbl_smart_ptr<vidl_mpegcodec>
// Include this file to use the smart pointer vidl_mpegcodec_sptr
//

class vidl_mpegcodec;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vidl_mpegcodec> vidl_mpegcodec_sptr;

#endif // ifndef VIDL_MPEGCODEC_REF_H
