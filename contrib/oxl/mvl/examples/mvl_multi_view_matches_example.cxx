#include <iostream>
#include <cstdlib>
#include <vxl_config.h>
#include <vcl_compiler.h>
#include <vul/vul_timer.h>
#include <mvl/mvl_multi_view_matches.h>

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
    for (int i=0; i < 10; ++i)
      view_indices.push_back(my_views[i]);

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

#if !VXL_STDLIB_HAS_DRAND48
    int mvl_sample_seed = 12345;
#endif

    for (int i=0; i < 50000; ++i) {
#if VXL_STDLIB_HAS_DRAND48
      f[i] = (int)(498*drand48());
      i1[i] = (int)(299*drand48());
      i2[i] = (int)(299*drand48());
#else
      mvl_sample_seed = (mvl_sample_seed*16807)%2147483647L;
      double u = double(mvl_sample_seed)/2147483711UL;
      f[i] = (int)(498*u);
      mvl_sample_seed = (mvl_sample_seed*16807)%2147483647L;
      u = double(mvl_sample_seed)/2147483711UL;
      i1[i] = (int)(299*u);
      mvl_sample_seed = (mvl_sample_seed*16807)%2147483647L;
      u = double(mvl_sample_seed)/2147483711UL;
      i2[i] = (int)(299*u);
#endif
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
