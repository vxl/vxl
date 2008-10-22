// This is brl/bseg/bmdl/bmdl_classify.txx
#ifndef bmdl_classify_txx_
#define bmdl_classify_txx_
//:
// \file
// \brief Classify each pixel in lidar images
//
// \author Matt Leotta
// \date 10/14/2008
//      

#include "bmdl_classify.h"
#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vcl_set.h>

#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>

//: Constructor 
// \param height_noise_stdev is the standard deviation in lidar height
//  This parameter can be set manually or estimated
template <class T>
bmdl_classify<T>::bmdl_classify(T height_noise_stdev)
: hgt_stdev_(height_noise_stdev),
  first_min_( vcl_numeric_limits<T>::infinity()), 
  first_max_(-vcl_numeric_limits<T>::infinity()),
  last_min_( vcl_numeric_limits<T>::infinity()), 
  last_max_(-vcl_numeric_limits<T>::infinity())
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
  vcl_vector<T> data;
  unsigned int ni = last_return_.ni();
  unsigned int nj = last_return_.nj();

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (vnl_math_isfinite(last_return_(i,j))) {
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
// returns the estimate and stores it internally for later use
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

  vcl_vector<T> diff;
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (!vnl_math_isfinite(last_return_(i,j)) || !vnl_math_isfinite(first_return_(i,j)))
        continue;
      diff.push_back(first_return_(i,j) - last_return_(i,j));
    }
  }
  vcl_sort(diff.begin(),diff.end());
  // discard top 5% of points for robustness
  T max_diff = diff[int(diff.size()*.95)];

  T mean = 0.0;
  fit_gaussian_to_peak(diff,0,max_diff,mean,hgt_stdev_);

  vcl_cout << "diff mean = "<< mean << " stdev = "<< hgt_stdev_ << vcl_endl;
  return hgt_stdev_;
}


//: Classify each pixel as Ground (0), Vegitation (1), or Building (>=2)
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
  vcl_vector<T> bld_heights;
  vcl_vector<unsigned int> sizes;
  cluster_buildings(bld_heights, sizes);
  
  // 3. Refine building regions with various morphological operations
  refine_buildings(bld_heights, sizes);
 
  // 4. Determine the heights to use for meshing
  heights_.set_size(ni,nj);
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j) > 1)
        heights_(i,j) = bld_heights[labels_(i,j)-2];
      else if (labels_(i,j) == 1)
        heights_(i,j) = first_return_(i,j);
      else
        heights_(i,j) = bare_earth_(i,j);
    }
  }
}


//: Perform an initial segementation at each pixel using thresholds
// classify each pixel as Ground (0), Vegitation (1), or Building (2)
// results are stored in the labels image
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

  // ground threshold (3 standard deviations from bare earth)
  T gthresh = 3.0*hgt_stdev_;
  // vegetation threshold (3 standard deviation from difference in returns)
  T vthresh = 3.0*vcl_sqrt(2.0)*hgt_stdev_;
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      // test for ground
      if (!vnl_math_isfinite(first_return_(i,j)) || 
          first_return_(i,j) - bare_earth_(i,j) < gthresh)
        labels_(i,j) = 0;
      // test for vegetation
      else if ((first_return_(i,j) - last_return_(i,j)) > vthresh)
        labels_(i,j) = 1;
      // otherwise building
      else
        labels_(i,j) = 2;
    }
  }
}


//: Cluster pixels on building into groups of adjacent pixels
// with similar heights.  Assign a new label to each groups.
// returns building mean heights and pixel counts by reference
template <class T>
void bmdl_classify<T>::cluster_buildings(vcl_vector<T>& means,
                                         vcl_vector<unsigned int>& sizes)
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

  // square threshold to compare against squared distances
  T zthresh = 0.25;

  vcl_vector<unsigned int> count;
  vcl_vector<unsigned int> merge_map;
  vcl_vector<T> mean;

  // handle first pixel
  if (labels_(0,0) >= 2)
  {
    count.push_back(1);
    merge_map.push_back(merge_map.size());
    mean.push_back(last_return_(0,0));
    labels_(0,0) = count.size()+2;
  }
  // handle first row
  for (unsigned int i=1; i<ni; ++i) {
    if (labels_(i,0)!=2)
      continue;

    int idx = labels_(i-1,0)-3;
    while (idx>=0 && merge_map[idx] != idx )
      idx = merge_map[idx];
    T val = last_return_(i,0);
    T last_val = last_return_(i-1,0);
    if (idx>=0 && vnl_math_sqr(val-last_val/*mean[idx]*/)<zthresh) {
      labels_(i,0) = idx+3;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else{
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels_(i,0) = count.size()+2;
    }
  }
  // handle first column
  for (unsigned int j=1; j<nj; ++j) {
    if (labels_(0,j)!=2)
      continue;

    int idx = labels_(0,j-1)-3;
    while (idx>=0 && merge_map[idx] != idx )
      idx = merge_map[idx];
    T val = last_return_(0,j);
    T last_val = last_return_(0,j-1);
    if (idx>=0 && vnl_math_sqr(val-last_val/*mean[idx]*/)<zthresh) {
      labels_(0,j) = idx+3;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else {
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels_(0,j) = count.size()+2;
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
      T last_val1 = last_return_(i-1,j), last_val2 = last_return_(i,j-1);
      int idx1 = labels_(i-1,j)-3, idx2 = labels_(i,j-1)-3;
      while (idx1>=0 && merge_map[idx1] != idx1 )
        idx1 = merge_map[idx1];
      while (idx2>=0 && merge_map[idx2] != idx2 )
        idx2 = merge_map[idx2];
      int idx = -1;
      if (idx1>=0 && vnl_math_sqr(val-last_val1/*mean[idx1]*/)<zthresh)
        idx = idx1;
      if (idx2>=0 && vnl_math_sqr(val-last_val2/*mean[idx2]*/)<zthresh)
      {
        if (idx == -1)
          idx = idx2;
        else {
          labels_(i,j) = idx1+3;
          mean[idx1] = (mean[idx1]*count[idx1] + mean[idx2]*count[idx2] + val)
                      /(count[idx1]+count[idx2]+1);
          count[idx1] += count[idx2]+1;
          count[idx2] = 0;
          merge_map[idx2] = idx1;
          continue;
        }
      }

      if (idx >= 0 ) {
        labels_(i,j) = idx+3;
        mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
        ++count[idx];
      }
      else {
        count.push_back(1);
        merge_map.push_back(merge_map.size());
        mean.push_back(val);
        labels_(i,j) = count.size()+2;
      }
    }
  }
  vcl_cout << "num labels = "<<count.size() << vcl_endl;

  // simplify merge map
  vcl_vector<vcl_pair<T,int> > unique;
  for (unsigned int i=0; i<merge_map.size(); ++i)
  {
    if (merge_map[i] == i) {
      unique.push_back(vcl_pair<T,int>(mean[i],i));
      continue;
    }
    unsigned int i2 = i;
    while (merge_map[i2] != i2 )
      merge_map[i] = i2 = merge_map[i2];
  }
  vcl_sort(unique.begin(), unique.end());

  vcl_cout << "num unique = "<<unique.size() << vcl_endl;
  means.resize(unique.size(),0.0);
  sizes.resize(unique.size(),0);

  vcl_vector<unsigned int> unique_map(merge_map.size(),0);
  for (unsigned int i=0; i<unique.size(); ++i) {
    unique_map[unique[i].second] = i;
    means[i] = unique[i].first;
    sizes[i] = count[unique[i].second];
    //vcl_cout << i<<" mean "<<means[i]<<vcl_endl;
  }
  for (unsigned int i=0; i<unique_map.size(); ++i)
    if (merge_map[i] != i)
      unique_map[i] = unique_map[merge_map[i]];

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j)>2)
        labels_(i,j) = unique_map[labels_(i,j)-3]+2;
    }
  }
}


//: Refine the building regions
template <class T>
void bmdl_classify<T>::refine_buildings(vcl_vector<T>& means,
                                        vcl_vector<unsigned int>& sizes)
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
  
  while (expand_buildings(means, sizes)) ;
  vcl_vector<bool> valid = close_buildings(means.size());

  vcl_vector<T> new_means;
  vcl_vector<unsigned int> idx_map(means.size(),0);
  unsigned cnt = 1;
  for (unsigned int i=0; i<valid.size(); ++i) {
    if (valid[i]) {
      new_means.push_back(means[i]);
      idx_map[i] = ++cnt;
    }
  }
  means.swap(new_means);
  
  // relabel buildings with reduced label set
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j) > 1) 
        labels_(i,j) = idx_map[labels_(i,j)-2];
    }
  }
}


//=========================================================
// Private helper functions

//: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
//  \returns the peak value by reference in \p y_peak
//  \returns the peak location offset from the x of \p y_0
template <class T>
T bmdl_classify<T>::interpolate_parabola(T y_1, T y_0, T y_2, T& root_offset) const
{
  T diff1 = y_2 - y_1;      // first derivative
  T diff2 = 2 * y_0 - y_1 - y_2; // second derivative
  //y_peak = y_0 + diff1 * diff1 / (8 * diff2);        // interpolate y as max/min
  root_offset = vcl_abs(vcl_sqrt(diff1*diff1/4 + 2*diff2*y_0) / diff2);
  return diff1 / (2 * diff2);   // interpolate x offset
}


//: compute a histogram of the data
// Does not reset the initial bin values to zero
template <class T>
void bmdl_classify<T>::histogram(const vcl_vector<T>& data, vcl_vector<unsigned int>& bins,
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
void bmdl_classify<T>::fit_gaussian_to_peak(const vcl_vector<T>& data, T minv, T maxv,
                          T& mean, T& stdev) const
{
  unsigned int num_bins = 100;
  T binsize = (maxv-minv)/num_bins;
  vcl_vector<unsigned int> hist(100,0);
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
  T pp = (peaki+1 < num_bins)?hist[peaki+1]:0;
  T pm = (peaki >= 1)?hist[peaki-1]:0;
  T ro;
  T shift = interpolate_parabola(pm,peakv,pp,ro);
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
  stdev = vcl_sqrt(stdev);
}


//: expand the range (minv, maxv) with the data in \a image
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
      if (vnl_math_isfinite(val)) {
        if (val > maxv) maxv = val;
        if (val < minv) minv = val;
      }
    }
  }
}


//: Search for nearby pixel that can be added to each building
// return true if any changes are made
template <class T>
bool bmdl_classify<T>::expand_buildings(vcl_vector<T>& means,
                                        vcl_vector<unsigned int>& sizes)
{
  bool changed = false;
  unsigned int ni=first_return_.ni();
  unsigned int nj=last_return_.nj();

  T zthresh = 0.25;

  vcl_vector<unsigned int> merge_map(means.size());
  for (unsigned int i=0; i<merge_map.size(); ++i)
    merge_map[i] = i;


  for (unsigned int j=0; j<nj; ++j)
  {
    for (unsigned int i=0; i<ni; ++i)
    {
      // only expand into non-buildings
      if (labels_(i,j) != 1)
        continue;

      // collect all adjacent building labels
      vcl_set<int> n;
      if (i>0 && labels_(i-1,j) > 1) {
        unsigned int idx = labels_(i-1,j)-2;
        while (merge_map[idx] != idx) idx = merge_map[idx];
        if (vnl_math_sqr(first_return_(i,j) - first_return_(i-1,j)) < zthresh ||
            vnl_math_sqr(last_return_(i,j) - last_return_(i-1,j)) < zthresh )
          n.insert(idx);
      }
      if (j>0 && labels_(i,j-1) > 1) {
        unsigned int idx = labels_(i,j-1)-2;
        while (merge_map[idx] != idx) idx = merge_map[idx];
        if (vnl_math_sqr(first_return_(i,j) - first_return_(i,j-1)) < zthresh ||
            vnl_math_sqr(last_return_(i,j) - last_return_(i,j-1)) < zthresh )
          n.insert(idx);
      }
      if (i<ni-1 && labels_(i+1,j) > 1) {
        unsigned int idx = labels_(i+1,j)-2;
        while (merge_map[idx] != idx) idx = merge_map[idx];
        if (vnl_math_sqr(first_return_(i,j) - first_return_(i+1,j)) < zthresh ||
            vnl_math_sqr(last_return_(i,j) - last_return_(i+1,j)) < zthresh )
          n.insert(idx);
      }
      if (j<nj-1 && labels_(i,j+1) > 1) {
        unsigned int idx = labels_(i,j+1)-2;
        while (merge_map[idx] != idx) idx = merge_map[idx];
        if (vnl_math_sqr(first_return_(i,j) - first_return_(i,j+1)) < zthresh ||
            vnl_math_sqr(last_return_(i,j) - last_return_(i,j+1)) < zthresh )
          n.insert(idx);
      }
      if (n.empty())
        continue;

      for (vcl_set<int>::iterator itr=n.begin(); itr!=n.end(); ++itr) {
        // test for merge
        if (labels_(i,j) > 1) {
          unsigned int other = labels_(i,j)-2;
          means[other] = (means[other]*sizes[other] + means[*itr]*sizes[*itr])
                        /(sizes[other]+sizes[*itr]);
          sizes[other] += sizes[*itr];
          sizes[*itr] = 0;
          merge_map[*itr] = other;
        }
        else {
          labels_(i,j) = *itr+2;
          changed = true;
        }
      }
    }
  }

  // simplify merge map
  vcl_vector<vcl_pair<T,int> > unique;
  for (unsigned int i=0; i<merge_map.size(); ++i)
  {
    if (merge_map[i] == i) {
      unique.push_back(vcl_pair<T,int>(means[i],i));
      continue;
    }
    unsigned int i2 = i;
    while (merge_map[i2] != i2 )
      merge_map[i] = i2 = merge_map[i2];
  }
  vcl_sort(unique.begin(), unique.end());
  //vcl_cout << "num unique = "<<unique.size() << vcl_endl;

  vcl_vector<T> new_means(unique.size(),0.0);
  vcl_vector<unsigned int> new_sizes(unique.size(),0);

  vcl_vector<unsigned int> unique_map(merge_map.size(),0);
  for (unsigned int i=0; i<unique.size(); ++i) {
    unique_map[unique[i].second] = i;
    new_means[i] = unique[i].first;
    new_sizes[i] = sizes[unique[i].second];
  }
  for (unsigned int i=0; i<unique_map.size(); ++i)
    if (merge_map[i] != i)
      unique_map[i] = unique_map[merge_map[i]];

  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      if (labels_(i,j)>1)
        labels_(i,j) = unique_map[labels_(i,j)-2]+2;
    }
  }

  means.swap(new_means);
  sizes.swap(new_sizes);
  return changed;
}


//: Morphological clean up on each building independently
template <class T>
vcl_vector<bool> 
bmdl_classify<T>::close_buildings(unsigned int num_labels)
{
  unsigned int ni=labels_.ni();
  unsigned int nj=labels_.nj();
  vil_image_view<unsigned int> new_labels(ni,nj);
  vcl_vector<vgl_box_2d<int> > building_bounds(num_labels);

  // transfer vegetation labels to the output
  // and build a bounding box around each building
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      new_labels(i,j) = (labels_(i,j)==1)?1:0;
      if(labels_(i,j)>1)
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

  vcl_cout << "max bounds = " << bni<<", "<<bnj<<vcl_endl;
  bni += 4;
  bnj += 4;
  vil_image_view<bool> full_mask(bni,bnj);
  vil_image_view<bool> full_work(bni,bnj);

  vil_structuring_element se;
  se.set_to_disk(1.5);
  vcl_vector<bool> valid(num_labels,false);
  for (unsigned l=0; l<num_labels; ++l) {
    const vgl_box_2d<int>& bbox = building_bounds[l];
    // skip buildings that vanish with a binary dilate
    if (bbox.width()==0 || bbox.height()==0)
      continue;

    //vcl_cout << "closing "<<l<<" of "<<num_labels<<vcl_endl;
    //vcl_cout << "  from "<<building_bounds[l].min_point()<<" to "<<building_bounds[l].max_point()<<vcl_endl;

    int min_x=bbox.min_x()-2, min_y=bbox.min_y()-2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    int max_x=bbox.max_x()+2, max_y=bbox.max_y()+2;
    if (max_x >= ni) max_x = ni-1;
    if (max_y >= nj) max_y = nj-1;

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
        if (mask(i,j)){
          new_labels(min_x+i,min_y+j) = l+2;
          valid[l] = true;
        }
      }
    }
  }
  labels_ = new_labels;
  return valid;
}



//------------------------------------------------------------------------------

#define BMDL_CLASSIFY_INSTANTIATE(T) \
template class bmdl_classify<T >; 


#endif // bmdl_classify_txx_
