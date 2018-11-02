// This is contrib/brl/bbas/bhdfs/bhdfs_fstream.h
#ifndef bhdfs_fstream_h_
#define bhdfs_fstream_h_
//:
// \file
// \brief A wrap around for file operations in libdhfs to access hadoop's file system (HDFS)
//
//        Caution: data types are defined as follows in libdhfs
//                 typedef int32_t   tSize; /// size of data for read/write io ops
//                 typedef time_t    tTime; /// time type in seconds
//                 typedef int64_t   tOffset;/// offset within the file
//                 typedef uint16_t  tPort; /// port
//        use casting accordingly when adding new methods, see definition of bhdfs_streampos below
//
// \author Ozge C. Ozcanli
// \date Dec 01, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <string>
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bhdfs_manager.h"

#if VXL_HAS_INT_64
typedef vxl_int_64 bhdfs_streampos;
#else //VXL_HAS_INT_64
typedef vxl_int_32 bhdfs_streampos;
#endif //VXL_HAS_INT_64

class bhdfs_fstream: public vbl_ref_count
{
 public:
  // supported flags are "r" (read only), "w" (for write and truncate), (append is not supported by libdhfs yet)
  bhdfs_fstream(std::string filename, char const* mode_flags);

  // implement an interface that is similar to vil_stream
  bool ok() const { return f_ != 0; }
  bhdfs_streampos write(void const* buf, bhdfs_streampos n);

  //: just try to read, if not successful returns -1
  bhdfs_streampos read(void* buf, bhdfs_streampos n);

  //: return current offset in the file, -1 if error
  bhdfs_streampos tell() const;

  void seek(bhdfs_streampos position);
  bool close();

  //: caution: libhdfs does not always return actual size, so may only return 0
  bhdfs_streampos file_size() const;

 protected:
  ~bhdfs_fstream();

 private:
  hdfsFile f_;
  std::string fname_;

};
typedef vbl_smart_ptr<bhdfs_fstream> bhdfs_fstream_sptr;

#endif // bhdfs_fstream_h_
