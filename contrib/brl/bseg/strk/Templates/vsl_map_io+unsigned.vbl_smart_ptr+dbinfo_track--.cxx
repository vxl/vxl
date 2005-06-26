// Instantiation of IO for vcl_map<unsigned, dbinfo_track_sptr>
#include <dbinfo/dbinfo_track.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_map_io.txx>
VSL_MAP_IO_INSTANTIATE(unsigned, dbinfo_track_sptr, vcl_less<unsigned>);
