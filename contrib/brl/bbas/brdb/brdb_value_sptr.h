// This is brl/brdb/brdb_value_sptr.h
#ifndef brdb_value_sptr_h
#define brdb_value_sptr_h

//:
// \file

// forward declaration
template< class T > class brdb_value_t;

class brdb_value;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<brdb_value> brdb_value_sptr;


#endif // brdb_value_sptr_h
