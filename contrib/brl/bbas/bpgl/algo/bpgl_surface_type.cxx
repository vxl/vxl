#include "bpgl_surface_type.h"
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vnl/vnl_math.h>
#include <bsta/bsta_histogram.h>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <bvrml/bvrml_write.h>

// enumeration to/from string
std::string
bpgl_surface_type::domain_to_string(bpgl_surface_type::domain d)
{
  if (d == RECTIFIED_TARGET) {
    return "RECTIFIED_TARGET";
  } else if (d == DSM) {
    return "DSM";
  } else if (d == FUSED_DSM) {
    return "FUSED_DSM";
  } else if (d == MOSAIC_DSM) {
    return "MOSAIC_DSM";
  }
  return "NO_DOMAIN";
}

bpgl_surface_type::domain
bpgl_surface_type::domain_from_string(std::string const& str)
{
  // upper case input
  std::string str_upper = str;
  std::transform(str_upper.begin(), str_upper.end(),
                 str_upper.begin(), ::toupper);

  // compare upper case
  if (str_upper == "RECTIFIED_TARGET") {
    return RECTIFIED_TARGET;
  } else if (str_upper == "DSM") {
    return DSM;
  } else if (str_upper == "FUSED_DSM") {
    return FUSED_DSM;
  } else if (str_upper == "MOSAIC_DSM") {
    return MOSAIC_DSM;
  }
  return NO_DOMAIN;
}


// single type_image from string input
vil_image_view<float>
bpgl_surface_type::type_image(std::string const& type_name) const
{
  return type_image(type_from_string(type_name));
}

// single type_image from stype enumeration
vil_image_view<float>
bpgl_surface_type::type_image(stype type) const
{
  vil_image_view<float> image;
  if (!type_image(type, image)) {
    throw std::runtime_error("bpgl_surface_type::type_image failure");
  }
  return image;
}

// type_images std::map
std::map<bpgl_surface_type::stype, vil_image_view<float> >
bpgl_surface_type::type_images() const
{
  return type_images_;
}

bool
bpgl_surface_type::read(std::string const& directory)
{
  if (!vul_file::is_directory(directory)) {
    std::cerr << "type directory not accessable "
              << directory << std::endl;
    return false;
  }

  std::string glob = directory+"/*.tif";
  for (vul_file_iterator fn = glob; fn; ++fn) {
    std::string path = fn();
    vil_image_view<float> type_img = vil_load(path.c_str());
    if (type_img.ni() == 0|| type_img.nj() == 0) {
      std::cerr << "can't load type image from " << path << std::endl;
      return false;
    }
    path = vul_file::strip_directory(path);
    std::string type_str = vul_file::strip_extension(path);
    stype t = this->type_from_string(type_str);
    if (t == NO_SURFACE_TYPE) {
      continue;
    }
    type_images_[t] = type_img;
  }
  std::ifstream istr(directory + "/" + "domain.txt");
  if (!istr) {
      std::cerr << "domain.txt missing from directory" << std::endl;
      return false;
  }
  std::string domain_str;
  istr >> domain_str;
  domain_ = this->domain_from_string(domain_str);

  ni_ = type_images_[NO_DATA].ni();
  nj_ = type_images_[NO_DATA].nj();
  return true;
}

bool
bpgl_surface_type::write(std::string const& directory) const
{
  if ((type_images_.size() == 0) || (ni_ == 0) || (nj_ == 0)) {
    std::cerr << "surface type images are not initalized or empty" << std::endl;
    return false;
  }

  if (!vul_file::is_directory(directory)) {
    std::cerr << "surface type data directory not accessable "
              << directory << std::endl;
    return false;
  }

  for (auto const& image : type_images_) {
    if (image.first == NO_SURFACE_TYPE) {
      continue;
    }
    std::string path = directory + "/" + type_to_string(image.first) + ".tif";
    if (!vil_save(image.second, path.c_str())) {
      std::cerr << "Can't save type image to " << path << std::endl;
      return false;
    }
  }

  std::string domain_path = directory + "/domain.txt";
  std::ofstream ostr(domain_path.c_str());
  if (!ostr) {
    std::cerr << "Can't write to " << domain_path << std::endl;
    return false;
  }
  ostr << domain_to_string(domain_) << std::endl;
  ostr.close();

  return true;
}

bool
bpgl_surface_type::apply(vil_image_view<bool> const& mask, stype type)
{
  if (type_images_.count(type) == 0) {
    std::cerr << "specified type " << type << " does not exist" << std::endl;
    return false;
  }
  vil_image_view<float>& type_img = type_images_[type];
  if( type_img.ni()!=ni_ || type_img.nj() != nj_) {
    std::cerr << "mismatch in type image dimensions" << std::endl;
    return false;
  }
  for (size_t j = 0; j<nj_; ++j) {
    for (size_t i = 0; i<ni_; ++i) {
      bool m = mask(i,j);
      if(m) type_img(i,j) = 1.0f;
      else type_img(i,j) = 0.0f;
    }
  }
  return true;
}

bool
bpgl_surface_type::apply(vil_image_view<float> const& prob, stype type)
{
  if (type_images_.count(type) == 0)
  {
    std::cerr << "specified type " << type << " does not exist" << std::endl;
    return false;
  }
  vil_image_view<float> & type_img = type_images_[type];
  if (type_img.ni() != ni_ || type_img.nj() != nj_)
  {
    std::cerr << "mismatch in type image dimensions" << std::endl;
    return false;
  }
  for (size_t j = 0; j < nj_; ++j) {
    for (size_t i = 0; i < ni_; ++i) {
      float pr = prob(i, j);
      type_img(i, j) = pr;
    }
  }
  return true;
}

bool
bpgl_surface_type::dsm_color_display(vil_image_view<float> const& dsm,
                                     vil_image_view<unsigned char>& display,
                                     float shadow_prob_cutoff,
                                     float shadow_step_prob_cutoff,
                                     float min_val,
                                     float max_val) const
{
  if (this->domain_ != DSM &&
      this->domain_ != FUSED_DSM &&
      this->domain_ != MOSAIC_DSM)
  {
    std::cerr << "dsm_color_display only applies to a DSM surface type" << std::endl;
    return false;
  }

  size_t ni = dsm.ni(), nj = dsm.nj();
  if (ni != ni_ || nj != nj_) {
    std::cerr << "inconsistent dsm dimensions" << std::endl;
    return false;
  }

  // display limits
  float low_frac = 0.05f;
  float high_frac = 1.0f - low_frac;

  if (std::isnan(min_val)) {
    vil_math_value_range_percentile(dsm, low_frac, min_val, true);
  }
  if (std::isnan(max_val)) {
    vil_math_value_range_percentile(dsm, high_frac, max_val, true);
  }

  std::cout << "dsm_color_display [min,max] = [" << min_val << ", " << max_val << "]" << std::endl;

  float scale = 1.0f;
  if(max_val-min_val > 0.0f)
    scale = 255.0f/(max_val-min_val);

  display.set_size(ni, nj, 3);
  display.fill(vxl_byte(255));

  for (size_t j = 0; j<nj; ++j) {
    for (size_t i = 0; i<ni; ++i) {
      float v = dsm(i,j);
      if (!vnl_math::isfinite(v))
        continue;
      float sv = (v-min_val)*scale;
      if(sv<0.0f) sv=0.0f;
      if(sv>255.0f) sv = 255.0f;
      size_t cind = static_cast<size_t>(sv);
      for (size_t c = 0; c<3; ++c) {
        display(i, j, c) = bvrml_custom_color::heatmap_custom[cind][c];
      }
    }
  }

  // apply shadow and shadow step color
  vil_image_view<float> shadow;
  vil_image_view<float> shadow_step;
  if (!type_image(SHADOW, shadow))
    return false;
  if (!type_image(SHADOW_STEP, shadow_step))
    return false;

  for (size_t j = 0; j<nj; ++j) {
    for (size_t i = 0; i<ni; ++i) {
      float rd = static_cast<float>(display(i,j,0)), gd = static_cast<float>(display(i,j,1)), bd = static_cast<float>(display(i,j,2));
      // write shadow type pixels first
      float s = shadow(i,j);
      if (s > shadow_prob_cutoff) {
        //black color
        display(i,j,0) = static_cast<vxl_byte>(0.0f);
        display(i,j,1) = static_cast<vxl_byte>(0.0f);
        display(i,j,2) = static_cast<vxl_byte>(0.0f);
      }
      //possibly overwrite with shadow step type
      float ss = shadow_step(i,j);
      if (ss > shadow_step_prob_cutoff) {
        // violet color
        display(i,j,0) = static_cast<vxl_byte>(190.0f);
        display(i,j,1) = static_cast<vxl_byte>(0.0f);
        display(i,j,2) = static_cast<vxl_byte>(190.0f);
      }
    }
  }
  return true;
}

bool
write_dsm_color_display(std::string const& dsm_path,
                        std::string const& surface_type_path,
                        std::string const& display_path,
                        float shadow_prob_cutoff,
                        float shadow_step_prob_cutoff,
                        float min_val,
                        float max_val)
{
  vil_image_view<float> dsm = vil_load(dsm_path.c_str());
  size_t ni = dsm.ni(), nj = dsm.nj();
  if (ni == 0 || nj == 0) {
    std::cerr << "Can't load dsm image from " << dsm_path << std::endl;
    return false;
  }
  bpgl_surface_type st(surface_type_path);
  if (st.ni() != ni || st.nj() != nj) {
    std::cerr << "inconsistent dimensions between dsm (" << ni << ' ' << nj << ") and surface type ("<< st.ni() << ' ' << st.nj() << ")" << std::endl;
    return false;
  }
  vil_image_view<vxl_byte> display;
  if (!st.dsm_color_display(dsm, display, shadow_prob_cutoff,
                            shadow_step_prob_cutoff, min_val, max_val))
  {
    std::cerr << "create display failed" << std::endl;
    return false;
  }
  return vil_save(display, display_path.c_str());
}
