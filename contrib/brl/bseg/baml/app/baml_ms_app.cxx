#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

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

//: The spectral bands present in a World View 3 multi-spectral image.
void baml_wv3_bands(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max)
{
  bands_min.resize(8);
  bands_max.resize(8);
  bands_min[0] = 0.400f; bands_max[0] = 0.450f; // Coastal
  bands_min[1] = 0.450f; bands_max[1] = 0.510f; // Blue
  bands_min[2] = 0.510f; bands_max[2] = 0.580f; // Green
  bands_min[3] = 0.585f; bands_max[3] = 0.625f; // Yellow
  bands_min[4] = 0.630f; bands_max[4] = 0.690f; // Red
  bands_min[5] = 0.705f; bands_max[5] = 0.745f; // Red Edge
  bands_min[6] = 0.770f; bands_max[6] = 0.895f; // Near-IR1
  bands_min[7] = 0.860f; bands_max[7] = 1.040f; // Near-IR2
};


//: Parse a single ASTER file to retrieve wavelength/spectra data and 
// interpolate spectra for the given bands.
bool baml_parse_aster_file(
  const std::string& file_name,
  const std::vector<float>& bands_min,
  const std::vector<float>& bands_max,
  std::string& sample_type,
  std::vector<float>& sample_spectra)
{
  int num_header_lines = 26;
  float tol = 0.000000001f;

  int num_bands = bands_min.size();

  sample_spectra.resize(bands_min.size());

  // Get the sample type from filename
  std::string file_namebase = vul_file::strip_directory(file_name);

  size_t first_period = file_namebase.find('.');
  size_t second_period = file_namebase.find('.', first_period + 1);
  size_t third_period = file_namebase.find('.', second_period + 1);

  sample_type = file_namebase.substr(
    second_period + 1, third_period - second_period - 1);

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

    wavelengths.push_back( new_w );
    spectra.push_back( atof( awk[1] ) );
  }
  if (wavelengths.size() == 0) return false;

  // Loop through wavelength bands
  for (int b = 0; b < num_bands; b++) {

    // Compute the band mean
    float mean = 0.5f*(bands_min[b] + bands_max[b]);

    // Check if mean is outside the wavelength range
    if (mean <= wavelengths.front() && mean <= wavelengths.back()) {
      sample_spectra[b] = 0.0f;
      continue;
    } else if (mean >= wavelengths.front() && mean >= wavelengths.back()) {
      sample_spectra[b] = 0.0f;
      continue;
    }

    // Otherwise find the two wavelengths in the list that contain the mean
    int idx = 1;
    for (; idx < wavelengths.size(); idx++)
      if ((wavelengths[idx] > mean) != (wavelengths[idx - 1] > mean)) break;

    // Interpolate between the two nearest wavelengths
    float dw = wavelengths[idx] - wavelengths[idx-1];
    float alpha = 0.0f;
    if (fabs(dw) > tol)
      alpha = (mean - wavelengths[idx-1]) / dw;
    sample_spectra[b] = 0.01f* // Convert to percent
      (spectra[idx-1] + alpha*(spectra[idx] - spectra[idx-1]));
    //std::cerr << bands_spectra[b] << '\n';
  }

  return true;
};


//: Parse a directory of ASTER files, adding to existing list.
bool baml_parse_aster_dir(
  const std::string& dir_glob,
  const std::vector<float>& bands_min,
  const std::vector<float>& bands_max,
  std::vector< std::string >& sample_types,
  std::vector< std::vector<float> >& sample_spectra)
{
  for (vul_file_iterator fi(dir_glob); fi; ++fi) {
    sample_types.push_back(std::string());
    sample_spectra.push_back(std::vector<float>(bands_min.size()));
    if (!baml_parse_aster_file(
      fi(), bands_min, bands_max, sample_types.back(), sample_spectra.back()))
        return false;
  }
  return true;
};


//: Make a condensed list of unique strings from sample_types and record the 
// index into this condensed list for each sample.
void baml_condense_sample_types(
  const std::vector< std::string >& sample_types,
  std::vector< std::string >& unique_types,
  std::vector< int >& sample_idx)
{
  unique_types.clear();
  sample_idx.resize(sample_types.size());

  for (int s = 0; s < sample_types.size(); s++) {

    // Search for the type in the current unique list
    sample_idx[s] = -1;
    for (int u = 0; u < unique_types.size(); u++) {
      if (sample_types[s] == unique_types[u]) {
        sample_idx[s] = u;
        break;
      }
    } //u
    
    // If not found, add to unique list
    if (sample_idx[s] == -1) {
      sample_idx[s] = unique_types.size();
      unique_types.push_back(sample_types[s]);
    }
  } //s
};


//: Normalize a vector of spectra to a unit vector.
void baml_normalize_spectra(
  std::vector<float>& spectra)
{
  float tol = 0.000000001f;

  float sumsq = 0.0f;
  for (int b = 0; b < spectra.size(); b++) 
    sumsq += spectra[b]* spectra[b];

  float norm_factor = 0.0f;
  if (sumsq > tol) norm_factor = 1.0f / sqrt(sumsq);
   
  for (int b = 0; b < spectra.size(); b++)
    spectra[b] *= norm_factor;
}


//: Compute spectral angle between two vectors of multi-spectral values
// as per the spectral angle mapper algorithm for a set of spectra samples.
void baml_compute_spectral_angles(
  const std::vector<float>& img_vals,
  const std::vector< std::vector<float> >& normalized_spectra_samples,
  std::vector<float>& angles )
{
  int num_bands = img_vals.size();
  int num_samples = normalized_spectra_samples.size();
  angles.resize(num_samples);

  // Normalize image vals
  std::vector<float> img_norm = img_vals;
  baml_normalize_spectra(img_norm);

  // Loop over samples
  for (int s = 0; s < num_samples; s++) {

    // Dot product 
    float sum = 0.0f;
    for (int b = 0; b < num_bands; b++) 
      sum += normalized_spectra_samples[s][b] * img_norm[b];

    // Spectral angle
    angles[s] = 1.0f - fabs(acosf(sum)) / (float)vnl_math::pi;
  }
};

//: Compute a normalized index image, used in remote sensing literature 
// to classify materials, given two bands.  Examples include:
// Normalized Difference Vegitation Index, b1=red, b2=NIR2
// WorldView Water Index, b1=coastal, b2=NIR2
// Normalized Difference Soil Index, b1=green, b2=yellow
// WorldView Non-Homogeneous Feature Difference, b1=red-edge, b2=coastal
bool baml_compute_normalized_index_image(
  const vil_image_view<float>& mul_img,
  unsigned b1,
  unsigned b2,
  vil_image_view<float>& idx) 
{
  float tol = 0.000000001f;

  if (mul_img.nplanes() != 8) return false;
  idx.set_size(mul_img.ni(), mul_img.nj());

  for (int y = 0; y < mul_img.nj(); y++) {
    for (int x = 0; x < mul_img.ni(); x++) {
      float denom = mul_img(x, y, b1) + mul_img(x, y, b2);
      if (fabs(denom) > tol) 
        idx(x, y) = (mul_img(x, y, b1) - mul_img(x, y, b2)) / denom;
    }
  }
  return true;
};


//: Port of Cara's matClass.m which will use a fair amount of memory
void baml_classify_material_cara(
  const vil_image_view<float>& mul_img,
  const std::vector<int>& sample_idx, 
  const std::vector< std::vector<float> >& normalized_sample_spectra,
  vil_image_view<int>& class_img,
  vil_image_view<float>& conf_img,
  float angle_threshold = 0.9f)
{
  int img_width = mul_img.ni(), img_height = mul_img.nj();
  int num_samples = normalized_sample_spectra.size();
  int num_bands = normalized_sample_spectra[0].size();

  class_img.set_size(img_width, img_height);
  conf_img.set_size(img_width, img_height);

  // Make a very large volume to store spectral angle measurements for each
  // pixel and sample.
  vil_image_view<float> angle_volume(
    img_width, img_height, 1, num_samples);

  std::vector<float> img_vals(num_bands);
  std::vector<float> angles(num_samples);
  //std::vector<float> max_angle(num_samples, 0.0f);

  // Populate the volume
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {

      // Copy the image values
      for (int b = 0; b < num_bands; b++)
        img_vals[b] = mul_img(x, y, b);

      // Compute spectral angles
      baml_compute_spectral_angles(
        img_vals, normalized_sample_spectra, angles);

      for (int s = 0; s < num_samples; s++) {

        // Update running max
        //max_angle[s] = std::max(max_angle[s], angles[s]);

        // Convert to byte for compactness
        //angle_volume(x, y, s) = (vxl_byte)(255 * angles[s]);
        angle_volume(x, y, s) = angles[s];
      }
    }
  }

  // Other processing?

  // Compute the max index
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {
      float max_angle = 0.0f;
      int max_idx = 0;
      for (int s = 0; s < num_samples; s++) {
        if (angle_volume(x, y, s) < max_angle) continue;
        max_idx = sample_idx[s];
        max_angle = angle_volume(x, y, s);
      }
      class_img(x, y) = max_idx;
      conf_img(x, y) = max_angle;
    }
  }

  /*/ DEBUG WRITE TO DISK
  for (int s = 0; s < num_samples; s++) {
    vil_image_view<vxl_byte> vis = vil_plane(angle_volume, s);
    std::stringstream ss;
    ss << "D:/results/ms/angle" << s << ".png";
    vil_save(vis, ss.str().c_str());
  }//*/
};


int main(int argc, char * argv[])
{
  // Set arguments
  std::string aster_dir(
    "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/new/*.spectrum.txt");
    //"C:/Users/thomas.pollard/Desktop/170306_MaterialClassification/aster_data/new/*.txt");
  std::string mul_file(
    //"D:/data/core3d/sevastopol_wv03/2017_01_10_WV03/056247234010_01_003/056247234010_01/056247234010_01_P001_MUL/17JAN10091703-M1BS-056247234010_01_P001.NTF");
    //"D:/data/core3d/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.NTF");
    "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.NTF");
    //"D:/data/core3d/sevastopol_wv03/2017_01_10_WV03/056247234010_01_003/056247234010_01/056247234010_01_P001_MUL/17JAN10091703-M1BS-056247234010_01_P001.TIF");
    //"D:/data/core3d/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.TIF");
  //float mean_albedo = 0.15f;

  std::string out_dir("C:/Users/sca0161/Documents/multispectral results/cpp/");

  unsigned crop_x = 0, crop_y = 0, crop_w = 3500, crop_h = 2000;
  //unsigned crop_x = 3500, crop_y = 500, crop_w = 3500, crop_h = 2000;

  // Setup WV3 bands
  std::vector<float> bands_min, bands_max;
  baml_wv3_bands(bands_min, bands_max);
  int num_bands = bands_min.size();

  // Read the ASTER materials
  std::vector<std::string> sample_types;
  std::vector< std::vector<float> > sample_spectra;
  baml_parse_aster_dir( 
    aster_dir, bands_min, bands_max, sample_types, sample_spectra );
  int num_samples = sample_spectra.size();
  std::cerr << "Found " << num_samples << " ASTER files\n";
  
  // Find and print unique types
  std::vector< std::string > unique_types;
  std::vector< int > sample_idx;
  baml_condense_sample_types(sample_types, unique_types, sample_idx);
  std::cerr << "Unique types found:\n";
  for (int u = 0; u < unique_types.size(); u++)
    std::cerr << u << ": " << unique_types[u] << '\n';

  for (int s = 0; s < num_samples; s++)
    baml_normalize_spectra(sample_spectra[s]);

  // Load the images
  //vil_image_resource_sptr pan_img = vil_load_image_resource(pan_file.c_str());
  //std::cerr << pan_img->nplanes() << ' ' << pan_img->ni() << ' ' << pan_img->nj() << '\n';
  vil_image_resource_sptr mul_img = vil_load_image_resource(mul_file.c_str());
  std::cerr << "Loaded a " << mul_img->ni() << 'x' << mul_img->nj() 
    << " image with " << mul_img->nplanes() << " channels\n";

  // Get a cropped view and convert to float
  vil_image_view<vxl_uint_16> mul_crop = 
    vil_crop( mul_img, crop_x, crop_w, crop_y, crop_h )->get_view();
  vil_image_view<float> mul_f;
  vil_convert_cast(mul_crop, mul_f);

  // Load metadata
  brad_image_metadata meta(vul_file::strip_extension(mul_file)+".IMD");

  // Calibrate the image, lifted from 
  // brad_nitf_abs_radiometric_calibration_process
  for (int b = 0; b < num_bands; b++) {
    vil_image_view<float> band = vil_plane(mul_f, b); 
    vil_math_scale_and_offset_values(
      band, meta.gains_[b + 1].first, meta.gains_[b + 1].second);
  }

  // Correct for atmospherics
  brad_atmospheric_parameters atmo;
  float mean_albedo = 0.3;
  //brad_estimate_atmospheric_parameters_multi(mul_f, meta, atmo);
  brad_estimate_atmospheric_parameters_multi(mul_f, meta, mean_albedo, atmo);
  brad_estimate_reflectance_image_multi(mul_f, meta, atmo, mul_f);//*/

  // Create an RGB image for visualization
  std::cerr << "Saving RGB image\n";
  vil_image_view<float> rgb_img(crop_w, crop_h, 3);
  for (int y = 0; y < crop_h; y++)
    for (int x = 0; x < crop_w; x++) {
      rgb_img(x, y, 0) = mul_f(x, y, 4);
      rgb_img(x, y, 1) = mul_f(x, y, 2);
      rgb_img(x, y, 2) = mul_f(x, y, 1);
    }

  vil_image_view<vxl_byte> vis;
  vil_convert_stretch_range_limited(
    rgb_img, vis, 0.0f, 0.3f);
  vil_save(vis, (out_dir + "rgb.png").c_str());

  // Classify using normalized indices
  std::cerr << "Computing normalized index images\n";
  vil_image_view<float> ndwi, ndvi, ndsi, nhfd;

  baml_compute_normalized_index_image(mul_f, 0, 7, ndwi);
  vil_convert_stretch_range_limited(ndwi, vis, -1.0f, 1.0f);
  vil_save(vis, (out_dir + "ndwi.png").c_str());
  baml_compute_normalized_index_image(mul_f, 7, 4, ndvi);
  vil_convert_stretch_range_limited(ndvi, vis, -1.0f, 1.0f);
  vil_save(vis, (out_dir + "ndvi.png").c_str());
  baml_compute_normalized_index_image(mul_f, 2, 3, ndsi);
  vil_convert_stretch_range_limited(ndsi, vis, -1.0f, 1.0f);
  vil_save(vis, (out_dir + "ndsi.png").c_str());
  baml_compute_normalized_index_image(mul_f, 5, 0, nhfd);
  vil_convert_stretch_range_limited(nhfd, vis, -1.0f, 1.0f);
  vil_save(vis, (out_dir + "nhfd.png").c_str());

  // Classify materials using ASTER
  std::cerr << "Classifying materials via ASTER\n";
  vil_image_view<int> aster_class;
  vil_image_view<float> aster_conf;
  baml_classify_material_cara(
    mul_f, sample_idx, sample_spectra, aster_class, aster_conf);
  vil_convert_stretch_range_limited(
    aster_class, vis, 0, 2 + (int)unique_types.size());
  vil_save(vis, (out_dir+"aster.png").c_str());

  // Ad-hoc fusion of classifiers
  std::cerr << "Computing final material class\n";
  vil_image_view<int> class_img(crop_w, crop_h);
  float ndwi_thresh = 0.5f, ndvi_thresh = 0.25f, aster_thresh = 0.95f;

  for (int y = 0; y < crop_h; y++) {
    for (int x = 0; x < crop_w; x++) {
      if (ndwi(x, y) > ndwi_thresh) class_img(x, y) = 1;
      else if(ndvi(x, y) > ndvi_thresh) class_img(x, y) = 2;
      else if (aster_conf(x, y) > aster_thresh) class_img(x, y) = aster_class(x, y) + 3;
      else class_img(x, y) = 0; //unknown
    }
  }
  vil_convert_stretch_range_limited(
    class_img, vis, 0, 2 + (int)unique_types.size());
  vil_save(vis, (out_dir + "class.png").c_str());

  /*/ Loop through bands
  for (int b = 0; b < num_bands; b++) {

    vil_image_view<vxl_uint_16> p_crop = vil_plane(mul_crop, b);
    vil_image_view<vxl_byte> vis;
    vil_convert_stretch_range_limited(p_crop, vis, (vxl_uint_16)0, (vxl_uint_16)2000);

    std::stringstream ss;
    ss << out_dir << b << ".png";
    vil_save(vis, ss.str().c_str() );
  }*/
  return 0;
};
