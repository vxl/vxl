// This is brl/bseg/bbgm/bbgm_viewer.h
#ifndef bbgm_viewer_h_
#define bbgm_viewer_h_
//:
// \file
// \brief Create a vil_image_view of an image of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 31, 2006

#include "bbgm_image_of.h"
#include "bbgm_image_sptr.h"
#include "bbgm_view_maker.h"
#include "bbgm_view_maker_sptr.h"
#include <vil/vil_image_view.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>

//: base class for viewers
class bbgm_viewer : public vbl_ref_count
{
 public:
  //: Constructor
  bbgm_viewer() : active_component_(0), view_maker_(bbgm_view_maker_sptr(nullptr))
  {fail_val_[0] = fail_val_[1] = fail_val_[2] = 0.0;}
  //: Destructor
  ~bbgm_viewer() override = default;

  //: Probe to see if this viewer can handle this image type
  bool probe(const bbgm_image_sptr& dimg) const;
  //: Return the maximum number of components if a mixture
  // If not a mixture return 1
  unsigned int max_components(const bbgm_image_sptr& dimg) const;
  //: Apply the operations to the distribution image to produce a vil image
  bool apply(const bbgm_image_sptr& dimg,
             vil_image_view<double>& image) const;
  //: Return the name of this view
  virtual std::string name() const = 0;

  //: Set the value for failure at a pixel
  void set_failure_value(const vnl_vector<double>& val) { fail_val_[0] = val[0];
                                                          fail_val_[1] = val[1];
                                                          fail_val_[2] = val[2]; }
  //: Set the value for failure at a pixel
  void set_active_component(unsigned int val) { active_component_ = val; }

  //: Return the active component
  unsigned int active_component() const { return active_component_; }

 protected:
  //: Return a pointer to the view_maker that applies to this image type.
  //  Return a null pointer if no such view_maker is found
  virtual bbgm_view_maker_sptr
  find_vm(const std::type_info& dist_type) const = 0;

  //: the data used when a pixel fails the operation
  double fail_val_[3];
  //: the active component to use if the distribution is a mixture
  unsigned int active_component_;
  //: a cached pointer to the view_maker object last found
  mutable bbgm_view_maker_sptr view_maker_;
};

//=============================================================================

//: A mean viewer
class bbgm_mean_viewer : public bbgm_viewer
{
 public:
  //: Return the name of this view
  std::string name() const override { return "mean"; }

  //: Register a new view_maker
  // \return true if successfully registered or false if invalid or already
  //         registered.
  static bool register_view_maker(const bbgm_view_maker_sptr& vm);

  //: Return a pointer to the view_maker that applies to this image type.
  //  Return a null pointer if no such view_maker is found
  static bbgm_view_maker_sptr
  find_view_maker(const std::type_info& dist_type);

 protected:
  //: A virtual wrapper around the static find function
  bbgm_view_maker_sptr
  find_vm(const std::type_info& dist_type) const override
  {
    return find_view_maker(dist_type);
  }

 private:
  //: The registered view maker objects
  static std::vector<bbgm_view_maker_sptr> reg_vms;
};

//=============================================================================

//: A variance viewer
class bbgm_variance_viewer : public bbgm_viewer
{
 public:
  //: Return the name of this view
  std::string name() const override { return "variance"; }

  //: Register a new view_maker
  // \return true if successfully registered or false if invalid or already
  //         registered.
  static bool register_view_maker(const bbgm_view_maker_sptr& vm);

  //: Return a pointer to the view_maker that applies to this image type.
  //  Return a null pointer if no such view_maker is found
  static bbgm_view_maker_sptr
  find_view_maker(const std::type_info& dist_type);

 protected:
  //: A virtual wrapper around the static find function
  bbgm_view_maker_sptr
  find_vm(const std::type_info& dist_type) const override
  {
    return find_view_maker(dist_type);
  }

 private:
  //: The registered view maker objects
  static std::vector<bbgm_view_maker_sptr> reg_vms;
};

//=============================================================================

//: A weight viewer
class bbgm_weight_viewer : public bbgm_viewer
{
 public:
  //: Return the name of this view
  std::string name() const override { return "weight"; }

  //: Register a new view_maker
  // \return true if successfully registered or false if invalid or already
  //         registered.
  static bool register_view_maker(const bbgm_view_maker_sptr& vm);

  //: Return a pointer to the view_maker that applies to this image type.
  //  Return a null pointer if no such view_maker is found
  static bbgm_view_maker_sptr
  find_view_maker(const std::type_info& dist_type);

 protected:
  //: A virtual wrapper around the static find function
  bbgm_view_maker_sptr
  find_vm(const std::type_info& dist_type) const override
  {
    return find_view_maker(dist_type);
  }

 private:
  //: The registered view maker objects
  static std::vector<bbgm_view_maker_sptr> reg_vms;
};


#endif // bbgm_viewer_h_
