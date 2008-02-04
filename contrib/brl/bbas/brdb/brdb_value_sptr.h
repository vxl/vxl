// This is brl/bbas/brdb/brdb_value_sptr.h
#ifndef brdb_value_sptr_h
#define brdb_value_sptr_h
//:
// \file

#include <vbl/vbl_smart_ptr.h>

// forward declarations
template<class T > class brdb_value_t;
class brdb_value;

typedef vbl_smart_ptr<brdb_value> brdb_value_sptr;

#endif // brdb_value_sptr_h
