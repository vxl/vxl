#include <iostream>
#include <cstdlib>
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <mvl/mvl_multi_view_matches.h>
#include <vnl/vnl_sample.h>

static const int W = -1;

int my_views[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

int track1[] = {1 ,2 ,3 ,4 ,W ,W ,W ,W ,W ,W};
int track2[] = {W ,W ,W ,W ,W ,W ,7 ,8 ,9 ,10};
int track3[] = {W ,W ,W ,4 ,5 ,6 ,W ,W ,9 ,W};
int track4[] = {W ,20,3, W, W, W, W, W, W, W};

void make_match(int* track, std::vector<int>& views, std::vector<int>& indices)
{
  views.clear();
  indices.clear();
  for (int i=0; i < 10; ++i)
    if (track[i] != W) {
      views.push_back(my_views[i]);
      indices.push_back(track[i]);
    }
}

int main ()
{
  // Test the insert and consistency check operations
  {
    std::vector<int> view_indices;
    for (int my_view : my_views)
      view_indices.push_back(my_view);

    mvl_multi_view_matches mvm(view_indices);

    std::vector<int> v;
    std::vector<int> indx;

    // add track1
    std::cerr << "Adding Track 1\n";
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    std::cerr << mvm << std::endl;
    // add track2 (no overlap with track 1)
    std::cerr << "Adding Track 2\n";
    make_match(track2, v, indx);
    mvm.add_track(v, indx);
    std::cerr << mvm << std::endl;
    // add track3 (overlaps 1 and 2, all merged into track 1)
    std::cerr << "Adding Track 3\n";
    make_match(track3, v, indx);
    mvm.add_track(v, indx);
    std::cerr << mvm << std::endl;
    // add track1 again
    std::cerr << "Adding Track 1\n";
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    std::cerr << mvm << std::endl;
    // add track4 (inconsistent, all deleted)
    std::cerr << "Adding Track 4\n";
    make_match(track4, v, indx);
    mvm.add_track(v, indx);
    std::cerr << mvm << std::endl;
  }
  // Now for the speed test
  {
    std::cerr << "Setting up 500 view multi_view_matches...\n";
    mvl_multi_view_matches mvm(500);
    std::cerr << "Adding 50000 random pair-wise matches...\n";

    int f[50000];
    int i1[50000];
    int i2[50000];

    for (int i=0; i < 50000; ++i) {
      f[i] = (int)(498*vnl_sample_uniform01());
      i1[i] = (int)(299*vnl_sample_uniform01());
      i2[i] = (int)(299*vnl_sample_uniform01());
    }
    vul_timer timer;
    for (int i=0; i < 50000; ++i) {
      mvm.add_pair(f[i], i1[i], f[i]+1, i2[i]);
    }
    std::cerr << timer.user()  << " msec\n"
             << mvm.num_tracks() << " distinct tracks\n";
  }
  return 0;
}
