#include <testlib/testlib_test.h>
#include <bsta/bsta_sampler.h>
#include <vnl/vnl_random.h>
#include <vcl_string.h>
#include <vcl_limits.h>
#include <vcl_iostream.h>

void test_sampler_helper(void)
{
  vcl_vector<unsigned> inp;
  vcl_vector<float> inp_p;
  vnl_random rand;
  float sum = 0.0f;
  for (unsigned i = 0; i < 4; i++) {
    inp.push_back(i);
    inp_p.push_back((float)rand.drand32(0,1-sum));
    sum += inp_p[i];
  }
  inp.push_back(4);
  inp_p.push_back(1-sum);

  vcl_vector<unsigned> out;
  bool done = bsta_sampler<unsigned>::sample(inp, inp_p, 50, out);
  TEST("sampler works", done, true);
  vcl_cout << " samples:\n";
  for (unsigned i = 0; i < inp.size(); i++) {
    vcl_cout << inp[i] << ' ' << inp_p[i] << vcl_endl;
  }

  vcl_cout << "\n selected:\n";
  for (unsigned i = 0; i < out.size(); i++) {
    vcl_cout << out[i] << vcl_endl;
  }
}

void test_sampler_helper2(void)
{
  vcl_vector<unsigned> inp;
  vcl_vector<float> inp_p;
  inp.push_back(0);
  inp.push_back(1);
  inp.push_back(2);
  inp.push_back(3);
  inp.push_back(4);
  inp.push_back(5);

  inp_p.push_back(0.99f);
  inp_p.push_back(0.0f);
  inp_p.push_back(0.0f);
  inp_p.push_back(0.0f);
  inp_p.push_back(0.0f);
  inp_p.push_back(0.0f);

  vcl_vector<unsigned> out;
  bool done = bsta_sampler<unsigned>::sample(inp, inp_p, 50, out);
  TEST("sampler works", done, true);
  vcl_cout << " samples:\n";
  for (unsigned i = 0; i < inp.size(); i++) {
    vcl_cout << inp[i] << ' ' << inp_p[i] << vcl_endl;
  }

  vcl_cout << "\n selected:\n";
  for (unsigned i = 0; i < out.size(); i++) {
    vcl_cout << out[i] << vcl_endl;
  }
}


static void test_sampler()
{
  test_sampler_helper();
  test_sampler_helper2();
}

TESTMAIN(test_sampler);


