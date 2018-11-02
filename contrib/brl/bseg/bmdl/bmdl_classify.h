// This is brl/bseg/bmdl/bmdl_classify.h
#ifndef bmdl_classify_h_
#define bmdl_classify_h_
//:
// \file
// \brief Classify each pixel in lidar images
//
// \author Matt Leotta
// \date Oct. 14, 2008

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

template <class T>
class bmdl_classify
{
 public:
  //: Constructor
  // \param height_noise_stdev is the standard deviation in lidar height
  //        This parameter can be set manually or estimated
  // \param area_threshold is the minimum area allowed for buildings
  // \param height_resolution is the height difference below which buildings are merged
  // \param gnd_threshold is the minimum height above the ground considered for buildings and vegetation
  // \param veg_threshold is the minimum distance between first and last returns for vegetation
  bmdl_classify(unsigned int area_threshold = 6,
                T height_resolution = 0.5,
                T gnd_threshold = 2.0,
                T veg_threshold_ = 1.0);

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
  // \note This parameter is not currently used.  Use ground and vegetation thresholds instead.
  void set_height_noise_stdev(T stdev) { hgt_stdev_ = stdev; }
  //: Specify the building area threshold
  void set_area_threshold(unsigned int area) { area_threshold_ = area; }
  //: Specify the ground threshold
  void set_ground_threshold(T gnd_threshold) { gnd_threshold_ = gnd_threshold; }
  //: Specify the vegetation threshold
  void set_vegetation_threshold(T veg_threshold) { veg_threshold_ = veg_threshold; }

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

  //: Classify each pixel as Ground (0), Vegetation (1), or Building (>=2)
  // Each building is given an index sequentially starting with 2
  // and sorted by mean height.
  // \note This is the main function you should call, it runs all the steps
  void label_lidar();

  //: Perform an initial segmentation at each pixel using thresholds
  // Classify each pixel as Ground (0), Vegetation (1), or Building (2)
  // Results are stored in the labels image
  void segment();

  //: Cluster pixels on buildings into groups of adjacent pixels with similar heights.
  //  Assign a new label to each groups.
  //  Returns building mean heights and pixel counts by reference
  void cluster_buildings();

  //: Replace small holes in buildings from ground to vegetation (unclaimed) labels
  // Holes are switch if area is less than \a min_size
  void fill_ground_holes(unsigned int min_size);

  //: Threshold buildings by area.
  // All buildings with area (in pixels) less than the threshold
  // are removed and replace with a vegetation label.
  void threshold_building_area();

  //: Dilate buildings into unclaimed (vegetation) pixel
  //  Only claim a vegetation pixel if surrounded by
  //  \a num pixels from the same building
  bool dilate_buildings(unsigned int num=6);

  //: Greedy merging of adjacent buildings
  bool greedy_merge();

  //: Refine the building regions
  void refine_buildings();

  //: Access the resulting label image
  const vil_image_view<unsigned int>& labels() const { return labels_; }

  //: Access the resulting height image
  const vil_image_view<T>& heights() const { return heights_; }
  //: Access the mean building heights
  const std::vector<T>& mean_heights() const { return building_mean_hgt_; }
  //: Access the building areas in pixels
  const std::vector<unsigned int>& building_area() const { return building_area_; }

 private:
  //: A helper class to manage merging of buildings
  class merge_map
  {
   public:
    //: Constructor
    merge_map(bmdl_classify<T>* c);
    //: Destructor - simplify merge map and apply to classifier
    ~merge_map();
    //: translate old index to temporary merged index
    unsigned int translate(unsigned int idx) const;
    //: merge two indices
    void merge(unsigned int idx1, unsigned int idx2);
   private:
    bmdl_classify<T>* classifier_;
    std::vector<unsigned int> idx_map_;
  };

  friend class merge_map;

  //: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
  //  \returns the peak value by reference in \p y_peak
  //  \returns the peak location offset from the x of \p y_0
  T interpolate_parabola(T y_1, T y_0, T y_2, T& root_offset) const;

  //: Compute a histogram of the data
  // Does not reset the initial bin values to zero
  void histogram(const std::vector<T>& data, std::vector<unsigned int>& bins,
                 T minv, T maxv) const;

  //: Find the maximum peak in the data and fit a gaussian to it
  // Search in the range \a minv to \a maxv
  void fit_gaussian_to_peak(const std::vector<T>& data, T minv, T maxv,
                            T& mean, T& stdev) const;

  //: Expand the range (minv, maxv) with the data in \a image
  // Only finite values count
  void range(const vil_image_view<T>& image,
             T& minv, T& maxv) const;

  //: Search for nearby pixel that can be added to each building
  //  Return true if any changes are made
  bool expand_buildings(std::vector<T>& means,
                        std::vector<unsigned int>& sizes);

  //: Group building pixel by height into bins of size \a binsize
  vil_image_view<unsigned int> bin_heights(T binsize = 0.5);


  //: Morphological clean up on each building independently
  std::vector<bool> close_buildings(unsigned int num_labels);

  //: first return image
  vil_image_view<T> first_return_;
  //: last return image
  vil_image_view<T> last_return_;
  //: bare earth estimate image
  vil_image_view<T> bare_earth_;

  //: The estimated standard deviation of noise in height
  T hgt_stdev_;
  //: A threshold on the minimum building area
  unsigned int area_threshold_;
  //: The minimum height difference that does not merge
  T height_resolution_;
  //: The minimum height above the ground for buildings
  T gnd_threshold_;
  //: The minimum distance between first and last returns for vegetation
  T veg_threshold_;
  //: The range spanned by the first returns
  T first_min_, first_max_;
  //: The range spanned by the last returns
  T last_min_, last_max_;

  //: The mean height of each building
  std::vector<T> building_mean_hgt_;
  //: The area in pixels of each building
  std::vector<unsigned int> building_area_;

  //: computed segmentation labels
  vil_image_view<unsigned int> labels_;
  //: clean up height estimates for use in meshing
  vil_image_view<T> heights_;
};

#endif // bmdl_classify_h_
