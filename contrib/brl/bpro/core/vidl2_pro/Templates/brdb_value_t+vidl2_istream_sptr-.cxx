#include <brdb/brdb_value.txx>

#include <vidl2/vidl2_istream_sptr.h>
#include <core/vidl2_pro/vidl2_io_istream.h>
typedef vidl2_istream_sptr istr_sptr;

BRDB_VALUE_INSTANTIATE(istr_sptr, "vidl2_istream_sptr");
