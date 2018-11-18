// This is brl/bseg/bbgm/bbgm_viewer.cxx
//:
// \file

#include "bbgm_viewer.h"
#include "bbgm_view_maker.h"
#include "bbgm_apply.h"
#include <vil/vil_image_view.h>


//: Probe to see if this viewer can handle this image type
bool bbgm_viewer::probe(const bbgm_image_sptr& dimg) const
{
  if (!dimg)
    return false;

  if (!view_maker_ || view_maker_->dist_typeid() != dimg->dist_typeid())
    view_maker_ = this->find_vm(dimg->dist_typeid());

  return view_maker_ != nullptr;
}


//: Return the maximum number of components if a mixture
// If not a mixture return 1
unsigned int bbgm_viewer::max_components(const bbgm_image_sptr& dimg) const
{
  if (!dimg)
    return 0;

  if (!view_maker_ || view_maker_->dist_typeid() != dimg->dist_typeid())
    view_maker_ = this->find_vm(dimg->dist_typeid());
  if (!view_maker_)
    return 0;
  return view_maker_->max_components(dimg);
}


//: Apply the operations to the distribution image to produce a vil image
bool bbgm_viewer::apply(const bbgm_image_sptr& dimg,
                        vil_image_view<double>& image) const
{
  if (!dimg)
    return false;

  if (!view_maker_ || view_maker_->dist_typeid() != dimg->dist_typeid())
    view_maker_ = this->find_vm(dimg->dist_typeid());
  if (!view_maker_)
    return false;
  return view_maker_->apply(dimg, image, fail_val_, active_component_);
}


//=============================================================================

//: initialize the static vector of registered types
std::vector<bbgm_view_maker_sptr> bbgm_mean_viewer::reg_vms;

//: Register a new view_maker
// \return true if successfully registered or false if invalid or already
//         registered.
bool bbgm_mean_viewer::register_view_maker(const bbgm_view_maker_sptr& vm)
{
  if (!vm) // don't add null pointers
    return false;

  // replace existing type match
  for (auto & reg_vm : reg_vms)
    if (reg_vm->dist_typeid() == vm->dist_typeid()){
      reg_vm = vm;
      return true;
    }

  // add to the collection if no duplicate found
  reg_vms.push_back(vm);
  return true;
}


//: Return a pointer to the view_maker that applies to this image type.
//  Return a null pointer if no such view_maker is found
bbgm_view_maker_sptr
bbgm_mean_viewer::find_view_maker(const std::type_info& dist_type)
{
  typedef std::vector<bbgm_view_maker_sptr>::const_iterator vm_itr;
  for (vm_itr i=reg_vms.begin(); i!=reg_vms.end(); ++i)
    if ((*i)->dist_typeid() == dist_type)
      return *i;
  return bbgm_view_maker_sptr(nullptr);
}

//=============================================================================

//: initialize the static vector of registered types
std::vector<bbgm_view_maker_sptr> bbgm_variance_viewer::reg_vms;

//: Register a new view_maker
// \return true if successfully registered or false if invalid or already
//         registered.
bool bbgm_variance_viewer::register_view_maker(const bbgm_view_maker_sptr& vm)
{
  if (!vm) // don't add null pointers
    return false;

  // replace existing type match
  for (auto & reg_vm : reg_vms)
    if (reg_vm->dist_typeid() == vm->dist_typeid()){
      reg_vm = vm;
      return true;
    }

  // add to the collection if no duplicate found
  reg_vms.push_back(vm);
  return true;
}


//: Return a pointer to the view_maker that applies to this image type.
//  Return a null pointer if no such view_maker is found
bbgm_view_maker_sptr
bbgm_variance_viewer::find_view_maker(const std::type_info& dist_type)
{
  typedef std::vector<bbgm_view_maker_sptr>::const_iterator vm_itr;
  for (vm_itr i=reg_vms.begin(); i!=reg_vms.end(); ++i)
    if ((*i)->dist_typeid() == dist_type)
      return *i;
  return bbgm_view_maker_sptr(nullptr);
}


//=============================================================================

//: initialize the static vector of registered types
std::vector<bbgm_view_maker_sptr> bbgm_weight_viewer::reg_vms;

//: Register a new view_maker
// \return true if successfully registered or false if invalid or already
//         registered.
bool bbgm_weight_viewer::register_view_maker(const bbgm_view_maker_sptr& vm)
{
  if (!vm) // don't add null pointers
    return false;

  // replace existing type match
  for (auto & reg_vm : reg_vms)
    if (reg_vm->dist_typeid() == vm->dist_typeid()){
      reg_vm = vm;
      return true;
    }

  // add to the collection if no duplicate found
  reg_vms.push_back(vm);
  return true;
}


//: Return a pointer to the view_maker that applies to this image type.
//  Return a null pointer if no such view_maker is found
bbgm_view_maker_sptr
bbgm_weight_viewer::find_view_maker(const std::type_info& dist_type)
{
  typedef std::vector<bbgm_view_maker_sptr>::const_iterator vm_itr;
  for (vm_itr i=reg_vms.begin(); i!=reg_vms.end(); ++i)
    if ((*i)->dist_typeid() == dist_type)
      return *i;
  return bbgm_view_maker_sptr(nullptr);
}
