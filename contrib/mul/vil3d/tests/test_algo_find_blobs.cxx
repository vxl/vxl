// This is mul/vil3d/tests/test_algo_find_blobs.cxx

#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vil3d/algo/vil3d_find_blobs.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/vil3d_print.h>
#include <vgl/vgl_point_3d.h>


//========================================================================
// Find the first voxel (in raster scan order) of each blob in a blob image
//========================================================================
static void first_voxel_of_blobs(const vil3d_image_view<unsigned>& img,
                                 std::map<unsigned, vgl_point_3d<unsigned> >& blob_vox)
{
  blob_vox.clear();

  for (unsigned k=0; k<img.nk(); ++k)
  {
    for (unsigned j=0; j<img.nj(); ++j)
    {
      for (unsigned i=0; i<img.ni(); ++i)
      {
        unsigned label = img(i,j,k);
        if (blob_vox.find(label)==blob_vox.end())
        {
          blob_vox[label] = vgl_point_3d<unsigned>(i,j,k);
        }
      }
    }
  }
}


//========================================================================
// Find blobs and check against true answer
//========================================================================
static void test_blob_image(const vil3d_image_view<bool>& src,
                            const vil3d_find_blob_connectivity conn,
                            const vil3d_image_view<unsigned>& tru,
                            const std::string& testname)
{
  vil3d_image_view<unsigned> dst;
  vil3d_find_blobs(src, conn, dst);

  // Check first voxels
  std::map<unsigned, vgl_point_3d<unsigned> > blob_vox;
  first_voxel_of_blobs(src, blob_vox);


  // NB FIX THIS: labels need not match but must have 1:1 relationship...
  TEST(testname.c_str(), vil3d_image_view_deep_equality(dst,tru), true);

#if 0
//#ifndef NDEBUG
  std::cout << "--------------------------------------------------\n"
           << testname << "\n\n";

  std::cout << "src image:\n";
  vil3d_print_all(std::cout, src);
  std::cout << '\n';

  std::cout << "dst image:\n";
  vil3d_print_all(std::cout, dst);
  std::cout << '\n';

  std::cout << "tru image:\n";
  vil3d_print_all(std::cout, tru);
  std::cout << '\n';

  std::cout << "first voxels:\n";
  for (std::map<unsigned, vgl_point_3d<unsigned> >::const_iterator it=blob_vox.begin();
       it!=blob_vox.end(); ++it)
  {
    std::cout << it->first << ": "
             << it->second.x() << ' '
             << it->second.y() << ' '
             << it->second.z() << '\n';
  }
  std::cout << '\n';

  std::cout << "--------------------------------------------------\n";
//#endif // NDEBUG
#endif
}


//========================================================================
// All-background image
//========================================================================
static void test_all_background()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn, tru,
                  "Test all-background image");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru,
                  "Test all-background image");
}


//========================================================================
// All-foreground image
//========================================================================
static void test_all_foreground()
{
  const unsigned ni=3, nj=4, nk=5, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(true);

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(1);

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn, tru,
                  "Test all-foreground image");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru,
                  "Test all-foreground image");
}


//========================================================================
// Single central blob
//========================================================================
static void test_single_central()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_crop(src, 3,3, 3,4, 3,5).fill(true);
  src(6,5,4)=true; src(4,7,5)=true; src(5,6,8)=true;

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  vil3d_crop(tru, 3,3, 3,4, 3,5).fill(1);
  tru(6,5,4)=1; tru(4,7,5)=1; tru(5,6,8)=1;

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn,  tru, "Test single central blob");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru, "Test single central blob");
}


//========================================================================
// Single blob at image edge
//========================================================================
static void test_single_edge()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_crop(src, 0,3, 3,4, 3,5).fill(true);

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  vil3d_crop(tru, 0,3, 3,4, 3,5).fill(1);

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn,  tru, "Test single edge blob");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru, "Test single edge blob");
}


//========================================================================
// Multiple separate blobs
// Design note: these blobs are all separate (even with 26-conn) in 3D;
// however, they appear to overlap in 2 of the 3 axes.
//========================================================================
static void test_multiple_separate()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_crop(src, 1,3, 1,4, 1,5).fill(true);
  vil3d_crop(src, 6,3, 2,4, 2,4).fill(true);
  vil3d_crop(src, 2,4, 7,3, 3,3).fill(true);
  vil3d_crop(src, 2,5, 2,5, 7,3).fill(true);

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  vil3d_crop(tru, 1,3, 1,4, 1,5).fill(1);
  vil3d_crop(tru, 6,3, 2,4, 2,4).fill(2);
  vil3d_crop(tru, 2,4, 7,3, 3,3).fill(3);
  vil3d_crop(tru, 2,5, 2,5, 7,3).fill(4);

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn,  tru, "Test multiple separate blobs");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru, "Test multiple separate blobs");
}

//========================================================================
// two blobs, with one that will invoke the connected merging code.
//========================================================================
static void test_merging_blocks()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_crop(src, 1,7, 1,7, 1,7).fill(true);
  vil3d_crop(src, 1,6, 1,6, 1,6).fill(false);
  vil3d_crop(src, 1,3, 1,3, 1,3).fill(true);

  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  vil3d_crop(tru, 1,7, 1,7, 1,7).fill(2);
  vil3d_crop(tru, 1,6, 1,6, 1,6).fill(0);
  vil3d_crop(tru, 1,3, 1,3, 1,3).fill(1);

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn,  tru, "Test simple merging case");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru, "Test simple merging case");
}


//========================================================================
// One diagonal line that exercises the merging code.
//========================================================================
static void test_merging_diagonal_line()
{
  const unsigned ni=9, nj=10, nk=11, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  for (unsigned i=0; i<ni; ++i)
  {
    src(ni-i-1, i, i) = true;
    tru(ni-i-1, i, i) = 1;
  }

  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru,
    "Test merging diagonal line");
}

//=============================================================================
// lots of limbs in a single blob that exercises the merging code.
// Check that renumber also makes label set compact.
//=============================================================================
static void test_multilimb_merging()
{
  const unsigned ni=9, nj=9, nk=9, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  for (unsigned i=0; i<ni-1; i+=2)
  {
    for (unsigned k=0; k<nk-1; k+=2)
    {
      src(i,  1, k  )=true;
      src(i,  2, k  )=true;
      src(i+1,2, k  )=true;
      src(i,  2, k+1)=true;
      src(i+1,2, k+1)=true;
      tru(i,  1, k  )=1;
      tru(i,  2, k  )=1;
      tru(i+1,2, k  )=1;
      tru(i,  2, k+1)=1;
      tru(i+1,2, k+1)=1;
    }
  }

  src(5, 5, 5)=true;
  tru(5, 5, 5)=2;

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn, tru,
                  "Test multilimb blob merging");
}

//=============================================================================
// lots of long awkward limbs in a single blob that exercises the merging code.
// Check that renumber also makes label set compact.
//=============================================================================
static void test_multilimb_merging2()
{
  const unsigned ni=9, nj=9, nk=9, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  for (unsigned i=1; i<ni-2; i+=2)
  {
    vil3d_crop(src, 1,ni-i-1, 2,1, i,1).fill(true);
    vil3d_crop(tru, 1,ni-i-1, 2,1, i,1).fill(1);
    src(ni-i-1, 2, i+1)=true;
    src(ni-i-1, 2, i+2)=true;
    src(ni-i-2, 2, i+2)=true;
    tru(ni-i-1, 2, i+1)=1;
    tru(ni-i-1, 2, i+2)=1;
    tru(ni-i-2, 2, i+2)=1;
  }

  src(5, 4, 5)=true;
  tru(5, 4, 5)=2;

  test_blob_image(src, vil3d_find_blob_connectivity_6_conn, tru,
                  "Test multilimb blob merging2");
  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru,
                  "Test multilimb blob merging2");
}


//=============================================================================
// lots of long awkward limbs in a single blob that exercises the merging code.
// Check that renumber also makes label set compact.
//=============================================================================
static void test_multilimb_merging3()
{
  const unsigned ni=9, nj=9, nk=9, np=1;

  vil3d_image_view<bool> src(ni, nj, nk, np);
  src.fill(false);
  vil3d_image_view<unsigned> tru(ni, nj, nk, np);
  tru.fill(0);
  for (unsigned i=1; i<ni-2; i+=2)
  {
    vil3d_crop(src, 1,ni-i-1, 2,1, i,1).fill(true);
    vil3d_crop(tru, 1,ni-i-1, 2,1, i,1).fill(1);
    src(ni-i,   2, i+1)=true;
    src(ni-i-1, 2, i+2)=true;
    src(ni-i-2, 2, i+2)=true;
    tru(ni-i,   2, i+1)=1;
    tru(ni-i-1, 2, i+2)=1;
    tru(ni-i-2, 2, i+2)=1;
  }

  src(5, 4, 5)=true;
  tru(5, 4, 5)=2;

  test_blob_image(src, vil3d_find_blob_connectivity_26_conn, tru,
                  "Test multilimb blob merging3");
}

//========================================================================
// Main function
//========================================================================
static void test_algo_find_blobs()
{
  std::cout << "**************************\n"
           << " Testing vil3d_find_blobs\n"
           << "**************************\n";

  test_all_background();
  test_all_foreground();
  test_single_central();
  test_single_edge();
  test_multiple_separate();
  test_merging_blocks();
  test_merging_diagonal_line();
  test_multilimb_merging();
  test_multilimb_merging2();
  test_multilimb_merging3();


  // Multiple blobs with diagonal connectivity

  // Multiple nested blobs

  // Test multi-plane image - no need for this, blob-finder only claims to handle first plane
}


//========================================================================
// test macro
//========================================================================
TESTMAIN(test_algo_find_blobs);
