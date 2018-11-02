// This is brl/bbas/volm/volm_conf_buffer.h
#ifndef volm_conf_buffer_h_
#define volm_conf_buffer_h_
//:
// \file
// \brief A templated class for binary IO of volm configurational content.  For each location hypothesis, keep a vector of content
//        Note for simplicity, the templated Type needs to have vsl_b_write / vsl_b_read to handle its binary IO
//
// \author Yi Dong
// \date August 21, 2014
// \verbatim
//   Modifications
// \endverbatim

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

template <class Type>
class volm_conf_buffer
{
 public:
  enum mode { READ = 0, WRITE = 1, NOT_INITIALIZED = 2 };
  // ================ constructor ===================

  //: default constructor
  volm_conf_buffer();
  //: constructor by specifying the max GBs on RAM for this class to use
  volm_conf_buffer(float const& buffer_capacity);
  //: destructor
  ~volm_conf_buffer();

  // ================ access ========================

  std::vector<int>& length_vec() { return length_vec_; }
  unsigned current_id()  const { return current_id_; }
  unsigned global_current_id() const { return global_current_id_; }
  unsigned buffer_size() const { return buffer_size_; }
  unsigned unit_size()   const { return unit_size_; }
  unsigned read_in()     const { return read_in_; }
  std::string file_name() const { return file_name_; }
  std::string index_mode() const {
    switch (m_)
    {
      case(READ):
        return "READ";   break;
      case(WRITE):
        return "WRITE";  break;
      default:
        return "NOT_INITIALIZED";  break;
    }
  }

  // ================ method ========================

  //: append values associated with certain location to the end of the current active buffer.
  //: caller is in charge of the internal order
  bool add_to_index(std::vector<Type> const& values);
  bool add_to_index(Type const& value);

  //: retrieve the next index from current active buffer.  if active_buffer has been retrieved, read form disc
  //: caller is in charge of the internal order
  bool get_next(std::vector<Type>& values);
  bool get_next(Type& value);


  // ================ IO ============================

  bool initialize_read(std::string const& filename);
  bool initialize_write(std::string const& filename);
  bool finalize();

 private:
  //: size of an volm_conf_object
  static const unsigned unit_size_ = sizeof(Type);

  //: current mode
  mode m_;

  //: memory chunk allocated for data
  Type* active_buffer_;

  //: vector to store number of configurational objects in each location
  std::vector<int> length_vec_;

  //: maximum number of configurational objects allowed in defined buffer
  unsigned buffer_size_;

  //: id on current active buffer
  unsigned current_id_;

  //: number of objects in active buffer
  unsigned global_current_id_;

  //: ids for recording reading position
  unsigned read_in_;
  int  prev_length_;
  //: binary filename
  std::string file_name_;

  //: file stream
  vsl_b_ofstream *ofs_ptr_;
  vsl_b_ifstream *ifs_ptr_;

  //: write activate buffer to disk
  bool write_to_disk();

  //: read from disk
  bool read_to_buffer();

};

#define VOLM_CONF_BUFFER_INSTANTIATE(T) extern "Please include volm/conf/volm_conf_buffer.txx instead"

#endif // volm_conf_buffer_h_
