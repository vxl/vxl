//*****************************************************************************
// File name: vcsl_graph_sptr.h
// Description: Smart pointer on a vcsl_graph
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/01| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_GRAPH_REF_H
#define VCSL_GRAPH_REF_H

class vcsl_graph;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_graph> vcsl_graph_sptr;

#endif // #ifndef VCSL_GRAPH_REF_H
