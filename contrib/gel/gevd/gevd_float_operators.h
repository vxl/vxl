#ifndef gevd_float_operators_h_
#define gevd_float_operators_h_
//:
// \file
// \brief Operators on float pixels in 2D images
//
// Operators on pixels in 2D image are divided into following groups:
// - convolution:
//   * gaussian smoothing, high pass filtering.
//   * 2D/1D separable convolution in x and y, even/odd.
// - detection:
//   * gradient to find step edges
//   * laplacian to find fold edges
//   * local orientation
// - pyramid:
//   * shrink/expand by factor of 2 using Burt filters.
//   * wavelet transform
// - other:
//   * correlation
//   * apply function
//   * projection onto axes
// - utilities:
//
// float is used throughout to avoid overflow/underflow and conversion
// on math operations like sqrt, sin, etc...
// All convolution operators have a running cache of the input, and so
// the from/to buffers can be the same.
//
// \verbatim
//  Modifications
//   Van-Duc Nguyen (1990) convolution to support image segmentation
//   Van-Duc Nguyen (1992) add pyramid and wavelet
//   Van-Duc Nguyen (1996) add circular/reflection at border
//   Van-Duc Nguyen (1997) add running sum
//   Chuck Stewart  (1997) added SurfaceNormalD, SurfaceCurvatureD, ShrinkBy2_D
//   Peter Vanroose July 1999 -made all methods static (class has no data)
//   Peter Vanroose Sept 2004 -all methods now support "from" & "to" being equal
// \endverbatim

class gevd_bufferxy;       // buffer of floats

class gevd_float_operators
{
// Do not instantiate this class; access methods by name scoping
  gevd_float_operators();

 public:
  // convolution
  static float Convolve(const gevd_bufferxy& from, const gevd_bufferxy& kernel,
                        gevd_bufferxy*& to);
  static gevd_bufferxy* Read2dKernel(const char* filename);
  static float Convolve(const gevd_bufferxy& from, gevd_bufferxy*& to,// can mutate in place
                        const float* xkernel, // when from==to
                        const int xradius, const bool xevenp,
                        const float* ykernel,
                        const int yradius, const bool yevenp,
                        const bool xwrap=false, const bool ywrap=false);
  static float Convolve(const gevd_bufferxy& from, gevd_bufferxy*& to,// can mutate in place
                        const float* xkernel, // when from==to
                        const int xradius, const bool xevenp,
                        const int yradius,    // running sum along y-axis
                        const bool xwrap=false, const bool ywrap=false);
  static float Convolve(const float* from, float*& to, const int len,
                        const float* kernel, const int radius,
                        const bool evenp, const bool wrap=false);
  static float RunningSum(float* from, float*& to, const int len,
                          const int radius, const bool wrap=false);
  static bool Read1dKernel(const char* filename,
                           float*& kernel, int& radius, bool& evenp);

  static float Gaussian(gevd_bufferxy& img, gevd_bufferxy*& smooth, const float sigma=1.0,
                        const bool xwrap=false, const bool ywrap=false);
  static bool Find1dGaussianKernel(const float sigma,
                                   float*& kernel, int& radius,
                                   const float fuzz=0.02);

  // detection
  static float Gradient(const gevd_bufferxy& smooth, // 1st-derivative
                        gevd_bufferxy*& mag,
                        gevd_bufferxy*& dirx, gevd_bufferxy*& diry,
                        const bool xwrap=false, const bool ywrap=false);
  static float Slope(float* from, float*& to, const int len,
                     const bool wrap=false);
  static float Hessian(const gevd_bufferxy& smooth, // 2nd-derivative
                       gevd_bufferxy*& mag, // max eigenvalue
                       gevd_bufferxy*& dirx, gevd_bufferxy*& diry,
                       const bool xwrap=false, const bool ywrap=false);
  static float Laplacian(const gevd_bufferxy& smooth,
                         gevd_bufferxy*& mag, // sum of 2 eigenvalue/curvature
                         gevd_bufferxy*& dirx, gevd_bufferxy*& diry,
                         const bool xwrap=false, const bool ywrap=false);
  static float Curvature(float* from, float*& to, const int len,
                         const bool wrap=false);
  static float Orientation(const gevd_bufferxy& smooth,
                           gevd_bufferxy*& theta,  gevd_bufferxy*& coherence,
                           const int frame=1);

  static void NonMaximumSuppression(const gevd_bufferxy& magnitude,
                                    const gevd_bufferxy& directionx,
                                    const gevd_bufferxy& directiony,
                                    const float threshold,
                                    gevd_bufferxy*& contour, gevd_bufferxy*& direction,
                                    gevd_bufferxy*& locationx, gevd_bufferxy*& locationy,
                                    const bool xwrap=false,
                                    const bool ywrap=false);
  static int NonMaximumSuppression(const float* from, const int len,
                                   const float threshold,
                                   int*& index, float*& mag, float*& locs,
                                   const bool wrap=false);
  static float InterpolateParabola(float y1, float y0, float y2, // return offset
                                   float&y); // real max/min
  static float BilinearInterpolate(const gevd_bufferxy& img, float x, float y);
  static void SupportAngle(const gevd_bufferxy& dirx, const gevd_bufferxy& diry,
                           const gevd_bufferxy& magnitude,
                           gevd_bufferxy*& angle);

  // specific to range images
  static void SurfaceNormal(const gevd_bufferxy& range, gevd_bufferxy*& normal);
  static void SurfaceCurvature(const gevd_bufferxy& normal, gevd_bufferxy*& curvature);

  static void SurfaceNormalD(const gevd_bufferxy& range, gevd_bufferxy*& normal,
                             float no_value, float pixel_distance=1.0);
  // -rgc- : does SurfaceCurvature need to calculate the actual distances
  // instead of deducing them from the grid?
//static void SurfaceCurvatureD(const gevd_bufferxy& normal, gevd_bufferxy*& curvature,
//                              float dflt, float pixel_distance=1.0);
  static void SurfaceCurvatureD(const gevd_bufferxy& normal, const gevd_bufferxy& surface,
                                gevd_bufferxy*& curvature, float dflt, float pixel_distance=1.0);

  // shrink/expand
  static float ShrinkBy2(const gevd_bufferxy& cfrom, gevd_bufferxy*& to,
                         const float burt_ka=0.359375); // Burt filter coeft
  static float ExpandBy2(const gevd_bufferxy& cfrom, gevd_bufferxy*& to,
                         const float burt_ka=0.359375); // Burt filter coeft
  static void ShrinkBy2_D (const gevd_bufferxy& from, gevd_bufferxy*& to,
                           float no_value, float burt_ka=0.359375 );
  static int Pyramid(const float* from, const int length,
                     float*& to, int& nlevels, int trim=0,
                     const float burt_ka=0.359375); // Burt filter coeft
  static int ShrinkBy2(const float* from, const int length,
                       float*& to, const float burt_ka=0.359375);

  // wavelet transform
  static bool WaveletTransform(float* array, const int n, // 1d array
                               const bool forwardp,
                               int nlevels,
                               const int waveletno=4);
  static bool WaveletTransformByBlock(float* array, // nd imge.
                                      const int* dims, const int ndim,
                                      const bool forwardp,
                                      int nlevels,
                                      const int waveletno=4);
  static bool WaveletTransformByIndex(float* array,
                                      const int* dims, const int ndim,
                                      const bool forwardp, int nlevels,
                                      const int waveletno=4);
  static bool FindWavelet(const int waveletno,
                          float*& cc, float*& cr, int& ncof);
  static void LowHighPyramid(float* highPass, float* lowPass,
                             int n, int nlevels,
                             const float* lo_filter,
                             const float* hi_filter,
                             int ncof,
                             float* wksp);

#if 0 // commented out
  static int DeleteMixedComponents(float* wave, // delete wavelet coefts
                                   const int* dims, const int ndim,
                                   const int nlevels)
  static int TruncateHighFrequencies(float* wave,
                                     const int* dims, const int ndim,
                                     const int nlevels, // throw all small
                                     const float threshold=0.1); // components
  static int TruncateLowestFrequency(float* wave,
                                     const int* dims, const int ndim,
                                     const int nlevels,
                                     float& average); // uniform average
#endif

  static bool WaveletTransformByBlock(const gevd_bufferxy& cfrom, gevd_bufferxy*& to,
                                      const bool forwardp,
                                      int nlevels,
                                      const int waveletno=4);
  static bool WaveletTransformByIndex(const gevd_bufferxy& cfrom, gevd_bufferxy*& to,
                                      const bool forwardp,
                                      int nlevels,
                                      const int waveletno=4);
  static int DeleteMixedComponents(gevd_bufferxy& wave,
                                   const int nlevels);
  static int TruncateHighFrequencies(gevd_bufferxy& wave,
                                     const int nlevels,
                                     const float threshold=0.1);
  static int TruncateLowestFrequency(gevd_bufferxy& wave, const int nlevels);
  static int DeleteBoundaryArtifacts(float* wave, const int n, const int nlevels);
  static int DeleteBoundaryArtifacts(gevd_bufferxy& wave, const int nlevels);

  static void ProjectWaveOntoX(const gevd_bufferxy& buf, float*& proj,
                               const int nlevels=0);
  static void ProjectWaveOntoY(const gevd_bufferxy& buf, float*& proj,
                               const int nlevels=0);

  // projections on the axes.
  static void ProjectOntoX(const gevd_bufferxy& buf, float*& proj,
                           int sizeX=0, int sizeY=0,
                           int origX=0, int origY=0);
  static void ProjectOntoY(const gevd_bufferxy& buf, float*& proj,
                           int sizeX=0, int sizeY=0,
                           int origX=0, int origY=0);

  // Search for maximum 1D/2D correlation
  static float Correlation(const gevd_bufferxy& from, const gevd_bufferxy& kernel,
                           gevd_bufferxy*& to);
  static float CorrelationAlongAxis(const gevd_bufferxy& from,
                                    const gevd_bufferxy& kernel,
                                    gevd_bufferxy*& to);

  static float Correlation(const float* data, const int length,// correlation at index
                           const float* pattern, const int radius,
                           const int index);
  static float* Correlations(const float* data, const int length, // cors around index
                             const float* pattern, const int radius,
                             const int index, const int search);
  static float BestCorrelation(const float* data, const int length, // search max cor
                               const float* pattern, const int radius,
                               int& shift, const int search);
  static float CoarseFineCorrelation(const float* dataPyr, const int dlength,
                                     const float* patternPyr, const int plength,
                                     float& shift, // pattern location in data
                                     const int coarse, const int fine, // level#
                                     const float cutoff=0, // early cutoff of search
                                     const float overlap=0.75, // min overlap
                                     float* matches=0); // trace correlations

#if 0 // commented out
  static gevd_bufferxy* Correlations(const gevd_bufferxy& data, const gevd_bufferxy& pattern,
                         const int* indexes, const int* search);
  static float Correlate(const gevd_bufferxy& data, const gevd_bufferxy& pattern,
                  const int* indexes);
  static float Correlate(const gevd_bufferxy& data, const gevd_bufferxy& pattern,
                  int*& shifts, const int* search);
#endif

  static void Apply(gevd_bufferxy& buf, float (*func)(float));

  // create test patterns
#if 0 // commented out
  static gevd_bufferxy* MakeImpulse(const float mag,
                                    int x=100, int y=100);
  static gevd_bufferxy* MakeGaussianNoise(const float mag, const float sigma,
                                          int x=100, int y=100);
  static gevd_bufferxy* MakeCircles(const float mag, float w1, float w2,
                                    int x=100, int y=100);
  static gevd_bufferxy* MakeEllipses(const float mag, const float w1, const float w2,
                                     const float excentricity,
                                     int x=100, int y=100);
  static gevd_bufferxy* MakeSuperellipses(const float mag,
                                          const float w1, const float w2,
                                          float, float, float,
                                          int x=100, int y=100);
#endif

  // binary operations
  static gevd_bufferxy* AbsoluteDifference(const gevd_bufferxy& buf1, const gevd_bufferxy& buf2);

  // utilities
  static gevd_bufferxy* Allocate(gevd_bufferxy* space, const gevd_bufferxy& model,
                                 int bits_per_pixel=0, int sizeX=0, int sizeY=0);
  static gevd_bufferxy* SimilarBuffer(const gevd_bufferxy& buffer, // use default
                                      int bits_per_pixel=0,// from buffer
                                      int sizeX=0, int sizeY=0);
  static bool IsSimilarBuffer(const gevd_bufferxy& buf1,    // same dimension
                              const gevd_bufferxy& buf2);       // & precision
  static gevd_bufferxy* Extract(const gevd_bufferxy& buf,
                                int sizeX=0, int sizeY=0,
                                int origX=0, int origY=0);
  static void Update(gevd_bufferxy& old_buf, const gevd_bufferxy& new_buf,
                     int origX=0, int origY=0);
  static gevd_bufferxy* TransposeExtract(const gevd_bufferxy & buf,
                                         int sizeX, int sizeY, int origX, int origY);
  static void TransposeUpdate(gevd_bufferxy & buf, const gevd_bufferxy& sub,
                              int origX, int origY);

  static void Fill(gevd_bufferxy& buf, const float value,
                   int sizeX=0, int sizeY=0,
                   int origX=0, int origY=0);
  static void FillFrameX(gevd_bufferxy& buf, const float value, const int width=1);
  static void FillFrameY(gevd_bufferxy& buf, const float value, const int width=1);
  static void DrawFrame(gevd_bufferxy& buf, const int loc, const float value=0);

  static gevd_bufferxy* PadToPowerOf2(gevd_bufferxy& buf);
  static gevd_bufferxy* UnpadFromPowerOf2(gevd_bufferxy& padded, int sizeX, int sizeY);

  static float Maximum(const gevd_bufferxy& buf,
                       int sizeX=0, int sizeY=0,
                       int origX=0, int origY=0);
  static float Minimum(const gevd_bufferxy& buf,
                       int sizeX=0, int sizeY=0,
                       int origX=0, int origY=0);
  static bool Maximum(const float* data, const int length,
                      int& index, float& value);

  static float Sum(const gevd_bufferxy& buf,
                   int sizeX=0, int sizeY=0,
                   int origX=0, int origY=0);
  static int Threshold(gevd_bufferxy& buf, float noise,
                       int sizeX=0, int sizeY=0,
                       int origX=0, int origY=0);

  static float TruncateToPositive(gevd_bufferxy& buf);
  static float TruncateToCeiling(gevd_bufferxy& buf, float ceil);
  static void Absolute(gevd_bufferxy& buf);
  static void Negate(gevd_bufferxy& buf);
  static void Scale(gevd_bufferxy& buf, float factor);
  static void ShiftToPositive(gevd_bufferxy& buf);
  static void Normalize(gevd_bufferxy& buf, const float lo, const float hi);

  // conversion
  static bool BufferToFloat(const gevd_bufferxy& from, gevd_bufferxy& floatto);
  static bool FloatToBuffer(const gevd_bufferxy& floatfrom, gevd_bufferxy& to);

 protected:
  static int SetupPipeline(const float* data, const int len,
                           const int kradius, const bool wrap,
                           float*& cache, float*& pipeline);
  static gevd_bufferxy* WrapAlongX(const gevd_bufferxy& img);
  static gevd_bufferxy* WrapAlongY(const gevd_bufferxy& img);
  static float Gaussian(const float x, const float sigma);
  static float ShrinkBy2AlongX(const gevd_bufferxy& cfrom, const int y,
                               float* yline, const int len,
                               const float ka, const float kb, const float kc);
  static void ShrinkBy2AlongX_D( const gevd_bufferxy& from, int from_sizeX,
                                 int sizeX, int y, float kernel[],
                                 float no_value, float* yline, float* wline );
  static float ExpandBy2AlongX(const gevd_bufferxy& cfrom, const int y,
                               float* yline, const int len,
                               const float ka, const float kb, const float kc);
  static void WaveletTransformStep(float* array, const int n,
                                   const bool forwardp,
                                   const float* lo_filter,
                                   const float* hi_filter,
                                   const int ncof,
                                   float* wksp);
  static void WaveletTransformStep(float* array, const int* dims, const int ndim,
                                   const bool forwardp,
                                   const float* lo_filter,
                                   const float* hi_filter,
                                   const int ncof,
                                   float* buffer, float* wksp);
  static void CopyNdRecursive(const float* from_array,
                              const int from_size, const int* from_dims,
                              float* to_array,
                              const int to_size, const int* to_dims,
                              const int ndim, const bool fullp=true);
  static void TestWavelets();
};

#endif // gevd_float_operators_h_
