//*****************************************************************************
// File name: vtol_block_sptr.h
// Description: A smart pointer on a vtol_block_
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_BLOCK_REF_H
#define VTOL_BLOCK_REF_H

class vtol_block;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_block> vtol_block_sptr;

#endif // #ifndef VTOL_BLOCK_REF_H
