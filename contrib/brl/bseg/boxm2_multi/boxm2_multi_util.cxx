#include "boxm2_multi_util.h"

//pick out data type
bool boxm2_multi_util::get_scene_appearances(boxm2_scene_sptr    scene,
                                             vcl_string&         data_type, 
                                             vcl_string&         options, 
                                             int&                apptypesize)
{
  bool foundDataType = false;
  vcl_vector<vcl_string> apps = scene->appearances();
  apptypesize = 0;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();
    }
  }
  if (!foundDataType) {
    vcl_cout<<"boxm2_multi_util ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  return true; 
}
