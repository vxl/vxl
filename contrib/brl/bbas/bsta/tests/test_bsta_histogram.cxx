//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <vcl_iostream.h>

//: Test bsta histograms
void test_bsta_histogram()
{

  double range = 128;
  int bins = 16;
  double delta = range/bins;
  bsta_histogram<double> h(range, bins);
  double v = 0;
  for (int b =0; b<bins; b++, v+=delta)
    h.upcount(v, 1.0);
  vcl_cout << "Bins\n";
  h.print();
  double area = h.area();
  double fraction_below = h.fraction_below(33.0);
  double fraction_above = h.fraction_above(96.0);
  double value_below = h.value_with_area_below(0.25);
  double value_above = h.value_with_area_above(0.25);

  vcl_cout << "area " << area 
           << " should be "  << 16 << '\n';

  vcl_cout << "fraction_below " << fraction_below 
           << " should be "  << 0.25 << '\n';

  vcl_cout << "fraction_above " << fraction_above 
           << " should be "  << 0.25 << '\n';

  vcl_cout << "value_below " << value_below 
           << " should be "  << 32.0 << '\n';

  vcl_cout << "value_above " << value_above 
           << " should be "  << 96.0 << '\n';

  TEST("test area and percentile methods", area==16&&
       fraction_below == 0.25&&
       fraction_above == 0.25&&
       value_below == 32 &&
       value_above == 96, true);

  //Test data constuctor
  vcl_vector<double> data(16, 1.0);
  bsta_histogram<double> hdata(0, 128, data);  
  hdata.upcount(32,1);
  vcl_cout << "Bins\n";
  hdata.print();
  vcl_cout << "p(32.0) " << hdata.p(32.0) << " should be " <<  0.117647 << '\n';
  TEST_NEAR("test data constructor", hdata.p(32.0), 0.117647, 1e-6);

  //Test entropy
  double ent = h.entropy();
  vcl_cout << "Uniform Entropy for " << bins << " bins = " << ent  << " bits\n";
  TEST_NEAR("test histogram uniform distribution entropy", ent, 4, 1e-9);


  //Joint Histogram Tests
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

  TEST_NEAR("test joint histogram uniform distribution entropy", jent, 8, 1e-9);
}

TESTMAIN(test_bsta_histogram);
