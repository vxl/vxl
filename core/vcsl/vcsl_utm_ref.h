//*****************************************************************************
// File name: vcsl_utm_ref.h
// Description: Smart pointer on a vcsl_utm
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_UTM_REF_H
#define VCSL_UTM_REF_H

class vcsl_utm;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_utm> vcsl_utm_ref;

#endif // #ifndef VCSL_UTM_REF_H
