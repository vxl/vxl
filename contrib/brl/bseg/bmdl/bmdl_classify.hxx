// This is brl/bseg/bmdl/bmdl_classify.hxx
#ifndef bmdl_classify_hxx_
#define bmdl_classify_hxx_
//:
// \file
// \brief Classify each pixel in lidar images
//
// \author Matt Leotta
// \date Oct. 14, 2008

#include <limits>
#include <iostream>
#include <algorithm>
#include <utility>
#include <set>
#include <map>
#include "bmdl_classify.h"
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>

//: Constructor
// \param height_noise_stdev is the standard deviation in lidar height
//  This parameter can be set manually or estimated
// \param area_threshold is the minimum area allowed for buildings
// \param height_resolution is the height difference below which buildings are merged
// \param gnd_threshold is the minimum height above the ground considered for buildings
// \param veg_threshold is the minimum distance between first and last returns for vegetation
template <class T>
bmdl_classify<T>::bmdl_classify(unsigned int area_threshold,
                                T height_resolution,
                                T gnd_threshold,
                                T veg_threshold)
: hgt_stdev_(T(0.01)),
  area_threshold_(area_threshold),
  height_resolution_(height_resolution),
  gnd_threshold_(gnd_threshold),
  veg_threshold_(veg_threshold),
  first_min_( std::numeric_limits<T>::infinity()),
  first_max_(-std::numeric_limits<T>::infinity()),
  last_min_ ( std::numeric_limits<T>::infinity()),
  last_max_ (-std::numeric_limits<T>::infinity())
{
}


//: Set the first and last return images
template <class T>
void bmdl_classify<T>::set_lidar_data(const vil_image_view<T>& first_return,
                                      const vil_image_view<T>& last_return)
{
  assert(first_return.ni() == last_return.ni());
  assert(first_return.nj() == last_return.nj());
  first_return_ = first_return;
  last_return_ = last_return;

  // find the range of finite values in the images
  bmdl_classify::range(first_return_,first_min_,first_max_);
  bmdl_classify::range(last_return_,last_min_,last_max_);
}


//: Set the bare earth image
template <class T>
void bmdl_classify<T>::set_bare_earth(const vil_image_view<T>& bare_earth)
{
  assert(first_return_.ni() == bare_earth.ni());
  assert(first_return_.nj() == bare_earth.nj());
  bare_earth_ = bare_earth;
}


//: Estimate a constant bare earth model
// This function returns the constant height
// and also uses it to fill the bare earth image
template <class T>
T bmdl_classify<T>::estimate_bare_earth()
{
  std::vector<T> data;
  unsigned int ni = last_return_.ni();
  unsigned int nj = last_return_.nj();

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (vnl_math::isfinite(last_return_(i,j))) {
        data.push_back(last_return_(i,j));
      }
    }
  }

  T mean = 0.0;
  T gnd_stdev = 0.0;
  fit_gaussian_to_peak(data,last_min_,last_max_,mean,gnd_stdev);

  bare_earth_.set_size(ni,nj);
  bare_earth_.fill(mean);

  return mean;
}


//: Estimate the standard deviation in height due to noise
//  Returns the estimate and stores it internally for later use
template <class T>
T bmdl_classify<T>::estimate_height_noise_stdev()
{
// Warning: this code is a rough approximation
// it probably should be rewritten to do it correctly

  unsigned int ni = first_return_.ni();
  unsigned int nj = first_return_.nj();
  // test that all inputs have been set
  assert(ni>0 && nj>0);
  assert(last_return_.ni() == ni);
  assert(last_return_.nj() == nj);

  std::vector<T> diff;
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (!vnl_math::isfinite(last_return_(i,j)) || !vnl_math::isfinite(first_return_(i,j)))
        continue;
      diff.push_back(first_return_(i,j) - last_return_(i,j));
    }
  }
  std::sort(diff.begin(),diff.end());
  // discard top 5% of points for robustness
  T max_diff = diff[int(diff.size()*.95)];

  if (max_diff == 0.0)
    std::cout << "Warning: first and last return images are identical" << std::endl;

  T mean = 0.0;
  fit_gaussian_to_peak(diff,0,max_diff,mean,hgt_stdev_);

  std::cout << "diff mean = "<< mean << " stdev = "<< hgt_stdev_ << std::endl;
  return hgt_stdev_;
}


//: Classify each pixel as Ground (0), Vegetation (1), or Building (>=2)
// Each building is given an index sequentially starting with 2
// and sorted by mean height.
template <class T>
void bmdl_classify<T>::label_lidar()
{
  unsigned int ni = first_return_.ni();
  unsigned int nj = first_return_.nj();
  // test that all inputs have been set
  assert(ni>0 && nj>0);
  assert(last_return_.ni() == ni);
  assert(last_return_.nj() == nj);
  assert(bare_earth_.ni() == ni);
  assert(bare_earth_.nj() == nj);
  assert(hgt_stdev_ > 0.0);

  // 1. First segment the image into ground, buildings, and vegetation
  segment();

  // 2. Cluster the pixels for buildings and apply unique labels
  cluster_buildings();

  // 3. Merge similar sections
  while (greedy_merge())
    ;

  // Replace small holes in buildings from ground to vegetation labels
  fill_ground_holes(5);

  // 4. Remove holes
  while (dilate_buildings(5))
    ;

  // 5. Remove buildings that are two small in area
  threshold_building_area();

  // 6. Remove holes
  while (dilate_buildings(5))
    ;

  // 7. Merge similar sections
  while (greedy_merge())
    ;

  // 8. Refine building regions with various morphological operations
  refine_buildings();

  // 9. Determine the heights to use for meshing
  heights_.set_size(ni,nj);
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j) > 1)
        heights_(i,j) = building_mean_hgt_[labels_(i,j)-2];
      else if (labels_(i,j) == 1)
        heights_(i,j) = first_return_(i,j);
      else
        heights_(i,j) = bare_earth_(i,j);
    }
  }
}


//: Perform an initial segmentation at each pixel using thresholds
//  Classify each pixel as Ground (0), Vegetation (1), or Building (2)
//  Results are stored in the labels image
template <class T>
void bmdl_classify<T>::segment()
{
  unsigned int ni = first_return_.ni();
  unsigned int nj = first_return_.nj();
  // test that all inputs have been set
  assert(ni>0 && nj>0);
  assert(last_return_.ni() == ni);
  assert(last_return_.nj() == nj);
  assert(bare_earth_.ni() == ni);
  assert(bare_earth_.nj() == nj);
  assert(hgt_stdev_ > 0.0);

  // resize the outputs
  labels_.set_size(ni,nj);

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      // test for ground
      if (!vnl_math::isfinite(first_return_(i,j)) ||
          first_return_(i,j) - bare_earth_(i,j) < gnd_threshold_)
        labels_(i,j) = 0;
      // test for vegetation
      else if ((first_return_(i,j) - last_return_(i,j)) > veg_threshold_)
        labels_(i,j) = 1;
      // otherwise building
      else
        labels_(i,j) = 2;
    }
  }
}


//: Cluster pixels on building into groups of adjacent pixels with similar heights.
//  Assign a new label to each groups.
//  Returns building mean heights and pixel counts by reference
template <class T>
void bmdl_classify<T>::cluster_buildings()
{
  unsigned int ni=first_return_.ni();
  unsigned int nj=first_return_.nj();
  // test that all inputs have been set
  assert(ni>0 && nj>0);
  assert(last_return_.ni() == ni);
  assert(last_return_.nj() == nj);
  assert(labels_.ni() == ni);
  assert(labels_.nj() == nj);
  assert(hgt_stdev_ > 0.0);

  building_mean_hgt_.clear();
  building_area_.clear();

  // bin building heights
  vil_image_view<unsigned int> bin_img = bin_heights(height_resolution_);

  std::vector<unsigned int> merge_map;
  std::vector<unsigned int> count;
  std::vector<T> mean;

  // handle first pixel
  if (labels_(0,0) >= 2)
  {
    count.push_back(1);
    merge_map.push_back(merge_map.size());
    mean.push_back(last_return_(0,0));
    labels_(0,0) = count.size()+1;
  }
  // handle first row
  for (unsigned int i=1; i<ni; ++i) {
    if (labels_(i,0)!=2)
      continue;

    int idx = labels_(i-1,0)-2;
    while (idx>=0 && merge_map[idx] != (unsigned int)idx )
      idx = merge_map[idx];
    T val = last_return_(i,0);
    if (idx>=0 && bin_img(i-1,0) == bin_img(i,0)) {
      labels_(i,0) = idx+2;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else{
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels_(i,0) = count.size()+1;
    }
  }
  // handle first column
  for (unsigned int j=1; j<nj; ++j) {
    if (labels_(0,j)!=2)
      continue;

    int idx = labels_(0,j-1)-2;
    while (idx>=0 && merge_map[idx] != (unsigned int)idx )
      idx = merge_map[idx];
    T val = last_return_(0,j);
    if (idx>=0 && bin_img(0,j-1) == bin_img(0,j)) {
      labels_(0,j) = idx+2;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else {
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels_(0,j) = count.size()+1;
    }
  }

  // handle the rest of the image
  for (unsigned int j=1; j<nj; ++j)
  {
    for (unsigned int i=1; i<ni; ++i)
    {
      if (labels_(i,j)!=2)
        continue;

      T val = last_return_(i,j);
      int idx1 = labels_(i-1,j)-2, idx2 = labels_(i,j-1)-2;
      while (idx1>=0 && merge_map[idx1] != (unsigned int)idx1 )
        idx1 = merge_map[idx1];
      while (idx2>=0 && merge_map[idx2] != (unsigned int)idx2 )
        idx2 = merge_map[idx2];
      int idx = -1;
      if (idx1>=0 && (bin_img(i-1,j) == bin_img(i,j)) )
        idx = idx1;
      if (idx2>=0 && (bin_img(i,j-1) == bin_img(i,j)) )
      {
        if (idx == -1 || idx1 == idx2)
          idx = idx2;
        else {
          labels_(i,j) = idx1+2;
          mean[idx1] = (mean[idx1]*count[idx1] + mean[idx2]*count[idx2] + val)
                      /(count[idx1]+count[idx2]+1);
          count[idx1] += count[idx2]+1;
          count[idx2] = 0;
          merge_map[idx2] = idx1;
          continue;
        }
      }

      if (idx >= 0 ) {
        labels_(i,j) = idx+2;
        mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
        ++count[idx];
      }
      else {
        count.push_back(1);
        merge_map.push_back(merge_map.size());
        mean.push_back(val);
        labels_(i,j) = count.size()+1;
      }
    }
  }
  std::cout << "num labels = "<<count.size() << std::endl;

  // simplify merge map
  std::vector<std::pair<T,int> > unique;
  for (unsigned int i=0; i<merge_map.size(); ++i)
  {
    if (merge_map[i] == i) {
      unique.push_back(std::pair<T,int>(mean[i],i));
      continue;
    }
    unsigned int i2 = i;
    while (merge_map[i2] != i2 )
      merge_map[i] = i2 = merge_map[i2];
  }
  std::sort(unique.begin(), unique.end());

  building_mean_hgt_.resize(unique.size(),0.0);
  building_area_.resize(unique.size(),0);

  std::vector<unsigned int> unique_map(merge_map.size(),0);
  for (unsigned int i=0; i<unique.size(); ++i) {
    unique_map[unique[i].second] = i;
    building_mean_hgt_[i] = unique[i].first;
    building_area_[i] = count[unique[i].second];
  }
  for (unsigned int i=0; i<unique_map.size(); ++i)
    if (merge_map[i] != i)
      unique_map[i] = unique_map[merge_map[i]];

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j)>=2)
        labels_(i,j) = unique_map[labels_(i,j)-2]+2;
    }
  }

  std::cout << "After clustering there are "
           <<building_area_.size()<<" buildings" << std::endl;
}


//: Replace small holes in buildings from ground to vegetation labels
// Holes are switch if area is less than \a min_size
template <class T>
void bmdl_classify<T>::fill_ground_holes(unsigned int min_size)
{
  unsigned int ni = labels_.ni();
  unsigned int nj = labels_.nj();

  vil_image_view<unsigned int> labels(ni,nj);
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j)>0)
        labels(i,j) = 0;
      else
        labels(i,j) = 1;
    }
  }

  std::vector<unsigned int> merge_map;
  std::vector<unsigned int> count;

  // handle first pixel
  if (labels(0,0) > 0)
  {
    count.push_back(1);
    merge_map.push_back(merge_map.size());
    labels(0,0) = count.size();
  }
  // handle first row
  for (unsigned int i=1; i<ni; ++i) {
    if (labels(i,0)!=1)
      continue;

    int idx = labels(i-1,0)-1;
    while (idx>=0 && merge_map[idx] != (unsigned int)idx )
      idx = merge_map[idx];
    if (idx>=0) {
      labels(i,0) = idx+1;
      ++count[idx];
    }
    else{
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      labels(i,0) = count.size();
    }
  }
  // handle first column
  for (unsigned int j=1; j<nj; ++j) {
    if (labels(0,j)!=1)
      continue;

    int idx = labels(0,j-1)-1;
    while (idx>=0 && merge_map[idx] != (unsigned int)idx )
      idx = merge_map[idx];
    if (idx>=0) {
      labels(0,j) = idx+1;
      ++count[idx];
    }
    else {
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      labels(0,j) = count.size();
    }
  }

  // handle the rest of the image
  for (unsigned int j=1; j<nj; ++j)
  {
    for (unsigned int i=1; i<ni; ++i)
    {
      if (labels(i,j)!=1)
        continue;

      int idx1 = labels(i-1,j)-1, idx2 = labels(i,j-1)-1;
      while (idx1>=0 && merge_map[idx1] != (unsigned int)idx1 )
        idx1 = merge_map[idx1];
      while (idx2>=0 && merge_map[idx2] != (unsigned int)idx2 )
        idx2 = merge_map[idx2];
      int idx = -1;
      if (idx1>=0)
        idx = idx1;
      if (idx2>=0)
      {
        if (idx == -1 || idx1 == idx2)
          idx = idx2;
        else {
          labels(i,j) = idx1+1;
          count[idx1] += count[idx2]+1;
          count[idx2] = 0;
          merge_map[idx2] = idx1;
          continue;
        }
      }

      if (idx >= 0 ) {
        labels(i,j) = idx+1;
        ++count[idx];
      }
      else {
        count.push_back(1);
        merge_map.push_back(merge_map.size());
        labels(i,j) = count.size();
      }
    }
  }
  std::cout << "num ground labels = "<<count.size() << std::endl;

  // update the original labels
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels(i,j)==0)
        continue;

      int idx = labels(i,j)-1;
      while (idx>=0 && merge_map[idx] != (unsigned int)idx )
        idx = merge_map[idx];
      if (count[idx] <= min_size) {
        labels_(i,j) = 1;
      }
    }
  }
}


//: Threshold buildings by area
// All buildings with area (in pixels) less than the threshold
// are removed and replace with a vegetation label
template <class T>
void bmdl_classify<T>::threshold_building_area()
{
  std::vector<T> new_means;
  std::vector<unsigned int> new_areas;
  std::vector<unsigned int> label_map(building_area_.size(),1);

  for (unsigned int i=0; i<building_area_.size(); ++i) {
    if (building_area_[i] >= area_threshold_)
    {
      label_map[i] = new_areas.size()+2;
      new_means.push_back(building_mean_hgt_[i]);
      new_areas.push_back(building_area_[i]);
    }
  }

  // update label image with new labels
  unsigned int ni = labels_.ni();
  unsigned int nj = labels_.nj();
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j)>=2)
        labels_(i,j) = label_map[labels_(i,j)-2];
    }
  }
  building_area_.swap(new_areas);
  building_mean_hgt_.swap(new_means);

  std::cout << "After thresholding by area there are "
           <<building_area_.size()<<" buildings" << std::endl;
}


//: Refine the building regions
template <class T>
void bmdl_classify<T>::refine_buildings()
{
  // test that all inputs have been set
  assert(first_return_.ni() > 0);
  assert(first_return_.nj() > 0);
  assert(last_return_.ni() == first_return_.ni());
  assert(last_return_.nj() == first_return_.nj());
  assert(labels_.ni() == first_return_.ni());
  assert(labels_.nj() == first_return_.nj());
  assert(hgt_stdev_ > 0.0);

  expand_buildings(building_mean_hgt_, building_area_);

  //while (expand_buildings(building_mean_hgt_, building_area_)) ;

#if 0
  std::vector<bool> valid = close_buildings(building_mean_hgt_.size());

  std::vector<T> new_means;
  std::vector<unsigned int> idx_map(building_mean_hgt_.size(),0);
  unsigned cnt = 1;
  for (unsigned int i=0; i<valid.size(); ++i) {
    if (valid[i]) {
      new_means.push_back(building_mean_hgt_[i]);
      idx_map[i] = ++cnt;
    }
  }
  building_mean_hgt_.swap(new_means);

  // relabel buildings with reduced label set
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j) > 1)
        labels_(i,j) = idx_map[labels_(i,j)-2];
    }
  }
#endif // 0
}


//=========================================================
// Private helper functions


// Merge map helper class
//=======================

//: Constructor
template <class T>
bmdl_classify<T>::merge_map::merge_map(bmdl_classify<T>* c)
: classifier_(c)
{
  assert(c);
  idx_map_.resize(classifier_->building_mean_hgt_.size());
  for (unsigned int i=0; i<idx_map_.size(); ++i)
    idx_map_[i] = i;
}

//: Destructor - simplify merge map and apply to classifier
template <class T>
bmdl_classify<T>::merge_map::~merge_map()
{
  std::vector<T>& mean = classifier_->building_mean_hgt_;
  std::vector<unsigned int>& count = classifier_->building_area_;

  // simplify merge map
  std::vector<std::pair<T,int> > unique;
  for (unsigned int i=0; i<idx_map_.size(); ++i)
  {
    if (idx_map_[i] == i) {
      unique.push_back(std::pair<T,int>(mean[i],i));
      continue;
    }
    unsigned int i2 = i;
    while (idx_map_[i2] != i2 )
      idx_map_[i] = i2 = idx_map_[i2];
  }
  std::sort(unique.begin(), unique.end());

  std::vector<T> new_mean(unique.size(),0.0);
  std::vector<unsigned int> new_count(unique.size(),0);

  std::vector<unsigned int> unique_map(idx_map_.size(),0);
  for (unsigned int i=0; i<unique.size(); ++i) {
    unique_map[unique[i].second] = i;
    new_mean[i] = unique[i].first;
    new_count[i] = count[unique[i].second];
  }
  for (unsigned int i=0; i<unique_map.size(); ++i)
    if (idx_map_[i] != i)
      unique_map[i] = unique_map[idx_map_[i]];

  vil_image_view<unsigned int>& labels = classifier_->labels_;
  unsigned int ni = labels.ni();
  unsigned int nj = labels.nj();
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels(i,j)>=2)
        labels(i,j) = unique_map[labels(i,j)-2]+2;
    }
  }
  mean.swap(new_mean);
  count.swap(new_count);
}

//: translate old index to temporary merged index
template <class T>
unsigned int
bmdl_classify<T>::merge_map::translate(unsigned int idx) const
{
  while (idx_map_[idx] != idx) idx = idx_map_[idx];
  return idx;
}

//: merge two indices
template <class T>
void bmdl_classify<T>::merge_map::merge(unsigned int idx1, unsigned int idx2)
{
  idx1 = translate(idx1);
  idx2 = translate(idx2);
  if (idx1 == idx2)
    return;

  std::vector<T>& mean = classifier_->building_mean_hgt_;
  std::vector<unsigned int>& count = classifier_->building_area_;

  mean[idx1] = (mean[idx1]*count[idx1] + mean[idx2]*count[idx2])
               /(count[idx1]+count[idx2]);
  count[idx1] += count[idx2];
  count[idx2] = 0;
  idx_map_[idx2] = idx1;
}

//=======================

//: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
//  \returns the peak value by reference in \p y_peak
//  \returns the peak location offset from the x of \p y_0
template <class T>
T bmdl_classify<T>::interpolate_parabola(T y_1, T y_0, T y_2, T& root_offset) const
{
  T diff1 = y_2 - y_1;      // first derivative
  T diff2 = 2 * y_0 - y_1 - y_2; // second derivative
  //y_peak = y_0 + diff1 * diff1 / (8 * diff2);        // interpolate y as max/min
  root_offset = std::abs(std::sqrt(diff1*diff1/4 + 2*diff2*y_0) / diff2);
  return diff1 / (2 * diff2);   // interpolate x offset
}


//: compute a histogram of the data
// Does not reset the initial bin values to zero
template <class T>
void bmdl_classify<T>::histogram(const std::vector<T>& data, std::vector<unsigned int>& bins,
                                 T minv, T maxv) const
{
  assert(maxv > minv);
  int num_bins = bins.size();
  assert(num_bins > 0);
  T binsize = (maxv-minv)/num_bins;

  for (unsigned int i=0; i<data.size(); ++i)
  {
    int bin = static_cast<int>((data[i]-minv)/binsize);
    if (bin >= num_bins || bin < 0)
      continue;
    ++bins[bin];
  }
}


//: find a peak in the data and fit a gaussian to it
// Search in the range \a minv to \a maxv
template <class T>
void bmdl_classify<T>::fit_gaussian_to_peak(const std::vector<T>& data, T minv, T maxv,
                                            T& mean, T& stdev) const
{
  unsigned int num_bins = 100;
  T binsize = (maxv-minv)/num_bins;
  std::vector<unsigned int> hist(100,0);
  histogram(data,hist,minv,maxv);

  //find peak
  unsigned int peakv = 0;
  unsigned int peaki = 0;
  for (unsigned int i=0; i<num_bins; ++i) {
    if (hist[i] > peakv) {
      peakv = hist[i];
      peaki = i;
    }
  }

  // fit a parabola to estimate the range of the peak
  unsigned int pp = (peaki+1 < num_bins)?hist[peaki+1]:0;
  unsigned int pm = (peaki >= 1)        ?hist[peaki-1]:0;
  T ro;
  T shift = interpolate_parabola(T(pm),T(peakv),T(pp),ro);
  ro *= 3;
  maxv = (peaki+shift+ro)*binsize+minv;
  minv = (peaki+shift-ro)*binsize+minv;

  // fit a Gaussian around this peak
  mean = 0;
  stdev = 0;
  unsigned count = 0;
  for (unsigned int i=0; i<data.size(); ++i) {
    if (data[i] > minv && data[i] < maxv) {
      ++count;
      mean += data[i];
      stdev += data[i]*data[i];
    }
  }

  mean /= count;
  stdev /= count;
  stdev -= mean*mean;
  stdev = std::sqrt(stdev);
}


//: Expand the range (minv, maxv) with the data in \a image
// Only finite values count
template <class T>
void bmdl_classify<T>::range(const vil_image_view<T>& image,
                             T& minv, T& maxv) const
{
  const unsigned int ni= image.ni();
  const unsigned int nj = image.nj();
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      const T& val = image(i,j);
      if (vnl_math::isfinite(val)) {
        if (val > maxv) maxv = val;
        if (val < minv) minv = val;
      }
    }
  }
}


//: Search for nearby pixel that can be added to each building
//  Return true if any changes are made
template <class T>
bool bmdl_classify<T>::expand_buildings(std::vector<T>& means,
                                        std::vector<unsigned int>& sizes)
{
  bool changed = false;
  unsigned int ni=first_return_.ni();
  unsigned int nj=last_return_.nj();

  T zthresh = T(0.01);
  assert(zthresh > 0); // make sure that T is not an integral type

  merge_map merge(this);

  for (unsigned int j=0; j<nj; ++j)
  {
    for (unsigned int i=0; i<ni; ++i)
    {
      // only expand into non-buildings
      if (labels_(i,j) != 1)
        continue;

      // collect all adjacent building labels
      std::set<int> n;
      if (i>0 && labels_(i-1,j) > 1) {
        unsigned int idx = merge.translate(labels_(i-1,j)-2);
        if (vnl_math::sqr(first_return_(i,j) - first_return_(i-1,j)) < zthresh ||
            vnl_math::sqr(last_return_(i,j) - last_return_(i-1,j)) < zthresh )
          n.insert(idx);
      }
      if (j>0 && labels_(i,j-1) > 1) {
        unsigned int idx = merge.translate(labels_(i,j-1)-2);
        if (vnl_math::sqr(first_return_(i,j) - first_return_(i,j-1)) < zthresh ||
            vnl_math::sqr(last_return_(i,j) - last_return_(i,j-1)) < zthresh )
          n.insert(idx);
      }
      if (i<ni-1 && labels_(i+1,j) > 1) {
        unsigned int idx = merge.translate(labels_(i+1,j)-2);
        if (vnl_math::sqr(first_return_(i,j) - first_return_(i+1,j)) < zthresh ||
            vnl_math::sqr(last_return_(i,j) - last_return_(i+1,j)) < zthresh )
          n.insert(idx);
      }
      if (j<nj-1 && labels_(i,j+1) > 1) {
        unsigned int idx = merge.translate(labels_(i,j+1)-2);
        if (vnl_math::sqr(first_return_(i,j) - first_return_(i,j+1)) < zthresh ||
            vnl_math::sqr(last_return_(i,j) - last_return_(i,j+1)) < zthresh )
          n.insert(idx);
      }
      if (n.empty())
        continue;

      for ( const auto & itr : n) {
        // test for merge
        if (labels_(i,j) > 1) {
          unsigned int other = labels_(i,j)-2;
          merge.merge(other,itr);
        }
        else {
          labels_(i,j) = itr+2;
          changed = true;
        }
      }
    }
  }

  return changed;
}


//: Dilate buildings into unclaimed (vegetation) pixel
//  Only claim a vegetation pixel if surrounded by
//  \a num pixels from the same building
template <class T>
bool bmdl_classify<T>::dilate_buildings(unsigned int num)
{
  bool changed = false;
  unsigned int ni=first_return_.ni()-1;
  unsigned int nj=last_return_.nj()-1;

  for (unsigned int j=1; j<nj; ++j)
  {
    for (unsigned int i=1; i<ni; ++i)
    {
      // only expand into non-buildings
      if (labels_(i,j) != 1)
        continue;

      // collect all adjacent building labels
      std::map<int,unsigned int> n;
      int offset_x[] = {-1, 0, 0, 1,-1, 1, 1,-1};
      int offset_y[] = { 0,-1, 1, 0,-1,-1, 1, 1};
      unsigned total = 0;
      for (unsigned int k=0; k<8; ++k)
      {
        int idx = labels_(i+offset_x[k], j+offset_y[k])-2;
        if (idx<0) continue;
        ++total;
        std::map<int,unsigned int>::iterator itr = n.find(idx);
        if (itr == n.end() && k < 4) // only consider 4-con neighbors
          n.insert(std::pair<int,unsigned int>(idx,1));
        else
          ++(itr->second);
      }

      if (n.empty())
        continue;

      if (total < num)
        continue;

      unsigned int max = 0;
      for (auto & itr : n)
      {
        if (itr.second > max) {
          max = itr.second;
          labels_(i,j) = itr.first+2;
          ++building_area_[itr.first];
          changed = true;
        }
      }
    }
  }

  return changed;
}


//: Greedy merging of adjacent buildings
template <class T>
bool bmdl_classify<T>::greedy_merge()
{
  unsigned int ni=labels_.ni();
  unsigned int nj=labels_.nj();
  // map of number of pixels adjacent between two buildings
  typedef std::pair<unsigned int,unsigned int> upair;
  std::set<upair> adjacent;

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=1; i<ni; ++i) {
      unsigned int l1 = labels_(i,j);
      unsigned int l2 = labels_(i-1,j);
      if (l1<2 || l2<2 || l1==l2) continue;
      if (l1>l2) std::swap(l1,l2);
      adjacent.insert(upair(l1-2,l2-2));
    }
  }

  for (unsigned int j=1; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      unsigned int l1 = labels_(i,j);
      unsigned int l2 = labels_(i,j-1);
      if (l1<2 || l2<2 || l1==l2) continue;
      if (l1>l2) std::swap(l1,l2);
      adjacent.insert(upair(l1-2,l2-2));
    }
  }

  std::vector<upair> to_merge;
  for (const auto & itr : adjacent)
  {
    unsigned int idx1 = itr.first;
    unsigned int idx2 = itr.second;
    if (std::abs(building_mean_hgt_[idx1] - building_mean_hgt_[idx2]) < height_resolution_)
      to_merge.push_back(itr);
  }

  if (to_merge.empty())
    return false;

  merge_map merge(this);

  for (auto & i : to_merge)
  {
    unsigned int idx1 = merge.translate(i.first);
    unsigned int idx2 = merge.translate(i.second);
    merge.merge(idx1,idx2);
  }

  return true;
}


//: Morphological clean up on each building independently
template <class T>
std::vector<bool>
bmdl_classify<T>::close_buildings(unsigned int num_labels)
{
  unsigned int ni=labels_.ni();
  unsigned int nj=labels_.nj();
  vil_image_view<unsigned int> new_labels(ni,nj);
  std::vector<vgl_box_2d<int> > building_bounds(num_labels);

  // transfer vegetation labels to the output
  // and build a bounding box around each building
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      new_labels(i,j) = (labels_(i,j)==1)?1:0;
      if (labels_(i,j)>1)
        building_bounds[labels_(i,j)-2].add(vgl_point_2d<int>(i,j));
    }
  }

  // find the maximum of all building sizes
  unsigned int bni=0, bnj=0;
  for (unsigned int l=0; l<num_labels; ++l) {
    unsigned int w = building_bounds[l].width()+1;
    unsigned int h = building_bounds[l].height()+1;
    if (w > bni) bni = w;
    if (h > bnj) bnj = h;
  }

  std::cout << "max bounds = " << bni<<", "<<bnj<<std::endl;
  bni += 4;
  bnj += 4;
  vil_image_view<bool> full_mask(bni,bnj);
  vil_image_view<bool> full_work(bni,bnj);

  vil_structuring_element se;
  se.set_to_disk(1);
  std::vector<bool> valid(num_labels,false);
  for (unsigned l=0; l<num_labels; ++l) {
    const vgl_box_2d<int>& bbox = building_bounds[l];
    // skip buildings that vanish with a binary dilate
    if (bbox.width()==0 || bbox.height()==0)
      continue;

#ifdef DEBUG
    std::cout << "closing "<<l<<" of "<<num_labels<<std::endl;
    std::cout << "  from "<<building_bounds[l].min_point()<<" to "<<building_bounds[l].max_point()<<std::endl;
#endif // DEBUG

    int min_x=bbox.min_x()-2, min_y=bbox.min_y()-2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    int max_x=bbox.max_x()+2, max_y=bbox.max_y()+2;
    if (max_x >= int(ni)) max_x = ni-1;
    if (max_y >= int(nj)) max_y = nj-1;

    unsigned lni = max_x - min_x + 1;
    unsigned lnj = max_y - min_y + 1;

    // create cropped views of the working image space
    vil_image_view<bool> mask(lni,lnj);// = vil_crop(full_mask,0,lni,0,lnj);
    vil_image_view<bool> work(lni,lnj);// = vil_crop(full_work,0,lni,0,lnj);

    // copy data into a binary mask
    for (unsigned int j=0; j<lnj; ++j) {
      for (unsigned int i=0; i<lni; ++i) {
        mask(i,j) = labels_(min_x+i,min_y+j)-2 == l;
      }
    }
    // binary closing
    vil_binary_dilate(mask,work,se);
    vil_binary_erode(work,mask,se);
    // binary opening
    vil_binary_erode(mask,work,se);
    vil_binary_dilate(work,mask,se);

    // copy mask back to labels
    for (unsigned int j=0; j<lnj; ++j) {
      for (unsigned int i=0; i<lni; ++i) {
        if (mask(i,j)) {
          new_labels(min_x+i,min_y+j) = l+2;
          valid[l] = true;
        }
      }
    }
  }
  labels_ = new_labels;
  return valid;
}


//: Group building pixel by height into bins of size \a binsize
template <class T>
vil_image_view<unsigned int> bmdl_classify<T>::bin_heights(T binsize)
{
  unsigned int ni=last_return_.ni();
  unsigned int nj=last_return_.nj();

  vil_image_view<unsigned int> bin_img(ni,nj);

  for (unsigned int j=0; j<nj; ++j)
  {
    for (unsigned int i=0; i<ni; ++i)
    {
      if (labels_(i,j) < 2)
      {
        bin_img(i,j) = labels_(i,j);
        continue;
      }
      int bin = static_cast<int>((last_return_(i,j) - last_min_)/binsize);
      bin_img(i,j) = bin+2;
    }
  }
  return bin_img;
}


//------------------------------------------------------------------------------

#define BMDL_CLASSIFY_INSTANTIATE(T) \
template class bmdl_classify<T >


#endif // bmdl_classify_hxx_
