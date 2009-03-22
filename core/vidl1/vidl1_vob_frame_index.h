// This is core/vidl/vidl_vob_frame_index.h
#ifndef vidl_vob_frame_index_h_
#define vidl_vob_frame_index_h_
//
// this file has been copied from oxp/oxl
// author:  AWF
// copied by l.e.galup
// 10-18-02
//
//
#include <vcl_vector.h>
#include <vcl_string.h>

struct vidl_vob_frame_index_entry {
  int lba;
  int frame;
};
 
struct vidl_vob_frame_index
{
  vcl_vector<vidl_vob_frame_index_entry> l;

  bool load(vcl_string const& filename);
  int frame_to_lba_of_prev_I_frame(int frame_number, int* f_actual = 0);
};

#endif // vidl_vob_frame_index_h_
