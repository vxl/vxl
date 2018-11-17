#include "boxm2_multi_util.h"

//pick out data type
bool boxm2_multi_util::get_scene_appearances(const boxm2_scene_sptr&    scene,
                                             std::string&         data_type,
                                             std::string&         options,
                                             int&                apptypesize)
{
  bool foundDataType = false;
  std::vector<std::string> apps = scene->appearances();
  apptypesize = 0;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();
    }
  }
  if (!foundDataType) {
    std::cout<<"boxm2_multi_util ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  return true;
}
