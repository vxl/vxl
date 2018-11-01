// This is core/vil/vil_image_resource_plugin.cxx
//:
// \file
// \brief Interface for loading new image formats
// This class provides an interface for loading images in new formats
// \author      Franck Bettinger
// \date        Sun Mar 17 22:57:00 2002

#include <vector>
#include "vil_image_resource_plugin.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

static std::vector<vil_image_resource_plugin*>
    *vil_image_resource_plugins_list_ = nullptr;

//=======================================================================

bool vil_image_resource_plugin::load_the_image (
    vil_image_view_base_sptr& image,
    const std::string & path, const std::string & filetype,
    const std::string & colour)
{
  if (vil_image_resource_plugins_list_==nullptr ||
      is_a()!=std::string("vil_image_resource_plugin"))
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
  if (plugin==nullptr || plugin->is_a()==std::string("vil_image_resource_plugin"))
  {
    return;
  }

  if (vil_image_resource_plugins_list_==nullptr)
  {
    vil_image_resource_plugins_list_ =
      new std::vector<vil_image_resource_plugin*>();
  }

  vil_image_resource_plugins_list_->push_back(plugin);
}

//=======================================================================

void vil_image_resource_plugin::delete_all_plugins()
{
  if (vil_image_resource_plugins_list_==nullptr) return;
  auto n = (unsigned int)(vil_image_resource_plugins_list_->size());
  for (unsigned int i=0;i<n;++i)
    delete vil_image_resource_plugins_list_->operator[](i);
  vil_image_resource_plugins_list_->resize(0);

  // Clean up the list itself
  delete vil_image_resource_plugins_list_;
  vil_image_resource_plugins_list_=nullptr;
}

//=======================================================================

bool vil_image_resource_plugin::can_be_loaded(const std::string& filename)
{
  if (vil_image_resource_plugins_list_==nullptr ||
      is_a()!=std::string("vil_image_resource_plugin"))
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
