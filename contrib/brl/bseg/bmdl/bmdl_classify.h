// This is brl/bseg/bmdl/bmdl_classify.h
#ifndef bmdl_classify_h_
#define bmdl_classify_h_
//:
// \file
// \brief Classify each pixel in lidar images
//
// \author Matt Leotta
// \date Oct. 14, 2008

#include <vcl_vector.h>
#include <vil/vil_image_view.h>
//#include <vgl/vgl_box_2d.h>

template <class T>
class bmdl_classify
{
 public:
  //: Constructor
  //  \param height_noise_stdev is the standard deviation in lidar height
  //  This parameter can be set manually or estimated
  bmdl_classify(T height_noise_stdev = 0.01);

  //: Set the first and last return images
  void set_lidar_data(const vil_image_view<T>& first_return,
                      const vil_image_view<T>& last_return);

  //: Set the bare earth image
  void set_bare_earth(const vil_image_view<T>& bare_earth);

  //: Estimate a constant bare earth model
  // This function returns the constant height
  // and also uses it to fill the bare earth image
  T estimate_bare_earth();

  //: Estimate the standard deviation in height due to noise
  //  Returns the estimate and stores it internally for later use
  T estimate_height_noise_stdev();

  //: Manually specify the standard deviation in lidar height from noise
  void set_height_noise_stdev(T stdev) { hgt_stdev_ = stdev; }

  //: Access the first returns image
  const vil_image_view<T>& first_return() const {return first_return_;}

  //: Access the last returns image
  const vil_image_view<T>& last_return() const {return last_return_;}

  //: Access the bare earth image
  const vil_image_view<T>& bare_earth() const {return bare_earth_;}

  //: Access the minimum height in the first return image
  T first_min() const {return first_min_;}
  //: Access the maximum height in the first return image
  T first_max() const {return first_max_;}
  //: Access the minimum height in the last return image
  T last_min() const {return last_min_;}
  //: Access the maximum height in the last return image
  T last_max() const {return last_max_;}

  //: Classify each pixel as Ground (0), Vegitation (1), or Building (>=2)
  // Each building is given an index sequentially starting with 2
  // and sorted by mean height.
  // \note This is the main function you should call, it runs all the steps
  void label_lidar();

  //: Perform an initial segementation at each pixel using thresholds
  // Classify each pixel as Ground (0), Vegitation (1), or Building (2)
  // Results are stored in the labels image
  void segment();

  //: Cluster pixels on buildings into groups of adjacent pixels with similar heights.
  //  Assign a new label to each groups.
  //  Returns building mean heights and pixel counts by reference
  void cluster_buildings(vcl_vector<T>& means,
                         vcl_vector<unsigned int>& sizes);

  //: Refine the building regions
  void refine_buildings(vcl_vector<T>& means,
                        vcl_vector<unsigned int>& sizes);

  //: Access the resulting label image
  vil_image_view<unsigned int> labels() const { return labels_; }

  //: Access the resulting height image
  vil_image_view<T> heights() const { return heights_; }

 private:

  //: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
  //  \returns the peak value by reference in \p y_peak
  //  \returns the peak location offset from the x of \p y_0
  T interpolate_parabola(T y_1, T y_0, T y_2, T& root_offset) const;

  //: Compute a histogram of the data
  // Does not reset the initial bin values to zero
  void histogram(const vcl_vector<T>& data, vcl_vector<unsigned int>& bins,
                 T minv, T maxv) const;

  //: Find the maximum peak in the data and fit a gaussian to it
  // Search in the range \a minv to \a maxv
  void fit_gaussian_to_peak(const vcl_vector<T>& data, T minv, T maxv,
                            T& mean, T& stdev) const;

  //: expand the range (minv, maxv) with the data in \a image
  // Only finite values count
  void range(const vil_image_view<T>& image,
             T& minv, T& maxv) const;

  //: Search for nearby pixel that can be added to each building
  //  Return true if any changes are made
  bool expand_buildings(vcl_vector<T>& means,
                        vcl_vector<unsigned int>& sizes);

  //: Morphological clean up on each building independently
  vcl_vector<bool> close_buildings(unsigned int num_labels);

  //: first return image
  vil_image_view<T> first_return_;
  //: last return image
  vil_image_view<T> last_return_;
  //: bare earth estimate image
  vil_image_view<T> bare_earth_;

  //: The estimated standard deviation of noise in height
  T hgt_stdev_;
  //: The range spanned by the first returns
  T first_min_, first_max_;
  //: The range spanned by the last returns
  T last_min_, last_max_;

  //: computed segmentation labels
  vil_image_view<unsigned int> labels_;
  //: clean up height estimates for use in meshing
  vil_image_view<T> heights_;
};

#endif // bmdl_classify_h_
