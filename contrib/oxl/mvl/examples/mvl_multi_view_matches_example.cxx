#include <vcl/vcl_fstream.h>
#include <vbl/vbl_timer.h>
#include <mvl/mvl_multi_view_matches.h>

#define W -1

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
    cerr << "Adding Track 1" << endl;
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    cerr << mvm << endl;
    // add track2 (no overlap with track 1)
    cerr << "Adding Track 2" << endl;
    make_match(track2, v, indx);
    mvm.add_track(v, indx);
    cerr << mvm << endl;
    // add track3 (overlaps 1 and 2, all merged into track 1)
    cerr << "Adding Track 3" << endl;
    make_match(track3, v, indx);
    mvm.add_track(v, indx);
    cerr << mvm << endl;
    // add track1 again
    cerr << "Adding Track 1" << endl;
    make_match(track1, v, indx);
    mvm.add_track(v, indx);
    cerr << mvm << endl;
    // add track4 (inconsistent, all deleted)
    cerr << "Adding Track 4" << endl;
    make_match(track4, v, indx);
    mvm.add_track(v, indx);
    cerr << mvm << endl;
  }
  // Now for the speed test
  {
    
    cerr << "Setting up 500 view multi_view_matches..." << endl;    
    mvl_multi_view_matches mvm(500);
    cerr << "Adding 50000 random pair-wise matches..." << endl;
    
    int f[50000];
    int i1[50000];
    int i2[50000];

    for (int i=0; i < 50000; ++i) {
      f[i] = (int)(498*drand48());
      i1[i] = (int)(299*drand48());
      i2[i] = (int)(299*drand48());
    }
    vbl_timer timer;
    for (int i=0; i < 50000; ++i) {
      mvm.add_pair(f[i], i1[i], f[i]+1, i2[i]);
    }
    cerr << timer.user()  << " msec" << endl;
    cerr << mvm.num_tracks() << " distinct tracks" << endl;
  }
}
