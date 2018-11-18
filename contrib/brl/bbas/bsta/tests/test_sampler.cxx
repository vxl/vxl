#include <string>
#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bsta/bsta_sampler.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_sampler_helper(void)
{
  std::vector<unsigned> inp;
  std::vector<float> inp_p;
  vnl_random rand;
  float sum = 0.0f;
  for (unsigned i = 0; i < 4; i++) {
    inp.push_back(i);
    inp_p.push_back((float)rand.drand32(0,1-sum));
    sum += inp_p[i];
  }
  inp.push_back(4);
  inp_p.push_back(1-sum);

  std::vector<unsigned> out;
  bool done = bsta_sampler<unsigned>::sample(inp, inp_p, 50, out);
  TEST("sampler works", done, true);
  std::cout << " samples:\n";
  for (unsigned i = 0; i < inp.size(); i++) {
    std::cout << inp[i] << ' ' << inp_p[i] << std::endl;
  }

  std::cout << "\n selected:\n";
  for (unsigned int i : out) {
    std::cout << i << std::endl;
  }
}

void test_sampler_helper2(void)
{
  std::vector<unsigned> inp;
  std::vector<float> inp_p;
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

  std::vector<unsigned> out;
  bool done = bsta_sampler<unsigned>::sample(inp, inp_p, 50, out);
  TEST("sampler works", done, true);
  std::cout << " samples:\n";
  for (unsigned i = 0; i < inp.size(); i++) {
    std::cout << inp[i] << ' ' << inp_p[i] << std::endl;
  }

  std::cout << "\n selected:\n";
  for (unsigned int i : out) {
    std::cout << i << std::endl;
  }
}


static void test_sampler()
{
  test_sampler_helper();
  test_sampler_helper2();
}

TESTMAIN(test_sampler);
