//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <vcl_iostream.h>
#if 0
#include <vcl_cstdlib.h> // for rand()
#endif

//: Test bsta histograms
void test_bsta_histogram()
{
#if 0
  int nbins = 256, trials = 100;
  double range = 100;
  for (int n =1; n<=1000; n++)
  {
    //      vcl_cout << "Nsamp = " << n << '\n';
    double hsum = 0, rsum = 0, hsumsq = 0, rsumsq = 0;
    for (int t=1; t<=trials; t++)
    {
      bsta_histogram<double> h(range, nbins);
      for (int i = 0; i<n; i++)
      {
        double span = range/5;
        double v = span*(vcl_rand()/(RAND_MAX+1.0));
        h.upcount(v, 1.0);
      }
      double ent = h.entropy(), rent = h.renyi_entropy();
      hsum += ent;
      rsum += rent;
      hsumsq += ent*ent;
      rsumsq += rent*rent;
#ifdef DEBUG
      vcl_cout << "Trial " << t << '\n';
      h.print();
      vcl_cout << '\n' << h.entropy() << '\t' << h.renyi_entropy() << '\n';
#endif
    }
    double hsd = (hsumsq/trials)-(hsum/trials)*(hsum/trials);
    double rsd = (rsumsq/trials)-(rsum/trials)*(rsum/trials);
    vcl_cout << n << '\t' << hsum/trials  << '\t'
             << rsum/trials <<  '\t' << hsd <<  '\t' << rsd << '\n';
  }
#endif
  double range = 128;
  int bins = 16;
  double delta = range/bins;
  bsta_histogram<double> h(range, bins);
  double v = 0;
  for (int b =0; b<bins; b++, v+=delta)
    h.upcount(v, 1.0);
  vcl_cout << "Bins\n";
  h.print();
  double ent = h.entropy();
  vcl_cout << "Uniform Entropy for " << bins << " bins = " << ent  << " bits\n";
  TEST("test histogram uniform distribution entropy", ent==4, true);

  bsta_joint_histogram<double> jh(range, bins);
  double va = 0;
  for (int a =0; a<bins; a++, va+=delta)
  {
    double vb = 0;
    for (int b =0; b<bins; b++, vb+=delta)
      jh.upcount(va, 1.0, vb, 1.0);
  }
  double jent = jh.entropy();
  vcl_cout << "Uniform Joint Entropy for " << bins*bins << " bins = "
           << jent  << " bits\n";
  TEST_NEAR("test joint histogram uniform distribution entropy", jent,8, 0.001);
}

TESTMAIN(test_bsta_histogram);
