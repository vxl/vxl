#include <vxl_config.h>
#include <vul/vul_timer.h>
#include <mvl/mvl_multi_view_matches.h>
#include <vcl_iostream.h>

static const int W = -1;

int my_views[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

int track1[] = {1 ,2 ,3 ,4 ,W ,W ,W ,W ,W ,W};
int track2[] = {W ,W ,W ,W ,W ,W ,7 ,8 ,9 ,10};
int track3[] = {W ,W ,W ,4 ,5 ,6 ,W ,W ,9 ,W};
int track4[] = {W ,20,3, W, W, W, W, W, W, W};

void make_match(int* track, vcl_vector<int>& views, vcl_vector<int>& indices)
{
  views.clear();
  indices.clear();
  for (int i=0; i < 10; ++i) {
    if (track[i] != W) {
      views.push_back(my_views[i]);
      indices.push_back(track[i]);
    }
  }
}

int main (int argc, char** argv)
{
  // Test the insert and consistency check operations
  {
    vcl_vector<int> view_indices;
    for (int i=0; i < 10; ++i)
      view_indices.push_back(my_views[i]);

    mvl_multi_view_matches mvm(view_indices);

    vcl_vector<int> v;
    vcl_vector<int> indx;

    // add track1
    vcl_cerr << "Adding Track 1" << vcl_endl;
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    vcl_cerr << mvm << vcl_endl;
    // add track2 (no overlap with track 1)
    vcl_cerr << "Adding Track 2" << vcl_endl;
    make_match(track2, v, indx);
    mvm.add_track(v, indx);
    vcl_cerr << mvm << vcl_endl;
    // add track3 (overlaps 1 and 2, all merged into track 1)
    vcl_cerr << "Adding Track 3" << vcl_endl;
    make_match(track3, v, indx);
    mvm.add_track(v, indx);
    vcl_cerr << mvm << vcl_endl;
    // add track1 again
    vcl_cerr << "Adding Track 1" << vcl_endl;
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    vcl_cerr << mvm << vcl_endl;
    // add track4 (inconsistent, all deleted)
    vcl_cerr << "Adding Track 4" << vcl_endl;
    make_match(track4, v, indx);
    mvm.add_track(v, indx);
    vcl_cerr << mvm << vcl_endl;
  }
  // Now for the speed test
  {
    vcl_cerr << "Setting up 500 view multi_view_matches..." << vcl_endl;
    mvl_multi_view_matches mvm(500);
    vcl_cerr << "Adding 50000 random pair-wise matches..." << vcl_endl;

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
    vcl_cerr << timer.user()  << " msec" << vcl_endl;
    vcl_cerr << mvm.num_tracks() << " distinct tracks" << vcl_endl;
  }
  return 0;
}
