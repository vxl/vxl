//*****************************************************************************
// File name: vcsl_composition_sptr.h
// Description: Smart pointer on a vcsl_composition
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/08| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_COMPOSITION_REF_H
#define VCSL_COMPOSITION_REF_H

class vcsl_composition;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_composition> vcsl_composition_sptr;

#endif // #ifndef VCSL_COMPOSITION_REF_H
