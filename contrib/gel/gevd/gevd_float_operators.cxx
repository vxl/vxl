// This is gel/gevd/gevd_float_operators.cxx
#include "gevd_float_operators.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h> // for pi_over_2
#include <vnl/vnl_vector.h>

#include <vcl_cmath.h>
#include <vcl_cstring.h>

#include "gevd_pixel.h"
#include "gevd_xpixel.h"
#include "gevd_bufferxy.h"
//#include <ImageProcessing/xpixel.h>
#ifdef DEBUG
# include <vul/vul_timer.h>
#endif

#if defined(VCL_VC) || defined(VCL_BORLAND)
inline static double rint(double v)
{
  return  v - vcl_floor(v) < 0.5  ?  vcl_floor(v)  :  vcl_ceil(v);
}
#endif

const unsigned char DIR0 = 8, DIR1 = 9, DIR2 = 10, DIR3 = 11, DIR4 = 12;
// const int DIS[] = { 1, 1, 0,-1,-1,-1, 0, 1, // 8-connected neighbors
//                     1, 1, 0,-1,-1,-1, 0, 1, // wrapped by 2PI
//                     1, 1, 0,-1,-1,-1, 0, 1};
// const int DJS[] = { 0, 1, 1, 1, 0,-1,-1,-1,
//                     0, 1, 1, 1, 0,-1,-1,-1,
//                     0, 1, 1, 1, 0,-1,-1,-1};

//: Convolves from image with 2D filter and stores values in to image.
// O(m*n*k). The filter kernel has odd width and height, and is either
// even or odd along x- or y-axis.
// Assume image data is in row-major order.

float
gevd_float_operators::Convolve(const gevd_bufferxy& from,
                               const gevd_bufferxy& kernel,
                               gevd_bufferxy*& to)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Convolve image";
#endif
  to = gevd_float_operators::Allocate(to, from);
  const int wx = kernel.GetSizeX(), wy = kernel.GetSizeY();
  const int rx = wx/2, ry = wy/2;
  const int xhi = from.GetSizeX() - wx + 1;
  const int yhi = from.GetSizeY() - wy + 1;
  for (int y = 0; y < yhi; y++)
    for (int x = 0; x < xhi; x++) {
      float conv = 0;           // more efficient if use even/odd symmetry
      for (int j = 0; j < wy; j++)
        for (int i = 0; i < wx; i++)
          conv += floatPixel(from, x+i, y+j) * floatPixel(kernel, i, j);
      floatPixel(*to, x+rx, y+ry) = conv;
    }
  FillFrameX(*to, 0, rx);       // pad border with 0
  FillFrameY(*to, 0, ry);
#ifdef DEBUG
  vcl_cout << " in " << t.real() << " msecs.\n";
#endif
  return 1;                     // extra scaling factor
}

//: Correlate from image with 2D filter and stores values in to image.
// O(m*n*k). The filter kernel has odd width and height, and is either
// even or odd along x- or y-axis.
// Assume image data is in row-major order.

float
gevd_float_operators::Correlation(const gevd_bufferxy& from,
                                  const gevd_bufferxy& kernel,
                                  gevd_bufferxy*& to)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Correlate image";
#endif
  to = gevd_float_operators::Allocate(to, from);
  const int wx = kernel.GetSizeX(), wy = kernel.GetSizeY();
  const int rx = wx/2, ry = wy/2;
  const int xhi = from.GetSizeX() - wx + 1;
  const int yhi = from.GetSizeY() - wy + 1;
  const int sum1 = wx*wy;
  for (int y = 0; y < yhi; y++)
    for (int x = 0; x < xhi; x++) {
      register double sumx=0, sumy=0, sumxx=0, sumyy=0, sumxy=0, xval, yval;
      for (int j = 0; j < wy; j++)
        for (int i = 0; i < wx; i++) {
          xval = floatPixel(from, x+i, y+j);
          yval = floatPixel(kernel, i, j);
        sumxy += xval * yval;           // accumulate correlation value
        sumx += xval;
        sumy += yval;
        sumxx += xval * xval;
        sumyy += yval * yval;
      }
      double varx = sum1 * sumxx - sumx * sumx; // all multiplied with sum1
      double vary = sum1 * sumyy - sumy * sumy;
      double cvar = sum1 * sumxy - sumx * sumy; // linear correlation coeft
      if (varx!=0 && vary!=0) cvar /= vcl_sqrt(varx * vary);
      floatPixel(*to, x+rx, y+ry) = (float)cvar;
    }
  FillFrameX(*to, 0, rx);       // pad border with 0
  FillFrameY(*to, 0, ry);
#ifdef DEBUG
  vcl_cout << " in " << t.real() << " msecs.\n";
#endif
  return 1;                     // extra scaling factor
}


float
gevd_float_operators::CorrelationAlongAxis(const gevd_bufferxy& from,
                                           const gevd_bufferxy& kernel,
                                           gevd_bufferxy*& to)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Correlate image";
#endif
  to = gevd_float_operators::Allocate(to, from);
  to->Clear();
  const int wx = kernel.GetSizeX(), wy = kernel.GetSizeY();
  const int rx = wx/2, ry = wy/2;
  const int xhi = from.GetSizeX() - wx + 1;
  const int yhi = from.GetSizeY() - wy + 1;
  const int sum1 = wx*wy;
  for (int x = 0, y = yhi/2; x < xhi; ++x) {
    register double sumx=0, sumy=0, sumxx=0, sumyy=0, sumxy=0, xval, yval;
    for (int j = 0; j < wy; j++)
      for (int i = 0; i < wx; i++) {
        xval = floatPixel(from, x+i, y+j);
        yval = floatPixel(kernel, i, j);
        sumxy += xval * yval;           // accumulate correlation value
        sumx += xval;
        sumy += yval;
        sumxx += xval * xval;
        sumyy += yval * yval;
      }
    double varx = sum1 * sumxx - sumx * sumx; // all multiplied with sum1
    double vary = sum1 * sumyy - sumy * sumy;
    double cvar = sum1 * sumxy - sumx * sumy;   // linear correlation coeft
    if (varx!=0 && vary!=0) cvar /= vcl_sqrt(varx * vary);
    floatPixel(*to, x+rx, y+ry) = (float)cvar;
  }
  for (int x = xhi/2, y = 0; y < yhi; ++y) {
    register double sumx=0, sumy=0, sumxx=0, sumyy=0, sumxy=0, xval, yval;
    for (int j = 0; j < wy; j++)
      for (int i = 0; i < wx; i++) {
        xval = floatPixel(from, x+i, y+j);
        yval = floatPixel(kernel, i, j);
        sumxy += xval * yval;           // accumulate correlation value
        sumx += xval;
        sumy += yval;
        sumxx += xval * xval;
        sumyy += yval * yval;
      }
    double varx = sum1 * sumxx - sumx * sumx; // all multiplied with sum1
    double vary = sum1 * sumyy - sumy * sumy;
    double cvar = sum1 * sumxy - sumx * sumy;   // linear correlation coeft
    if (varx!=0 && vary!=0) cvar /= vcl_sqrt(varx * vary);
    floatPixel(*to, x+rx, y+ry) = (float)cvar;
  }
#ifdef DEBUG
  vcl_cout << " in " << t.real() << " msecs.\n";
#endif
  return 1;                     // extra scaling factor
}


//: Read 2D kernel from a file: width height k_x_y .....

gevd_bufferxy*
gevd_float_operators::Read2dKernel(const char* filename)
{
  vcl_ifstream infile(filename, vcl_ios_in); // open the file
  if (!infile)
    return NULL;
  int width, height;
  infile >> width;
  infile >> height;
  if (width < 1 || height < 1) return NULL;
  gevd_bufferxy* kernel = new gevd_bufferxy(width, height, bits_per_float);
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      infile >> floatPixel(*kernel, x, y);
  return kernel;
}

//: Convolves from image with two separable filters, along directions x and y, and stores values in to image.
// O(m*n*k).
// The filter kernel has length 2*radius + 1, and is either even or odd.
// Assume image data is in row-major order.

float
gevd_float_operators::Convolve(gevd_bufferxy& from, gevd_bufferxy*& to,
                               const float* xkernel, const int xradius,
                               const bool xevenp,
                               const float* ykernel, const int yradius,
                               const bool yevenp,
                               const bool xwrap, const bool ywrap)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Convolve image";
#endif
  to = gevd_float_operators::Allocate(to, from);
  const int sizeX = to->GetSizeX(), sizeY = to->GetSizeY();
  const int ylo = yradius, yhi = sizeY - yradius;
  const int kborder = 2*yradius;

  // 1. Setup the pipeline of 4*yradius+1 lines, convolved along x-axis
  float** cache = new float*[4*yradius+1];
  float** pipeline = cache+yradius;
  float* row;
  for (int p = 0; p <= kborder; ++p) {
    pipeline[p] = row = new float[sizeX];
    gevd_float_operators::Convolve(&floatPixel(from, 0, p), // row-major order
                                   row, sizeX,
                                   xkernel, xradius, xevenp, xwrap);
  }
  if (ywrap)                    // circular wrap at 2 end rows
    for (int r = 1; r <= yradius; r++) {
      pipeline[-r] = row = new float[sizeX];
      gevd_float_operators::Convolve(&floatPixel(from, 0, sizeY-r), // row-major order
                                     row, sizeX,
                                     xkernel, xradius, xevenp, xwrap);
      pipeline[kborder+r] = row = new float[sizeX];
      for (int i = 0; i < sizeX; i++) // copy previous convolved result
        row[i] = pipeline[r-1][i];
    }
  else                          // reflection at 2 end rows
    for (int r = 1; r <= yradius; r++) {
      pipeline[-r] = row = new float[sizeX];
      for (int i = 0; i < sizeX; i++) // copy previous convolved result
        row[i] = pipeline[r][i];
      pipeline[kborder+r] = row = new float[sizeX];
      gevd_float_operators::Convolve(&floatPixel(from, 0, sizeY-1-r), // row-major order
                                     row, sizeX,
                                     xkernel, xradius, xevenp, xwrap);
    }

  // 2. Convolve along y-axis, shifting pipeline by 1 each time.
  if (yevenp)
  {
    int y=0;
    for (int yy = 0; y < ylo; ++y, ++yy) { // reflect/wrap at ymin
      row = &floatPixel(*to, 0, y); // row-major order
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yy][x];
        for (int k = 1; k <= yradius; k++)
          sum += ykernel[yradius-k] * (pipeline[yy-k][x] + pipeline[yy+k][x]);
        row[x] = sum;
      }
    }
    int p = kborder+1;
    for ( ; y < yhi; ++y) {     // convolution along y-axis
      row = &floatPixel(*to, 0, y);
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yradius][x];
        for (int k = 1; k <= yradius; k++)
          sum += ykernel[yradius-k] * (pipeline[yradius-k][x] + // even kernel
                                       pipeline[yradius+k][x]);
        row[x] = sum;
      }
      if (p < sizeY) {
        row = pipeline[0];      // next line
        for (int k = 0; k < kborder; k++) // shift the lines of
          pipeline[k] = pipeline[k+1]; // the pipeline by 1
        gevd_float_operators::Convolve(&floatPixel(from, 0, p++), // row-major order
                                       row, sizeX,
                                       xkernel, xradius, xevenp, xwrap); // new line x-conv
        pipeline[kborder] = row; // update pipeline
      }
    }
    for (int yy = yradius+1; y < sizeY; y++, yy++) {  // reflect/wrap at ymax
      row = &floatPixel(*to, 0, y); // row-major order
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yy][x];
        for (int k = 1; k <= yradius; k++)  // convolution along y-axis
          sum += ykernel[yradius-k] * (pipeline[yy-k][x] + pipeline[yy+k][x]);
        row[x] = sum;
      }
    }
  } else {
    int y=0;
    for (int yy = 0; y < ylo; y++, yy++) { // reflect at ymin
      row = &floatPixel(*to, 0, y); // row-major order
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yy][x];
        for (int k = 1; k <= yradius; k++)
          sum += ykernel[yradius-k] * (pipeline[yy-k][x] - pipeline[yy+k][x]);
        row[x] = sum;
      }
    }
    int p = kborder+1;
    for ( ; y < yhi; y++) {
      row = &floatPixel(*to, 0, y);
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yradius][x];
        for (int k = 1; k <= yradius; k++)  // convolution along y-axis
          sum += ykernel[yradius-k] * (pipeline[yradius-k][x] - // odd kernel
                                       pipeline[yradius+k][x]);
        row[x] = sum;
      }
      if (p < sizeY) {
        row = pipeline[0];      // next line
        for (int k = 0; k < kborder; k++)   // shift the lines of
          pipeline[k] = pipeline[k+1];  // the pipeline by 1
        gevd_float_operators::Convolve(&floatPixel(from, 0, p++), // row-major order
                                       row, sizeX,
                                       xkernel, xradius, xevenp, xwrap); // new line x-conv
        pipeline[kborder] = row; // update pipeline
      }
    }
    for (int yy = yradius+1; y < sizeY; y++, yy++) { // reflect/wrap at ymax
      row = &floatPixel(*to, 0, y); // row-major order
      for (int x = 0; x < sizeX; x++) {
        float sum = ykernel[yradius] * pipeline[yy][x];
        for (int k = 1; k <= yradius; k++)  // convolution along y-axis
          sum += ykernel[yradius-k] * (pipeline[yy-k][x] - pipeline[yy+k][x]);
        row[x] = sum;
      }
    }
  }
  for (int p = 0; p <= 4*yradius; p++)
    delete[] cache[p];         // Free lines in pipeline cache
  delete[] cache;
#ifdef DEBUG
  vcl_cout << " in " << t.real() << " msecs.\n";
#endif
  return 1;                     // assume normalized kernel
}


//: Convolves from image with a filter along x, and a running sum along y-axis, then stores values in to image.
// O(m*n*k).

float
gevd_float_operators::Convolve(gevd_bufferxy& from, gevd_bufferxy*& to,
                               const float* xkernel, const int xradius,
                               const bool xevenp,
                               const int yradius,
                               const bool xwrap, const bool ywrap)
{
  float* ykernel = new float[2*yradius+1];
  const bool yevenp = true;
  for (int i = 0; i < 2*yradius+1; i++)
    ykernel[i] = 1;
  float fact = gevd_float_operators::Convolve(from, to,
                                              xkernel, xradius, xevenp,
                                              ykernel, yradius, yevenp,
                                              xwrap, ywrap);
  delete[] ykernel;
  return fact;
}

#if 0 // commented out
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Convolve image";
#endif
  to = gevd_float_operators::Allocate(to, from);
  const int sizeX = to->GetSizeX(), sizeY = to->GetSizeY();
  const int ylo = yradius, yhi = sizeY - yradius;
  const int kborder = 2*yradius;

  // 1. Setup the pipeline of 4*yradius+1 lines, convolved along x-axis
  float** cache = new float*[4*yradius+1];
  float** pipeline = cache+yradius;
  double* rsum = new double[sizeX]; // avoid addition/subtraction errors
  float* row = NULL;            // current row
  for (int p = 0; p <= kborder; p++) {
    pipeline[p] = row = new float[sizeX];
    gevd_float_operators::Convolve(&floatPixel(from, 0, p), // row-major order
                                   row, sizeX,
                                   xkernel, xradius, xevenp, xwrap);
  }
  // **** wrapping of the from buffer at the end ****
  if (ywrap)                    // circular wrap at 2 end rows
    for (int r = 1; r <= yradius; r++) {
      pipeline[-r] = row = new float[sizeX];
      gevd_float_operators::Convolve(&floatPixel(from, 0, sizeY-r), // row-major order
                                     row, sizeX,
                                     xkernel, xradius, xevenp, xwrap);
      pipeline[kborder+r] = row = new float[sizeX];
      for (int i = 0; i < sizeX; i++) // copy previous convolved result
        row[i] = pipeline[r-1][i];
    }
  else                          // reflection at 2 end rows
    for (int r = 1; r <= yradius; r++) {
      pipeline[-r] = row = new float[sizeX];
      for (int i = 0; i < sizeX; i++) // copy previous convolved result
        row[i] = pipeline[r][i];
      pipeline[kborder+r] = row = new float[sizeX];
      gevd_float_operators::Convolve(&floatPixel(from, 0, sizeY-1-r), // row-major order
                                     row, sizeX,
                                     xkernel, xradius, xevenp, xwrap);
    }

  // 2. Running sum along y-axis, shifting pipeline by 1 each time.
  int y = 0, yy = 0;
  row = &floatPixel(*to, 0, y); // row-major order
  for (int x = 0; x < sizeX; x++) { // running sum at first row
    double sum = pipeline[yy][x];
    for (int k = 1; k <= yradius; k++)
      sum += (pipeline[yy-k][x] + pipeline[yy+k][x]);
    row[x] = float(rsum[x] = sum);
  }
  for (y++, yy++; y < ylo; y++, yy++) { // reflect/wrap at ymin
    row = &floatPixel(*to, 0, y); // row-major order
    for (int x = 0; x < sizeX; x++)
      row[x] = float(rsum[x] = rsum[x] -
                     pipeline[yy-yradius-1][x] + pipeline[yy+yradius][x]);
  }
  for ( ; y < yhi; y++) {       // convolution along y-axis
    row = &floatPixel(*to, 0, y);
    for (int x = 0; x < sizeX; x++)
      row[x] = float(rsum[x] = rsum[x] -
                     pipeline[-1][x] + pipeline[kborder][x]);
    if (p < sizeY) {
      row = pipeline[0];        // next line
      for (int k = 0; k < kborder; k++) // shift the lines of
        pipeline[k] = pipeline[k+1]; // the pipeline by 1
      gevd_float_operators::Convolve(&floatPixel(from, 0, p++), // row-major order
                                     row, sizeX,
                                     xkernel, xradius, xevenp, xwrap); // new x-convolution
      pipeline[kborder] = row; // update pipeline
    }
  }
  for (yy = yradius+1; y < sizeY; y++, yy++) {  // reflect/wrap at ymax
    row = &floatPixel(*to, 0, y); // row-major order
    for (int x = 0; x < sizeX; x++)
      row[x] = float(rsum[x] = rsum[x] -
                     pipeline[yy-yradius-1][x] + pipeline[yy+yradius][x]);
  }
  for (int p = 0; p <= 4*yradius; p++)
    delete[] cache[p];         // Free lines in pipeline cache
  delete[] cache;
  delete[] rsum;
#ifdef DEBUG
  vcl_cout << " in " << t.real() << " msecs.\n";
#endif
  return 2*yradius+1;           // return multiplication factor
}
#endif

//: Convolve a linear array of data, wrapping at the 2 ends.

float
gevd_float_operators::Convolve(float* from, float*& to, const int len,
                               const float* kernel, const int kradius,
                               const bool evenp, const bool wrap)
{
  if (!to) to = new float[len];
  const int xlo = kradius, xhi = len - kradius;
  const int kborder = 2*kradius;
  float *cache, *pipeline;
  int p = gevd_float_operators::SetupPipeline(from, len, kradius, wrap,
                                              cache, pipeline);

  // Convolution along x with above pipeline
  float sum;
  if (evenp) {
    int x=0;
    for (int xx = 0; x < xlo; ++x, ++xx) {
      sum = kernel[kradius] * pipeline[xx];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[xx-k] + pipeline[xx+k]);
      to[x] = sum;
    }
    for ( ; x < xhi; x++) {
      sum = kernel[kradius] * pipeline[kradius];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[kradius-k] + // plus for
                                    pipeline[kradius+k]); // symmetric
      to[x] = sum;
      if (p < len) {
        for (int k = 0; k < kborder; k++)   // shift the floats of
          pipeline[k] = pipeline[k+1];  // the pipeline by 1
        pipeline[kborder] = from[p++];  // update pipeline
      }
    }
    for (int xx = kradius+1; x < len; x++, xx++) {
      sum = kernel[kradius] * pipeline[xx];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[xx-k] + pipeline[xx+k]);
      to[x] = sum;
    }
  } else {
    int x=0;
    for (int xx = 0; x < xlo; ++x, ++xx) {
      sum = kernel[kradius] * pipeline[xx];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[xx-k] - pipeline[xx+k]);
      to[x] = sum;
    }
    for ( ; x < xhi; x++) {
      sum = kernel[kradius] * from[x];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[kradius-k] - // minus for
                                    pipeline[kradius+k]); // antisymmetric
      to[x] = sum;
      if (p < len) {
        for (int k = 0; k < kborder; k++)   // shift the floats of
          pipeline[k] = pipeline[k+1];  // the pipeline by 1
        pipeline[kborder] = from[p++];  // update pipeline
      }
    }
    for (int xx = kradius+1; x < len; x++, xx++) {
      sum = kernel[kradius] * pipeline[xx];
      for (int k = 1; k <= kradius; k++)
        sum += kernel[kradius-k] * (pipeline[xx-k] - pipeline[xx+k]);
      to[x] = sum;
    }
  }
  delete[] cache;
  return 1;                     // assume normalized kernel
}

//: For large smoothing sigma, use running sum to avoid floating multiplications.

float
gevd_float_operators::RunningSum(float* from, float*& to, const int len,
                                 const int kradius, const bool wrap)
{
  if (!to) to = new float[len];
  const int xlo = kradius, xhi = len - kradius;
  const int kborder = 2*kradius;
  float *cache, *pipeline;
  int p = gevd_float_operators::SetupPipeline(from, len, kradius, wrap,
                                              cache, pipeline);
  // Running sum along x with above pipeline
  int x = 0, xx = 0;
  float sum = pipeline[x];      // pre-compute running sum
  for (int k = 1; k <= kradius; k++)
    sum += (pipeline[xx-k] + pipeline[xx+k]);
  to[x] = sum;
  for (x++, xx++; x < xlo; x++, xx++) {
    sum += pipeline[xx+kradius] - pipeline[xx-kradius-1];
    to[x] = sum;
  }
  for ( ; x < xhi; x++) {
    sum += pipeline[kborder] - pipeline[-1];
    to[x] = sum;
    if (p < len) {
      for (int k = -1; k < kborder; k++)    // shift the floats of
        pipeline[k] = pipeline[k+1];    // the pipeline by 1
      pipeline[kborder] = from[p++];    // update pipeline
    }
  }
  for ( ; x < len; x++, xx++) {
    sum += pipeline[xx+kradius] - pipeline[xx-kradius-1];
    to[x] = sum;
  }
  delete[] cache;
  return float(2*kradius+1);            // magnification factor
}

//: Read 1d odd/even kernel from a file: width k_i .....

bool
gevd_float_operators::Read1dKernel(const char* filename,
                                   float*& kernel, int& radius, bool& evenp)
{
  vcl_ifstream infile(filename, vcl_ios_in); // open the file
  if (!infile)
    return false;
  int width;
  infile >> width;
  if (width < 1) return false;
  radius = (width - 1)/2;
  width = 2*radius + 1;
  delete[] kernel;
  kernel = new float[width];
  for (int i = 0; i < width; i++)
    infile >> kernel[i];
  evenp = true;
  for (int i = 1; i <= radius; i++)
    if (kernel[radius-i] != kernel[radius+i])
      evenp = false;
  if (evenp)                    // double check that kernel is even
    return true;
  for (int i = 1; i <= radius; i++)
    if (kernel[radius-i] != -kernel[radius+i])
      evenp = true;
  if (!evenp)                   // double check that kernel is odd
    return true;
  delete[] kernel; kernel = NULL;
  return false;                 // invalid kernel
}


//:
// Convolves the from image with a 2D Gaussian filter
// with standard deviation sigma (default = 1). O(m*n*k).
// The 2D convolution is decomposed into 2 1D convolutions:
// Gxy * I = Gy * (Gx * I).
// The frame with width equal to the radius of the Gaussian kernel is
// filled with zero.

float
gevd_float_operators::Gaussian(gevd_bufferxy& from, gevd_bufferxy*& to, const float sigma,
                               const bool xwrap, const bool ywrap)
{
  float* kernel = NULL;
  int radius = 0;
  if (!gevd_float_operators::Find1dGaussianKernel(sigma, kernel, radius)) {
    to = gevd_float_operators::Allocate(to, from);
    if (to != &from)
      gevd_float_operators::Update(*to, from);  // just a copy, no smoothing needed
  } else {
    bool evenp = true;
    gevd_float_operators::Convolve(from, to,    // 2 1D convolutions O(k), instead
                                   kernel, radius, evenp,       // of a 2D convolution O(k^2).
                                   kernel, radius, evenp,
                                   xwrap, ywrap);
  }
  delete[] kernel;
  return 1;                     // return multiplication factor
}


//:
// Returns the kernel array for Gaussian with given standard deviation
// sigma [pixel], and cutoff at min/max = fuzz.
// The area under the discrete Gaussian is normalized to 1.

bool
gevd_float_operators::Find1dGaussianKernel(const float sigma,
                                           float*& kernel, int& radius,
                                           const float fuzz)
{
  for (radius = 0; Gaussian(float(radius), sigma) > fuzz; radius++)
    {;}                                         // find radius
  if (radius == 0)
    return false;

  kernel = new float[2*radius + 1];             // create kernel
  for (int i=0; i<=radius; ++i)
    kernel[radius+i] = kernel[radius-i] = Gaussian(float(i), sigma);
  float sum = 0;
  for (int i= 0; i <= 2*radius; ++i)
    sum += kernel[i];                           // find integral of weights
  for (int i= 0; i <= 2*radius; ++i)
    kernel[i] /= sum;                           // normalize by integral
#ifdef DEBUG
  vcl_cout << "Gaussian kernel = ";
  for (int i= 0; i <= 2*radius; ++i)
    vcl_cout << kernel[i] << ' ';
  vcl_cout << vcl_endl;
#endif
  return true;
}


float
gevd_float_operators::Gaussian(const float x, const float sigma)
{
  double x_on_sigma = x / sigma;
  return (float)vcl_exp(- x_on_sigma * x_on_sigma / 2);
}


//:
// Setup the cache for reflection/wrapping at the borders,
// and the center pipeline. Only cache should be deleted after
// you're done, not the pipeline, since pipeline shares the same space.

int
gevd_float_operators::SetupPipeline(const float* data, const int len,
                                    const int kradius, const bool wrap,
                                    float*& cache, float*& pipeline)
{
  cache = new float[4*kradius+1]; // Setup the cache of 4*kradius+1 floats
  pipeline = cache+kradius;     // center pipeline is 2*kradius+1 floats
  int p = 0;
  for (; p <= 2*kradius; p++) pipeline[p] = data[p];
  if (wrap)                     // circular wrap at 2 end points
    for (int r = 1; r <= kradius; r++) {
      pipeline[-r] = data[len-r];
      pipeline[2*kradius+r] = data[r-1];
    }
  else                          // reflection at 2 end points
    for (int r = 1; r <= kradius; r++) {
      pipeline[-r] = data[r];
      pipeline[2*kradius+r] = data[len-1-r];
    }
  return p;                     // current index in pipeline
}

//: Create a buffer that wraps rows/columns.

gevd_bufferxy*
gevd_float_operators::WrapAlongX(const gevd_bufferxy& img)
{
  const int lx = img.GetSizeX()-1, sy = img.GetSizeY();
  gevd_bufferxy* wrap = gevd_float_operators::SimilarBuffer(img, bits_per_float,  4, sy);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < sy; j++) {
      floatPixel(*wrap, 2+i, j) = floatPixel(img, i, j);
      floatPixel(*wrap, 1-i, j) = floatPixel(img, lx-i, j);
    }
  return wrap;
}


gevd_bufferxy*
gevd_float_operators::WrapAlongY(const gevd_bufferxy& img)
{
  const int sx = img.GetSizeX(), ly = img.GetSizeY()-1;
  gevd_bufferxy* wrap = gevd_float_operators::SimilarBuffer(img, bits_per_float, sx, 4);
  for (int j = 0; j < 2; j++)
    for (int i = 0; i < sx; i++) {
      floatPixel(*wrap, i, 2+j) = floatPixel(img, i, j);
      floatPixel(*wrap, i, 1-j) = floatPixel(img, i, ly-j);
    }
  return wrap;
}

// Local gradient magnitude and direction in 3x3 window

inline void
LocalGradient(const gevd_bufferxy& smooth, const int i, const int j,
              float& mag, float& gx, float& gy)
{
  gx = floatPixel(smooth, i+1, j) - floatPixel(smooth, i-1, j);
  gy = floatPixel(smooth, i, j+1) - floatPixel(smooth, i, j-1);
  mag = vcl_sqrt(gx*gx + gy*gy);
}

//: Compute the gradient of the intensity surface. O(m*n).
// The intensity surface is assumed smoothed by a Gaussian filter,
// or convolved with a low-pass filter.
// Return at each pixel, the magnitude, the vector (dG * I),
// and the multiplication factor to normalize the magnitude.
// The product of detection |dG * I| and localization |dddG * I| of
// the step relative to noise is invariant to filter-size for the
// ideal step edge, and depends only on the shape of the step edge.
// As the filter size (sigma) increases by k, detection or
// signal-to-noise ratio increases by sqrt(k), but localization
// decreases by 1/sqrt(k). This invariance is consistent with the
// uncertainty principle.

float
gevd_float_operators::Gradient(const gevd_bufferxy& smooth,
                               gevd_bufferxy*& magnitude,
                               gevd_bufferxy*& gradx, gevd_bufferxy*& grady,
                               const bool xwrap, const bool ywrap)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Compute local gradient magnitude/direction";
#endif
  magnitude = gevd_float_operators::Allocate(magnitude, smooth);
  gradx = gevd_float_operators::Allocate(gradx, smooth);
  grady = gevd_float_operators::Allocate(grady, smooth);

  // 1. Inside image frame, compute values based on 3x3 window.
  const int frame = 1;
  const int highx = smooth.GetSizeX() - frame; // exclusive bounds
  const int highy = smooth.GetSizeY() - frame;
  for (int j = frame; j < highy; ++j)
    for (int i = frame; i < highx; ++i)
      LocalGradient(smooth, i, j,
                    floatPixel(*magnitude, i, j),
                    floatPixel(*gradx, i, j),
                    floatPixel(*grady, i, j));

  // 2. Along horizontal/vertical borders
  if (xwrap) {                  //  each row wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongX(smooth);
    for (int j = 1; j < highy; ++j) {
      LocalGradient(*pad, lo, j,
                    floatPixel(*magnitude, 0, j),
                    floatPixel(*gradx, 0, j),
                    floatPixel(*grady, 0, j));
      LocalGradient(*pad, hi, j,
                    floatPixel(*magnitude, highx, j),
                    floatPixel(*gradx, highx, j),
                    floatPixel(*grady, highx, j));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameX(*magnitude, 0, frame);
    gevd_float_operators::FillFrameX(*gradx, 0, frame);
    gevd_float_operators::FillFrameX(*grady, 0, frame);
  }
  if (ywrap) {                  // each column wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongY(smooth);
    for (int i = 1; i < highx; ++i) {
      LocalGradient(*pad, i, lo,
                    floatPixel(*magnitude, i, 0),
                    floatPixel(*gradx, i, 0),
                    floatPixel(*grady, i, 0));
      LocalGradient(*pad, i, hi,
                    floatPixel(*magnitude, i, highy),
                    floatPixel(*gradx, i, highy),
                    floatPixel(*grady, i, highy));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameY(*magnitude, 0, frame);
    gevd_float_operators::FillFrameY(*gradx, 0, frame);
    gevd_float_operators::FillFrameY(*grady, 0, frame);
  }
#ifdef DEBUG
  vcl_cout << ", in " << t.real() << " msecs.\n";
#endif
  return 2;                     // return multiplication factor
}

//: Compute slope or first-difference, in linear/circular array.

float
gevd_float_operators::Slope(float* from, float*& to, const int len,
                            const bool wrap)
{
  if (!to) to = new float[len];
  const int xlo = 1, xhi = len - 1;
  float *cache = NULL, *pipeline;
  int p = gevd_float_operators::SetupPipeline(from, len, 1, wrap, // current pipeline and index
                                              cache, pipeline);
  to[0] = pipeline[+1] - pipeline[-1];
  for (int x = xlo; x < xhi; x++) {
    to[x] = pipeline[2] - pipeline[0];
    if (p < len) {
      for (int k = 0; k < 2; k++) // shift the floats of
        pipeline[k] = pipeline[k+1]; // the pipeline by 1
      pipeline[2] = from[p++];  // update pipeline
    }
  }
  to[xhi] = pipeline[3] - pipeline[1];
  delete[] cache;
  return 2;                     // magnification factor
}

// Local Hessian magnitude and direction in 3x3 window
// corresponding to largest eigenvalue, in absolute magnitude.
// Encode the sign of the curvature in the angle (dirx, diry).

void
LocalHessian(const gevd_bufferxy& smooth, const int i, const int j,
             float& mag, float& dirx, float& diry)
{
  float two_pij = 2 * floatPixel(smooth, i, j);
  float ddx = floatPixel(smooth, i+1, j) + floatPixel(smooth, i-1, j) - two_pij;
  float ddy = floatPixel(smooth, i, j+1) + floatPixel(smooth, i, j-1) - two_pij;
  float two_dxdy = (floatPixel(smooth, i+1, j+1) +
                    floatPixel(smooth, i-1, j-1) -
                    floatPixel(smooth, i-1, j+1) -
                    floatPixel(smooth, i+1, j-1)) / 2;
  float ddx_plus_ddy = ddx + ddy;
  float ddx_minus_ddy = ddx - ddy;
  float theta = (two_dxdy==0 && ddx_minus_ddy==0) ? 0 : // DOMAIN cond. on atan2
                (float)vcl_atan2(two_dxdy, ddx_minus_ddy) / 2; // modulo PI
  if (ddx_plus_ddy < 0) {
    mag = - ddx_plus_ddy        // most negative eigenvalue
          + vcl_sqrt((ddx_minus_ddy * ddx_minus_ddy) + (two_dxdy * two_dxdy));
    theta += (float)vnl_math::pi_over_2;// angle in range [0 pi]
  } else {
    mag = + ddx_plus_ddy        // most positive eigenvalue
          + vcl_sqrt((ddx_minus_ddy * ddx_minus_ddy) + (two_dxdy * two_dxdy));
    if (theta > 0)
      theta -= (float)vnl_math::pi;    // angle in range [-pi 0]
  }
  dirx = (float)vcl_cos(theta); // eigenvector corresponding to
  diry = (float)vcl_sin(theta); // largest eigenvalue/curvature
}

//: Compute the Hessian of the intensity surface. O(m*n).
// The Hessian is directional, and described by the largest absolute
// eigenvalue, and its corresponding eigenvector.
// The intensity surface is assumed smoothed by a Gaussian filter,
// or convolved with a low-pass filter.
// Return at each pixel, the largest absolute value of the two
// eigenvalues, the corresponding eigenvector, and the
// multiplication factor to normalize the magnitude.

float
gevd_float_operators::Hessian(const gevd_bufferxy& smooth,
                              gevd_bufferxy*& magnitude,
                              gevd_bufferxy*& dirx, gevd_bufferxy*& diry,
                              const bool xwrap, const bool ywrap)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Compute local Hessian magnitude/direction";
#endif
  magnitude = gevd_float_operators::Allocate(magnitude, smooth);
  dirx = gevd_float_operators::Allocate(dirx, smooth);
  diry = gevd_float_operators::Allocate(diry, smooth);

  // 1. Inside image frame, compute values based on 3x3 window.
  const int frame = 1;
  const int highx = smooth.GetSizeX() - frame;  // exclusive bounds
  const int highy = smooth.GetSizeY() - frame;
  for (int j = frame; j < highy; ++j)
    for (int i = frame; i < highx; ++i)
      LocalHessian(smooth, i, j,
                   floatPixel(*magnitude, i, j),
                   floatPixel(*dirx, i, j),
                   floatPixel(*diry, i, j));

  // 2. Along horizontal/vertical borders
  if (xwrap) {                  //  each row wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongX(smooth);
    for (int j = 1; j < highy; ++j) {
      LocalHessian(*pad, lo, j,
                   floatPixel(*magnitude, 0, j),
                   floatPixel(*dirx, 0, j),
                   floatPixel(*diry, 0, j));
      LocalHessian(*pad, hi, j,
                   floatPixel(*magnitude, highx, j),
                   floatPixel(*dirx, highx, j),
                   floatPixel(*diry, highx, j));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameX(*magnitude, 0, frame);
    gevd_float_operators::FillFrameX(*dirx, 0, frame);
    gevd_float_operators::FillFrameX(*diry, 0, frame);
  }
  if (ywrap) {                  // each column wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongY(smooth);
    for (int i = 1; i < highx; ++i) {
      LocalHessian(*pad, i, lo,
                   floatPixel(*magnitude, i, 0),
                   floatPixel(*dirx, i, 0),
                   floatPixel(*diry, i, 0));
      LocalHessian(*pad, i, hi,
                   floatPixel(*magnitude, i, highy),
                   floatPixel(*dirx, i, highy),
                   floatPixel(*diry, i, highy));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameY(*magnitude, 0, frame);
    gevd_float_operators::FillFrameY(*dirx, 0, frame);
    gevd_float_operators::FillFrameY(*diry, 0, frame);
  }
#ifdef DEBUG
  vcl_cout << ", in " << t.real() << " msecs.\n";
#endif
  return 2;                     // multiplication factor for magnitude
}

// Local Laplacian in 3x3 neighborhood, sum of curvature,
// and eigenvector corresponding to largest absolute eigenvalue.

void
LocalLaplacian(const gevd_bufferxy& smooth, const int i, const int j,
               float& mag, float& dirx, float& diry)
{
  float two_pij = 2 * floatPixel(smooth, i, j);
  float ddx = floatPixel(smooth, i+1, j) + floatPixel(smooth, i-1, j) - two_pij;
  float ddy = floatPixel(smooth, i, j+1) + floatPixel(smooth, i, j-1) - two_pij;
  float diag1 = floatPixel(smooth, i+1, j+1) + floatPixel(smooth, i-1, j-1);
  float diag2 = floatPixel(smooth, i-1, j+1) + floatPixel(smooth, i+1, j-1);
  mag = 4*(ddx + ddy) - 2*two_pij + diag1 + diag2; // save division by 6
#if 0 // commented out
  mag = (floatPixel(smooth, i+1, j+1) + floatPixel(smooth, i-1, j-1) +
         floatPixel(smooth, i-1, j+1) + floatPixel(smooth, i+1, j-1) +
         4 * (floatPixel(smooth, i+1, j) + floatPixel(smooth, i-1, j) +
              floatPixel(smooth, i, j+1) + floatPixel(smooth, i, j-1)) +
         -20 * floatPixel(smooth, i, j)) / 6;
#endif
  float theta = (diag1==diag2 && ddx==ddy) ? 0 : // DOMAIN condition on atan2
                (float)vcl_atan2((diag1 - diag2) / 2, ddx - ddy) / 2; // modulo PI
  if (mag < 0) {
    mag = - mag;                  // absolute magnitude
    theta += (float)vnl_math::pi_over_2; // other eigenvector
  }
  dirx = (float)vcl_cos(theta);   // eigenvector corresponding to
  diry = (float)vcl_sin(theta);   // largest eigenvalue/curvature
}

//: Compute the Laplacian of the intensity surface. O(m*n).
// The Laplacian is rotational symmetric, and is the sum of the 2
// eigenvalues/curvatures, with positive/negative sign for concave/convex.
// The intensity surface is assumed smoothed by a Gaussian filter,
// or convolved with a low-pass filter.
// Return at each pixel, the absolute value of the laplacian,
// the eigenvector corresponding to the largest absolute
// eigenvalue/curvature, and the multiplication factor to
// normalize the magnitude.

float
gevd_float_operators::Laplacian(const gevd_bufferxy& smooth,
                                gevd_bufferxy*& magnitude,
                                gevd_bufferxy*& dirx, gevd_bufferxy*& diry,
                                const bool xwrap, const bool ywrap)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Compute local Laplacian magnitude/direction";
#endif
  magnitude = gevd_float_operators::Allocate(magnitude, smooth);
  dirx = gevd_float_operators::Allocate(dirx, smooth);
  diry = gevd_float_operators::Allocate(diry, smooth);

  // 1. Inside image frame, compute values based on 3x3 window.
  const int frame = 1;
  const int highx = smooth.GetSizeX() - frame;// exclusive bounds
  const int highy = smooth.GetSizeY() - frame;
  for (int j = frame; j < highy; ++j)
    for (int i = frame; i < highx; ++i)
      LocalLaplacian(smooth, i, j,
                     floatPixel(*magnitude, i, j),
                     floatPixel(*dirx, i, j),
                     floatPixel(*diry, i, j));

  // 2. Along horizontal/vertical borders
  if (xwrap) {                  //  each row wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongX(smooth);
    for (int j = 1; j < highy; ++j) {
      LocalLaplacian(*pad, lo, j,
                     floatPixel(*magnitude, 0, j),
                     floatPixel(*dirx, 0, j),
                     floatPixel(*diry, 0, j));
      LocalLaplacian(*pad, hi, j,
                     floatPixel(*magnitude, highx, j),
                     floatPixel(*dirx, highx, j),
                     floatPixel(*diry, highx, j));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameX(*magnitude, 0, frame);
    gevd_float_operators::FillFrameX(*dirx, 0, frame);
    gevd_float_operators::FillFrameX(*diry, 0, frame);
  }
  if (ywrap) {                  // each column wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* pad = gevd_float_operators::WrapAlongY(smooth);
    for (int i = 1; i < highx; ++i) {
      LocalLaplacian(*pad, i, lo,
                     floatPixel(*magnitude, i, 0),
                     floatPixel(*dirx, i, 0),
                     floatPixel(*diry, i, 0));
      LocalLaplacian(*pad, i, hi,
                     floatPixel(*magnitude, i, highy),
                     floatPixel(*dirx, i, highy),
                     floatPixel(*diry, i, highy));
    }
    delete pad;
  } else {                      // zero by default
    gevd_float_operators::FillFrameY(*magnitude, 0, frame);
    gevd_float_operators::FillFrameY(*dirx, 0, frame);
    gevd_float_operators::FillFrameY(*diry, 0, frame);
  }
#ifdef DEBUG
  vcl_cout << ", in " << t.real() << " msecs.\n";
#endif
  return 6;                     // multiplication factor for magnitude
}

//: Compute curvature, or 2nd-difference in linear/circular array.

float
gevd_float_operators::Curvature(float* from, float*& to, const int len,
                                const bool wrap)
{
  if (!to) to = new float[len];
  const int xlo = 1, xhi = len - 1;
  float *cache, *pipeline;
  int p = gevd_float_operators::SetupPipeline(from, len, 1, wrap, // current pipeline and index
                                              cache, pipeline);
  to[0] = pipeline[+1] + pipeline[-1] - 2*pipeline[0];
  for (int x = xlo; x < xhi; x++) {
    to[x] = pipeline[2] + pipeline[0] - 2*pipeline[1];
    if (p < len) {
      for (int k = 0; k < 2; k++)       // shift the floats of
        pipeline[k] = pipeline[k+1];    // the pipeline by 1
      pipeline[2] = from[p++];  // update pipeline
    }
  }
  to[xhi] = pipeline[3] + pipeline[1] - 2*pipeline[2];
  delete[] cache;
  return 1;                     // magnification factor
}


//:
// Compute the local orientation at each pixel in the image,
// and returns 2 images: twice the angle, and coherence measure (0,1).

float
gevd_float_operators::Orientation(const gevd_bufferxy& smooth,
                                  gevd_bufferxy*& theta, gevd_bufferxy*& coherence,
                                  const int frame)

{
  theta = gevd_float_operators::Allocate(theta, smooth);
  coherence = gevd_float_operators::Allocate(coherence, smooth);
  gevd_bufferxy& thetaI = *theta;
  gevd_bufferxy& coherenceI = *coherence;
  int highx = smooth.GetSizeX() - frame;        // exclusive bounds
  int highy = smooth.GetSizeY() - frame;
  float dx, dy;
  float p_ij, ox, oy;
  for (int j = frame; j < highy; ++j)
    for (int i = frame; i < highx; ++i) {
      p_ij = floatPixel(smooth, i, j);
      dx = floatPixel(smooth, i+1, j) - p_ij;   // assume 2D gradient
      dy = floatPixel(smooth, i, j+1) - p_ij;   // onto x and y axes
      ox = (dy * dy) - (dx * dx);
      oy = 2 * dx * dy;
      floatPixel(thetaI, i, j) = (float)vcl_atan2(oy, ox);
      floatPixel(coherenceI, i, j) = ((ox * ox + oy * oy) /
                                      (dx + dy) * (dx + dy));
    }
  gevd_float_operators::FillFrameX(thetaI, 0, frame);
  gevd_float_operators::FillFrameY(thetaI, 0, frame);
  gevd_float_operators::FillFrameX(coherenceI, 0, frame);
  gevd_float_operators::FillFrameY(coherenceI, 0, frame);
  return 1;
}

// Non maximum suppression in 3x3 window

inline void
LocalMaximum(const gevd_bufferxy& magnitude,
             const gevd_bufferxy& directionx, const gevd_bufferxy& directiony,
             const int i, const int j, const float threshold,
             float& contour, unsigned char& dir, // response & direction
             float& locx, float& locy)  // subpixel location
{
  const float tan_pi_8 = (float)vcl_tan(vnl_math::pi_over_4/2);
  float strength = floatPixel(magnitude, i, j);
  if (strength > threshold) { // eliminate noisy responses
    float dx = floatPixel(directionx, i, j);
    float dy = floatPixel(directiony, i, j);
    if (dy < 0) {
      dx = -dx, dy = -dy;       // modulo PI
      dir = DIR4 - DIR0;
    } else
      dir = 0;
    float sl, sr, r;
    if (dx > 0) {               // which octant?
      if (dx > dy) {    // 0-45 degree
        r = dy / dx;
        sl = (r*floatPixel(magnitude, i-1, j-1) +
              (1-r)*floatPixel(magnitude, i-1, j));
        sr = (r*floatPixel(magnitude, i+1, j+1) +
              (1-r)*floatPixel(magnitude, i+1, j));
        dx = 1, dy = r; // range in location
        dir += (r < tan_pi_8)? DIR0 : DIR1;
      } else {          // 45-90 degree
        r = dx / dy;
        sl = (r*floatPixel(magnitude, i-1, j-1) +
              (1-r)*floatPixel(magnitude, i, j-1));
        sr = (r*floatPixel(magnitude, i+1, j+1) +
              (1-r)*floatPixel(magnitude, i, j+1));
        dy = 1, dx = r;
        dir += (r < tan_pi_8)? DIR2 : DIR1;
      }
    } else {
      dx = -dx;         // absolute value
      if (dy > dx) {    // 90-135 degree
        r = dx / dy;
        sl = (r*floatPixel(magnitude, i-1, j+1) +
              (1-r)*floatPixel(magnitude, i, j+1));
        sr = (r*floatPixel(magnitude, i+1, j-1) +
              (1-r)*floatPixel(magnitude, i, j-1));
        dy = -1, dx = r;
        dir += (r < tan_pi_8)? DIR2 : DIR3;
      } else {          // 135-180 degree
        r = dy / dx;
        sl = (r*floatPixel(magnitude, i+1, j-1) +
              (1-r)*floatPixel(magnitude, i+1, j));
        sr = (r*floatPixel(magnitude, i-1, j+1) +
              (1-r)*floatPixel(magnitude, i-1, j));
        dx = -1, dy = r;
        dir += (r < tan_pi_8)? DIR0 : DIR3;
      }
    }
    if (sl < strength &&        // usually a strict local maximum
        strength >= sr) {       // equality is for continuity for b&w images
      r = gevd_float_operators::InterpolateParabola(sl, strength, sr, // interpolated offset
                                                    contour); // interpolated max response
      locx = r*dx;
      locy = r*dy;
    } else
      contour = 0;              // mark NULL contour
  } else
    contour = 0;                // mark NULL contour
}


//:
// Detect contour pixels as strict local maxima, and
// interpolate the strength/location with a parabola through 3 points.
// The location[xy]/direction[xy] buffers no longer share the same
// space to avoid bogus values when the contour/junction pixels move
// around by +/- 1 pixel.

void
gevd_float_operators::NonMaximumSuppression(const gevd_bufferxy& magnitude,
                                            const gevd_bufferxy& directionx,
                                            const gevd_bufferxy& directiony,
                                            const float threshold,
                                            gevd_bufferxy*& contour,
                                            gevd_bufferxy*& direction,
                                            gevd_bufferxy*& locationx,
                                            gevd_bufferxy*& locationy,
                                            const bool xwrap,
                                            const bool ywrap)
{
#ifdef DEBUG
  vul_timer t;
  vcl_cout << "Non maximum suppression to find edge elements > " << threshold;
#endif
  contour = gevd_float_operators::Allocate(contour, magnitude);
  direction = gevd_float_operators::Allocate(direction, magnitude, bits_per_byte);
  locationx = gevd_float_operators::Allocate(locationx, magnitude);
  locationy = gevd_float_operators::Allocate(locationy, magnitude);
  locationx->Clear(); locationy->Clear(); // clear subpixel locs

  // 1. Inside image frame, compute values based on 3x3 window.
  const int frame = 1;
  const int highx = magnitude.GetSizeX() - frame;// exclusive bounds
  const int highy = magnitude.GetSizeY() - frame;
  for (int j = frame; j < highy; ++j)
    for (int i = frame; i < highx; ++i)
      LocalMaximum(magnitude, directionx, directiony,
                   i, j, threshold,
                   floatPixel(*contour, i, j),
                   bytePixel(*direction, i, j),
                   floatPixel(*locationx, i, j),
                   floatPixel(*locationy, i, j));

  // 2. Along horizontal/vertical borders
  if (xwrap) {                  //  each row wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* mag = gevd_float_operators::WrapAlongX(magnitude);
    gevd_bufferxy* dirx = gevd_float_operators::WrapAlongX(directionx);
    gevd_bufferxy* diry = gevd_float_operators::WrapAlongX(directiony);
    for (int j = 1; j < highy; ++j) {
      LocalMaximum(*mag, *dirx, *diry,
                   lo, j, threshold,
                   floatPixel(*contour, 0, j),
                   bytePixel(*direction, 0, j),
                   floatPixel(*locationx, 0, j),
                   floatPixel(*locationy, 0, j));
      LocalMaximum(*mag, *dirx, *diry,
                   hi, j, threshold,
                   floatPixel(*contour, highx, j),
                   bytePixel(*direction, highx, j),
                   floatPixel(*locationx, highx, j),
                   floatPixel(*locationy, highx, j));
    }
    delete mag; delete dirx; delete diry;
  } else {                      // zero by default
    gevd_float_operators::FillFrameX(*contour, 0, frame);
    for (int j = 1; j < highy; ++j) {
      bytePixel(*direction, 0, j) = 0;
      bytePixel(*direction, highx, j) = 0;
    }
  }
  if (ywrap) {                  // each column wrap at border
    const int lo = 2, hi = 1;
    gevd_bufferxy* mag = gevd_float_operators::WrapAlongY(magnitude);
    gevd_bufferxy* dirx = gevd_float_operators::WrapAlongY(directionx);
    gevd_bufferxy* diry = gevd_float_operators::WrapAlongY(directiony);
    for (int i = 1; i < highx; ++i) {
      LocalMaximum(*mag, *dirx, *diry,
                   i, lo, threshold,
                   floatPixel(*contour, i, 0),
                   bytePixel(*direction, i, 0),
                   floatPixel(*locationx, i, 0),
                   floatPixel(*locationy, i, 0));
      LocalMaximum(*mag, *dirx, *diry,
                   i, hi, threshold,
                   floatPixel(*contour, i, highy),
                   bytePixel(*direction, i, highy),
                   floatPixel(*locationx, i, highy),
                   floatPixel(*locationy, i, highy));
    }
    delete mag; delete dirx; delete diry;
  } else {                      // zero by default
    gevd_float_operators::FillFrameY(*contour, 0, frame);
    for (int i = 1; i < highx; ++i) {
      bytePixel(*direction, i, 0) = 0;
      bytePixel(*direction, i, highy) = 0;
    }
  }
#ifdef DEBUG
  vcl_cout << ", in " << t.real() << " msecs.\n";
#endif
}

//: Detect local maxima in linear/circular array.

int
gevd_float_operators::NonMaximumSuppression(const float* data, const int len,
                                            const float threshold,
                                            int*& index, float*& mag, float*& loc,
                                            const bool wrap)
{
  if (!index) index = new int[len/2];
  if (!mag) mag = new float[len/2];
  if (!loc) loc = new float[len/2];
  const int xlo = 1, xhi = len - 1;
  float *cache, *pipeline;
  int p = gevd_float_operators::SetupPipeline(data, len, 1, wrap, // current pipeline and index
                                              cache, pipeline);
  int nmax = 0;                 // number of maxima found
  if (pipeline[0] > threshold &&
      pipeline[0] > pipeline[+1] &&
      pipeline[0] > pipeline[-1]) {
    index[nmax] = 0;
    loc[nmax] = gevd_float_operators::InterpolateParabola(pipeline[-1], pipeline[0],
                                                          pipeline[+1], mag[nmax]);
    nmax++;
  }
  for (int x = xlo; x < xhi; x++) {
    if (pipeline[1] > threshold &&
        pipeline[1] > pipeline[2] &&
        pipeline[1] > pipeline[0]) {
      index[nmax] = x;
      loc[nmax] = gevd_float_operators::InterpolateParabola(pipeline[0], pipeline[1],
                                                            pipeline[2], mag[nmax]);
      nmax++;
    }
    if (p < len) {
      for (int k = 0; k < 2; k++)       // shift the floats of
        pipeline[k] = pipeline[k+1];    // the pipeline by 1
      pipeline[2] = data[p++];  // update pipeline
    }
  }
  if (pipeline[2] > threshold &&
      pipeline[2] > pipeline[3] &&
      pipeline[2] > pipeline[1]) {
    index[nmax] = xhi;
    loc[nmax] = gevd_float_operators::InterpolateParabola(pipeline[1], pipeline[2],
                                                          pipeline[3], mag[nmax]);
    nmax++;
  }
  delete[] cache;
  return nmax;                  // number of maxima found
}

//: Fit a parabola through 3 points with strict local max/min.
// Return the offset location, and value of the maximum/minimum.

float
gevd_float_operators::InterpolateParabola(float y_1, float y_0, float y_2,
                                          float&y)
{
  float diff1 = y_2 - y_1;      // first derivative
  float diff2 = 2 * y_0 - y_1 - y_2; // second derivative
  y = y_0 + diff1 * diff1 / (8 * diff2);        // interpolate y as max/min
  return diff1 / (2 * diff2);   // interpolate x offset
}

//: Return the float value of pixel (x, y) in float buffer, using bilinear interpolation.

float
gevd_float_operators::BilinearInterpolate(const gevd_bufferxy& buffer,
                                          float x, float y)
{
  register int ix = int(x);             // integral parts
  register int iy = int(y);
  float fx = x - ix;                    // fractional parts
  float fy = y - iy;
  float c00 = floatPixel(buffer, ix, iy); // 4 corners of rectangle
  float c01 = floatPixel(buffer, ix, iy+1);
  float c10 = floatPixel(buffer, ix+1, iy);
  float c11 = floatPixel(buffer, ix+1, iy+1);
  float c0 = c00 + fy * (c01 - c00);    // interpolate along y
  float c1 = c10 + fy * (c11 - c10);
  return  c0  + fx * (c1 - c0);         // interpolate along x
}


//: Find angle of the line of support, in degrees.

void
gevd_float_operators::SupportAngle(const gevd_bufferxy& dirx, const gevd_bufferxy& diry,
                                   const gevd_bufferxy& magnitude,
                                   gevd_bufferxy*& angLe)
{
  angLe = gevd_float_operators::Allocate(angLe, magnitude);
  const int highx = magnitude.GetSizeX();       // exclusive bounds
  const int highy = magnitude.GetSizeY();
  float theta;
  for (int j = 0; j < highy; ++j)
    for (int i = 0; i < highx; ++i)
      if (floatPixel(magnitude, i, j) > 0) {
        theta = (float)vcl_atan2(floatPixel(diry, i, j), floatPixel(dirx, i, j));
        if (theta < 0) theta += (float)vnl_math::pi;
        floatPixel(*angLe, i, j) = theta * 180 * (float)vnl_math::one_over_pi;
      }
}


//: Find local surface normal at all pixels in the range z(x,y) image.
// The normal is estimated from the cross-product of the two tangent
// vectors along the x- and y- axes.

void
gevd_float_operators::SurfaceNormal(const gevd_bufferxy& range, gevd_bufferxy*& normal)
{
  const int frame = 1;
  const int highx = range.GetSizeX()-frame, highy = range.GetSizeY()-frame;
  normal = gevd_float_operators::Allocate(normal, range, bits_per_ptr);
  normal->Clear();              // NULL vector on border
  vnl_vector<float> tx(3, 0.0f), ty(3, 0.0f); // tangents x-y axes
  tx[0]=2.0f; ty[1]=2.0f;
  for (int j = frame; j < highy; j++)
    for (int i = frame; i < highx; i++) { // for all grid points
      tx[3] = floatPixel(range, i+1, j) - floatPixel(range, i-1, j);
      ty[3] = floatPixel(range, i, j+1) - floatPixel(range, i, j-1);

      vnl_vector<float> *nz= new vnl_vector<float>(cross_3d(tx,ty));

      float mag = nz->magnitude();
      if (mag != 0) *nz /= mag; // make unit vector
      fvectorPixel(*normal, i, j) = nz;
    }
}

//:
// Find local gaussian curvature at all pixels from the local surface
// normals, previously computed from the range z(x,y) image.
// The gaussian curvature is estimated as the root mean square of the
// two curvatures along the x- and y- axes.

void
gevd_float_operators::SurfaceCurvature(const gevd_bufferxy& normal, gevd_bufferxy*& curvature)
{
  const int frame = 2;
  const int highx = normal.GetSizeX()-frame, highy = normal.GetSizeY()-frame;
  curvature = gevd_float_operators::Allocate(curvature, normal, bits_per_float);
  for (int j = frame; j < highy; j++)
    for (int i = frame; i < highx; i++) { // for all grid points
      float max_curv2 =         // along 0 degree
        cross_3d(*fvectorPixel(normal, i+1, j),
                 *fvectorPixel(normal, i-1, j)).squared_magnitude();
      float curv2 =             // along 90 degree
        cross_3d(*fvectorPixel(normal, i, j+1),
                 *fvectorPixel(normal, i, j-1)).squared_magnitude();
      if (max_curv2 < curv2) max_curv2 = curv2;
      curv2 =                   // along 45 degree
        cross_3d(*fvectorPixel(normal, i+1, j+1),
                 *fvectorPixel(normal, i-1, j-1)).squared_magnitude()/2;
      if (max_curv2 < curv2) max_curv2 = curv2;
      curv2 =                   // along 135 degree
        cross_3d(*fvectorPixel(normal, i+1, j-1),
                 *fvectorPixel(normal, i-1, j+1)).squared_magnitude()/2;
      if (max_curv2 < curv2) max_curv2 = curv2;
      floatPixel(*curvature, i, j) = vcl_sqrt(max_curv2);
    }
  gevd_float_operators::FillFrameX(*curvature, 0, frame); // zero curvature around frame.
  gevd_float_operators::FillFrameY(*curvature, 0, frame);
}


//  Find the tangent at pixel x,y by looking at neighboring pixels
//  low_x,low_y and high_x,high_y taken from along a given direction.
//  If no value is recorded at x,y or no value is recorded at both
//  low_x,low_y and high_x,high_y then no tangent value may be
//  calculated.  Otherwise, the tangent is calculated from the two
//  farthest apart pixels (preferably low_x,low_y and high_x,high_y).
//  Returned are the pixel distance between points used to calculate
//  the tangent and the change in z.  Together, these may be used to
//  form the tangent vector.  Note that the distance returned is
//  either 1.0 or 2.0, so this must be scaled by sqrt(2) if the points
//  are taken from along a diagonal.
static bool
_TangentComponents(const gevd_bufferxy& range,
                   int low_x, int low_y,
                   int x, int y,
                   int high_x, int high_y,
                   float no_value,          // value if there is no valid Z
                   float & distance,
                   float & delta_z )
{
  float low_z  = floatPixel( range, low_x, low_y );
  float z      = floatPixel( range, x, y );
  float high_z = floatPixel( range, high_x, high_y );

  if ( z == no_value || ( low_z == no_value && high_z == no_value ) )
    return false;

  if ( low_z == no_value ) {
    distance = 1.0;
    delta_z = high_z - z;
#ifdef DEBUG
    vcl_cout << "TC  low missing:  distance = " << distance << ", delta_z = " << delta_z << vcl_endl;
#endif
  }
  else if ( high_z == no_value ) {
    distance = 1.0;
    delta_z = z - low_z;
#ifdef DEBUG
    vcl_cout << "TC  high missing:  distance = " << distance << ", delta_z = " << delta_z << vcl_endl;
#endif
  }
  else {
    distance = 2.0;
    delta_z = high_z - low_z;
#ifdef DEBUG
    vcl_cout << "TC  neither missing:  distance = " << distance << ", delta_z = " << delta_z << vcl_endl;
#endif
  }
  return true;
}


//: Same as gevd_float_operators::SurfaceNormal with two exceptions.
// First, it explicitly recognizes and avoids pixels that have no
// range value.  These are often called "dropouts", hence the "D" at
// the end of the function name.  Second, it uses an optional
// "pixel_distance" value to put the inter-pixel distances in the same
// coordinate system as the range values.
//
void
gevd_float_operators::SurfaceNormalD(const gevd_bufferxy& range,
                                     gevd_bufferxy*& normal,
                                     float no_value,
                                     float pixel_distance)
{
  const int frame = 1;
  const int highx = range.GetSizeX()-frame, highy = range.GetSizeY()-frame;
  normal = gevd_float_operators::Allocate(normal, range, bits_per_ptr);
  normal->Clear();              // NULL vector on border

  for (int j = frame; j < highy; j++)
    for (int i = frame; i < highx; i++) { // for all grid points
      float d_x, d_z_x;
      float d_y, d_z_y;
      if (_TangentComponents(range, i-1, j, i, j, i+1, j, no_value, d_x, d_z_x) &&
          _TangentComponents(range, i, j-1, i, j, i, j+1, no_value, d_y, d_z_y) )
        {
          //vnl_vector<float> tx(3, d_x*pixel_distance,0.f,d_z_x);
          //vnl_vector<float> ty(3, 0.f,d_y*pixel_distance,d_z_y);
          vnl_vector<float> tx(3,0.0), ty(3,0.0);
          tx[0]=d_x*pixel_distance; tx[2]=d_z_x;
          ty[1]=d_y*pixel_distance; ty[2]=d_z_y;
          vnl_vector<float>* nz = new vnl_vector<float>(cross_3d(tx, ty));

#ifdef DEBUG
          vcl_cout << "Tx = " << tx << ",  Ty = " << ty << vcl_endl;
#endif
          float mag = nz->magnitude();
          if (mag != 0) {
            *nz /= mag;   // make unit vector
#ifdef DEBUG
            vcl_cout << "Normal = " << *nz << vcl_endl;
#endif
            fvectorPixel(*normal, i, j) = nz;
          }
          else {
            delete nz;
            fvectorPixel(*normal, i, j) = NULL;
          }
        }
      else {
        fvectorPixel(*normal, i, j) = NULL;
      }
    }
}

//  Approximate the curvature at pixel x,y along a given direction by
//  looking at neighboring pixels low_x,low_y and high_x,high_y taken
//  from along that direction.  If no normal is recorded at x,y or no normal
//  is recorded at both low_x,low_y and high_x,high_y then no
//  curvature may be calculated.  Otherwise, the curvature is
//  calculated from the two farthest apart pixels (preferably
//  low_x,low_y and high_x,high_y).  Returned are the pixel distance
//  between points used to calculate the curvature and the
//  unnormalized curvature approximation.
//
//  NOTE: This approximation breaks down at the boundaries for
//  high curvature, because while in the normal case the
//  curvature at point p is sampled (along whatever direction) at
//  points p-1 and p+1, and then assigned to point p, at the
//  boundaries it's sampled at p and p+1 (or p and p-1) and then
//  assigned to p when it *should* be assigned to some point
//  between the two.
static bool
_CurvatureInDir(const gevd_bufferxy& normal,
                const gevd_bufferxy& surface,
                int low_x, int low_y,
                int x, int y,
                int high_x, int high_y,
                float & sq_dist,
                float & sq_curve )
{
  vnl_vector<float> * low_norm  = fvectorPixel( normal, low_x, low_y );
  vnl_vector<float> * norm      = fvectorPixel( normal, x, y );
  vnl_vector<float> * high_norm = fvectorPixel( normal, high_x, high_y );

  float zval1, zval2, dz;
  int dx, dy;

  if ( norm == NULL || ( low_norm == NULL && high_norm == NULL ) )
    return false;

  if ( low_norm == NULL ) {
    // -rgc-   sq_dist = 1.0;
    zval1 = floatPixel(surface, high_x, high_y);
    zval2 = floatPixel(surface, x, y);
    dz = zval1 - zval2;
    dx = high_x - x;
    dy = high_y - y;
    sq_dist = (dx*dx) + (dy*dy) + (dz*dz);
    sq_curve = cross_3d( *high_norm, *norm ).squared_magnitude();
#ifdef DEBUG
    vcl_cout << "CinDir  low missing:  sq_dist = " << sq_dist
             << ",  high = " << *high_norm
             << ",  norm = " << *norm
             << ",  sq_curve = " << sq_curve << vcl_endl;
#endif
  }
  else if ( high_norm == NULL ) {
    // -rgc-   sq_dist = 1.0;
    zval1 = floatPixel(surface, x, y);
    zval2 = floatPixel(surface, low_x, low_y);
    dz = zval1 - zval2;
    dx = x - low_x;
    dy = y - low_y;
    sq_dist = (dx*dx) + (dy*dy) + (dz*dz);
    sq_curve = cross_3d( *norm, *low_norm ).squared_magnitude();
#ifdef DEBUG
    vcl_cout << "CinDir  high missing:  sq_dist = " << sq_dist
             << ",  norm = " << *norm
             << ",  low = " << *low_norm
             << ",  sq_curve = " << sq_curve << vcl_endl;
#endif
  }
  else {
    // -rgc-   sq_dist = 4.0;
    zval1 = floatPixel(surface, high_x, high_y);
    zval2 = floatPixel(surface, low_x, low_y);
    dz = zval1 - zval2;
    dx = high_x - low_x;
    dy = high_y - low_y;
    sq_dist = (dx*dx) + (dy*dy) + (dz*dz);
    sq_curve = cross_3d( *high_norm, *low_norm ).squared_magnitude();
#ifdef DEBUG
    vcl_cout << "CinDir  neither missing:  sq_dist = " << sq_dist
             << ",  high = " << *high_norm
             << ",  low = " << *low_norm
             << ",  sq_curve = " << sq_curve << vcl_endl;
#endif
  }
  return true;
}


//:
// Estimate the maximum curvature at all pixels from the local
// surface normals, previously computed from the range z(x,y) image.
// This is similar to SurfaceCurvature above with two exceptions.
// First, it explicitly recognizes image locations where no normal has
// been calculated.  Second, it normalizes the curvature by the
// pixel_distance, converting the curvature to a real-world quantity
// rather than a pixel quantity.
//
void
gevd_float_operators::SurfaceCurvatureD(const gevd_bufferxy& normal,
                                        const gevd_bufferxy& surface,
                                        gevd_bufferxy*& curvature,
                                        float dflt,
                                        float pixel_distance )
{
  const int frame = 2;
  const int highx = normal.GetSizeX()-frame, highy = normal.GetSizeY()-frame;
  curvature = gevd_float_operators::Allocate(curvature, normal, bits_per_float);
  float sq_unit_normalize = 1.0f/(pixel_distance*pixel_distance);

  for (int j = frame; j < highy; j++)
    for (int i = frame; i < highx; i++) { // for all grid points
      if ( fvectorPixel( normal, i, j ) == NULL ) {
        floatPixel(*curvature, i, j) = dflt;
        continue;
      }
#ifdef DEBUG
      vcl_cout << "i,j:" << i << ' ' << j << vcl_endl;
#endif
      float max_sq_curve = -1;
      float sq_dist, sq_curve;
      if ( _CurvatureInDir( normal, surface, i-1, j, i, j, i+1, j,  // horizontal
                            sq_dist, sq_curve ) ) {
        sq_curve *= sq_unit_normalize / sq_dist;
#ifdef DEBUG
        vcl_cout << "  sq_curve(h) = " << sq_curve << vcl_endl;
#endif
        if ( sq_curve > max_sq_curve ) max_sq_curve = sq_curve;
      }
      if ( _CurvatureInDir( normal, surface,i, j-1, i, j, i, j+1,  // vertical
                            sq_dist, sq_curve ) ) {
        sq_curve *= sq_unit_normalize / sq_dist;
#ifdef DEBUG
        vcl_cout << "  sq_curve(v) = " << sq_curve << vcl_endl;
#endif
       if ( sq_curve > max_sq_curve ) max_sq_curve = sq_curve;
      }
      if ( _CurvatureInDir( normal, surface, i-1, j-1, i, j, i+1, j+1, //  45 degrees
                            sq_dist, sq_curve ) ) {
        sq_curve *= sq_unit_normalize / sq_dist;
#ifdef DEBUG
        vcl_cout << "  sq_curve(45) = " << sq_curve << vcl_endl;
#endif
        if ( sq_curve > max_sq_curve ) max_sq_curve = sq_curve;
      }
      if ( _CurvatureInDir( normal, surface, i-1, j+1, i, j, i+1, j-1, // 135 degrees
                            sq_dist, sq_curve ) ) {
        sq_curve *= sq_unit_normalize / sq_dist;
#ifdef DEBUG
        vcl_cout << "  sq_curve(135) = " << sq_curve << vcl_endl;
#endif
        if ( sq_curve > max_sq_curve ) max_sq_curve = sq_curve;
      }
#ifdef DEBUG
      vcl_cout << "  max_sq_curve = " << max_sq_curve << vcl_endl;
#endif
      if ( max_sq_curve < 0 )
        floatPixel(*curvature, i, j) = dflt;
      else
        floatPixel(*curvature, i, j) = vcl_sqrt(max_sq_curve);
#ifdef DEBUG
      vcl_cout << "curvature in 1/inches: " << i << ' ' << j << ' ' << floatPixel(*curvature, i, j) << vcl_endl;
#endif
    }
  gevd_float_operators::FillFrameX(*curvature, dflt, frame);    // default curvature
  gevd_float_operators::FillFrameY(*curvature, dflt, frame);    // around frame.
}


//: Shrinks the from image by 2 and stores into to image, using Burt-Adelson reduction algorithm.
// Convolution with a 5-point kernel [(0.5-ka)/2, 0.25, ka, 0.25, (0.5-ka)/2]
// ka = 0.6  maximum decorrelation, wavelet for image compression.
// ka = 0.5  linear interpolation,
// ka = 0.4  Gaussian filter
// ka = 0.359375 min aliasing, wider than Gaussian
// The image indexes are mapped with: to.ij = from.ij / 2
// The image sizes are related by: to.size = (from.size+1)/2.

float
gevd_float_operators::ShrinkBy2(const gevd_bufferxy& from, gevd_bufferxy*& to,
                                const float burt_ka)
{
  const int sizeX = (from.GetSizeX() + 1) / 2;
  const int sizeY = (from.GetSizeY() + 1) / 2;
  to = gevd_float_operators::Allocate(to, from, 0, sizeX, sizeY);
  const float ka = burt_ka;
  const float kb = 0.25f;
  const float kc = (0.5f - burt_ka) / 2;

  int p = 0;                            // pipeline of 5 lines
  float* yline0 = new float[sizeX];     // shrink_by_2 along x
  float* yline1 = new float[sizeX];
  float* yline2 = new float[sizeX];
  float* yline3 = new float[sizeX];
  float* yline4 = new float[sizeX];
  gevd_float_operators::ShrinkBy2AlongX(from, p++, yline0, sizeX, ka, kb, kc);
  gevd_float_operators::ShrinkBy2AlongX(from, p++, yline1, sizeX, ka, kb, kc);
  gevd_float_operators::ShrinkBy2AlongX(from, p++, yline2, sizeX, ka, kb, kc);
  gevd_float_operators::ShrinkBy2AlongX(from, p++, yline3, sizeX, ka, kb, kc);
  gevd_float_operators::ShrinkBy2AlongX(from, p++, yline4, sizeX, ka, kb, kc);

  // shrink_by_2 along y-axis.
  for (int x = 0; x < sizeX; x++)
    floatPixel(*to, x, 0) = (ka * yline0[x] + // reflect at image border
                             2 * (kb * yline1[x] + kc * yline2[x]));
  for (int y = 1; y < sizeY; y++) {
    for (int x = 0; x < sizeX; x++)
      floatPixel(*to, x, y) = (ka * yline2[x] +
                               kb * (yline1[x] + yline3[x]) +
                               kc * (yline0[x] + yline4[x]));
    float* next0 = yline0;              // shift pipeline by 2 lines
    float* next1 = yline1;
    yline0 = yline2;
    yline1 = yline3;
    yline2 = yline4;
    yline3 = next0;
    yline4 = next1;
    if (y < sizeY-2) {
      gevd_float_operators::ShrinkBy2AlongX(from, p++, next0, sizeX, // new ylines
                                            ka, kb, kc);             // for pipeline
      gevd_float_operators::ShrinkBy2AlongX(from, p++, next1, sizeX, // new ylines
                                            ka, kb, kc);             // for pipeline
    } else {                            // reflect at image border
      vcl_memcpy(next0, yline1, sizeX*sizeof(float));
      vcl_memcpy(next1, yline0, sizeX*sizeof(float));
    }
  }
  delete[] yline0; delete[] yline1; delete[] yline2;
  delete[] yline3; delete[] yline4;
  return 1;
}

//:
// Shrinks the yline by 2 along the x-axis. We compute every 2 pixels,
// the convolution of the 5 pixels along x, with the 5-point kernel.
// [kc, kb, ka, kb, kc].

float
gevd_float_operators::ShrinkBy2AlongX(const gevd_bufferxy& from, const int y,
                                      float* yline, const int sizeX,
                                      const float ka, const float kb,
                                      const float kc)
{
  int p = 0;                            // setup pipeline of 5 x values
  float x0 = floatPixel(from, p++, y);
  float x1 = floatPixel(from, p++, y);
  float x2 = floatPixel(from, p++, y);
  float x3 = floatPixel(from, p++, y);
  float x4 = floatPixel(from, p++, y);
  yline[0] = ka * x0 + 2 * (kb * x1 + kc * x2); // reflect at border
  for (int x = 1; x < sizeX; x++) {
    yline[x] = (ka * x2 +               // shrink_by_2 along x axis
                kb * (x1 + x3) +
                kc * (x0 + x4));
      x0 = x2;                                  // shift by 2
      x1 = x3;
      x2 = x4;
    if (x < sizeX-2) {
      x3 = floatPixel(from, p++, y);
      x4 = floatPixel(from, p++, y);
    } else {
      x3 = x1;
      x4 = x0;
    }
  }
  return 1;
}


void
PrintAllPipes( float * y_s[],
               float * w_s[],
               int length )
{
  for (int i=0; i<5; i++ ) {
    vcl_cout << "\nPipe " << i << vcl_endl;
    for (int j=0; j<length; j++ ) {
      vcl_cout << j << ",  yline[j] = " << y_s[i][j] << ",  wline[j] = "
               << w_s[i][j] << vcl_endl;
    }
  }
}


//:
// Same as ShrinkBy2 except that it properly handles pixels that
// have usable value ("dropouts" --- hence the appended "_D" in the
// name).  These are pixel values "no_value".  One problem with this
// function is that one pixel wide structures may or may not be
// retained in the shrunken image, depending on the position of the
// structure.  This should not be a problem for the initial
// application.

void
gevd_float_operators::ShrinkBy2_D(const gevd_bufferxy& from,
                                  gevd_bufferxy*& to,
                                  float no_value,
                                  float burt_ka )
{
  const int sizeX = (from.GetSizeX() + 1) / 2;
  const int sizeY = (from.GetSizeY() + 1) / 2;
  to = gevd_float_operators::Allocate(to, from, 0, sizeX, sizeY);

  //  Build a kernel of smoothing weights.  kernel[2] is the center.
  float kernel[5];
  kernel[2] = burt_ka;
  kernel[1] = kernel[3] = 0.25f;
  kernel[0] = kernel[4] = (0.5f - burt_ka) / 2;

  //  Smoothing and subsampling will occur first along each row and
  //  then between rows.  So, a pipeline of 5 smoothed and subsampled
  //  rows is needed.  There is a pipeline of rows and a pipeline of
  //  associated weights.  Allocate these and, in addition, allocate
  //  and fill space for an empty row.
  //
  float * yline[5];
  for (int i=0; i<5; i++ ) yline[i] = new float[sizeX];
  float * wline[5];
  for (int i=0; i<5; i++ ) wline[i] = new float[sizeX];
  float * y_empty = new float[sizeX];
  float * w_empty = new float[sizeX];

  //  Set the values for the empty rows.  These are used for the top
  //  and bottom of the image.
  //
  for (int i=0; i<sizeX; i++ ) {
    y_empty[i] = no_value;
    w_empty[i] = 0.0;
  }

  //  Fill the top two rows of the pipelines with "empty" values.
  //  This will allow the algorithm to mimic having a buffer of
  //  "no_value" surrounding the image.
  //
  vcl_memcpy(yline[0], y_empty, sizeX*sizeof(float));
  vcl_memcpy(wline[0], w_empty, sizeX*sizeof(float));
  vcl_memcpy(yline[1], y_empty, sizeX*sizeof(float));
  vcl_memcpy(wline[1], w_empty, sizeX*sizeof(float));

  //  Fill the center and bottom half of the pipelines with the top
  //  three rows of the image.
  //
  gevd_float_operators::ShrinkBy2AlongX_D(from, from.GetSizeX(), sizeX, 0,
                                          kernel, no_value, yline[2], wline[2]);
  gevd_float_operators::ShrinkBy2AlongX_D(from, from.GetSizeX(), sizeX, 1,
                                          kernel, no_value, yline[3], wline[3]);
  gevd_float_operators::ShrinkBy2AlongX_D(from, from.GetSizeX(), sizeX, 2,
                                          kernel, no_value, yline[4], wline[4]);
  int p = 3;

  //  Do the actual weighted smoothing and subsampling.  Only record a
  //  value at a pixel if the summed weights are above 0.5.  After all
  //  subsampled values are created in a row, shift the pipeline down.
  //
  for (int y=0; y<sizeY; y++ ) {
#ifdef DEBUG
    vcl_cout << "\nNew row:  y= " << y << "\nHere are the pipes.\n";
    PrintAllPipes( yline, wline, sizeX );
#endif
    for (int x=0; x<sizeX; x++) {
      float sum_w = 0, sum_z = 0;
      for (int i=0; i<5; i++ ) {
        sum_w += kernel[i] * wline[i][x];
        sum_z += kernel[i] * yline[i][x];
      }
#ifdef DEBUG
      vcl_cout << "Assigning:  x,y = " << x << ", " << y << "  ";
#endif
      floatPixel(*to, x, y) = sum_w < 0.5 ? no_value : sum_z / sum_w;
#ifdef DEBUG
      vcl_cout << ",  sum_w = " << sum_w << ",  value = " << floatPixel(*to, x, y) << vcl_endl;
#endif
    }

    //  Shift the pipeline down by two rows.  Where it overlaps the
    //  bottom of the image use empty values.
    //
    float* prev_y0 = yline[0];
    float* prev_y1 = yline[1];
    float* prev_w0 = wline[0];
    float* prev_w1 = wline[1];
    for (int i=0; i<3; i++ ) {
      yline[i] = yline[i+2];
      wline[i] = wline[i+2];
    }
    yline[3] = prev_y0;
    wline[3] = prev_w0;
    yline[4] = prev_y1;
    wline[4] = prev_w1;
    if ( p < from.GetSizeY() )
      ShrinkBy2AlongX_D(from, from.GetSizeX(), sizeX, p++, kernel,
                        no_value, yline[3], wline[3] );
    else {
      vcl_memcpy(yline[3], y_empty, sizeX*sizeof(float));
      vcl_memcpy(wline[3], w_empty, sizeX*sizeof(float));
    }
    if ( p < from.GetSizeY() )
      ShrinkBy2AlongX_D(from, from.GetSizeX(), sizeX, p++, kernel,
                        no_value, yline[4], wline[4] );
    else {
      vcl_memcpy(yline[4], y_empty, sizeX*sizeof(float));
      vcl_memcpy(wline[4], w_empty, sizeX*sizeof(float));
    }
  }

  //  Delete the scratch memory.
  for (int i=0; i<5; i++ ) {
    delete[] yline[i];
    delete[] wline[i];
  }
  delete[] y_empty;
  delete[] w_empty;
}

void
PrintPipe( float values[] )
{
  for (int i=0; i<4; i++ ) vcl_cout << values[i] << ", ";
  vcl_cout << values[4];
}

//:
// Create a smoothed and subsampled array of x values in the given
// row.  This will return the weighted (but unnormalized) values (in
// yline) and the weights (wline).
//
void
gevd_float_operators::ShrinkBy2AlongX_D(const gevd_bufferxy& from,
                                        int from_sizeX,
                                        int sizeX,
                                        int y,
                                        float kernel[],
                                        float no_value,
                                        float* yline,
                                        float* wline )
{
#ifdef DEBUG
  vcl_cout << "\nIn gevd_float_operators::ShrinkBy2AlongX_D:\n";

  vcl_cout << "Here is the original data:\n";
  for (int xx=0; xx<from.GetSizeX(); xx++ )
    vcl_cout << xx << ":  " << floatPixel( from, xx, y ) << vcl_endl;
#endif

  // setup pipeline of 5 x values
  float xs[5];
  xs[0] = no_value;
  xs[1] = no_value;
  xs[2] = floatPixel( from, 0, y );
  xs[3] = floatPixel( from, 1, y );
  xs[4] = floatPixel( from, 2, y );
  int p = 3;

  for (int x = 0; x < sizeX; x++ ) {
#ifdef DEBUG
    vcl_cout << "Data pipe:  ";  PrintPipe( xs ); vcl_cout << vcl_endl;
#endif
    wline[x] = yline[x] = 0.0;
    for (int i = 0; i<5; i++ ) {
      if ( xs[i] != no_value ) {
        wline[x] += kernel[i];
        yline[x] += kernel[i] * xs[i];
      }
    }
#ifdef DEBUG
    vcl_cout << "x = " << x << ",  yline[x] = " << yline[x]
             << ", wline[x] = " << wline[x] << vcl_endl;
#endif
    for (int i=0; i<3; i++ ) xs[i] = xs[i+2];
    xs[3] = (p < from_sizeX) ? floatPixel(from, p++, y) : no_value;
    xs[4] = (p < from_sizeX) ? floatPixel(from, p++, y) : no_value;
  }
}


//: Expands the from image by 2 and store into to image, using Burt-Adelson expansion algorithm.
// Convolution with a 5-point kernel [(0.5-ka), 0.5, 2*ka, 0.5, (0.5-ka)]
// ka = 0.6  maximum decorrelation, low-pass filter for image compression.
// ka = 0.5  linear interpolation,
// ka = 0.4  Gaussian filter, smoothing at tail.
// ka = 0.3  wider than Gaussian, for more smoothing.
// The image indexes are mapped with: to.ij = from.ij * 2
// The image sizes are related by: to.size = from.size * 2.

float
gevd_float_operators::ExpandBy2(const gevd_bufferxy& from, gevd_bufferxy*& to,
                                const float burt_ka)
{
  const int sizeX = 2 * from.GetSizeX();
  const int sizeY = 2 * from.GetSizeY();
  to = gevd_float_operators::Allocate(to, from, 0, sizeX, sizeY);
  const float ka = burt_ka * 2;
  const float kb = 0.5f;
  const float kc = 0.5f - burt_ka;
  int p = 0;                            // pipeline of 3 lines
  float* yline0 = new float[sizeX];     // to cache ExpandBy2AlongX
  float* yline1 = new float[sizeX];
  float* yline2 = new float[sizeX];
  gevd_float_operators::ExpandBy2AlongX(from, p++, yline1, sizeX, ka, kb, kc);
  gevd_float_operators::ExpandBy2AlongX(from, p++, yline2, sizeX, ka, kb, kc);
  vcl_memcpy(yline0, yline2, sizeX*sizeof(float));// first line is wrapped

  // Convolve and expand along x-axis.
  for (int y = 0; y < sizeY; y += 2) {
    int yy = y+1;
    for (int x = 0; x < sizeX; x++) {
      floatPixel(*to, x, y) = (ka * yline1[x] +     // even index
                               kc * (yline0[x] + yline2[x]));
      floatPixel(*to, x, yy) = kb * (yline1[x] + yline2[x]); // odd index
    }
    float* next = yline0;
    yline0 = yline1;
    yline1 = yline2;
    yline2 = next;
    if (y < sizeY-4)
      gevd_float_operators::ExpandBy2AlongX(from, p++, next, sizeX, ka, kb, kc);
    else                        // last line is wrapped
      vcl_memcpy(next, yline0, sizeX*sizeof(float));
  }
  delete[] yline0;
  delete[] yline1;
  delete[] yline2;
  return 1;
}

//: Expands the yline by 2 along the x-axis.
// Interpolation is done by convolution with pixels at integral indexes only.

float
gevd_float_operators::ExpandBy2AlongX(const gevd_bufferxy& from, const int y,
                                      float* yline, const int sizeX,
                                      const float ka, const float kb,
                                      const float kc)
{
  int p = 0;
  float x1 = floatPixel(from, p++, y);  // setup pipeline of values
  float x2 = floatPixel(from, p++, y);
  float x0 = x2;                // wrap at border
  for (int x = 0; x < sizeX; x += 2) {
    yline[x] = ka * x1 + kc * (x0 + x2); // even index
    yline[x+1] = kb * (x1 + x2); // odd index
    x0 = x1;
    x1 = x2;
    if (x < sizeX-4)
      x2 = floatPixel(from, p++, y);
    else                        // wrap at border
      x2 = x0;
  }
  return 1;
}


//:
// Compute the pyramid by shrinking data sequence
// by 2, nlevels-1 times, and return final shrunk length,
// and reset number of levels in pyramid. O(n) time.
// Coarse to fine is stored from left to right.
// The left and right trim borders are set to 0.

int
gevd_float_operators::Pyramid(const float* data, const int length,
                              float*& pyramid, int& nlevels, int trim,
                              const float burt_ka)
{
  const int MINLENGTH = 16;     // coarsest length for reliable correlation
  if (!pyramid)
    pyramid = new float[length << 1]; // allocate or reuse space
  int i, ii;
  for (ii = 0; ii < length; ii++) // avoid boundary conditions
    pyramid[ii] = 0;
  for (i = 0; i < trim; i++, ii++) // trim left border to 0.
    pyramid[ii] = 0;
  for ( ; i < length-trim; i++, ii++) // level = 0
    pyramid[ii] = data[i];
  for ( ; i < length; i++, ii++) // trim right border to 0.
    pyramid[ii] = 0;
  int l, len1, len2;
  float *from, *to;             // pointers
  for (l = 1, len1 = length, len2 = length >> 1;
       l < nlevels && len2 > MINLENGTH; l++) {
    from = pyramid + len1, to = pyramid + len2;
    len1 = ShrinkBy2(from, len1, to, burt_ka);
    len2 = len1 >> 1;
  }
  nlevels = l;
  return len1;
}


int
gevd_float_operators::ShrinkBy2(const float* from, const int length,
                                float*& to, const float burt_ka)
{
  const int slength = length >> 1;
  if (!to)
    to = new float[slength];    // allocate or reuse space
  const float ka = burt_ka;
  const float kb = 0.25f;
  const float kc = (0.5f - burt_ka) / 2;
  int p = 0;                            // setup pipeline of 5 x values
  float x0 = from[p++];
  float x1 = from[p++];
  float x2 = from[p++];
  float x3 = from[p++];
  float x4 = from[p++];
  to[0] = ka * x0 + 2 * (kb * x1 + kc * x2);    // reflect at border
  for (int x = 1; x <  slength; x++) {
    to[x] = (ka * x2 +          // shrink_by_2 along x axis
             kb * (x1 + x3) +
             kc * (x0 + x4));
      x0 = x2;                                  // shift by 2
      x1 = x3;
      x2 = x4;
    if (x < slength-2) {
      x3 = from[p++];
      x4 = from[p++];
    } else {
      x3 = x1;
      x4 = x0;
    }
  }
  return slength;
}


static float haar2 [] =
{                                               // Haar wavelet
  0.7071067811865f,                             // 1/sqrt(2.0)
  0.7071067811865f,
  0.f
};

static float daubechies4 [] =
{
  0.4829629131445341f,                           // Daubechies wavelet
  0.8365163037378079f,
  0.2241438680420134f,
 -0.1294095225512604f,
  0
};

static float daubechies6 [] =
{
  0.3326705529500825f,
  0.8068915093110924f,
  0.4598775021184914f,
 -0.1350110200102546f,
 -0.0854412738820267f,
  0.0352262918857095f,
  0
};

static float daubechies8 [] =
{
  0.2303778133088964f,
  0.7148465705529154f,
  0.6308807679398587f,
 -0.0279837694168599f,
 -0.1870348117190931f,
  0.0308413818355607f,
  0.0328830116668852f,
 -0.0105974017850690f,
  0
};

static float daubechies10 [] =
{
  0.1601023979741929f,
  0.6038292697971895f,
  0.7243085284377726f,
  0.1384281459013203f,
 -0.2422948870663823f,
 -0.0322448695846381f,
  0.0775714938400459f,
 -0.0062414902127983f,
 -0.0125807519990820f,
  0.0033357252854738f,
  0
};

static float daubechies12 [] =
{
  0.111540743350f,
  0.494623890398f,
  0.751133908021f,
  0.315250351709f,
 -0.226264693965f,
 -0.129766867567f,
  0.097501605587f,
  0.027522865530f,
 -0.031582039318f,
  0.000553842201f,
  0.004777257511f,
 -0.001077301085f,
  0
};

static float daubechies20 [] =
{
  0.026670057901f,
  0.188176800078f,
  0.527201188932f,
  0.688459039454f,
  0.281172343661f,
 -0.249846424327f,
 -0.195946274377f,
  0.127369340336f,
  0.093057364604f,
 -0.071394147166f,
 -0.029457536822f,
  0.033212674059f,
  0.003606553567f,
 -0.010733175483f,
  0.001395351747f,
  0.001992405295f,
 -0.000685856695f,
 -0.000116466855f,
  0.000093588670f,
 -0.000013264203f,
  0
};

static float coifman9 [] =
{
  0.019849565349356158f,                        // ***double-check as Coifman
 -0.04309091740554781f,                         // wavelets ****
 -0.05188793647806494f,
  0.2932311998087948f,
  0.5637961774509238f,
  0.2932311998087948f,
 -0.05188793647806494f,
 -0.04309091740554781f,
  0.019849565349356158f,
  0
};

static float coifman11 [] =
{
  0.007987761489921101f,
  0.02011649866148413f,
 -0.05015758257647976f,
 -0.12422330961337678f,
  0.29216982108655865f,
  0.7082136219037853f,
  0.29216982108655865f,
 -0.12422330961337678f,
 -0.05015758257647976f,
  0.02011649866148413f,
  0.007987761489921101f,
  0
};

static float coifman15 [] =
{
 -0.0012475221f,
 -0.0024950907f,
  0.0087309530f,
  0.0199579580f,
 -0.0505290000f,
 -0.1205509700f,
  0.2930455800f,
  0.7061761600f,
  0.2930455800f,
 -0.1205509700f,
 -0.0505290000f,
  0.0199579580f,
  0.0087309530f,
 -0.0024950907f,
 -0.0012475221f,
  0
};

static float dual_wavelet [20] = {0};

//: Looks up and stores the wavelet pair in (lo-filter, hi-filter).
// The wavelet number is 2 for Haar wavelet, 4-20 even numbers for Daubechies
// wavelet, and 9-15 odd numbers for symmetric Coifman wavelets.

bool
gevd_float_operators::FindWavelet(const int waveletno,
                                  float*& lo_filter, float*& hi_filter, int& ncof)
{
  ncof = waveletno;
  switch (waveletno) {
   case 2:
    lo_filter = haar2;
    break;
   case 4:
    lo_filter = daubechies4;
    break;
   case 6:
    lo_filter = daubechies6;
    break;
   case 8:
    lo_filter = daubechies8;
    break;
   case 10:
    lo_filter = daubechies10;
    break;
   case 12:
    lo_filter = daubechies12;
    break;
   case 20:
    lo_filter = daubechies20;
    break;
   case 9:                                       // wavelets from epic
    lo_filter = coifman9;
    break;
   case 11:
    lo_filter = coifman11;
    break;
   case 15:
    lo_filter = coifman15;
    break;
   default:
    ncof = 0;
    lo_filter = hi_filter = NULL;
    vcl_cerr << "Unknown wavelet: " << waveletno << vcl_endl;
    return false;
  }
  // find hi-filter wavelet, dual of the lo-filter wavelet
  if (lo_filter) {
    hi_filter = dual_wavelet;
    if ((waveletno%2) == 0) {
      int sign = -1;                            // reverse and
      for (int k = 0; k < ncof; k++) {          // flip sign on odd coefts
        hi_filter[ncof-1-k] = sign * lo_filter[k];
        sign = - sign;
      }
    } else {                                    // odd filter is symmetric
      int sign = 1;
      int ctr = ncof/2;                         // flip sign on odd coefts
      for (int k = 0; k <= ncof/2; k++) {       // starting from center
        hi_filter[ctr+k] = sign * lo_filter[ctr-k];
        hi_filter[ctr-k] = sign * lo_filter[ctr+k];
        sign = - sign;
      }
      vcl_cerr << "Scale factor need to be fixed up too!!!\n";
    }
    // find area of lo_filter and hi_filter
    float lo_area = 0;
    float hi_area = 0;
    for (int k = 0; k < ncof; k++) {
      lo_area += lo_filter[k];
      hi_area += hi_filter[k];
    }
    lo_filter[ncof] = lo_area;
    hi_filter[ncof] = hi_area;
  }
#ifdef DEBUG
  vcl_cout << "lo-filter wavelet " << waveletno << ':'; // print wavelets
  for (int i = 0; i < ncof; i++)
    vcl_cout << ' ' << lo_filter[i];
  vcl_cout << "\nhi-filter wavelet " << waveletno << ':';
  for (int i = 0; i < ncof; i++)
    vcl_cout << ' ' << hi_filter[i];
  vcl_cout << vcl_endl;
#endif
  return true;
}

//:
// Convolution with wavelets (lo_filter, hi_filter) and gather results into
// consecutive blocks, with convolution of lo-filter (resp. hi-filter)
// on the sides of low (resp. high) indices.
// Wrap around edges of the array is done with modulo(n) replaced by
// bit masking with n-1, when n is a power of 2.
// Assumes n >= 4.

void
gevd_float_operators::WaveletTransformStep(float* array, const int n,
                                           const bool forwardp,
                                           const float* lo_filter,
                                           const float* hi_filter,
                                           const int ncof,
                                           float* wksp)
{
  for (int j = 0; j < n; j++)
    wksp[j] = 0;                                // clear workspace
  int nmid = n / 2;                             // round off towards 0
  int nmod = nmid * 2;                          // even and <= n.
  if (forwardp) {                               // forward transform
    for (int i=0, ii=0; i < nmod; i +=2, ++ii)  // every pair
      for (int k = 0; k < ncof; k++) {          // convolution with filters
        int j = (i + k) % nmod;                 // wrap around
        // int j = (i + k) & (nmod - 1);        // when n is power of 2
        wksp[ii] += lo_filter[k] * array[j]; // lo-filter results
        wksp[ii+nmid] += hi_filter[k] * array[j]; // hi-filter results
      }
    float scale = vcl_max(lo_filter[ncof], hi_filter[ncof]);
    for (int j = 0; j < nmod; j++)             // normalize results.
      wksp[j] /= scale;
  } else {                                      // inverse transform
    for (int i=0, ii=0; i < nmod; i+=2, ++ii) { // every pair
      float lo = array[ii];
      float hi = array[ii+nmid];
      for (int k = 0; k < ncof; k++) {          // multiplication with inverse
        int j = (i + k) % nmod;                 // matrix, or its transpose
        // int j = (i + k) & (nmod - 1);        // when n is power of 2
        wksp[j] += (lo_filter[k] * lo +
                    hi_filter[k] * hi);
      }
    }
    float scale = vcl_max(lo_filter[ncof], hi_filter[ncof]);
    for (int j = 0; j < nmod; j++)              // unnormalize results.
      wksp[j] *= scale;
  }
  for (int j = 0; j < nmod; j++)                // copy only nmod results
    array[j] = wksp[j];                         // back to array
}


//: Compute the forward/inverse wavelet transform of a 1d array of data.
// Convolution with low (resp. high) filter is stored on the side
// of low (resp. high) indices.

bool
gevd_float_operators::WaveletTransform(float* array, const int n,
                                       const bool forwardp, int nlevels,
                                       const int waveletno)
{
  if (n >= 4) {
    float* lo_filter = NULL;
    float* hi_filter = NULL;
    int ncof = 0;
    FindWavelet(waveletno, lo_filter, hi_filter, ncof);
#ifdef DEBUG
    vcl_cout << "Input:";
    for (int i = 0; i < n; i++)
      vcl_cout << ' ' << array[i];
    vcl_cout << vcl_endl;
#endif

    float* wksp = new float[n];
    if (forwardp) {                             // forward transform
      for (int nn = n; nn >= 4 && nlevels > 0; nn /= 2, nlevels--)
        WaveletTransformStep(array, nn, forwardp,
                             lo_filter, hi_filter, ncof,
                             wksp);
    } else {                                    // inverse transform
      const int sz = int(vcl_log(double(n))/vcl_log(2.0));
      int* sizes = new int[sz];
      int s = 0;
      for (int nn = n; nn >= 4 && nlevels > 0; nn /= 2, nlevels--, s++)
        sizes[s] = nn;                          // reverse sequence of sizes nn
      for (s--; s >= 0; s--)                    // to undo forward transform
        WaveletTransformStep(array, sizes[s], forwardp,
                             lo_filter, hi_filter, ncof,
                             wksp);
      delete[] sizes;
    }
    delete[] wksp;
#ifdef DEBUG
    vcl_cout << "Output:";
    for (int i = 0; i < n; i++)
      vcl_cout << ' ' << array[i];
    vcl_cout << vcl_endl;
#endif
  }
  return true;
}

//: Find pyramid of low/high pass components, using wavelet transform.

void
gevd_float_operators::LowHighPyramid(float* highPass, float* lowPass,
                                     int n, int nlevels,
                                     const float* lo_filter,
                                     const float* hi_filter,
                                     int ncof,
                                     float* wksp)
{
  const bool forwardp = true;   // use forward wavelet transform
  int nn = n;
  for (int l = nlevels;         // to find pyramid
       nn >= 4 && l > 0; nn /= 2, l--) {
    gevd_float_operators::WaveletTransformStep(highPass, nn, forwardp,
                                               lo_filter, hi_filter, ncof,
                                               wksp);
    int m = nn / 2;
    float* lows = lowPass + m;
    for (int j = 0; j < m; j++)         // copy over the low pass components
      lows[j] = highPass[j];            // for pyramid
    if ((nn % 2) == 1) {                // delete boundary effects
      int bound = nn-1;                 // because of odd size
      highPass[bound] = highPass[bound-1];
      lowPass[bound] = lowPass[bound-1];
    }
  }
  const float LO = lowPass[nn];
  for (int j = 0; j < nn; j++) {
    highPass[j] = 0;
    lowPass[j] = LO;
  }
}


//: n-dimensional wavelet transform of an n-dimensional array.
// Convolution with low (resp. high) filter is stored on the side
// of low (resp. high) indices.
// Assumes data are stored consecutively with right-most index varying
// fastest, consistent with convention in C m(i,j) = m[i][j].
// This version does a 1d-FWT recursively on each dimension,
// rather than an nd-FWT on each recursive nd sub-block that are convolved
// with lo-filters.

bool
gevd_float_operators::WaveletTransformByIndex(float* array,
                                              const int* dims, const int ndim,
                                              const bool forwardp, int nlevels,
                                              const int waveletno)
{
  int ntot = 1, maxn = 0;
  {for (int d = 0; d < ndim; d++) {
    int dim = dims[d];
    ntot *= dim;
    if (dim > maxn) maxn = dim;
  }}
  float* lo_filter = NULL;
  float* hi_filter = NULL;
  int ncof = 0;
  if (!FindWavelet(waveletno, lo_filter, hi_filter, ncof)) // look up wavelets
    return false;
  float* buffer = new float[maxn];              // working buffers for
  float* wksp = new float[maxn];                // 1d wavelet transformation
  const int sz = int(vcl_log(double(maxn))/vcl_log(2.0));
  int* sizes = new int[sz];                     // cache sizes in pyramid

#ifdef DEBUG
  vcl_cout << "Input:";
  for (int i = 0; i < ntot; i++)
    vcl_cout << ' ' << array[i];
  vcl_cout << vcl_endl;
#endif

  int nprev = 1;
  for (int d = ndim-1; d >= 0; d--) {           // dimension varies most, first
    int n = dims[d];
    int nnew = n * nprev;
    if (n >= 4) {
      for (int i2 = 0; i2 < ntot; i2 += nnew)
        for (int i1 = 0; i1 < nprev; i1++) {
          int i3 = i1 + i2;
          {for (int k = 0; k < n; k++) {
            buffer[k] = array[i3];              // copy data to convolve
            i3 += nprev;
          }}
          if (forwardp) {                       // forward 1d transform
            for (int nn = n, l = nlevels;
                 nn >= 4 && l > 0;
                 nn /= 2, l--)
              WaveletTransformStep(buffer, nn, forwardp,
                                   lo_filter, hi_filter, ncof,
                                   wksp);
          } else {                              // inverse 1d transform
            int s = 0;
            for (int nn = n, l = nlevels;
                 nn >= 4 && l > 0;
                 nn /= 2, l--, s++)
              sizes[s] = nn;                    // reverse sizes to undo
            for (s--; s >= 0; s--)              // forward transform
              WaveletTransformStep(buffer, sizes[s], forwardp,
                                   lo_filter, hi_filter, ncof,
                                   wksp);
          }
          i3 = i1 + i2;                         // copy back results
          for (int k = 0; k < n; k++) {
            array[i3] = buffer[k];
            i3 += nprev;
          }
        }
    }
    nprev = nnew;
  }
  delete[] buffer;                             // free working arrays
  delete[] wksp;
  delete[] sizes;
#ifdef DEBUG
  vcl_cout << "Output:";
  for (int i = 0; i < ntot; i++)
    vcl_cout << ' ' << array[i];
  vcl_cout << vcl_endl;
#endif
  return true;
}

// one step of n-dimensional wavelet transform of an n-dimensional array.

void
gevd_float_operators::WaveletTransformStep(float* array,
                                           const int* dims, const int ndim,
                                           const bool forwardp,
                                           const float* lo_filter,
                                           const float* hi_filter,
                                           const int ncof,
                                           float* buffer, float* wksp)
{
  int ntot = dims[ndim];
  int nprev = 1;
  for (int d = ndim-1; d >= 0; d--) {   // dimension varies most, first
    int n = dims[d];
    int nnew = n * nprev;
    if (n >= 4) {
      for (int i2 = 0; i2 < ntot; i2 += nnew)
        for (int i1 = 0; i1 < nprev; i1++) {
          int i3 = i1 + i2;
          {for (int k = 0; k < n; k++) {
            buffer[k] = array[i3];              // copy data to convolve
            i3 += nprev;
          }}
          WaveletTransformStep(buffer, n, forwardp,
                               lo_filter, hi_filter, ncof,
                               wksp);
          i3 = i1 + i2;                         // copy back results
          for (int k = 0; k < n; k++) {
            array[i3] = buffer[k];
            i3 += nprev;
          }
        }
    }
    nprev = nnew;
  }
}


//: Recursively copies elements of n-dimensional arrays.
// Extra elements that can not fit in both arrays are not
// read or written. A flag fullp (default = true) is used to copy from/to
// a full n-dimensional array or only a sub-block of the array.

void
gevd_float_operators::CopyNdRecursive(const float* from_array,
                                      const int from_size, const int* from_dims,
                                      float* to_array,
                                      const int to_size, const int* to_dims,
                                      const int ndim,
                                      const bool fullp)
{
  if (ndim == 1) {                              // end of recursion
    int size = vcl_min(from_size, to_size);
    for (int i = 0; i < size; i++)              // copy 1d array for
      to_array[i] = from_array[i];              // common indices only
  } else {
    int from_n = from_dims[0], to_n = to_dims[0];
    int from_nsize = from_size / from_n;
    int to_nsize = to_size / to_n;
    int n = vcl_min(from_n, to_n);
    for (int i = 0; i < n; i++) {               // copy n common subarrays
      CopyNdRecursive(from_array, from_nsize, from_dims+1,
                      to_array, to_nsize, to_dims+1,
                      ndim-1, fullp);
      if (fullp) {
        from_array += from_nsize;
        to_array += to_nsize;
      } else {
        int block_size = vcl_max(from_nsize, to_nsize);
        from_array += block_size;               // inc pointer of arrays
        to_array += block_size;                 // to next block
      }
    }
  }
}


//: n-dimensional wavelet transform of an n-dimensional array.
// Convolution with low (resp. high) filter is stored on the side
// of low (resp. high) indices.
// Assumes data are stored consecutively with right-most index varying
// fastest, consistent with convention in C m(i,j) = m[i][j].
// This version decomposes only the n-dimensional sub-block of
// low frequency convolutions. The wavelet are self-similar in
// n-dimensional space, rather than elongated.

bool
gevd_float_operators::WaveletTransformByBlock(float* array,
                                              const int* dims, const int ndim,
                                              const bool forwardp, int nlevels,
                                              const int waveletno)
{
  int ntot = 1, maxn = 0;
  {for (int d = 0; d < ndim; d++) {
    int dim = dims[d];
    ntot *= dim;
    if (dim > maxn) maxn = dim;
  }}

  float* lo_filter = NULL;
  float* hi_filter = NULL;
  int ncof = 0;
  if (!FindWavelet(waveletno, lo_filter, hi_filter, ncof)) // look up wavelets
    return false;

  int** level_dims = new int*[nlevels];
  level_dims[0] =  new int[ndim+1];
  for (int d = 0; d < ndim; d++)
    level_dims[0][d] = dims[d];
  level_dims[0][ndim] = ntot;
  for (int l = 1; l < nlevels; l++) {
    level_dims[l] = new int[ndim+1];
    int n = 1;
    for (int d = 0; d < ndim; d++) {
      int nd = level_dims[l-1][d];
      if (nd >= 4)
        nd /= 2;
      level_dims[l][d] = nd;
      n *= nd;
    }
    level_dims[l][ndim] = n;
  }
  if (!forwardp) {                              // reverse order of the
    int* swap = NULL;                           // dimensions of sub_array
    for (int l = 0; l < nlevels/2; l++) {
      swap = level_dims[l];
      level_dims[l] = level_dims[nlevels-1-l];
      level_dims[nlevels-1-l] = swap;
    }
  }
  float* sub_array = new float[ntot];           // sub-block of low frequency
  float* buffer = new float[maxn];              // working buffers for
  float* wksp = new float[maxn];                // 1d wavelet transformation
  {for (int l = 0; l < nlevels; l++) {
    int* sub_dims = level_dims[l];
    int n = sub_dims[ndim];
    CopyNdRecursive(array, ntot, dims,
                    sub_array, n, sub_dims,     // consecutive elmts.
                    ndim);
    WaveletTransformStep(sub_array, sub_dims, ndim,
                         forwardp,
                         lo_filter, hi_filter, ncof,
                         buffer, wksp);
    CopyNdRecursive(sub_array, n, sub_dims,
                    array, ntot, dims,
                    ndim);
  }}
  delete[] sub_array;                          // free working arrays
  for (int l = 0; l < nlevels; l++)
    delete[] level_dims[l];
  delete[] level_dims;
  delete[] buffer;
  delete[] wksp;
  return true;
}

#if 0 // unimplemented - TODO
int
gevd_float_operators::DeleteMixedComponents
(float* wave, // delete wavelet coefts
 const int* dims, const int ndim,
 const int nlevels)
{
}

int
gevd_float_operators::TruncateHighFrequencies
(float* wave,
 const int* dims, const int ndim,
 const int nlevels,                  // throw all small
 const float threshold=0.1)          // components
{
}

int
gevd_float_operators::TruncateLowestFrequency
(float* wave,
 const int* dims, const int ndim,
 const int nlevels,
 float& average)                     // uniform average
{
}
#endif


void
gevd_float_operators::TestWavelets()
{
#if 0 // 1d testing commented out
  vcl_cout << "Testing wavelet transforms on 1d buffers\n";
  for (int n = 2; n <= 16; n++) {
    float* data = new float[n];
    for (int k = 2; k <= 12; k+=2) {
      for (int i = 0; i < n; i++)
        data[i] = i+1;
      wavelet_transform(data, n, true, k, 2);
      wavelet_transform(data, n, false, k, 2);
      float max_err = 0;
      for (int i = 0; i < n; i++) {
        float err = vcl_fabs(data[i] - i-1);
        if (err > max_err)
          max_err = err;
      }
      vcl_cout << "  |data| = " << n
               << "  |wavelet| = " << k
               << "  |error| = " << max_err << vcl_endl;
    }
    delete[] data;
  }
#endif

  vcl_cout << "Testing wavelet transforms on nd buffers\n";
  for (int ndim = 1; ndim <= 4; ndim++)
    for (int s = 3; s <= 8; s++)
    {
      int* dims = new int[ndim+1];
      int ntot = 1;
      for (int d = 0; d < ndim; d++) {
        dims[d] = s;
        ntot *= s;
      }
      dims[ndim] = ntot;
      float* data = new float[ntot];
      for (int k = 2; k <= 12; k+=2)
      {
        for (int i = 0; i < ntot; i++) data[i] = float(i);
        int nlevels = 2;
        WaveletTransformByBlock(data, dims, ndim, true, nlevels, k);
        WaveletTransformByBlock(data, dims, ndim, false, nlevels, k);
        float max_err = 0;
        for (int i = 0; i < ntot; i++) {
          float err = (float)vcl_fabs(data[i] - i);
          if (err > max_err)
            max_err = err;
        }
        vcl_cout << "  |dims| = " << ndim
                 << "  |data| = " << ntot
                 << "  |wavelet| = " << k
                 << "  |error| = " << max_err << vcl_endl;
      }
      delete[] data;
      delete[] dims;
    }
}


//--------------------------------------------------------------------

//: Compute the Fast Wavelet Transform of the 2d array.
// Higher number wavelets are less compact, but give more accurate
// decomposition of the image into linear combinations of mother wavelets.

bool
gevd_float_operators::WaveletTransformByIndex(const gevd_bufferxy& from, gevd_bufferxy*& to,
                                              const bool forwardp,    // or inverse
                                              int nlevels,
                                              const int waveletno)
{
  to = gevd_float_operators::Allocate(to, from);
  int dims[3];
  dims[0] = to->GetSizeY();
  dims[1] = to->GetSizeX();
  dims[2] = dims[0] * dims[1];
  const float* from_array = (const float*) from.GetBuffer(); // copy the image
  float* to_array = (float*) to->GetBuffer();
  for (int i = 0; i < dims[2]; i++)
    to_array[i] = from_array[i];
  return gevd_float_operators::WaveletTransformByIndex(to_array, // transform in place
                                                       dims, 2,
                                                       forwardp, nlevels,
                                                       waveletno);
}

//: Compute the Fast Wavelet Transform of the image.
// Higher number wavelets are less compact, but give more accurate
// decomposition of the image into linear combinations of mother wavelets.
// Image compression uses mother wavelets around 12, while image
// segmentation uses compact wavelets around 2 or 4.

bool
gevd_float_operators::WaveletTransformByBlock(const gevd_bufferxy& from, gevd_bufferxy*& to,
                                              const bool forwardp,    // or inverse
                                              int nlevels,
                                              const int waveletno)
{
  to = gevd_float_operators::Allocate(to, from);
  int dims[3];
  dims[0] = to->GetSizeY();
  dims[1] = to->GetSizeX();
  dims[2] = dims[0] * dims[1];
  const float* from_array = (const float*) from.GetBuffer(); // copy the image
  float* to_array = (float*) to->GetBuffer();
  for (int i = 0; i < dims[2]; i++)
    to_array[i] = from_array[i];
  return gevd_float_operators::WaveletTransformByBlock(to_array, // transform in place
                                                       dims, 2,
                                                       forwardp, nlevels,
                                                       waveletno);
}

//:
// Delete all wavelet components, which have high frequency along both
// x- and y- axes. They are diagonal blocks, except the lowest frequency block.
// This will throw away high frequency point-like features.
// Return the number of coefficients deleted.

int
gevd_float_operators::DeleteMixedComponents(gevd_bufferxy& wave,
                                            const int nlevels)
{
  int sx = wave.GetSizeX(), sy = wave.GetSizeY(), count = 0;
  for (int l = 0; l < nlevels; l++) {
    int ssx = sx / 2, ssy = sy / 2;
    for (int y = ssy; y < sy; y++)
      for (int x = ssx; x < sx; x++)
        if (floatPixel(wave, x, y) != 0) {
          floatPixel(wave, x, y) = 0;
          count++;
        }
    sx = ssx; sy = ssy;
  }
  return count;
}

//:
// Truncate to 0, all wavelet components with high frequency along
// either x- or y- axes only, not both. The relative magnitude of the
// components must be smaller than the given threshold.
// This will simulate reduced accuracy in presence of transmission errors.
// Return the number of coefficients deleted.

int
gevd_float_operators::TruncateHighFrequencies(gevd_bufferxy& wave,
                                              const int nlevels,
                                              const float threshold)
{
  int sx = wave.GetSizeX(), sy = wave.GetSizeY(), count = 0;
  for (int l = 0; l < nlevels; l++) {
    int ssx = sx / 2, ssy = sy / 2;
    float low = gevd_float_operators::Maximum(wave, sx-ssx, ssy, ssx, 0) * threshold;
    for (int y = 0; y < ssy; y++)           // lowY x highX block
      for (int x = ssx; x < sx; x++)
        if (floatPixel(wave, x, y) < low) {
          floatPixel(wave, x, y) = 0;
          count++;
        }
    low = gevd_float_operators::Maximum(wave, ssx, sy-ssy, 0, ssy) * threshold;
    for (int y = ssy; y < sy; y++)          // highY x lowX block
      for (int x = 0; x < ssx; x++)
        if (floatPixel(wave, x, y) < low) {
          floatPixel(wave, x, y) = 0;
          count++;
        }
    sx = ssx; sy = ssy;
  }
  return count;
}

//: Truncate to average value, the lowest frequency component.
// This will simulate throwing away the lowest frequencies in image.
// Return the number of coefficients deleted.

int
gevd_float_operators::TruncateLowestFrequency(gevd_bufferxy& wave,
                                              const int nlevels)
{
  int s = 1 << nlevels; // scaling factor
  int sx = wave.GetSizeX() / s, sy = wave.GetSizeY() / s; // size of block
  float average = gevd_float_operators::Sum(wave, sx, sy, 0, 0) / (sx * sy);
  int count = 0;
  for (int y = 0; y < sy; y++)
    for (int x = 0; x < sx; x++)
      if (floatPixel(wave, x, y) != average) {
        floatPixel(wave, x, y) = average;
        count++;
      }
  return count;                         // number of pixels changed
}

//:
// Delete boundary artifacts in 1d-array, because its length is not
// a power of 2, and so wrapping will skip the last odd element.

int
gevd_float_operators::DeleteBoundaryArtifacts(float* wave, const int n,
                                              const int nlevels)
{
  if (nlevels == 0)
    return 0;
  else {
    int s = n, count = 0;
    for (int l = 0; l < nlevels; l++) {
      int ns = s / 2;                   // integer division rounds to
      int ss = ns * 2;                  // smaller value
      if (ss < s) {
        wave[ss] = 0;
        count += 1;
      }
      s = ns;
    }
    return count;
  }
}

//:
// Delete boundary artifacts because the dimension of the image
// is not a power of 2, and so wrapping will skip the last odd element.

int
gevd_float_operators::DeleteBoundaryArtifacts(gevd_bufferxy& wave, const int nlevels)
{
  if (nlevels == 0)
    return 0;
  else {
    int sx = wave.GetSizeX(), sy = wave.GetSizeY(), count = 0;
    for (int l = 0; l < nlevels; l++) {
      int ssx = sx / 2, ssy = sy / 2;   // integer division rounds to
      int xx = ssx * 2, yy = ssy * 2;   // smaller value
      if (xx < sx) {
        for (int y = 0; y < sy; y++)
          floatPixel(wave, xx, y) = 0;
        count += sy;
      }
      if (yy < sy) {
        for (int x = 0; x < sx; x++)
          floatPixel(wave, x, yy) = 0;
        count += sx;
      }
      sx = ssx; sy = ssy;
    }
    return count;
  }
}


//: Project wavelet components onto the axes.

void
gevd_float_operators::ProjectWaveOntoX(const gevd_bufferxy& wave,
                                       float*& proj, const int nlevels)
{
  if (nlevels == 0)
    gevd_float_operators::ProjectOntoX(wave, proj);
  else {
    int sx = wave.GetSizeX(), sy = wave.GetSizeY();
    for (int l = 0; l < nlevels; l++) {
      int ssx = sx / 2, ssy = sy / 2;
      gevd_float_operators::ProjectOntoX(wave, proj, sx-ssx, ssy, ssx, 0);
      sx = ssx; sy = ssy;
    }
  }
}

void
gevd_float_operators::ProjectWaveOntoY(const gevd_bufferxy& wave, float*& proj,
                                       const int nlevels)
{
  if (nlevels == 0)
    gevd_float_operators::ProjectOntoY(wave, proj);
  else {
    int sx = wave.GetSizeX(), sy = wave.GetSizeY();
    for (int l = 0; l < nlevels; l++) {
      int ssx = sx / 2, ssy = sy / 2;
      gevd_float_operators::ProjectOntoY(wave, proj, ssx, sy-ssy, 0, ssy);
      sx = ssx; sy = ssy;
    }
  }
}


//: Project the image data in ROI onto the x- and y- axes. O(n*m).
// The 1d-array is returned with projections being the sum
// normalized by the number of elements projected.

void
gevd_float_operators::ProjectOntoX(const gevd_bufferxy& buf, float*& proj,
                                   int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  if (proj == NULL)
    proj = new float [buf.GetSizeX()];
  int hiX = origX + sizeX, hiY = origY + sizeY;
  for (int x = origX; x < hiX; x++) {   // projection onto the x-axis
    float projx = 0;
    for (int y = origY; y < hiY; y++)
      projx += floatPixel(buf, x, y);
    proj[x] = projx / sizeY;
  }
}

void
gevd_float_operators::ProjectOntoY(const gevd_bufferxy& buf, float*& proj,
                                   int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  if (proj == NULL)
    proj = new float [buf.GetSizeY()];
  int hiX = origX + sizeX, hiY = origY + sizeY;
  for (int y = origY; y < hiY; y++) {   // projection onto the y-axis
    float projy = 0;
    for (int x = origX; x < hiX; x++)
      projy += floatPixel(buf, x, y);
    proj[y] = projy / sizeX;
  }
}

// Full correlation must be implemented with FFT.
// Local correlation with limited search is faster with direct convolution.

//:
// Find correlation of given pattern to data, matching
// pattern[radius+i] with data[index+i]. The linear correlation
// coefficient, also called Pearson r, is computed, O(|pattern|).
// Assumed pattern has length = 2*radius + 1.

float
gevd_float_operators::Correlation(const float* data, const int length,
                                  const float* pattern, const int radius,
                                  const int index)
{
  int xmin = index-radius, xmax = index+radius+1; // pad boundary with 0
  int ymin = 0;                                   // or check for bounds

  if (xmin < 0) { ymin = -xmin; xmin = 0; }
  if (xmax > length) xmax = length;
  int sum1 = xmax - xmin;
  if (sum1 < radius)
    return 0;
  else {
    register double sumx=0, sumy=0, sumxx=0, sumyy=0, sumxy=0, xval, yval;
    for (register int x = xmin, y = ymin; x < xmax; x++, y++) {
      xval = data[x]; yval = pattern[y];
      sumxy += xval * yval;             // accumulate correlation value
      sumx += xval;
      sumy += yval;
      sumxx += xval * xval;
      sumyy += yval * yval;
    }
    double varx = sum1 * sumxx - sumx * sumx; // all multiplied with sum1
    double vary = sum1 * sumyy - sumy * sumy;
    double cvar = sum1 * sumxy - sumx * sumy;
    if (varx!=0 && vary!=0) cvar /= vcl_sqrt(varx * vary);
    return (float)cvar / (float)vcl_sqrt(varx * vary); // linear correlation coefficient
  }
}


//:
// Find correlations of given pattern to data, given maximum search
// from index. O(|pattern|*shift). Returns the array of
// correlation values, with positive translation starting from
// result[search+1], and negative translation starting from
// result[search-1].

float*
gevd_float_operators::Correlations(const float* data, const int length,
                                   const float* pattern, const int radius,
                                   const int index, const int search)
{
  int ns = 2*search + 1;
  float* result = new float[ns];
  result[search] = gevd_float_operators::Correlation(data, length,
                                                     pattern, radius,
                                                     index);
  for (int s = 1; s <= search; s++) {
    result[search+s] = gevd_float_operators::Correlation(data, length, // translate patten in
                                                         pattern, radius, // positive direction
                                                         index+s);
    result[search-s] = gevd_float_operators::Correlation(data, length, // translate pattern in
                                                         pattern, radius, // negative direction
                                                         index-s);
  }
#ifdef DEBUG
  vcl_cout << "correlations =";
  for (int s = 0; s < ns; s++)
    vcl_cout << ' ' << result[s];
  vcl_cout << vcl_endl;
#endif
  return result;
}


//: Find correlation, and return correlation and shift values.

float
gevd_float_operators::BestCorrelation(const float* data, const int length,
                                      const float* pattern, const int radius,
                                      int& shift, const int search)
{
  float* cors = gevd_float_operators::Correlations(data, length,
                                                   pattern, radius,
                                                   (length/2) + shift, // correlate at center
                                                   search);
  int index = 0; float peak = 0;
  bool found = gevd_float_operators::Maximum(cors, 2*search+1, index, peak);
  delete[] cors;
  if (found) {
    shift += index - search;
    return peak;
  }
  return 0;
}


//:
// Search for best correlation, from coarse to fine,
// starting at a priori shift, and requiring minimum overlap. O(n) time.
// Return last best correlation, and its corresponding shift.
// The search is cutoff early, if no maximum is found, or
// maximum correlation <= cutoff.
// Accumulate match values if matches non null.
// Assumes data and pattern are pyramids.
// Compile with -DDEBUG to trace the coarse-to-fine search
// of the best correlation,

float
gevd_float_operators::CoarseFineCorrelation(const float* dataPyr, const int dlength,
                                            const float* patternPyr, const int plength,
                                            float& shift,
                                            const int coarse, const int fine,
                                            const float cutoff,
                                            const float overlap,
                                            float* matches)
{
  const float NOISE = 0.2f;      // valid maximum correlation

  // 1. Complete search of the best correlation at coarsest level
  // given required minimum overlap.
#ifdef DEBUG
  vcl_cout << "shift0 = " << shift << vcl_endl;
#endif
  if (shift != 0)               // search from a priori shift
    shift /= (1 << coarse);
  shift = (float)rint(shift);
  int dlen = dlength >> coarse, plen = plength >> coarse;
  int rmax = int((1 - overlap) * dlen + 0.5); // for minimum overlap
  if (rmax < 1) rmax = 1;       // boundary case, for 100% overlap
  float* cors = Correlations(dataPyr+dlen, dlen,
                             patternPyr+plen, plen,
                             int(shift), rmax);
  const int NOMATCH = -1;
  float match = NOISE; int mi = NOMATCH; // best correlation
  const int lmax = (rmax << 1);
  for (int i = 1; i < lmax; i++) {      // find largest correlation
    float cor = cors[i];
    if (cor > match &&
        cors[i-1] < cor && cor > cors[i+1]) {
      match = cor;
      mi = i;
    }
  }
  if (mi == NOMATCH)            // no local maximum found
    match = cors[rmax];         // search lower level, at same place
  else {
    float left = cors[mi-1], mid = cors[mi], right = cors[mi+1];
    shift += (mi - rmax) + InterpolateParabola(left, mid, right, match);
#ifdef DEBUG
  vcl_cout << left << ' ' << mid << ' ' << right << vcl_endl
           << "level = " << coarse
           << "  shift = " << shift * (1 << coarse)
           << "  match = " << match << vcl_endl;
#endif
  }
  delete[] cors;

  // 2. Track best correlation to finest level
  const int SEARCH = 1, RMAX = 3;// local search 3 times
  int k = coarse-1;
  for (; ; k--) {
    shift = (float)rint(2*shift);
    dlen = dlength >> k, plen = plength >> k;
    cors = Correlations(dataPyr+dlen, dlen,
                        patternPyr+plen, plen,
                        int(shift), SEARCH);
    float local = 0;            // radial search from center out
    int r = 0;
    for (; r < RMAX; r++) {
      float left = cors[0], mid = cors[1], right = cors[2];
#ifdef DEBUG
      vcl_cout << left << ' ' << mid << ' ' << right << vcl_endl;
#endif
      if (left <= mid && mid >= right && mid > NOISE) {
        local += InterpolateParabola(left, mid, right, match);
        if (matches) matches[k] += match;
        break;                  // radial search succeeds
      }
      if (r < RMAX-1) {         // search next radius?
        if (right > left) {     // search towards right
          local += 1;
          cors[0] = cors[1];
          cors[1] = cors[2];
          cors[2] = Correlation(dataPyr+dlen, dlen,
                                patternPyr+plen, plen,
                                int(local)+SEARCH);
        }
        if (left > right) {     // search towards left
          local -= 1;
          cors[2] = cors[1];
          cors[1] = cors[0];
          cors[0] = Correlation(dataPyr+dlen, dlen,
                                patternPyr+plen, plen,
                                int(local)-SEARCH);
        }
      }
    }
    delete[] cors;
    if (r == RMAX)              // fail local search
      break;                    // early exit
    shift += local;             // shift further
#ifdef DEBUG
    vcl_cout << "level = " << k
             << "  shift = " << shift * (1 << k)
             << "  match = " << match << vcl_endl;
#endif
    if (match <= cutoff || k == fine) // early cutoff because
      break;                    // weak correlation
  }
  if (k > 0) shift *= (1 << k); // shift at level 0
  if (k > fine)                 // penalize for no fine match
    match *= float(coarse - k + 1) / (coarse - fine + 1);
  return match;
}


//: Apply function to all elements in buffer.

void
gevd_float_operators::Apply(gevd_bufferxy& buf, float (*func)(float))
{
  int size = buf.GetSizeX() * buf.GetSizeY();
  float* data = (float*) buf.GetBuffer();
  for (int i = 1; i < size; i++)
    data[i] = func(data[i]);
}


//: Allocate new space if desired depth and sizes are not the same.

gevd_bufferxy*
gevd_float_operators::Allocate(gevd_bufferxy* space, const gevd_bufferxy& model,
                               int bits_per_pixel, int sizeX, int sizeY)
{
  if (!bits_per_pixel)          // find defaults from model
    bits_per_pixel = model.GetBitsPixel();
  if (!sizeX)
    sizeX = model.GetSizeX();
  if (!sizeY)
    sizeY = model.GetSizeY();
  if (space == NULL ||          // check if need reallocation
      space->GetBitsPixel() != bits_per_pixel ||
      space->GetSizeX() < sizeX || space->GetSizeY() < sizeY) {
    delete space;
    space = new gevd_bufferxy(sizeX, sizeY, bits_per_pixel);
  }
  return space;
}

//: Creates a new buffer similar to buf, unless dimension and precision are given.

gevd_bufferxy*
gevd_float_operators::SimilarBuffer(const gevd_bufferxy& buf,
                                    int bits_per_pixel,
                                    int sizeX, int sizeY)

{
  if (bits_per_pixel == 0)                      // find default pixel
    bits_per_pixel = buf.GetBitsPixel();
  if (sizeX == 0)                               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  return new gevd_bufferxy(sizeX, sizeY, bits_per_pixel);
}


//: Two buffers are similar if they have the same dimensions, and precision (bits_per_pixel).

bool
gevd_float_operators::IsSimilarBuffer(const gevd_bufferxy& buf1,
                                      const gevd_bufferxy& buf2)
{
  return buf1.GetSizeX() == buf2.GetSizeX() &&
         buf1.GetSizeY() == buf2.GetSizeY() &&
         buf1.GetBitsPixel() == buf2.GetBitsPixel();
}


//:
// Extract from buf, a float sub-buffer with dimensions (sizeX, sizeY),
// from top-left corner (origX, origY).
// Faster copying can be done with read/write chunks of memory.

gevd_bufferxy*
gevd_float_operators::Extract(const gevd_bufferxy & buf,
                              int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)                               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  gevd_bufferxy& sub = *gevd_float_operators::SimilarBuffer(buf, 0, sizeX, sizeY);
  for (int y = 0; y < sizeY; y++)
    for (int x = 0; x < sizeX; x++)
      floatPixel(sub, x, y) = floatPixel(buf, origX+x, origY+y);
  return &sub;
}

//: Update a float sub-buffer of buf, from top-left corner (origX, origY), with values in sub.
// Faster copying can be done with read/write chunks of memory.

void
gevd_float_operators::Update(gevd_bufferxy& buf, const gevd_bufferxy& sub,
                             int origX, int origY)
{
  int sizeX = sub.GetSizeX(), sizeY = sub.GetSizeY();
  for (int y = 0; y < sizeY; y++)
    for (int x = 0; x < sizeX; x++)
      floatPixel(buf, origX+x, origY+y) = floatPixel(sub, x, y);
}

//: Set all pixels in ROI to value.

void
gevd_float_operators::Fill(gevd_bufferxy& buf, const float value,
                           int sizeX, int sizeY,
                           int origX, int origY)
{
  if (sizeX == 0)                       // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  int endX = origX + sizeX, endY = origY + sizeY;
  for (int y = origY; y < endY; y++)
    for (int x = origX; x < endX; x++)
      floatPixel(buf, x, y) = value;
}

//: Sets all pixels in the frame region to value (default = 0).
// O((n+m)*width). The frame region is a rectangular band with given width,
// framing at two ends of x/y axes.

void
gevd_float_operators::FillFrameX(gevd_bufferxy& buf, const float value, const int width)
{
  const int hix = buf.GetSizeX(), hiy = buf.GetSizeY();
  const int framelox = width, framehix = hix - width;
  for (int y = 0; y < hiy; ++y)             // left
    for (int x = 0; x < framelox; ++x)
      floatPixel(buf, x, y) =  value;
  for (int y = 0; y < hiy; ++y)             // right
    for (int x = framehix; x < hix; ++x)
      floatPixel(buf, x, y) =  value;
}

//:

void
gevd_float_operators::FillFrameY(gevd_bufferxy& buf, const float value, const int width)
{
  int lox = 0, hix = buf.GetSizeX();
  int loy = 0, hiy = buf.GetSizeY();
  int frameloy = width, framehiy = hiy - width;
  for (int y = loy; y < frameloy; y++)              // bottom
    for (int x = lox; x < hix; x++)
      floatPixel(buf, x, y) =  value;
  for (int y = framehiy; y < hiy; y++)              // top
    for (int x = lox; x < hix; x++)
      floatPixel(buf, x, y) =  value;
}

//: Sets all pixels on frame contour, such as buf(loc, i), to value.
// O(n+m).

void
gevd_float_operators::DrawFrame(gevd_bufferxy& buf, const int loc, const float value)
{
  const int hix = buf.GetSizeX()-loc-1;
  const int hiy = buf.GetSizeY()-loc-1;
  for (int x = loc; x <= hix; x++) {
    floatPixel(buf, x, loc) = value; // bottom
    floatPixel(buf, x, hiy) = value; // top
  }
  for (int y = loc; y <= hiy; y++) {
    floatPixel(buf, loc, y) = value; // left
    floatPixel(buf, hix, y) = value; // right
  }
}

//: Returns the maximum value in float buffer.

float
gevd_float_operators::Maximum(const gevd_bufferxy& buf,
                              int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)                               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  int maxx = origX + sizeX, maxy = origY + sizeY;
  float hi = floatPixel(buf, origX, origY), f;
  for (int y = origY; y < maxy; y++)
    for (int x = origX; x < maxx; x++) {
      f = floatPixel(buf, x, y);
      if (f > hi) hi = f;
    }
  return hi;
}

//: Returns the minimum value in float buffer.

float
gevd_float_operators::Minimum(const gevd_bufferxy& buf,
                              int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)                               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  int maxx = origX + sizeX, maxy = origY + sizeY;
  float lo = floatPixel(buf, origX, origY), f;
  for (int y = origY; y < maxy; y++)
    for (int x = origX; x < maxx; x++) {
      f = floatPixel(buf, x, y);
      if (f < lo) lo = f;
    }
  return lo;
}

//: Returns the sum of all values in float buffer.

float
gevd_float_operators::Sum(const gevd_bufferxy& buf,
                          int sizeX, int sizeY, int origX, int origY)
{
  if (sizeX == 0)                               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  int maxx = origX + sizeX, maxy = origY + sizeY;
  float sum = 0;
  for (int y = origY; y < maxy; y++)
    for (int x = origX; x < maxx; x++)
      sum += floatPixel(buf, x, y);
  return sum;
}

//: Truncate all values in ROI to 0, if below noise.
// Return the number of pixels changed.

int
gevd_float_operators::Threshold(gevd_bufferxy& buf, float noise,
                                int sizeX, int sizeY,
                                int origX, int origY)
{
  if (sizeX == 0)               // find default size
    sizeX = buf.GetSizeX();
  if (sizeY == 0)
    sizeY = buf.GetSizeY();
  int maxx = origX + sizeX, maxy = origY + sizeY;
  int count = 0; float pix;
  for (int y = origY; y < maxy; y++)
    for (int x = origX; x < maxx; x++)
      if ((pix = floatPixel(buf, x, y)) && pix < noise) {
        floatPixel(buf, x, y) = 0;
        count++;
      }
  return count;
}


//:
// Normalizes a float buffer so that the pixel values range
// from lo to hi, inclusive. If the buffer has constant value,
// the value is mapped to lo. O(n*m).

void
gevd_float_operators::Normalize(gevd_bufferxy& buf, const float lo, const float hi)
{
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  float flo, fhi, f;
  flo = fhi = data[0];
  for (int i = 1; i < size; i++) {
    f = data[i];
    if (f < flo) flo = f;
    if (f > fhi) fhi = f;
  }
  if (fhi == flo)
    for (int i = 0; i < size; i++)
      data[i] = lo;
  else {
    float scale = (hi - lo) / (fhi - flo);
    for (int i = 0; i < size; i++)
      data[i] = scale * (data[i] - flo)  + lo;
  }
}

//: Shift to positive values, by adding 30.0000, and truncate all values to 0-60.000.
//  O(n*m).

void
gevd_float_operators::ShiftToPositive(gevd_bufferxy& buf)
{
  const float zero = 30000, lo = 0, hi = 60000;
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  for (int i = 0; i < size; i++) {
    float f = data[i] + zero;
    if (f < lo)
      f = lo;
    else if (f > hi)
      f = hi;
    data[i] = f;
  }
}


//: Zeros out all negative values.

float
gevd_float_operators::TruncateToPositive(gevd_bufferxy& buf)
{
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  float diff = 0, d;
  for (int i = 0; i < size; i++) {
    d = data[i];
    if (d < 0) {
      data[i] = 0;
      d = - d;
      if (d > diff) diff = d;
    }
  }
  return diff;
}

//: Scale all values by factor.

void
gevd_float_operators::Scale(gevd_bufferxy& buf, float factor)
{
  if (factor != 0) {
    int size = (buf.GetSizeX() * buf.GetSizeY());
    float* data = (float*) buf.GetBuffer();
    for (int i = 0; i < size; i++)
      data[i] *= factor;
  }
}

//: Replace with absolute values.

void
gevd_float_operators::Absolute(gevd_bufferxy& buf)
{
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  for (int i = 0; i < size; i++)
    if (data[i] < 0)
      data[i] = - data[i];
}

//: Negate all values.

void
gevd_float_operators::Negate(gevd_bufferxy& buf)
{
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  for (int i = 0; i < size; i++)
    data[i] = - data[i];
}


float
gevd_float_operators::TruncateToCeiling(gevd_bufferxy& buf, float ceilng)
{
  int size = (buf.GetSizeX() * buf.GetSizeY());
  float* data = (float*) buf.GetBuffer();
  float diff = 0, d;
  for (int i = 0; i < size; i++) {
    d = data[i];
    if (d > ceilng) {
      data[i] = ceilng;
      d -= ceilng;
      if (d > diff) diff = d;
    }
  }
  return diff;
}


//:
// Converts from a unsigned char (8-bit) or a short (16-bit) buffer to a float buffer
// to avoid overflow/underflow and conversion for subsequent math computations.
// O(n*m).

bool
gevd_float_operators::BufferToFloat(const gevd_bufferxy& from, gevd_bufferxy& to)
{
  if (from.GetSizeX() != to.GetSizeX() ||
      from.GetSizeY() != to.GetSizeY() ||
      to.GetBytesPixel() != sizeof(float))
    return false;
  int size = (to.GetSizeX() * to.GetSizeY());
  switch (from.GetBytesPixel()) {
   case sizeof(unsigned char):
    {
      const unsigned char* frombuf = (const unsigned char*) from.GetBuffer();
      float* tobuf = (float*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = (float) frombuf[i];
    }
    break;
   case sizeof(short):
    {
      const short* frombuf = (const short*) from.GetBuffer();
      float* tobuf = (float*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = (float) frombuf[i];
    }
    break;
   case 3*sizeof(unsigned char): // assume RGB, and take luminance
    {
      vcl_cerr << "gevd_float_operators::BufferToFloat: taking luminance of RGB buffer\n";
      const unsigned char* frombuf = (const unsigned char*) from.GetBuffer();
      float* tobuf = (float*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = 0.299f*frombuf[3*i]+0.587f*frombuf[3*i+1]+0.114f*frombuf[3*i+2];
    }
    break;
   case sizeof(int):
    {
      const unsigned int* frombuf = (const unsigned int*) from.GetBuffer();
      float* tobuf = (float*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = (float)frombuf[i];
    }
    break;
   default:
    vcl_cerr << "Can only convert unsigned char/short/int/RGB buffer to float\n";
    return false;
  }
  return true;
}

bool
gevd_float_operators::FloatToBuffer (const gevd_bufferxy& from, gevd_bufferxy& to)
{
  if (from.GetSizeX() != to.GetSizeX() ||
      from.GetSizeY() != to.GetSizeY() ||
      from.GetBytesPixel() != sizeof(float))
    return false;
  int size = (to.GetSizeX() * to.GetSizeY());
  switch (to.GetBytesPixel()) {
   case sizeof(unsigned char):
    {
      const float* frombuf = (const float*) from.GetBuffer();
      unsigned char* tobuf = (unsigned char*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = (unsigned char) int(frombuf[i]);
    }
    return true;
   case sizeof(short):
    {
      const float* frombuf = (const float*) from.GetBuffer();
      short* tobuf = (short*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[i] = (short) int(frombuf[i]);
    }
    return true;
   case 3*sizeof(unsigned char): // assume RGB ==> restore luminance
    {
      const float* frombuf = (const float*) from.GetBuffer();
      unsigned char* tobuf = (unsigned char*) to.GetBuffer();
      for (int i = 0; i < size; i++)
        tobuf[3*i] = tobuf[3*i+1] = tobuf[3*i+2] = (unsigned char) int(frombuf[i]);
    }
    return true;
   default:
    vcl_cerr << "Can only convert float to unsigned char/short/RGB buffer\n";
    return false;
  }
}


//: Detect maximum in 1d-array of values.

bool
gevd_float_operators::Maximum(const float* data, const int length,
                              int& index, float& value)
{
  index = 0;
  float left, mid = data[0], right = data[1];
  for (int r = 2; r < length; r++) {
    left = mid;
    mid = right;
    right = data[r];
    if (mid > left && mid > right) {
      if (index) {
        if (mid > value) {
          value = mid;
          index = r-1;
        }
      } else {
        value = mid;
        index = r-1;
      }
    }
  }
  return !(index == 0);
}

//:
// Pad the buffer by repeating values at the border, so that the
// new buffer has dimensions being powers of 2. The original buffer
// is centered in the new buffer. Returns the buffer unchanged if it
// already has dimensions being powers of 2.

gevd_bufferxy*
gevd_float_operators::PadToPowerOf2(gevd_bufferxy& buf)
{
  int sizeX = buf.GetSizeX();
  int sizeY = buf.GetSizeY();
  int newSizeX = sizeX, newSizeY = sizeY;
  {
    double exptX = vcl_log(double(sizeX))/vcl_log(2.0),
           exptY = vcl_log(double(sizeY))/vcl_log(2.0);
    double ceilX = vcl_ceil(exptX), ceilY = vcl_ceil(exptY);
    if (exptX < ceilX)          // round up to nearest power of 2
      newSizeX = 1 << int(ceilX);
    if (exptY < ceilY)
      newSizeY = 1 << int(ceilY);
  }
  if (newSizeX == sizeX && newSizeY == sizeY)
    return &buf;
  int lpadX = (newSizeX - sizeX) / 2, lpadY = (newSizeY - sizeY) / 2;
  gevd_bufferxy& padded = *gevd_float_operators::SimilarBuffer(buf, bits_per_float,
                                                               newSizeX, newSizeY);
  padded.Clear();
  gevd_float_operators::Update(padded, buf, lpadX, lpadY);
  int hpadX = newSizeX-lpadX-sizeX, hpadY = newSizeY-lpadY-sizeY;
  lpadX++; lpadY++;
  hpadX++; hpadY++;
  float corner = (2*floatPixel(buf, 0, 0) +
                  floatPixel(buf, 1, 0) + floatPixel(buf, 0, 1)) / 4;
  gevd_float_operators::Fill(padded, corner, // fill corner values
                             lpadX, lpadY, 0, 0);
  corner = (2*floatPixel(buf, sizeX-1, 0) +
            floatPixel(buf, sizeX-2, 0) + floatPixel(buf, sizeX-1, 1)) / 4;
  gevd_float_operators::Fill(padded, corner,
                             hpadX, lpadY, newSizeX-hpadX, 0);
  corner = (2*floatPixel(buf, 0, sizeY-1) +
            floatPixel(buf, 1, sizeY-1) + floatPixel(buf, 0, sizeY-2)) / 4;
  gevd_float_operators::Fill(padded, corner,
                             lpadX, hpadY, 0, newSizeY-hpadY);
  corner = (2*floatPixel(buf, sizeX-1, sizeY-1) +
            floatPixel(buf, sizeX-2, sizeY-1) +
            floatPixel(buf, sizeX-1, sizeY-2)) / 4;
  gevd_float_operators::Fill(padded, corner,
                             hpadX, hpadY, newSizeX-hpadX, newSizeY-hpadY);
  int x_1, x_2, y_1, y_2;
  x_1 = lpadX; x_2 = newSizeX-hpadX;
  for (int x = x_1; x < x_2; x++) {
    float top = (floatPixel(buf, x-lpadX, 0) +
                 2*floatPixel(buf, x-lpadX+1, 0) +
                 floatPixel(buf, x-lpadX+2, 0)) / 4;
    y_1 = 0; y_2 = lpadY-1;
    for (int y = y_1; y < y_2; y++)
      floatPixel(padded, x, y) = top;
    float bottom = (floatPixel(buf, x-lpadX, sizeY-1) +
                    2*floatPixel(buf, x-lpadX+1, sizeY-1) +
                    floatPixel(buf, x-lpadX+2, sizeY-1)) / 4;
    y_1 = newSizeY-hpadY+1; y_2 = newSizeY;
    for (int y = y_1; y < y_2; y++)
      floatPixel(padded, x, y) = bottom;
  }
  y_1 = lpadY; y_2 = newSizeY-hpadY;
  for (int y = y_1; y < y_2; y++) {
    float left = (floatPixel(buf, 0, y-lpadY) +
                  2*floatPixel(buf, 0, y-lpadY+1) +
                  floatPixel(buf, 0, y-lpadY+2)) / 4;
    x_1 = 0; x_2 = lpadX-1;
    for (int x = x_1; x < x_2; x++)
      floatPixel(padded, x, y) = left;
    float right = (floatPixel(buf, sizeX-1, y-lpadY) +
                   2*floatPixel(buf, sizeX-1, y-lpadY+1) +
                   floatPixel(buf, sizeX-1, y-lpadY+2)) / 4;
    x_1 = newSizeX-hpadX+1; x_2 = newSizeX;
    for (int x = x_1; x < x_2; x++)
      floatPixel(padded, x, y) = right;
  }
  return &padded;
}

//: Inverse of the above operation.

gevd_bufferxy*
gevd_float_operators::UnpadFromPowerOf2(gevd_bufferxy& padded, int sizeX, int sizeY)
{
  int newSizeX = padded.GetSizeX();
  int newSizeY = padded.GetSizeY();
  if (newSizeX == sizeX && newSizeY == sizeY)
    return &padded;
  else
    return gevd_float_operators::Extract(padded,
                                         sizeX, sizeY,
                                         (newSizeX - sizeX)/2,
                                         (newSizeY - sizeY)/2);
}

gevd_bufferxy*
gevd_float_operators::TransposeExtract(const gevd_bufferxy & buf,
                                       int sizeX, int sizeY, int origX, int origY)
{
  gevd_bufferxy& sub = *gevd_float_operators::SimilarBuffer(buf, 0, sizeY, sizeX);
  for (int y = 0; y < sizeY; y++)
    for (int x = 0; x < sizeX; x++)
      floatPixel(sub, y, x) = floatPixel(buf, origX+x, origY+y);
  return &sub;
}

void
gevd_float_operators::TransposeUpdate(gevd_bufferxy& buf, const gevd_bufferxy& sub,
                                      int origX, int origY)
{
  int sizeX = sub.GetSizeY(), sizeY = sub.GetSizeX();
  for (int y = 0; y < sizeY; y++)
    for (int x = 0; x < sizeX; x++)
      floatPixel(buf, origX+x, origY+y) = floatPixel(sub, y, x);
}


gevd_bufferxy*
gevd_float_operators::AbsoluteDifference(const gevd_bufferxy& buf1,
                                         const gevd_bufferxy& buf2)
{
  if (!IsSimilarBuffer(buf1, buf2))
    return NULL;
  else {
    int sizeX = buf1.GetSizeX(), sizeY = buf1.GetSizeY();
    gevd_bufferxy& buf = *gevd_float_operators::SimilarBuffer(buf1);
    for (int y = 0; y < sizeY; y++)
      for (int x = 0; x < sizeX; x++) {
        float diff = floatPixel(buf1, x, y) - floatPixel(buf2, x, y);
        if (diff < 0) diff = -diff;
        floatPixel(buf, x, y) = diff;
      }
    return &buf;
  }
}
