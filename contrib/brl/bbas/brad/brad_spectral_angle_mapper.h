// This is brl/bbas/brad/brad_ms_aster.h
#ifndef brad_spectral_angle_mapper_h
#define brad_spectral_angle_mapper_h

#include <vector>
#include <vil/vil_image_view.h>
#include <brad_multispectral_functions.h>

//:
// \file
// \brief Compares image pixel spectra to ASTER spectral library
// \author Tom Pollard and Selene Chew
// \date March 24, 2017

class brad_spectral_angle_mapper {
public:
  brad_spectral_angle_mapper(std::vector<float>& bands_min, 
    std::vector<float>& bands_max);

  //: Compute the max spectral angle map for all spectra whose name includes keyword
  bool compute_sam_img(const vil_image_view<float>& image,
    const std::string keyword, 
    vil_image_view<float>& spectral_angle);

  //: Port of Cara's matClass.m which will use a fair amount of memory
  // in class_img value -1 corresponds to unknown category (when no category meets threshold)
  // other values correspond to index of keyword in keywords
  bool aster_classify_material(
    const vil_image_view<float>& image,
    const std::vector<std::string>& keywords,
    const float threshold,
    vil_image_view<int>& class_img,
    vil_image_view<float>& conf_img);

  //: add a new material to the library from labelled image pixels
  bool add_material(const std::string type,
    const vil_image_view<float>& image,
    const vil_image_view<bool>& mask);

  //: Find all files that only contain valid characters and add their spectra to library
  void add_aster_dir(const std::string aster_dir);

protected:
  std::vector<std::string> file_names_; // list of all valid file names
  std::vector<std::vector<float> > spectra_; // normalized spectra corresponding to materials in file_names_
  std::vector<float> bands_min_;
  std::vector<float> bands_max_;


  //: Parse a single ASTER file to retrieve wavelength/spectra data and 
  // interpolate spectra for the given bands.
  bool parse_aster_file(
    const std::string& file_name,
    std::vector<float>& sample_spectra);

  //: Compute spectral angle between two vectors of multi-spectral values
  // as per the spectral angle mapper algorithm for a set of spectra samples.
  void compute_spectral_angles(
    const std::vector<float>& img_vals,
    const std::vector<std::vector<std::vector<float> > >& normalized_spectra_samples,
    std::vector<float>& angles);
};
#endif // brad_spectral_angle_mapper_h