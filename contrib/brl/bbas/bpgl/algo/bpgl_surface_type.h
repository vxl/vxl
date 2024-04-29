// This is//terra/bpgl_surface_type_image.h
#ifndef bpgl_surface_type_image_h
#define bpgl_surface_type_image_h

//:
// \file
// \brief A class to store surface_type probabilities
// \author J.L. Mundy
// \date August 4, 2021
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <limits>
#include <string>
#include <vil/vil_image_view.h>
#include <stdexcept>

// an overlay image with colors indicating dsm height and highlighed colors for shadow and shadow_step
bool write_dsm_color_display(std::string const& dsm_path,
                             std::string const& surface_type_path,
                             std::string const& display_path,
                             float shadow_prob_cutoff=0.25f,
                             float shadow_step_prob_cutoff=0.25f,
                             float min_val=NAN,
                             float max_val=NAN);

class bpgl_surface_type
{
 public:
  enum stype { NO_DATA, INVALID_DATA, SHADOW, SHADOW_STEP, GEOMETRIC_CONSISTENCY, NO_SURFACE_TYPE};
  enum domain { RECTIFIED_TARGET, DSM, FUSED_DSM, MOSAIC_DSM, NO_DOMAIN};

  static std::string domain_to_string(domain d);
  static domain domain_from_string(std::string const& str);

  bpgl_surface_type()
    : ni_(0), nj_(0)
  {
    init_type_names();
  }

  bpgl_surface_type(domain s, size_t ni, size_t nj)
    : domain_(s), ni_(ni), nj_(nj)
  {
    init_type_names();
    init_type_images();
  }

  //: from list of surface_type images
  bpgl_surface_type(domain const s,
                    std::map<stype, vil_image_view<float> > const& type_images)
    : domain_(s), type_images_(type_images)
  {
    init_type_names();
    ni_ = type_images_[NO_DATA].ni();
    nj_ = type_images_[NO_DATA].nj();
  }

  //:load from tif files
  bpgl_surface_type(std::string const& directory)
  {
    init_type_names();
    this->read(directory);
  }

  //: set size and intialize
  void set_size(domain const s, size_t ni, size_t nj)
  {
    domain_ = s;
    ni_ = ni;
    nj_ = nj;
    init_type_images();
  }

  //: set type image layer
  bool set_type_image(stype type, vil_image_view<float> const& type_image)
  {
    if ((type_image.ni() != ni_) || (type_image.nj() != nj_)) {
      return false;
    }
    type_images_[type] = type_image;
    return true;
  }

  //: get type probability (set as well)
  float& p(size_t i, size_t j, stype type)
  {
    if (type_images_.count(type) == 0||i>=ni_||j>=nj_) {
      throw std::runtime_error("invalid type or i, j out of bounds");
    }
    return type_images_[type](i, j);
  }

  //: const probability accessor
  float const_p(size_t i, size_t j, stype type) const
  {
    if (type_images_.count(type) == 0 || i >= ni_ || j >= nj_) {
      throw std::runtime_error("invalid type or i, j out of bounds");
    }
    std::map<stype, vil_image_view<float> >::const_iterator it = type_images_.find(type);
    return it->second(i, j);
  }

  //: apply a bool image to set probabilites to 1.0f == true, 0.0f == false
  bool apply(vil_image_view<bool> const& mask, stype type);

  //: apply a probability image to set probabilites
  bool apply(vil_image_view<float> const & prob, stype type);

  //: apply a source image with a threshold to set p = 1.0f or 0.0f
  template <class T>
  bool apply(vil_image_view<T> const& source, T thresh, stype type)
  {
    size_t ni = source.ni(), nj = source.nj();
    if (ni != ni_ || nj != nj_) {
      std::cout << "mismatch in source image size " << ni << ' ' << nj << std::endl;
      return false;
    }
    vil_image_view<bool> temp(ni, nj);
    temp.fill(false);
    for (size_t j = 0; j<nj; ++j) {
      for (size_t i = 0; i<ni; ++i) {
        if (p(i, j, INVALID_DATA)>0.0f) {
          continue;
        }
        T v = source(i, j);
        temp(i,j) = (v<=thresh);
      }
    }
    return apply(temp, type);
  }

  //: map string to surface_type index
  stype type_from_string(std::string const& type_string) const
  {
    for (auto const& type_name : type_names_) {
      if (type_name.second == type_string) {
        return type_name.first;
      }
    }
    return NO_SURFACE_TYPE;
  }

  //: map surface_type index to string
  std::string type_to_string(stype const& type) const
  {
    return type_names_.at(type);
  }

  bool read(std::string const& path);

  bool write(std::string const& path) const;

  //: accessors
  size_t ntypes() const {return type_images_.size();}
  size_t ni() const {return ni_;}
  size_t nj() const {return nj_;}
  domain domain_id() const {return domain_;}

  bool type_image(std::string const& type_name, vil_image_view<float>& type_image )
  {
    return type_image(type_from_string(type_name), type_image);
  }

  bool type_image(stype type, vil_image_view<float>& type_image) const
  {
    //map [] operator is non_const so need the implementation below
    std::map<stype, vil_image_view<float> >::const_iterator it = type_images_.find(type);
    if (it == type_images_.end())
      return false;
    type_image = it->second;
    return true;
  }

  vil_image_view<float> type_image(std::string const& type_name) const;
  vil_image_view<float> type_image(stype type) const;
  std::map<stype, vil_image_view<float> > type_images() const;

  //: the available types
  std::vector<bpgl_surface_type::stype>& stypes() { return types_; }

  std::vector<std::string> string_types() const
  {
    std::vector<std::string> ret;
    for (std::map<stype, std::string>::const_iterator nit = type_names_.begin();
         nit != type_names_.end(); nit++)
    {
      ret.push_back(nit->second);
    }
    return ret;
  }

  //: display methods
  // display a type channel on the source image
  template <class T>
  bool color_type_display(domain d, stype type, vil_image_view<T> const& source,
                          vil_image_view<float>& color_img) const
  {
    if (d != this->domain_) {
      std::cout << "incorrect domain " << domain_to_string(d) << " vs. " << domain_to_string(this->domain_) << std::endl;
      return false;
    }
    size_t ni = source.ni(), nj = source.nj();
    if (ni != ni_ || nj != nj_) {
      std::cout << "mismatch in source image size " << ni << ' ' << nj << std::endl;
      return false;
    }
    vil_image_view<float> t;
    if (!type_image(type, t)) {
      return false;
    }
    color_img.set_size(ni_, nj_, 3);
    for (size_t j = 0; j < nj_; ++j) {
      for (size_t i = 0; i < ni_; ++i) {
        float p = t(i, j);
        T v = source(i, j);
        color_img(i, j, 0) = float(v) /(1 - p);
        color_img(i, j, 1) = float(v);
        color_img(i, j, 1) = float(v);
      }
    }
    return true;
  }

  // displays shadow and shadow step on elevation colored fused dsm
  bool dsm_color_display(vil_image_view<float> const& dsm,
                         vil_image_view<vxl_byte>& display,
                         float shadow_prob_cutoff=0.25f,
                         float shadow_step_prob_cutoff=0.25f,
                         float min_val=NAN,
                         float max_val=NAN) const;

 private:

  // internal methods
  void init_type_names()
  {
    type_names_[NO_DATA] = "no_data";
    types_.push_back(NO_DATA);

    type_names_[INVALID_DATA] = "invalid_data";
    types_.push_back(INVALID_DATA);

    type_names_[SHADOW] = "shadow";
    types_.push_back(SHADOW);

    type_names_[SHADOW_STEP] = "shadow_step";
    types_.push_back(SHADOW_STEP);

    type_names_[GEOMETRIC_CONSISTENCY] = "geometric_consistency";
    types_.push_back(GEOMETRIC_CONSISTENCY);

    type_names_[NO_SURFACE_TYPE] = "no_surface_type";
  }

  void init_type_images()
  {
    for(std::map<stype, std::string>::iterator nit = type_names_.begin();
        nit != type_names_.end(); ++nit)
    {
      type_images_[nit->first] = vil_image_view<float>(ni_, nj_);
      type_images_[nit->first].fill(0.0f);
    }
  }

  // members
  domain domain_;
  size_t ni_;
  size_t nj_;
  std::vector<stype> types_;
  std::map<stype, std::string> type_names_;
  std::map<stype, vil_image_view<float> > type_images_;

};

#endif//bpgl_surface_type_image_h
