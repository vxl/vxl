// This is brl/bseg/baml/baml_ms_aster.h
#ifndef baml_ms_aster_h
#define baml_ms_aster_h

#include <vector>
#include <vil/vil_image_view.h>
#include <baml_ms_utilities.h>

//:
// \file
// \brief Compares image pixel spectra to ASTER spectral library
// \author Tom Pollard and Selene Chew
// \date March 20, 2017

class baml_ms_aster {
public:
  baml_ms_aster(const std::string aster_dir);

  //: Compute the max spectral angle map for all spectra whose name includes keyword
  bool compute_sam_img(const vil_image_view<float>& image,
    const std::string keyword, 
    vil_image_view<float>& spectral_angle);

  //: Port of Cara's matClass.m which will use a fair amount of memory
  void classify_material_cara(
    const vil_image_view<float>& image,
    const std::vector<std::string>& keywords,
    vil_image_view<int>& class_img,
    vil_image_view<float>& conf_img);

protected:
  std::string dir_; // directory containing aster files
  std::vector<std::string> file_names_; // list of all valid file names
  std::vector<std::vector<float> > spectra_; // normalized spectra corresponding to materials in file_names_
  std::vector<float> bands_min_;
  std::vector<float> bands_max_;


  //: Find all files that only contain valid characters
  void setup_libraries();

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
#endif // baml_ms_aster_h