// This is brl/bseg/bbgm/bbgm_viewer.h
#ifndef bbgm_viewer_h_
#define bbgm_viewer_h_

//:
// \file
// \brief Create a vil_image_view of an image of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 01/31/06
//
// \verbatim
//  Modifications
// \endverbatim


#include "bbgm_image_of.h"
#include "bbgm_image_sptr.h"
#include <vil/vil_image_view.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>

//: base class for viewers
class bbgm_viewer : public vbl_ref_count
{
  public:
    //: Constructor
    bbgm_viewer() : active_component_(0) {fail_val_[0] = fail_val_[1] = fail_val_[2] = 0.0;}
    //: Destructor
    virtual ~bbgm_viewer() {}

    //: Probe to see if this viewer can handle this image type
    virtual bool probe(const bbgm_image_sptr& dimg) const = 0;
    //: Return the maximum number of components if a mixture
    // If not a mixture return 1
    virtual unsigned int max_components(const bbgm_image_sptr& dimg) const = 0;
    //: Apply the operations to the distribution image to produce a vil image
    virtual bool apply(const bbgm_image_sptr& dimg,
                       vil_image_view<double>& image) const = 0;
    //: Return the name of this view
    virtual vcl_string name() const = 0;

    //: Set the value for failure at a pixel
    void set_failure_value(const vnl_vector<double>& val) { fail_val_[0] = val[0];
                                                            fail_val_[1] = val[1];
                                                            fail_val_[2] = val[2]; }
    //: Set the value for failure at a pixel
    void set_active_component(unsigned int val) { active_component_ = val; }

    //: Return the active component
    unsigned int active_component() const { return active_component_; }

  protected:
    double fail_val_[3];
    unsigned int active_component_;
};


//: A mean viewer
class bbgm_mean_viewer : public bbgm_viewer
{
  public:
    //: Probe to see if this viewer can handle this image type
    virtual bool probe(const bbgm_image_sptr& dimg) const;

    //: Return the maximum number of components if a mixture
    // If not a mixture return 1
    virtual unsigned int max_components(const bbgm_image_sptr& dimg) const;

    //: Apply the operations to the distribution image to produce a vil image
    virtual bool apply(const bbgm_image_sptr& dimg,
                       vil_image_view<double>& image) const;

    //: Return the name of this view
    virtual vcl_string name() const { return "mean"; }
};


//: A variance viewer
class bbgm_variance_viewer : public bbgm_viewer
{
  public:
    //: Probe to see if this viewer can handle this image type
    virtual bool probe(const bbgm_image_sptr& dimg) const;

    //: Return the maximum number of components if a mixture
    // If not a mixture return 1
    virtual unsigned int max_components(const bbgm_image_sptr& dimg) const;

    //: Apply the operations to the distribution image to produce a vil image
    virtual bool apply(const bbgm_image_sptr& dimg,
                       vil_image_view<double>& image) const;

    //: Return the name of this view
    virtual vcl_string name() const { return "variance"; }
};


//: A weight viewer
class bbgm_weight_viewer : public bbgm_viewer
{
  public:
    //: Probe to see if this viewer can handle this image type
    virtual bool probe(const bbgm_image_sptr& dimg) const;

    //: Return the maximum number of components if a mixture
    // If not a mixture return 1
    virtual unsigned int max_components(const bbgm_image_sptr& dimg) const;

    //: Apply the operations to the distribution image to produce a vil image
    virtual bool apply(const bbgm_image_sptr& dimg,
                       vil_image_view<double>& image) const;

    //: Return the name of this view
    virtual vcl_string name() const { return "weight"; }
};


#endif // bbgm_viewer_h_
