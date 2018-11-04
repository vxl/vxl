// This is core/vnl/vnl_sample.cxx

#include <cmath>
#include <ctime>
#include <random>

#include "vnl_sample.h"
#include <cstdlib>
#include <vnl/vnl_math.h>

static std::random_device private_vnl_sample_rd;  //Will be used to obtain a seed for the random number engine

#if 1 // Use linear_congruential_engine similar to drand48
//minstd_rand	std::linear_congruential_engine<std::uint_fast32_t, 48271, 0, 2147483647>
//Newer "Minimum standard", recommended by Park, Miller, and Stockmeyer in 1993
static std::minstd_rand private_vnl_sample_gen(private_vnl_sample_rd()); //
#else
//Standard mersenne_twister_engine seeded with rd()
static std::mt19937 private_vnl_sample_gen(private_vnl_sample_rd());
#endif

void vnl_sample_reseed()
{
	private_vnl_sample_gen.seed();
}

void vnl_sample_reseed(int seed)
{
	private_vnl_sample_gen.seed(seed);
}

double vnl_sample_uniform01()
{
	return std::generate_canonical<double, 48>(private_vnl_sample_gen); // uniform on [0, 1)
}

double vnl_sample_uniform(double a, double b)
{
  const double u = std::generate_canonical<double, 48>(private_vnl_sample_gen); // uniform on [0, 1)
  return (1.0 - u)*a + u*b;
}

void vnl_sample_normal_2(double *x, double *y)
{
  double u     = vnl_sample_uniform(1, 0); // not (0,1): should not return 0
  double theta = vnl_sample_uniform(0, vnl_math::twopi);

  double r = std::sqrt(-2*std::log(u));

  if (x) *x = r * std::cos(theta);
  if (y) *y = r * std::sin(theta);
}

double vnl_sample_normal(double mean, double sigma)
{
  double x;
  vnl_sample_normal_2(&x, nullptr);
  return mean + sigma * x;
}

// Implementation of Bernoulli sampling by Peter Vanroose
int vnl_sample_bernoulli(double q)
{
  // quick return if possible:
  if (q==0.0) return 0;
  if (q==1.0) return 1;
  if (q<0.0 || q>1.0) return -1;
  // q should be the probability of returning 0:
  return (vnl_sample_uniform(0.0, 1.0/q) >= 1.0) ? 1 : 0;
}

// Implementation of binomial sampling by Peter Vanroose
int vnl_sample_binomial(int n, double q)
{
  // Returns a random "k" value, between 0 and n, viz. the sum of n random
  // and independent drawings from a Bernoulli distribution with parameter q.

  if (n <= 0 || q<0.0 || q>1.0) return -1; // That is: when the input makes no sense, return nonsense "-1".
  if (q==0.0) return 0;
  if (q==1.0) return n;
  int k = 0;
  for (int i=n-1; i>=0; --i) {
    k += vnl_sample_bernoulli(q);
  }
  return k;
}
