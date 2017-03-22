#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vul/vul_awk.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>
#include <baml/baml_ms_utilities.h>
#include "baml_ms_aster.h"

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
baml_ms_aster::baml_ms_aster(
  const std::string aster_dir) {
  dir_ = aster_dir;
  baml_wv3_bands(bands_min_, bands_max_);
  setup_libraries();
}

//---------------------------------------------------------------------------
//: Spectral Angle Map
//---------------------------------------------------------------------------
bool baml_ms_aster::compute_sam_img(const vil_image_view<float>& image,
  const std::string keyword, 
  vil_image_view<float>& spectral_angle)
{
  int num_bands = bands_min_.size();
  if (image.nplanes() != num_bands) return false;
  spectral_angle.set_size(image.ni(), image.nj());
  spectral_angle.fill(0.0);

  // find all relevant spectra
  size_t found;
  std::vector<std::vector<std::vector<float> > > spec;
  for (int i = 0; i < file_names_.size(); i++) {
    found = file_names_[i].find(keyword);
    if (found != std::string::npos) { // if our file contains the keyword, we want to compare it to our sample spectra
      spec[0].push_back(spectra_[i]); // get corresponding spectra
    }
  }

  // find the best spectral angle
  std::vector<float> img_vals;
  std::vector<float> angle; // will be a vector of size 1
  img_vals.resize(num_bands);
  for (int y = 0; y < image.nj(); y++) {
    for (int x = 0; x < image.ni(); x++) {
      for (int b = 0; b < num_bands; b++)
        img_vals[b] = image(x, y, b);
      compute_spectral_angles(img_vals, spec, angle);
      spectral_angle(x, y) = angle[0];
    }
  }
  return true;
}

//!!!!!!!!!!!!!!!!!!!!!!STILL NEEDS DEBUGGING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//---------------------------------------------------------------------------
//: Port of Cara's matClass.m which will use a fair amount of memory
//---------------------------------------------------------------------------
void baml_ms_aster::classify_material_cara(
  const vil_image_view<float>& image,
  const std::vector<std::string>& keywords,
  vil_image_view<int>& class_img,
  vil_image_view<float>& conf_img)
{
  int img_width = image.ni(), img_height = image.nj();
  int num_categories = keywords.size();
  int num_bands = bands_min_.size();

  class_img.set_size(img_width, img_height);
  conf_img.set_size(img_width, img_height);

  // Make a very large volume to store spectral angle measurements for each
  // pixel and sample.
  vil_image_view<float> angle_volume(
    img_width, img_height, 1, num_categories);

  std::vector<float> img_vals(num_bands);
  std::vector<float> angles(num_categories);

  size_t found;
  std::vector<std::vector<std::vector<float> > > spec;
  for (int c = 0; c < num_categories; c++) {
    for (int i = 0; i < file_names_.size(); i++) {
      found = file_names_[i].find(keywords[c]);
      if (found != std::string::npos) { // if our file contains the keyword, we want to compare it to our sample spectra
        spec[c].push_back(spectra_[i]); // get corresponding spectra
      }
    }
  }

  // Populate the volume
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {

      // Copy the image values
      for (int b = 0; b < num_bands; b++)
        img_vals[b] = image(x, y, b);

      // Compute spectral angles
      compute_spectral_angles(img_vals, spec, angles);

      for (int c = 0; c < num_categories; c++) {

        // Update running max
        //max_angle[s] = std::max(max_angle[s], angles[s]);

        // Convert to byte for compactness
        //angle_volume(x, y, s) = (vxl_byte)(255 * angles[s]);
        angle_volume(x, y, c) = angles[c];
      }
    }
  }
  // Compute the max index
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {
      float max_angle = 0.0f;
      int max_idx = 0;
      for (int c = 0; c < num_categories; c++) {
        if (angle_volume(x, y, c) < max_angle) continue;
        max_idx = c;
        max_angle = angle_volume(x, y, c);
      }
      class_img(x, y) = max_idx;
      conf_img(x, y) = max_angle;
    }
  }
}

//---------------------------------------------------------------------------
//: Set up function (should only be called in constructor)
//---------------------------------------------------------------------------
void baml_ms_aster::setup_libraries() {

  bool good_file;
  for (vul_file_iterator fi(dir_); fi; ++fi) {
    good_file = true;
    //ensure file is good before proceding
    std::ifstream is(fi());     // open file
    char c;
    while (is.get(c)) {          // loop getting single characters
      if (c <= -1 || c >= 255) { // check if the file contains bad character
        good_file = false;
        break;
      }
    }
    is.close();
    if (good_file) { // file name and corresponding spectra
      file_names_.push_back(vul_file::strip_directory(fi()));
      spectra_.push_back(std::vector<float>(bands_min_.size()));
      parse_aster_file(fi(), spectra_.back());
    }
  }
}

//---------------------------------------------------------------------------
//: Parse a single ASTER file to retrieve wavelength/spectra data and 
// interpolate spectra for the given bands.
//---------------------------------------------------------------------------
bool baml_ms_aster::parse_aster_file(
  const std::string& file_name,
  std::vector<float>& sample_spectra)
{
  int num_header_lines = 26;
  float tol = 0.000000001f;

  int num_bands = bands_min_.size();

  sample_spectra.resize(bands_min_.size());

  // Attempt to load the file
  std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary);
  if (!ifs.good()) return false;
  vul_awk awk(ifs, vul_awk::backslash_continuations);

  // Burn header lines
  for (int l = 0; l < num_header_lines; l++) {
    ++awk;
  }
  std::vector<float> wavelengths;
  std::vector<float> spectra;

  // Read each line of wavelength/spectra data
  for (; awk; ++awk) {
    if (awk.NF() < 2) break;

    // Prune duplicate values
    float new_w = atof(awk[0]);
    if (wavelengths.size() > 0)
      if (fabs(new_w - wavelengths.back()) < tol) {
        std::cerr << "Duplicate wavelength value removed\n";
        continue;
      }

    wavelengths.push_back(new_w);
    spectra.push_back(atof(awk[1]));
  }
  if (wavelengths.size() == 0) return false;

  // Loop through wavelength bands
  for (int b = 0; b < num_bands; b++) {

    // Compute the band mean
    float mean = 0.5f*(bands_min_[b] + bands_max_[b]);

    // Check if mean is outside the wavelength range
    if (mean <= wavelengths.front() && mean <= wavelengths.back()) {
      sample_spectra[b] = 0.0f;
      continue;
    }
    else if (mean >= wavelengths.front() && mean >= wavelengths.back()) {
      sample_spectra[b] = 0.0f;
      continue;
    }

    // Otherwise find the two wavelengths in the list that contain the mean
    int idx = 1;
    for (; idx < wavelengths.size(); idx++)
      if ((wavelengths[idx] > mean) != (wavelengths[idx - 1] > mean)) break;

    // Interpolate between the two nearest wavelengths
    float dw = wavelengths[idx] - wavelengths[idx - 1];
    float alpha = 0.0f;
    if (fabs(dw) > tol)
      alpha = (mean - wavelengths[idx - 1]) / dw;
    sample_spectra[b] = 0.01f* // Convert to percent
      (spectra[idx - 1] + alpha*(spectra[idx] - spectra[idx - 1]));
    //std::cerr << bands_spectra[b] << '\n';
  }

  // normalize the spectra
  baml_normalize_spectra(sample_spectra);

  return true;
};

//---------------------------------------------------------------------------
//: Compute spectral angle between two vectors of multi-spectral values
// as per the spectral angle mapper algorithm for a set of spectra samples.
//---------------------------------------------------------------------------
void baml_ms_aster::compute_spectral_angles(
  const std::vector<float>& img_vals,
  const std::vector<std::vector<std::vector<float> > >& normalized_spectra_samples,
  std::vector<float>& angles)
{
  int num_bands = img_vals.size();
  int num_categories = normalized_spectra_samples.size();
  angles.resize(num_categories);
  // Normalize image vals
  std::vector<float> img_norm = img_vals;
  baml_normalize_spectra(img_norm);
  for (int c = 0; c < num_categories; c++) { // loop over categories
    float angle = 0.0;
    int num_samples = normalized_spectra_samples[c].size();
    for (int s = 0; s < num_samples; s++) { // loop over smaple
      // Dot product 
      float sum = 0.0f;
      for (int b = 0; b < num_bands; b++)
        sum += normalized_spectra_samples[c][s][b] * img_norm[b];

      // Spectral angle
      angle = std::max(angle, 1.0f - fabs(acosf(sum)) / (float)vnl_math::pi);
    } // s
    angles[c] = angle;
  } // c
};

