// This is core/vil/vil_image_resource_plugin.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Interface for loading new image formats 
// This class provides an interface for loading images in new formats
// \author      Franck Bettinger
// \date        Sun Mar 17 22:57:00 2002
//
// \verbatim
// Modifications
// \endverbatim

#include "vil_image_resource_plugin.h"
#include <vcl_vector.h>

//=======================================================================

static vcl_vector<vil_image_resource_plugin*>
    *vil_image_resource_plugins_list_ = 0;

//=======================================================================

vil_image_resource_plugin::vil_image_resource_plugin()
{
  filetype_="";
  colour_="";
  width_=-1;
  height_=-1;
}

//=======================================================================

vil_image_resource_plugin::~vil_image_resource_plugin()
{
}

//=======================================================================

vcl_string vil_image_resource_plugin::is_a() const
{
  return vcl_string("vil_image_resource_plugin");
}

//=======================================================================

void vil_image_resource_plugin::set_colour(const vcl_string & colour)
{
  colour_=colour;
}

//=======================================================================

void vil_image_resource_plugin::set_filetype(const vcl_string & filetype)
{
  filetype_=filetype;
}

//=======================================================================

void vil_image_resource_plugin::set_size(int width, int height)
{
  width_=width;
  height_=height;
}

//=======================================================================

bool vil_image_resource_plugin::load_the_image (
    vil_image_view_base_sptr& image,
    const vcl_string & path)
{
  return load_the_image(image,path,filetype_,colour_);
}

//=======================================================================

bool vil_image_resource_plugin::load_the_image (
    vil_image_view_base_sptr& image,
    const vcl_string & path, const vcl_string & filetype,
    const vcl_string & colour)
{
  if (vil_image_resource_plugins_list_==0 ||
      is_a()!=vcl_string("vil_image_resource_plugin")) 
  {
    return false;
  }

  for (unsigned int i=0;i<vil_image_resource_plugins_list_->size();i++)
  {
    if (vil_image_resource_plugins_list_->operator[](i)->load_the_image(
        image,path,filetype,colour))
    {
      return true;
    }
  }

  return false;
}

//=======================================================================

void vil_image_resource_plugin::register_plugin(
    vil_image_resource_plugin* plugin)
{
  if (plugin==0 || plugin->is_a()==vcl_string("vil_image_resource_plugin"))
  {
    return;
  }

  if (vil_image_resource_plugins_list_==0)
  {
    vil_image_resource_plugins_list_ =
      new vcl_vector<vil_image_resource_plugin*>();
  }

  vil_image_resource_plugins_list_->push_back(plugin);
}

//=======================================================================

void vil_image_resource_plugin::delete_all_plugins()
{
  if (vil_image_resource_plugins_list_==0) return;
  unsigned int n = vil_image_resource_plugins_list_->size();
  for (unsigned int i=0;i<n;++i)
    delete vil_image_resource_plugins_list_->operator[](i);
  vil_image_resource_plugins_list_->resize(0);

  // Clean up the list itself
  delete vil_image_resource_plugins_list_;
  vil_image_resource_plugins_list_=0;
}

//=======================================================================

bool vil_image_resource_plugin::can_be_loaded(const vcl_string& filename)
{
  if (vil_image_resource_plugins_list_==0 ||
      is_a()!=vcl_string("vil_image_resource_plugin"))
  {
    return false;
  }

  for (unsigned int i=0;i<vil_image_resource_plugins_list_->size();i++)
  {
    if (vil_image_resource_plugins_list_->operator[](i)->can_be_loaded(
        filename))
    {
      return true;
    }
  }
  return false;
}
