// This is mul/mbl/tests/test_correspond_points.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_correspond_points.h>
#include <vnl/vnl_random.h>
#include <testlib/testlib_test.h>

void test_correspond_points()
{
  vcl_cout << "*******************************\n"
           << " Testing mbl_correspond_points\n"
           << "*******************************\n";

  mbl_correspond_points cp;
  vnl_random mz_random(325746);

  unsigned n1 = 10;
  vcl_vector<vgl_point_2d<double> > points1(n1),points2(n1),points3(n1),points4(n1),points5(n1);

  for (unsigned i=0;i<n1;++i)
  {
    double x = mz_random.drand64(), y = mz_random.drand64();
    points1[i] = vgl_point_2d<double>(x,y);
    points2[i] = vgl_point_2d<double>(x+0.12,y-0.34);
    points3[i] = vgl_point_2d<double>(1.2*x+1.0,1.3*y-0.7);
    points4[n1-1-i] = points3[i];
    vgl_vector_2d<double> n(mz_random.normal64(),mz_random.normal64());
    points5[i] = points1[i] + 0.005*n;
  }

  vcl_vector<unsigned> matches;
  double sigma = 0.05;
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

  cp.correspond(points1,points5,matches,sigma);
  n_correct = 0;
  for (unsigned i=0;i<matches.size();++i)
  {
    if (matches[i]==i) n_correct++;
  }
  vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl;
  TEST("All matches correct (gaussian noise)",n_correct,n1);
  vcl_cout<<"Evals1: "<<cp.evals1()<<vcl_endl
          <<"Evals2: "<<cp.evals2()<<vcl_endl;

  vcl_cout<<"Add one clutter point and move it around\n";
  points2.push_back(vgl_point_2d<double>(0,0));

  for (unsigned i=0;i<5;++i)
  {
    double x = mz_random.drand64(), y = mz_random.drand64();
    points2[n1] = vgl_point_2d<double>(x+0.12,y-0.34);

    cp.correspond(points1,points2,matches,sigma);
    n_correct = 0;
    for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
    vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl
            <<"Evals1: "<<cp.evals1()<<vcl_endl
            <<"Evals2: "<<cp.evals2()<<vcl_endl;
  }

  vcl_cout<<"Add two clutter points and move them around\n";
  points2.push_back(vgl_point_2d<double>(0,0));

  for (unsigned i=0;i<5;++i)
  {
    double x = mz_random.drand64(), y = mz_random.drand64();
    points2[n1] = vgl_point_2d<double>(x+0.12,y-0.34);
    x = mz_random.drand64(); y = mz_random.drand64();
    points2[n1+1] = vgl_point_2d<double>(x+0.12,y-0.34);

    cp.correspond(points1,points2,matches,sigma);
    n_correct = 0;
    for (unsigned i=0;i<matches.size();++i) if (matches[i]==i) n_correct++;
    vcl_cout<<n_correct<<" out of "<<n1<<vcl_endl
            <<"Evals1: "<<cp.evals1()<<vcl_endl
            <<"Evals2: "<<cp.evals2()<<vcl_endl;
  }

#if 0
  TEST("All matches correct (invariant to translation+scale + clutter)",n_correct,n1);
  // The above test currently fails.
  // We need to re-order the eigenvectors by absolute magnitude to fix it.
#endif // 0
}

TESTMAIN(test_correspond_points);
