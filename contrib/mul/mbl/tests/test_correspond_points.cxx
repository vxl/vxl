// This is mul/mbl/tests/test_correspond_points.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_correspond_points.h>
#include <mbl/mbl_mz_random.h>

void test_correspond_points()
{
  vcl_cout << "********************\n"
           << " Testing mbl_correspond_points\n"
           << "********************\n";

  mbl_correspond_points cp;
  mbl_mz_random mz_random(325746);

  unsigned n1 = 20;
  vcl_vector<vgl_point_2d<double> > points1(n1),points2(n1),points3(n1),points4(n1);

  for (unsigned i=0;i<n1;++i)
  {
    double x = mz_random.drand64(), y = mz_random.drand64();
    points1[i] = vgl_point_2d<double>(x,y);
    points2[i] = vgl_point_2d<double>(x+0.12,y-0.34);
    points3[i] = vgl_point_2d<double>(1.2*x+1.0,1.3*y-0.7);
    points4[n1-1-i] = points3[i];
  }

  vcl_vector<unsigned> matches;
  double sigma = 0.01;
  cp.correspond(points1,points1,matches,sigma);

  unsigned int n_correct = 0;
  for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;

  TEST("All matches correct (pts1-pts1)",n_correct,n1);

  cp.correspond(points1,points2,matches,sigma);

  n_correct = 0;
  for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;

  TEST("All matches correct (invariant to translation)",n_correct,n1);

  cp.correspond(points1,points3,matches,sigma);

  n_correct = 0;
  for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;

  TEST("All matches correct (invariant to translation+scale)",n_correct,n1);

  cp.correspond(points1,points4,matches,sigma);

  n_correct = 0;
  for (unsigned i=0;i<matches.size();++i)
  {
    if (matches[i]==n1-1-i) n_correct++;
  }
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;

  TEST("All matches correct (data reversed)",n_correct,n1);

  // Add some clutter
  for (unsigned i=0;i<5;++i)
  {
    double x = mz_random.drand64(), y = mz_random.drand64();
    points3.push_back(vgl_point_2d<double>(1.2*x+1.0,1.3*y-0.7));
  }

  cp.correspond(points1,points3,matches,sigma);
  n_correct = 0;
  for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;

//  TEST("All matches correct (invariant to translation+scale + clutter)",n_correct,n1);
 // The above test currently fails.
 // We need to re-order the eigenvectors by absolute magnitude to fix it.
}

TESTLIB_DEFINE_MAIN(test_correspond_points);
