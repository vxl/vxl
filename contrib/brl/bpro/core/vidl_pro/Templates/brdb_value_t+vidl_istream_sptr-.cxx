#include <brdb/brdb_value.hxx>

#include <vidl/vidl_istream_sptr.h>
#include <core/vidl_pro/vidl_io_istream.h>
typedef vidl_istream_sptr istr_sptr;

BRDB_VALUE_INSTANTIATE(istr_sptr, "vidl_istream_sptr");
