
#include <vcl/vcl_vector.h>
#include <vbl/vbl_timer.h>

double vnl_fastops_dot(const double* a, const double* b, int n);

#ifdef OPTIMIZED
int optimized = 1;
#else
int optimized = 0;
#endif

main()
{
  vcl_vector<double> x(10000);
  for(int i = 0; i < 10000; ++i)
    x[i] = sqrt(i);
  
  vbl_timer t;
  for(int n = 0; n < 2000; ++n)
    vnl_fastops_dot(x.begin(),x.begin(),x.size());
  cerr << "Method = " << METHOD << ", OPTIMIZE = " << optimized << ", ";
  t.print(cerr);
}

double vnl_fastops_dot(const double* a, const double* b, int n)
{
  double accum = 0;
#if METHOD == 1
  const double* aend = a + n;
  while (a != aend)
    accum += *a++ * *b++;
#endif
#if METHOD == 2
  for(int k = 0; k < n; ++k)
    accum += a[k] * b[k];
#endif
#if METHOD == 3
  while(n--)
    accum += a[n] * b[n];
#endif
  return accum;
}
