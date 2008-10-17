// This is brl/bseg/bmdl/bmdl_classify.cxx
#include "bmdl_classify.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_set.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_utility.h>

#include <vnl/vnl_math.h>

#include <vil/vil_crop.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_closing.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>

namespace {
  //: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
  //  \returns the peak value by reference in \p y_peak
  //  \returns the peak location offset from the x of \p y_0
  double interpolate_parabola(double y_1, double y_0, double y_2,
                              double& root_offset)
  {
    double diff1 = y_2 - y_1;      // first derivative
    double diff2 = 2 * y_0 - y_1 - y_2; // second derivative
    //y_peak = y_0 + diff1 * diff1 / (8 * diff2);        // interpolate y as max/min
    root_offset = vcl_abs(vcl_sqrt(diff1*diff1/4 + 2*diff2*y_0) / diff2);
    return diff1 / (2 * diff2);   // interpolate x offset
  }

  //: compute a histogram of the data
  // does not reset the initial bin values to zero
  void histogram(const vcl_vector<double>& data, vcl_vector<unsigned int>& bins,
                 double minv, double maxv)
  {
    assert(maxv > minv);
    int num_bins = bins.size();
    assert(num_bins > 0);
    double binsize = (maxv-minv)/num_bins;

    for(unsigned int i=0; i<data.size(); ++i)
    {
      int bin = static_cast<int>((data[i]-minv)/binsize);
      if(bin >= num_bins || bin < 0)
        continue;
      ++bins[bin];
    }
  }

  //: find a peak in the data and fit a gaussian to it
  // search in the range \a minv to \a maxv
  void fit_gaussian_to_peak(const vcl_vector<double>& data, double minv, double maxv,
                            double& mean, double& stdev)
  {
    unsigned int num_bins = 100;
    double binsize = (maxv-minv)/num_bins;
    vcl_vector<unsigned int> hist(100,0);
    histogram(data,hist,minv,maxv);

    vcl_cout << "histogram"<<vcl_endl;
    //find peak
    unsigned int peakv = 0;
    unsigned int peaki = 0;
    for(unsigned int i=0; i<num_bins; ++i){
      vcl_cout << hist[i]<<vcl_endl;
      if(hist[i] > peakv){
        peakv = hist[i];
        peaki = i;
      }
    }


    // fit a parabola to estimate the range of the peak
    double pp = (peaki+1 < num_bins)?hist[peaki+1]:0;
    double pm = (peaki >= 1)?hist[peaki-1]:0;
    double ro;
    double shift = interpolate_parabola(pm,peakv,pp,ro);
    ro *= 3;
    maxv = (peaki+shift+ro)*binsize+minv;
    minv = (peaki+shift-ro)*binsize+minv;

    // fit a Gaussian around this peak
    mean = 0;
    stdev = 0;
    unsigned count = 0;
    for(unsigned int i=0; i<data.size(); ++i){
      if(data[i] > minv && data[i] < maxv){
        ++count;
        mean += data[i];
        stdev += data[i]*data[i];
      }
    }
    vcl_cout << "gaussian count = " << count << vcl_endl;
    mean /= count;
    stdev /= count;
    stdev -= mean*mean;
    stdev = vcl_sqrt(stdev);
  }
}



//: fill infinite values with the median of its 8 neighborhood
void bmdl_classify::median_fill(vil_image_view<double>& image)
{
  int ni = image.ni();
  int nj = image.nj();
  for(int i=0; i<ni; ++i){
    for(int j=0; j<nj; ++j){
      if(!vnl_math_isfinite(image(i,j)))
      {
        vcl_vector<double> v;
        for(int si = i-1; si<=i+1; ++si){
          if(si < 0) continue;
          if(si >= ni) break;
          for(int sj = j-1; sj<=j+1; ++sj){
            if(sj < 0) continue;
            if(si == i && sj == j) continue;
            if(sj >= nj) break;
            if(vnl_math_isfinite(image(si,sj)))
              v.push_back(image(si,sj));
          }
        }
        if(!v.empty()){
          // partial sort
          unsigned int r = v.size()/2;
          vcl_nth_element(v.begin(), v.begin()+r, v.end());
          image(i,j) = v[r];
        }
      }
    }
  }
}

//: expand the range (minv, maxv) with the data in \a image
// only finite values count
void bmdl_classify::range(const vil_image_view<double>& image,
                          double& minv, double& maxv)
{
  for(unsigned int i=0; i<image.ni(); ++i){
    for(unsigned int j=0; j<image.nj(); ++j){
      if(vnl_math_isfinite(image(i,j))){
        if(image(i,j) > maxv) maxv = image(i,j);
        if(image(i,j) < minv) minv = image(i,j);
      }
    }
  }
}

#if 0

//: read one or more FLIMAP ASCII files and build a pair of LIDAR images (like Buckeye format)
// \param return1 returns the first return (max point) at each pixel
// \param return2 returns the last return (min point) at each pixel
// \param rgb_img return the colors at the first returns
void bmdl_classify::
generate_lidar_images(const vcl_string& glob,
                      const vgl_box_2d<double>& bbox,
                      vil_image_view<double>& return1,
                      vil_image_view<double>& return2,
                      vil_image_view<vxl_byte>& rgb_img)
{
  // find all files that match the glob and contain ".xyz" in the file name 
  vcl_vector<vcl_string> xyz_file_name_list;
  for (vul_file_iterator fn=glob; fn; ++fn) {
    vcl_string curr_image_filename = fn();
    vcl_string::size_type pos_xyz = curr_image_filename.find(".xyz");
    if (pos_xyz != vcl_string::npos) {
      xyz_file_name_list.push_back(curr_image_filename);
    }
  }
  if(xyz_file_name_list.size() < 1){
    vcl_cerr << "Glob did not match any .xyz files"<<vcl_endl;
    exit(-1);
  }

  double pixel_size = 0.75;
  int ni = (bbox.max_x() - bbox.min_x())/pixel_size;
  int nj = (bbox.max_y() - bbox.min_y())/pixel_size;

  // resize the images and fill with default values
  return1.set_size(ni,nj);
  return1.fill(-vcl_numeric_limits<double>::infinity());
  return2.set_size(ni,nj);
  return2.fill(vcl_numeric_limits<double>::infinity());
  rgb_img.set_size(ni,nj,3);
  rgb_img.fill(0);

  // read each 3D point file and map the points into the images
  unsigned count=0;
  for(unsigned xyz=0; xyz<xyz_file_name_list.size(); ++xyz){
    vnl_matrix<double> points_colors;
    vcl_cout << "Started Reading " << xyz_file_name_list[xyz] << "\n";
    flimap_reader::read_flimap_file(xyz_file_name_list[xyz],points_colors);
    vcl_cout << "Finished Reading " << xyz_file_name_list[xyz] << "\n";

    vcl_cout << "Started Processing " << xyz_file_name_list[xyz] << "\n";
    for(unsigned p=0; p<points_colors.rows(); ++p){
      // Calculate the pixel position for 3D point
      int i = (points_colors(p,0) - bbox.min_x())/pixel_size;
      int j = (points_colors(p,1) - bbox.min_y())/pixel_size;

      // Check if the pixel is inside the image
      if(i < 0 || i >= ni || j < 0 || j >= nj){
        continue;
      }

      ++count;
      // The value of the pixel is basically the height of the 3D point
      double val = points_colors(p,2);

      // Update first return and last return values
      if(val < return2(i,j)){
        return2(i,j) = val;
      }
      if(val > return1(i,j)){
        return1(i,j) = val;
        rgb_img(i,j,0) = static_cast<char>(points_colors(p,3));
        rgb_img(i,j,1) = static_cast<char>(points_colors(p,4));
        rgb_img(i,j,2) = static_cast<char>(points_colors(p,5));
      }
    }
    vcl_cout << "Finished Processing " << xyz_file_name_list[xyz] << " with "<<count<<" points"<< "\n";
  }

  // use a median filter to fill in isolated pixels with missing data
  median_fill(return1);
  median_fill(return2);
}

#endif


//: classify each pixel as Ground (0), Vegitation (1), or Building (2)
// also return the ground height and a cleaned up image of heights
double bmdl_classify::label_lidar(const vil_image_view<double>& first_return,
                                  const vil_image_view<double>& last_return,
                                  vil_image_view<unsigned int>& labels,
                                  vil_image_view<double>& heights)
{
  unsigned int ni = first_return.ni();
  unsigned int nj = first_return.nj();
  assert(last_return.ni() == ni);
  assert(last_return.nj() == nj);

  labels.set_size(ni,nj);
  heights.set_size(ni,nj);

  // find the range of finite values in the last returns
  double minv = vcl_numeric_limits<double>::infinity();
  double maxv = -vcl_numeric_limits<double>::infinity();
  bmdl_classify::range(last_return,minv,maxv);

  // find the ground plane value and uncertainty
  // assuming a constant height ground
  double gnd_stdev = 0.0;
  double ground = find_ground(last_return,minv,maxv, gnd_stdev);
  
  vcl_cout << "ground mean = "<< ground << " stdev = "<< gnd_stdev << vcl_endl;

  // expand the range with the first returns
  bmdl_classify::range(first_return,minv,maxv);

  vcl_vector<double> diff;
  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(!vnl_math_isfinite(last_return(i,j)) || !vnl_math_isfinite(first_return(i,j)))
        continue;
      diff.push_back(first_return(i,j) - last_return(i,j));
    }
  }
  vcl_sort(diff.begin(),diff.end());
  // discard top 5% of points for robustness 
  double max_diff = diff[int(diff.size()*.95)];
  vcl_cout << "max_diff = " << max_diff <<vcl_endl;

  double mean = 0.0;
  double stdev = 0.0;
  fit_gaussian_to_peak(diff,0,max_diff,mean,stdev);

  vcl_cout << "diff mean = "<< mean << " stdev = "<< stdev << vcl_endl;

  vil_image_view<bool> gnd_image(ni,nj);
  gnd_image.fill(false);
  vil_image_view<bool> veg_image(ni,nj);
  veg_image.fill(false);
  vil_image_view<bool> bld_image(ni,nj);
  bld_image.fill(false);

  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(!vnl_math_isfinite(first_return(i,j)) || first_return(i,j) < ground+ 3.0*gnd_stdev)
        gnd_image(i,j) = true;
      else if((first_return(i,j) - last_return(i,j)) > mean + 3.0*stdev)
        veg_image(i,j) = true;
      else
        bld_image(i,j) = true;
    }
  }

  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(bld_image(i,j))
        labels(i,j) = 2;
      else if(veg_image(i,j))
        labels(i,j) = 1;
      else
        labels(i,j) = 0;
    }
  }
    

  vcl_vector<double> bld_heights;
  vcl_vector<unsigned int> sizes;
  cluster_buildings(first_return, last_return, 0.5/*3*stdev*/,labels, bld_heights, sizes);  
  while(expand_buildings(first_return, last_return,0.5/*stdev*/,labels, bld_heights, sizes));
  vcl_vector<bool> valid = close_buildings(labels, bld_heights.size());
  
  vcl_vector<double> new_bld_heights;
  vcl_vector<unsigned int> idx_map(bld_heights.size(),0);
  unsigned cnt = 1;
  for(unsigned int i=0; i<valid.size(); ++i){
    if(valid[i]){
      new_bld_heights.push_back(bld_heights[i]);
      idx_map[i] = ++cnt;
    }
  }
  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(labels(i,j) > 1){
        labels(i,j) = idx_map[labels(i,j)-2];
        heights(i,j) = new_bld_heights[labels(i,j)-2];
      }
      else if (labels(i,j) == 1)
        heights(i,j) = first_return(i,j);
      else
        heights(i,j) = ground;
    }
  }

  vcl_cout << "final size = "<<new_bld_heights.size() << vcl_endl;
  
  return ground;
}



//: find the ground value as the most common point
// estimate the standard deviation in the ground by locally fitting a gaussian
double bmdl_classify::find_ground(const vil_image_view<double>& image,
                                  double minv, double maxv,
                                  double& gnd_stdev)
{
  vcl_vector<double> data;

  for(unsigned int i=0; i<image.ni(); ++i){
    for(unsigned int j=0; j<image.nj(); ++j){
      if(vnl_math_isfinite(image(i,j))){
        data.push_back(image(i,j));
      }
    }
  }

  double mean = 0.0;
  fit_gaussian_to_peak(data,minv,maxv,mean,gnd_stdev);
  return mean;

}


void bmdl_classify::cluster_buildings(const vil_image_view<double>& first_return,
                                      const vil_image_view<double>& last_return,
                                      double zthresh,
                                      vil_image_view<unsigned int>& labels,
                                      vcl_vector<double>& means,
                                      vcl_vector<unsigned int>& sizes)
{
  unsigned int ni=first_return.ni();
  unsigned int nj=last_return.nj();

  // square threshold to compare against squared distances
  zthresh *= zthresh;

  vcl_vector<unsigned int> count;
  vcl_vector<unsigned int> merge_map;
  vcl_vector<double> mean;

  // handle first pixel
  if(labels(0,0) >= 2)
  {
    count.push_back(1);
    merge_map.push_back(merge_map.size());
    mean.push_back(last_return(0,0));
    labels(0,0) = count.size()+2;
  }
  // handle first row
  for(unsigned int i=1; i<ni; ++i){
    if(labels(i,0)!=2)
      continue;

    int idx = labels(i-1,0)-3;
    while(idx>=0 && merge_map[idx] != idx )
      idx = merge_map[idx];
    double val = last_return(i,0);
    double last_val = last_return(i-1,0);
    if(idx>=0 && vnl_math_sqr(val-last_val/*mean[idx]*/)<zthresh){
      labels(i,0) = idx+3;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else{
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels(i,0) = count.size()+2;
    }
  }
  // handle first column
  for(unsigned int j=1; j<nj; ++j){
    if(labels(0,j)!=2)
      continue;

    int idx = labels(0,j-1)-3;
    while(idx>=0 && merge_map[idx] != idx )
      idx = merge_map[idx];
    double val = last_return(0,j);
    double last_val = last_return(0,j-1);
    if(idx>=0 && vnl_math_sqr(val-last_val/*mean[idx]*/)<zthresh){
      labels(0,j) = idx+3;
      mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
      ++count[idx];
    }
    else{
      count.push_back(1);
      merge_map.push_back(merge_map.size());
      mean.push_back(val);
      labels(0,j) = count.size()+2;
    }
  }
  
  for(unsigned int i=1; i<ni; ++i){
    for(unsigned int j=1; j<nj; ++j){
      if(labels(i,j)!=2)
        continue;

      double val = last_return(i,j);
      double last_val1 = last_return(i-1,j), last_val2 = last_return(i,j-1);
      int idx1 = labels(i-1,j)-3, idx2 = labels(i,j-1)-3;
      while(idx1>=0 && merge_map[idx1] != idx1 )
        idx1 = merge_map[idx1];
      while(idx2>=0 && merge_map[idx2] != idx2 )
        idx2 = merge_map[idx2];
      int idx = -1;
      if(idx1>=0 && vnl_math_sqr(val-last_val1/*mean[idx1]*/)<zthresh)
        idx = idx1;
      if(idx2>=0 && vnl_math_sqr(val-last_val2/*mean[idx2]*/)<zthresh)
      {
        if(idx == -1)
          idx = idx2;
        else{
          labels(i,j) = idx1+3;
          mean[idx1] = (mean[idx1]*count[idx1] + mean[idx2]*count[idx2] + val)
                      /(count[idx1]+count[idx2]+1);
          count[idx1] += count[idx2]+1;
          count[idx2] = 0;
          merge_map[idx2] = idx1;
          continue;
        }
      }

      if(idx >= 0 ){
        labels(i,j) = idx+3;
        mean[idx] = (mean[idx]*count[idx] + val)/(count[idx]+1);
        ++count[idx];
      }
      else{
        count.push_back(1);
        merge_map.push_back(merge_map.size());
        mean.push_back(val);
        labels(i,j) = count.size()+2;
      }
    }
  }
  vcl_cout << "num labels = "<<count.size() << vcl_endl;

  // simplify merge map
  vcl_vector<vcl_pair<double,int> > unique;
  for(unsigned int i=0; i<merge_map.size(); ++i)
  {
    if(merge_map[i] == i){
      unique.push_back(vcl_pair<double,int>(mean[i],i));
      continue;
    }
    unsigned int i2 = i;
    while(merge_map[i2] != i2 )
      merge_map[i] = i2 = merge_map[i2];
  }
  vcl_sort(unique.begin(), unique.end());

  vcl_cout << "num unique = "<<unique.size() << vcl_endl;
  means.resize(unique.size(),0.0);
  sizes.resize(unique.size(),0);

  vcl_vector<unsigned int> unique_map(merge_map.size(),0);
  for(unsigned int i=0; i<unique.size(); ++i){
    unique_map[unique[i].second] = i;
    means[i] = unique[i].first;
    sizes[i] = count[unique[i].second];
    //vcl_cout << i<<" mean "<<means[i]<<vcl_endl;
  }
  for(unsigned int i=0; i<unique_map.size(); ++i)
    if(merge_map[i] != i)
      unique_map[i] = unique_map[merge_map[i]];

  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(labels(i,j)>2)
        labels(i,j) = unique_map[labels(i,j)-3]+2;
    }
  }
}


bool bmdl_classify::expand_buildings(const vil_image_view<double>& first_return,
                                     const vil_image_view<double>& last_return,
                                     double zthresh,
                                     vil_image_view<unsigned int>& labels,
                                     vcl_vector<double>& means,
                                     vcl_vector<unsigned int>& sizes)
{
  bool changed = false;
  unsigned int ni=first_return.ni();
  unsigned int nj=last_return.nj();
  
  zthresh *= zthresh;

  vcl_vector<unsigned int> merge_map(means.size());
  for(unsigned int i=0; i<merge_map.size(); ++i)
    merge_map[i] = i;

  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      // only expand into non buildings
      if(labels(i,j) != 1)
        continue;
      
      // collect all adjacent building labels
      vcl_set<int> n;
      if(i>0 && labels(i-1,j) > 1){
        unsigned int idx = labels(i-1,j)-2;
        while(merge_map[idx] != idx) idx = merge_map[idx];
        if( vnl_math_sqr(first_return(i,j) - first_return(i-1,j)) < zthresh ||
            vnl_math_sqr(last_return(i,j) - last_return(i-1,j)) < zthresh )
          n.insert(idx);
      }
      if(j>0 && labels(i,j-1) > 1){
        unsigned int idx = labels(i,j-1)-2;
        while(merge_map[idx] != idx) idx = merge_map[idx];
        if( vnl_math_sqr(first_return(i,j) - first_return(i,j-1)) < zthresh ||
            vnl_math_sqr(last_return(i,j) - last_return(i,j-1)) < zthresh )
          n.insert(idx);
      }
      if(i<ni-1 && labels(i+1,j) > 1){
        unsigned int idx = labels(i+1,j)-2;
        while(merge_map[idx] != idx) idx = merge_map[idx];
        if( vnl_math_sqr(first_return(i,j) - first_return(i+1,j)) < zthresh ||
           vnl_math_sqr(last_return(i,j) - last_return(i+1,j)) < zthresh )
          n.insert(idx);
      }
      if(j<nj-1 && labels(i,j+1) > 1){
        unsigned int idx = labels(i,j+1)-2;
        while(merge_map[idx] != idx) idx = merge_map[idx];
        if( vnl_math_sqr(first_return(i,j) - first_return(i,j+1)) < zthresh ||
           vnl_math_sqr(last_return(i,j) - last_return(i,j+1)) < zthresh )
          n.insert(idx);
      }
      if(n.empty())
        continue;
      
      for(vcl_set<int>::iterator itr=n.begin(); itr!=n.end(); ++itr){ 
        // test for merge
        if(labels(i,j) > 1){
          unsigned int other = labels(i,j)-2;
          means[other] = (means[other]*sizes[other] + means[*itr]*sizes[*itr])
                        /(sizes[other]+sizes[*itr]);
          sizes[other] += sizes[*itr];
          sizes[*itr] = 0;
          merge_map[*itr] = other;
        }
        else{
          labels(i,j) = *itr+2;
          changed = true;
        }
      }
    }
  }

  // simplify merge map
  vcl_vector<vcl_pair<double,int> > unique;
  for(unsigned int i=0; i<merge_map.size(); ++i)
  {
    if(merge_map[i] == i){
      unique.push_back(vcl_pair<double,int>(means[i],i));
      continue;
    }
    unsigned int i2 = i;
    while(merge_map[i2] != i2 )
      merge_map[i] = i2 = merge_map[i2];
  }
  vcl_sort(unique.begin(), unique.end());
  //vcl_cout << "num unique = "<<unique.size() << vcl_endl;

  vcl_vector<double> new_means(unique.size(),0.0);
  vcl_vector<unsigned int> new_sizes(unique.size(),0);

  vcl_vector<unsigned int> unique_map(merge_map.size(),0);
  for(unsigned int i=0; i<unique.size(); ++i){
    unique_map[unique[i].second] = i;
    new_means[i] = unique[i].first;
    new_sizes[i] = sizes[unique[i].second];
  }
  for(unsigned int i=0; i<unique_map.size(); ++i)
    if(merge_map[i] != i)
      unique_map[i] = unique_map[merge_map[i]];

  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(labels(i,j)>1)
        labels(i,j) = unique_map[labels(i,j)-2]+2;
    }
  }

  means.swap(new_means);
  sizes.swap(new_sizes);
  return changed;
}


vcl_vector<bool>
bmdl_classify::close_buildings(vil_image_view<unsigned int>& labels,
                               unsigned int num_labels)
{
  unsigned int ni=labels.ni();
  unsigned int nj=labels.nj();
  vil_image_view<unsigned int> new_labels(ni,nj);
  vcl_vector<vgl_box_2d<int> > building_bounds(num_labels);
  
  // transfer vegetation labels to the output
  // and build a bounding box around each building
  for(unsigned int j=0; j<nj; ++j){
    for(unsigned int i=0; i<ni; ++i){
      new_labels(i,j) = (labels(i,j)==1)?1:0;
      building_bounds[labels(i,j)-2].add(vgl_point_2d<int>(i,j));
    }
  }
  
  // find the maximum of all building sizes
  unsigned int bni=0, bnj=0;
  for(unsigned int l=0; l<num_labels; ++l){
    unsigned int w = building_bounds[l].width()+1;
    unsigned int h = building_bounds[l].height()+1;
    if(w > bni) bni = w;
    if(h > bnj) bnj = h;
  }
  
  vcl_cout << "max bounds = " << bni<<", "<<bnj<<vcl_endl;
  bni += 4;
  bnj += 4;
  vil_image_view<bool> full_mask(bni,bnj);
  vil_image_view<bool> full_work(bni,bnj);

  vil_structuring_element se;
  se.set_to_disk(1.5);
  vcl_vector<bool> valid(num_labels,false);
  for(unsigned l=0; l<num_labels; ++l){
    const vgl_box_2d<int>& bbox = building_bounds[l];
    // skip buildings that vanish with a binary dilate
    if(bbox.width()==0 || bbox.height()==0)
      continue;
    
    //vcl_cout << "closing "<<l<<" of "<<num_labels<<vcl_endl;
    //vcl_cout << "  from "<<building_bounds[l].min_point()<<" to "<<building_bounds[l].max_point()<<vcl_endl;
    
    int min_x=bbox.min_x()-2, min_y=bbox.min_y()-2;
    if(min_x < 0) min_x = 0;
    if(min_y < 0) min_y = 0;
    int max_x=bbox.max_x()+2, max_y=bbox.max_y()+2;
    if(max_x >= ni) max_x = ni-1;
    if(max_y >= nj) max_y = nj-1;
    
    unsigned lni = max_x - min_x + 1;
    unsigned lnj = max_y - min_y + 1;
    
    // create cropped views of the working image space
    vil_image_view<bool> mask(lni,lnj);// = vil_crop(full_mask,0,lni,0,lnj); 
    vil_image_view<bool> work(lni,lnj);// = vil_crop(full_work,0,lni,0,lnj);
    
    // copy data into a binary mask
    for(unsigned int j=0; j<lnj; ++j){
      for(unsigned int i=0; i<lni; ++i){
        mask(i,j) = labels(min_x+i,min_y+j)-2 == l;
      }
    }
    // binary closing
    vil_binary_dilate(mask,work,se);
    vil_binary_erode(work,mask,se);
    // binary opening
    vil_binary_erode(mask,work,se);
    vil_binary_dilate(work,mask,se);
    
    // copy mask back to labels
    for(unsigned int j=0; j<lnj; ++j){
      for(unsigned int i=0; i<lni; ++i){
        if(mask(i,j)){
          new_labels(min_x+i,min_y+j) = l+2;
          valid[l] = true;
        }
      }
    }
  }
  labels = new_labels;
  return valid;
}


