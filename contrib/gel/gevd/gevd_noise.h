#ifndef gevd_noise_h_
#define gevd_noise_h_

// .NAME gevd_noise - Estimation of noise from histogram of weak responses
//
// Fit a Raleigh distribution to the histogram curve of all
// edgels with low magnitudes, h(x), to estimate the sensor noise,
// as would be zero-crossing of dh(x), and texture noise as the dominant
// peak in h(x).
//
// Setting the threshold at 3 times the sensor/texture noise response
// found would eliminate 99% of all noisy edges.
// Knowing the filter, the standard deviation of noise in the original
// image can be deduced from the above response values.
// Assume no intensity truncation, if not, the sensor/texture noise has
// been already artificially removed from the image, and so can not
// be estimated.

// - Input:      edgels in a typical ROI.
// - Output:     histogram of low response edgels, and
//               estimate of sensor/texture responses.
// - Complexity: O(|data|) time and space.
//
// \verbatim
//  Authors
//   Harry Voorhees  (1987) SM Thesis
//   Van-Duc Nguyen  (1989) CLOS implementation
//   Van-Duc Nguyen  (1996) C++ implementation
// \endverbatim

class gevd_bufferxy;

class gevd_noise
{
 public:
  friend class DetectionUI;
  gevd_noise(const float* data, const int n, // data in a typical region
             const int number_of_bins=200);  // granularity of histogram
  ~gevd_noise();

  static float* EdgelsInCenteredROI(const gevd_bufferxy& magnitude,
                                    const gevd_bufferxy& dirx,
                                    const gevd_bufferxy& diry,
                                    int& nedgel,
                                    const int roiArea=250*250); // ROI
  bool EstimateSensorTexture(float& sensor, // sensor noise is would-be zc
                             float& texture) const; // texture is real zc

  const float* Histogram(int& n) const { n = nbin; return hist; }
  float BinSize() const { return binsize; } // size of bin in data unit

 protected:
  float* hist;   // histogram curve of low responses only
  int nbin;      // number of bins
  float binsize; // size of bin in data unit

 protected:
  static bool WouldBeZeroCrossing(const float* dhist, const int nbin,
                                  float& index);
  static bool RealZeroCrossing(const float* dhist, const int nbin,
                               float& index);
 private:
  gevd_noise() = delete;
};

#endif // gevd_noise_h_
