#ifndef oxp_vob_frame_index_h_
#define oxp_vob_frame_index_h_

#include <vcl_vector.h>

struct oxp_vob_frame_index_entry {
  int lba;
  int frame;
};
 
struct oxp_vob_frame_index
{
  vcl_vector<oxp_vob_frame_index_entry> l;

  void add(int lba, int frame) { 
    oxp_vob_frame_index_entry t;
    t.lba = lba;
    t.frame = frame;
    l.push_back(t);
  }
  bool load(char const* filename);
  int frame_to_lba_of_prev_I_frame(int frame_number, int* f_actual = 0);
};

#endif // oxp_vob_frame_index_h_
