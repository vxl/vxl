//*****************************************************************************
// File name: vtol_two_chain_ref.h
// Description: A smart pointer on a vtol_two_chain
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/11| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_TWO_CHAIN_REF_H
#define VTOL_TWO_CHAIN_REF_H

class vtol_two_chain;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_two_chain> vtol_two_chain_ref;

#endif // #ifndef VTOL_TWO_CHAIN_REF_H
