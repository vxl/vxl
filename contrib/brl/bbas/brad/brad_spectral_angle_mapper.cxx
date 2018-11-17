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
#include <brad/brad_multispectral_functions.h>
#include "brad_spectral_angle_mapper.h"

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
brad_spectral_angle_mapper::brad_spectral_angle_mapper(std::vector<float>& bands_min,
  std::vector<float>& bands_max) {
  bands_min_ = bands_min;
  bands_max_ = bands_max;
}

//---------------------------------------------------------------------------
//: Create Spectral Angle Map
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::compute_sam_img(const vil_image_view<float>& image,
  const std::string& keyword,
  vil_image_view<float>& spectral_angle)
{
  // Ensure our library has at least one material
  if (file_names_.size() == 0) {
    std::cerr << "Spectral library empty\n";
    return false;
  }

  // set up
  int num_bands = image.nplanes(); ;
  if (bands_min_.size() < num_bands) {
    std::cerr << "Image does not have the correct number of spectral bands\n";
    return false;
  }
  spectral_angle.set_size(image.ni(), image.nj());
  spectral_angle.fill(0.0);

  // find all relevant spectra
  size_t found;
  bool found_keyword = false;
  std::vector<std::vector<std::vector<float> > > spec; // each vector entry corresponds to a material categroy, which holds a vector of relevant spectra (which are also vectors)
  spec.resize(1); // size is one because we are only looking at one material category, namely 'keyword'
  for (int i = 0; i < file_names_.size(); i++) {
    found = file_names_[i].find(keyword);
    if (found != std::string::npos) { // if our file contains the keyword, we want to compare it to our sample spectra
      spec[0].push_back(spectra_[i]); // get corresponding spectra
      found_keyword = true;
    }
  }

  // make sure at least one relevant spectra was found
  if (!found_keyword) {
    std::cerr << "keyword (" + keyword + ") not found in spectral library\n";
    return false;
  }

  // find the best spectral angle
  std::vector<float> img_vals;
  std::vector<float> angle; // will be a vector of size 1 (the max angle corresponding to our single material category, 'keyword')
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

//---------------------------------------------------------------------------
/*/: Compute Spectral Angle Difference
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::compute_sdm_img(const vil_image_view<float>& image,
  std::string keyword,
  vil_image_view<float>& spectral_diff)
{

  // Ensure our library has at least one material
  if (file_names_.size() == 0) {
    std::cerr << "Spectral library empty\n";
    return false;
  }

  // set up
  int num_bands = image.nplanes(); ;
  if (bands_min_.size() < num_bands) {
    std::cerr << "Image does not have the correct number of spectral bands\n";
    return false;
  }
  spectral_diff.set_size(image.ni(), image.nj());
  spectral_diff.fill(0.0);

  // find all relevant spectra
  size_t found;
  bool found_keyword = false;
  std::vector<std::vector<std::vector<float> > > spec; // each vector entry corresponds to a material categroy, which holds a vector of relevant spectra (which are also vectors)
  spec.resize(1); // size is one because we are only looking at one material category, namely 'keyword'
  for (int i = 0; i < file_names_.size(); i++) {
    found = file_names_[i].find(keyword);
    if (found != std::string::npos) { // if our file contains the keyword, we want to compare it to our sample spectra
      spec[0].push_back(spectra_[i]); // get corresponding spectra
      found_keyword = true;
    }
  }

  // make sure at least one relevant spectra was found
  if (!found_keyword) {
    std::cerr << "keyword not found in spectral library\n";
    return false;
  }
  std::vector<float> s = spectra_[0];
  for (int i = 0; i < 8; i++) {
    std::cout << s[i] << "\n";
  }

  // find the best spectral difference
  std::vector<float> img_vals;
  std::vector<float> angle; // will be a vector of size 1 (the max angle corresponding to our single material category, 'keyword')
  img_vals.resize(num_bands);
  for (int y = 0; y < image.nj(); y++) {
    for (int x = 0; x < image.ni(); x++) {
      for (int b = 0; b < num_bands; b++)
        img_vals[b] = image(x, y, b);
      compute_spectral_diffs(img_vals, spec, angle);
      spectral_diff(x, y) = angle[0];
    }
  }
  return true;
}*/

//---------------------------------------------------------------------------
//: Port of Cara's matClass.m which will use a fair amount of memory
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::aster_classify_material(
  const vil_image_view<float>& image,
  const std::vector<std::string>& keywords,
  const float threshold,
  vil_image_view<int>& class_img,
  vil_image_view<float>& conf_img)
{

  // set up
  int img_width = image.ni(), img_height = image.nj();
  int num_categories = keywords.size();
  class_img.set_size(img_width, img_height);
  conf_img.set_size(img_width, img_height);
  conf_img.fill(0.0);

  // Ensure our library has at least one material
  if (file_names_.size() == 0) {
    std::cerr << "Spectral library empty\n";
    return false;
  }

  // Ensure image correct number or spectral bands
  int num_bands = image.nplanes(); ;
  if (bands_min_.size() < num_bands) {
    std::cerr << "Image does not have the correct number of spectral bands\n";
    return false;
  }

  // Make a volume to store maximum spectral angle measurements for each
  // pixel and sample category.
  vil_image_view<float> angle_volume(
    img_width, img_height, 1, num_categories);

  std::vector<float> img_vals(num_bands);
  std::vector<float> angles(num_categories);

  // find all spectra that are representative of each keyword category
  size_t found;
  std::vector<std::vector<std::vector<float> > > spec; // each vector entry corresponds to a material categroy, which holds a vector of relevant spectra (which are also vectors)
  spec.resize(num_categories);
  bool found_keyword;
  bool found_any_keyword = false;
  for (int c = 0; c < num_categories; c++) {
    found_keyword = false;
    for (int i = 0; i < file_names_.size(); i++) {
      found = file_names_[i].find(keywords[c]);
      if (found != std::string::npos) { // if our file contains the keyword, we want to compare it to our sample spectra
        found_keyword = true;
        found_any_keyword = true;
        spec[c].push_back(spectra_[i]); // get corresponding spectra
      }
    } // i
    if (!found_keyword) std::cerr << "WARNING: did not find keyword " << keywords[c] << "\n";
  } // c

  if (!found_any_keyword) {
    std::cerr << "No keyword was found\n";
    return false;
  }

  // Populate the volume
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {

      // Copy the image spectra
      for (int b = 0; b < num_bands; b++)
        img_vals[b] = image(x, y, b);

      // Compute spectral angles
      compute_spectral_angles(img_vals, spec, angles);

      for (int c = 0; c < num_categories; c++) {
        angle_volume(x, y, c) = angles[c];
      } // c
    } // y
  } // x

  // Compute the max index
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {
      float max_angle = 0.0f;
      int max_idx = -1;
      for (int c = 0; c < num_categories; c++) {
        if (angle_volume(x, y, c) < max_angle) continue;
        max_angle = angle_volume(x, y, c);
        if (max_angle > threshold) max_idx = c; // class -1: no category had high enough angle measurement
      }
      class_img(x, y) = max_idx;
      if (max_idx != -1) conf_img(x, y) = max_angle;
    }
  }
  return true;
}

//---------------------------------------------------------------------------
//: add a material to the library
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::add_material(const std::string& type,
  const vil_image_view<float>& image,
  const vil_image_view<bool>& mask)
{

  // ensure image correct number or spectral bands
  int num_bands = image.nplanes(); ;
  if (bands_min_.size() < num_bands) {
    std::cerr << "Image does not have the correct number of spectral bands\n";
    return false;
  }
  if (image.ni() != mask.ni() || image.nj() != mask.nj()) {
    std::cerr << "image and mask must be the same size. image is size " << image.ni() << " by " << image.nj() << " and mask is size " << image.ni() << " by " << image.nj() << "\n";
    return false;
  }

  // new spectra to be added to the library
  std::vector<float> new_spectrum;
  new_spectrum.resize(num_bands);

  //std::vector<std::vector<float> > img_spectra;
  //std::vector<float> cur_spectrum;
  //cur_spectrum.resize(num_bands);
  int num_sample_spectra = 0;
  std::vector<float> spectra_sum;
  spectra_sum.resize(num_bands);
  // obtain relevant spectra from image
  for (int y = 0; y < image.nj(); y++) {
    for (int x = 0; x < image.ni(); x++) {
      if (mask(x, y)) { // if this spectrum is relevant then save it
        num_sample_spectra++;
        for (int s = 0; s < num_bands; s++) {
          spectra_sum[s] += image(x, y, s);
        } // s
      }
    } // x
  } // y

  // combine relevant spectra into single representative spectra
  for (int s = 0; s < num_bands; s++) {
    new_spectrum[s] = spectra_sum[s] / num_sample_spectra;
  } // s

  // normalize the spectra
  brad_normalize_spectra(new_spectrum.data(), new_spectrum.size());

  // add spectra and type to our library
  spectra_.push_back(new_spectrum);
  file_names_.push_back(type);
  return true;
}

//---------------------------------------------------------------------------
//: add a material to the library
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::add_material_per_pixel(const std::string& type,
  const vil_image_view<float>& image,
  const vil_image_view<bool>& mask)
{

  // ensure image correct number or spectral bands
  int num_bands = image.nplanes(); ;
  if (bands_min_.size() < num_bands) {
    std::cerr << "Image does not have the correct number of spectral bands\n";
    return false;
  }
  if (image.ni() != mask.ni() || image.nj() != mask.nj()) {
    std::cerr << "image and mask must be the same size. image is size " << image.ni() << " by " << image.nj() << " and mask is size " << image.ni() << " by " << image.nj() << "\n";
    return false;
  }

  // obtain relevant spectra from image
  for (int y = 0; y < image.nj(); y++) {
    for (int x = 0; x < image.ni(); x++) {
      if (mask(x, y)) { // if this spectrum is relevant then save it
        std::vector<float> new_spectrum;
        new_spectrum.resize(num_bands);
        for (int s = 0; s < num_bands; s++) {
          new_spectrum[s] += image(x, y, s);
        } // s
          // normalize the spectra
        brad_normalize_spectra(new_spectrum.data(), new_spectrum.size());

        // add spectra and type to our library
        spectra_.push_back(new_spectrum);
        file_names_.push_back(type);
      }
    } // x
  } // y
  return true;
}

//---------------------------------------------------------------------------
//: Adds all valid files in the directory to the library
//---------------------------------------------------------------------------
void brad_spectral_angle_mapper::add_aster_dir(const std::string& aster_dir) {


  bool good_file;
  for (vul_file_iterator fi(aster_dir); fi; ++fi) {
    good_file = true;
    //ensure file is good before proceding
    std::ifstream is(fi());     // open file
    char c;
    while (is.get(c)) {          // loop getting single characters
      if (c <= -1 || c == 255) { // check if the file contains bad character
        good_file = false;
        std::cerr << "File " << vul_file::strip_directory(fi()) << " contains an invalid character. Continuing to next file...\n";
        break;
      }
    }
    is.close();
    if (good_file) { // file name and corresponding spectra
      std::vector<float> sample_spec;
      if (parse_aster_file(fi(), sample_spec)) { // attempt to parse aster file
        file_names_.push_back(vul_file::strip_directory(fi()));
        spectra_.push_back(sample_spec);
      }
      else {
        std::cerr << "File " << vul_file::strip_directory(fi()) << " could not be parsed correctly. Continuing to next file...\n";
      }
    }
  }

}

//---------------------------------------------------------------------------
//: Clear the library of all materials
//---------------------------------------------------------------------------
void brad_spectral_angle_mapper::clear_library() {
  file_names_.clear();
  spectra_.clear();
}

//---------------------------------------------------------------------------
//: Parse a single ASTER file to retrieve wavelength/spectra data and
// interpolate spectra for the given bands.
//---------------------------------------------------------------------------
bool brad_spectral_angle_mapper::parse_aster_file(
  const std::string& file_name,
  std::vector<float>& sample_spectra)
{
  int num_header_lines = 26;
  float tol = 0.000000001f;

  int num_bands = bands_min_.size();

  sample_spectra.resize(num_bands);

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

  bool good_band = false; // make sure at least one spectrum contains valid reflectance
  // Loop through wavelength bands
  for (int b = 0; b < num_bands; b++) {

    // Compute the band mean
    float mean = 0.5f*(bands_min_[b] + bands_max_[b]);

    // Check if mean is outside the wavelength range, set to -1 so that band is not used in SAM computation
    if (mean <= wavelengths.front() && mean <= wavelengths.back()) {
      sample_spectra[b] = -1.0f;
      continue;
    }
    else if (mean >= wavelengths.front() && mean >= wavelengths.back()) {
      sample_spectra[b] = -1.0f;
      continue;
    }

    good_band = true;

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
    if (sample_spectra[b] < 0) return false; // aster file should not have negative reflectance values
    //std::cerr << bands_spectra[b] << '\n';
  }

  if (!good_band) return false; // file does not overlap spectral range of bands

  // normalize the spectra
  brad_normalize_spectra(sample_spectra.data(), sample_spectra.size());
  return true;
};

//---------------------------------------------------------------------------
//: Compute spectral angle between two vectors of multi-spectral values
// as per the spectral angle mapper algorithm for a set of spectra samples.
//---------------------------------------------------------------------------
void brad_spectral_angle_mapper::compute_spectral_angles(
  const std::vector<float>& img_vals,
  const std::vector<std::vector<std::vector<float> > >& normalized_spectra_samples,
  std::vector<float>& angles)
{
  int num_channels = img_vals.size();
  int num_categories = normalized_spectra_samples.size();
  angles.resize(num_categories);

  // Normalize image vals
  std::vector<float> img_norm = img_vals;
  brad_normalize_spectra(img_norm.data(), img_norm.size());

  // Loop over categories
  for (int c = 0; c < num_categories; c++) {
    float angle = vnl_math::pi;
    int num_samples = normalized_spectra_samples[c].size();

    // For each category, compute max angle
    for (int s = 0; s < num_samples; s++) {
      angle = std::min(angle, brad_compute_spectral_angle(
        img_norm.data(), normalized_spectra_samples[c][s].data(), num_channels));
      /*/ Dot product
      float sum = 0.0f;
      for (int b = 0; b < num_bands; b++) {
        if (normalized_spectra_samples[c][s][b] == -1.0) continue;
        sum += normalized_spectra_samples[c][s][b] * img_norm[b];
      }

      // Spectral angle
        angle = std::max(angle, (float)(1.0f) - (float)fabs(acosf(sum)) / (float)vnl_math::pi);*/
    } // s
    angles[c] = 1.0f - angle / vnl_math::pi;
  } // c
};

/*/---------------------------------------------------------------------------
//: Compute spectral difference (sum of squared difference) between two vectors
//  of multi-spectral values
//---------------------------------------------------------------------------
void brad_spectral_angle_mapper::compute_spectral_diffs(
  const std::vector<float>& img_vals,
  const std::vector<std::vector<std::vector<float> > >& normalized_spectra_samples,
  std::vector<float>& differences)
{
  int num_bands = img_vals.size();
  int num_categories = normalized_spectra_samples.size();
  differences.resize(num_categories);

  // Normalize image vals
  std::vector<float> img_norm = img_vals;
  brad_normalize_spectra(img_norm.data(), img_norm.size());

  for (int c = 0; c < num_categories; c++) { // loop over categories
    float diff = 0.0;
    int num_samples = normalized_spectra_samples[c].size();

    for (int s = 0; s < num_samples; s++) { // loop over sample
      // Sum of Squared Difference
      float sum = 0.0f;
      for (int b = 0; b < num_bands; b++)
        sum += pow(normalized_spectra_samples[c][s][b] - img_norm[b],2);

      // Spectral angle
      diff = std::max(diff, (float)sqrt(sum));
    } // s
    differences[c] = diff;
  } // c
};*/


//---------------------------------------------------------------------------
float brad_compute_spectral_angle(
  const float* norm_spectra1,
  const float* norm_spectra2,
  int num_channels )
{
  float sum = 0.0f;
  for (int c = 0; c < num_channels; c++) {
    if (norm_spectra1[c] < 0.0f || norm_spectra2[c] < 0.0f) continue;
    sum += norm_spectra1[c] * norm_spectra2[c];
  }

  return (float)fabs(acosf(sum));
};

//---------------------------------------------------------------------------
float brad_compute_cos_spectral_angle(
  const float* norm_spectra1,
  const float* norm_spectra2,
  int num_channels)
{
  float sum = 0.0f;
  for (int c = 0; c < num_channels; c++) {
    sum += norm_spectra1[c] * norm_spectra2[c];
  }
  return sum;
};
