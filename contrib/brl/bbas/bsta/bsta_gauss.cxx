#include <vcl_cmath.h> // for log(), exp() ..
#include <bsta/bsta_gauss.h>

double bsta_gauss::bsta_gaussian(const double x, const double sigma)
{
  double x_on_sigma = x / sigma;
  return (double)vcl_exp(- x_on_sigma * x_on_sigma / 2);
}

//:generate a 1-d Gaussian kernel  fuzz=0.02 is a good value
void bsta_gauss::bsta_1d_gaussian_kernel(const double sigma,
                                    const double fuzz,
                                    int& radius,
                                    vcl_vector<double>& kernel)
{
  for (radius = 0; bsta_gaussian(double(radius), sigma) > fuzz; radius++)
  {;}                                         // find radius

  kernel.resize(2*radius + 1);
  if (!radius)
  {
    kernel[0]=1;
    return;
  }
  for (int i=0; i<=radius; ++i)
    kernel[radius+i] = kernel[radius-i] = bsta_gaussian(double(i), sigma);
  double sum = 0;
  for (int i= 0; i <= 2*radius; ++i)
    sum += kernel[i];                           // find integral of weights
  for (int i= 0; i <= 2*radius; ++i)
    kernel[i] /= sum;                           // normalize by integral
}

// convolve a 1-d array with a Gaussian kernel.  Handle the borders by
// setting the kernel to zero outside the data array.  Adjust the output
// to obtain unit norm, i.e, normalize by the sum of the weights.
//
void bsta_gauss::bsta_1d_gaussian(const double sigma,
                                  vcl_vector<double> const& in_buf,
                                  vcl_vector<double>& out_buf)
{
  int n = in_buf.size(), r = 0;
  if (!n)
    return;
  out_buf.resize(n);
  if (n==1)
  {
    out_buf[0]=in_buf[0];
    return;
  }
  //the general case
  vcl_vector<double> ker;
  bsta_1d_gaussian_kernel(sigma, 0.02, r, ker);
  for (int i = 0; i<n; i++)
  {
    double sum = 0;
    //case a)
    //the full kernel is applied
    if (i>r&&((n-1)-i>=r))
    {
      for (int k = -r; k<=r; k++)
        sum += ker[k+r]*in_buf[i+k];
      out_buf[i]=sum;
      continue;
    }
    //case b)
    // full kernel can't be used
    int r_minus = i;
    if (r_minus>r)
      r_minus=r;
    int r_plus = (n-1)-i;
    if (r_plus>r)
      r_plus=r;
    double ker_sum =0;
    for (int k = -r_minus; k<=r_plus; k++)
    {
      ker_sum += ker[k+r];
      sum += ker[k+r]*in_buf[i+k];
    }
    out_buf[i]=sum/ker_sum;
  }
}

//convolve a 2-d array with a Gaussian kernel.  Since the Gaussian is
//separable, first convolve along cols and then along rows
void bsta_gauss::bsta_2d_gaussian(const double sigma,
                                  vbl_array_2d<double> const& in_buf,
                                  vbl_array_2d<double>& out_buf)
{
  int n = in_buf.cols(), m = in_buf.rows();
  out_buf.resize(m, n);

  //convolve columns
  for (int row = 0; row<m; row++)
  {
    vcl_vector<double> row_buf(n), temp;
    for (int col = 0; col<n; col++)
      row_buf[col]=in_buf[row][col];
    bsta_1d_gaussian(sigma, row_buf, temp);
    for (int col = 0; col<n; col++)
      out_buf[row][col]=temp[col];
  }
  //convolve rows
  for (int col = 0; col<n; col++)
  {
    vcl_vector<double> col_buf(m), temp;
    for (int row = 0; row<m; row++)
      col_buf[row]=out_buf[row][col];
    bsta_1d_gaussian(sigma, col_buf, temp);
    for (int row = 0; row<m; row++)
      out_buf[row][col]=temp[row];
  }
}
