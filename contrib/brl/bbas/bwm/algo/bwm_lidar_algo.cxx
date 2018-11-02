#include <iostream>
#include <cmath>
#include "bwm_lidar_algo.h"

#include <vil/vil_image_view.h>
#include <vil/algo/vil_histogram.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void bwm_lidar_algo::label_lidar(vil_image_view<float> first_ret,
                                 vil_image_view<float> last_ret,
                                 lidar_labeling_params& params,
                                 vil_image_view<vxl_byte> &labeled)
{
#if 0 // Parameters: should these be passed in to function or taken from user input?
  double gnd_thresh = 1.0;         // maximum ground height
  double bld_diff_thresh = 0.5;    // maximum first/last return diff for building
  double min_bld_height = 2.5;     // minimum building height
  double min_bld_area = 35;        // minimum building area (in pixels)
  double max_veg_height = 10.0;    // anything above will automatically be labeled building
  double veg_diff_thresh = 0.75;   // minimum first/last return difference for vegitation
#endif // *
  int GND_PLANE = 2; //blue
  int VEG_PLANE = 1; // green
  int BLD_PLANE = 0; // red


  // make sure images are same size
  if ( (first_ret.ni() != last_ret.ni()) || (first_ret.nj() != last_ret.nj()) ) {
    std::cerr << "error, first return and last return images should be same size!\n";
    return;
  }
  if ( (labeled.ni() != first_ret.ni()) || (labeled.nj() != first_ret.nj()) ) {
    std::cerr << "error, lidar and labeled images should be same size!\n";
    return;
  }
  if (labeled.nplanes() != 3) {
    std::cerr << "error, segmented image must have 3 planes (rgb)\n";
    return;
  }

  double min_elev = last_ret(0,0);
  double max_elev = first_ret(0,0);

  // find min and max values, as well as
  // fix weird points where last return is higher than first return
  for (unsigned i=0; i< first_ret.ni(); i++) {
    for (unsigned j=0; j<first_ret.nj(); j++) {
      if (first_ret(i,j) < last_ret(i,j)) {
        float temp = first_ret(i,j);
        first_ret(i,j) = last_ret(i,j);
        last_ret(i,j) = temp;
      }
      if (first_ret(i,j) > max_elev)
        max_elev = first_ret(i,j);
      if (last_ret(i,j) < min_elev)
        min_elev = last_ret(i,j);
    }
  }

  // assume ground is horizontal and most common height value
  std::vector<double> histo;
  double bin_size = 0.5; // 0.5 meters per bin
  int nbins = (int)std::ceil((max_elev - min_elev)/bin_size);
  vil_histogram(last_ret, histo, min_elev, max_elev, nbins);
  double max_count = 0;
  double gnd_val = 0;
  for (unsigned i=0; i < histo.size(); i++) {
    if (histo[i] > max_count) {
      max_count = histo[i];
      gnd_val = min_elev + bin_size*i;
    }
  }

  // initialize segmentation
  for (unsigned i=0; i < labeled.ni(); i++) {
    for (unsigned j=0; j < labeled.nj(); j++) {
      double diff = first_ret(i,j) - last_ret(i,j);
      labeled(i,j,BLD_PLANE) = 0;
      labeled(i,j,VEG_PLANE) = 0;
      if (last_ret(i,j) <= gnd_val + params.gnd_thresh_)
        labeled(i,j,GND_PLANE) = 255;
      else {
        labeled(i,j,GND_PLANE) = 0;
        if (diff > params.veg_diff_thresh_)
          labeled(i,j,VEG_PLANE) = 255;
        if ( ((diff < params.bld_diff_thresh_) && (first_ret(i,j) > gnd_val + params.min_bld_height_)) ||
             (first_ret(i,j) > gnd_val + params.max_veg_height_) ) {
             labeled(i,j,BLD_PLANE) = 255;
        }
      }
    }
  }

#if 0  // TODO: clean up with some morphology- MATLAB code:

  veg = imdilate(imerode(veg0,ones(3)),ones(3));

  % erode to try and disconnect thin patches of trees
  bld0 = imerode(bld0,ones(3));

  labeled = bwlabel(bld0);
  props = regionprops(labeled,'area');
  nregions = length(props);
  bld1 = zeros(size(bld0));
  for i=1:nregions
      if (props(i).Area > min_bld_area)
          bld1 = bld1 | (labeled == i);
      end
  end
  % dilate buildings to get edges back
  bld2 = imdilate(bld1,ones(5)) & ~gnd & ~veg;
  bld = bld2;


  % // dilate vegetation mask, don't overtake buldings
  veg = imdilate(veg,ones(3)) & ~bld;

  % swallow up any small unclassified points with ground
  gnd = imdilate(gnd,ones(3)) & ~(veg | bld);

  % label rest as vegitation
  veg = veg | ~(bld | gnd);

#endif // 0

  return;
}
