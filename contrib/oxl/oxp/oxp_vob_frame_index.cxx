// This is oxl/oxp/oxp_vob_frame_index.cxx
#include "oxp_vob_frame_index.h"
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_fstream.h>
#include <vul/vul_awk.h>

bool oxp_vob_frame_index::load(char const* filename)
{
  vcl_vector<oxp_vob_frame_index_entry> tmp;

  vcl_ifstream f(filename, vcl_ios_binary);
  if (!f.good()) {
    vcl_cerr << "oxp_vob_frame_index: Cannot read IDX file ["<< filename <<"]\n";
    return false;
  }
  for (vul_awk awk(f); awk; ++awk) {
    // Skip comment and ----- lines
    oxp_vob_frame_index_entry e;
    if (vcl_sscanf(awk.line(), " %x | %d", &e.lba, &e.frame) == 2)
      tmp.push_back(e);
  }
  l = tmp;

  // assert that l is sorted by frame
  for (unsigned int i = 0; i+1 < l.size(); ++i)
    assert(l[i+1].frame > l[i].frame);
  vcl_fprintf(stderr, "Loaded %ld entries from [%s]\n", l.size(), filename);
  if (l.size() == 0) {
    vcl_fprintf(stderr, "WARNING: No index entries -- all seeks from start\n");
  }
  return true;
}

int oxp_vob_frame_index::frame_to_lba_of_prev_I_frame(int f, int* f_actual)
{
  int lo = 0;
  int hi = l.size()-1;
  if (f < l[lo].frame || f > l[hi].frame) {
    vcl_cerr << "urk: frame " << f << " out of IDX range\n";
    return -1;
  }
  while (lo < hi-1) {
    int half = (lo + hi) / 2;
    int f_half = l[half].frame;
    if (f < f_half)
      hi = half;
    else if (f > f_half)
      lo = half;
    else {
      lo = half;
      break;
    }
  }
  // vcl_fprintf(stderr, "oxp_vob_frame_index: [%5d %5d] -> [%5d %5d]\n", lo, hi, l[lo].frame, l[hi].frame);
  if (f_actual)
    *f_actual = l[lo].frame;
  return l[lo].lba;
}
