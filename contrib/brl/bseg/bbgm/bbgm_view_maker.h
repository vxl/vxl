// This is brl/bseg/bbgm/bbgm_view_maker.h
#ifndef bbgm_mean_viewer_h_
#define bbgm_mean_viewer_h_
//:
// \file
// \brief Helper classes to make a vil_image_view from an image of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date Mar 15, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>
#include <vbl/vbl_ref_count.h>
#include <bbgm/bbgm_viewer.h>
#include <bbgm/bbgm_apply.h>
#include <vpdl/vpdt/vpdt_mixture_accessors.h>
#include <vpdl/vpdt/vpdt_enable_if.h>
#include <vil/vil_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: base class for objects that extract the mean from an image of distribution
class bbgm_view_maker_base : public vbl_ref_count
{
 public:
  //: return the type_info for the distribution type
  virtual const std::type_info& dist_typeid() const=0;

  //: Return the maximum number of components if a mixture
  // If not a mixture return 1
  virtual unsigned int max_components(const bbgm_image_sptr& dimg) const=0;

  //: Compute the mean of the distribution image to produce a vil image
  // \param component_idx indicated the component index to operate on.
  //        If < 0 or not a mixture then operate on the whole distribution
  virtual bool apply(const bbgm_image_sptr& dimg,
                     vil_image_view<double>& image,
                     const double* fail_val,
                     int component_idx = -1) const=0;
};


//: base class for objects that extract the mean from an image of distribution
template <class dist_type, class func_type, class Disambiguate= void>
class bbgm_view_maker
  : public bbgm_view_maker_base
{
  //: return the type_info for the distribution type
  const std::type_info& dist_typeid() const override
  { return typeid(dist_type); }

  //: Return the maximum number of components if a mixture
  // If not a mixture return 1
  unsigned int max_components(const bbgm_image_sptr& dimg) const override
  { return 1; }

  //: Compute the mean of the distribution image to produce a vil image
  // \param component_idx indicated the component index to operate on.
  //        If < 0 or not a mixture then operate on the whole distribution
  bool apply(const bbgm_image_sptr& dimg,
                     vil_image_view<double>& image,
                     const double* fail_val,
                     int component_idx = -1) const override
  {
    typedef bbgm_image_of<dist_type> dist_image;
    const dist_image* d = dynamic_cast<const dist_image*>(dimg.ptr());
    if (!d)
      return false;

    func_type func;
    bbgm_apply(*d, func, image, fail_val);
    return true;
  }
};


//: helper class to determine the maximum number of components in an image of mixtures
template <class dist_type, class = void>
struct bbgm_num_components
{
  static unsigned int max(const bbgm_image_of<dist_type>& dist_image)
  {
    vpdt_num_components_accessor<dist_type> func;
    vil_image_view<unsigned int> count;
    unsigned int fail_val = 0;
    bbgm_apply(dist_image, func, count, &fail_val);
    unsigned int min_val, max_val;
    vil_math_value_range(count, min_val, max_val);
    return max_val;
  }
};

//: helper class to determine the maximum number of components in an image of mixtures
// For fixed sized mixtures \return the fixed maximum number of components
template <class dist_type>
struct bbgm_num_components<dist_type, typename dist_type::max_components>
{
  static unsigned int max(const bbgm_image_of<dist_type>& dist_image)
  {
    return dist_type::max_components;
  }
};


//: base class for objects that extract the mean from an image of distribution
// Specialization for mixtures
template <class dist_type, class func_type>
class bbgm_view_maker<dist_type, func_type,
                      typename vpdt_enable_if<vpdt_is_mixture<dist_type> >::type>
  : public bbgm_view_maker_base
{
  //: Return the type_info for the distribution type
  const std::type_info& dist_typeid() const override
  { return typeid(dist_type); }

  //: Return the maximum number of components if a mixture
  // If not a mixture return 1
  unsigned int max_components(const bbgm_image_sptr& dimg) const override
  {
    typedef bbgm_image_of<dist_type> dist_image;
    const dist_image* d = dynamic_cast<const dist_image*>(dimg.ptr());
    assert(d);
    return bbgm_num_components<dist_type>::max(*d);
  }

  //: Compute the mean of the distribution image to produce a vil image
  // \param component_idx indicated the component index to operate on.
  //        If < 0 or not a mixture then operate on the whole distribution
  bool apply(const bbgm_image_sptr& dimg,
                     vil_image_view<double>& image,
                     const double* fail_val,
                     int component_idx = -1) const override
  {
    typedef bbgm_image_of<dist_type> dist_image;
    typedef typename dist_type::component_type comp_type;
    const dist_image* d = dynamic_cast<const dist_image*>(dimg.ptr());
    if (!d)
      return false;

    typedef typename func_type::template rebind<comp_type>::other func_comp;
    typedef vpdt_mixture_accessor<dist_type, func_comp> func_mix;
    if (component_idx < 0 || (func_type::valid_functor && !func_mix::valid_functor))
    {
      func_type func;
      bbgm_apply(*d, func, image, fail_val);
      return true;
    }
    else if (func_mix::valid_functor)
    {
      func_mix func(component_idx);
      bbgm_apply(*d, func, image, fail_val);
      return true;
    }
    return false;
  }
};


#endif // bbgm_mean_viewer_h_
