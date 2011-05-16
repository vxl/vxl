#include "boxm_scene_base.h"
#include "boxm_scene_parser.h"
#include <vcl_cstdio.h>

bool boxm_scene_base::load_scene(vcl_string filename, boxm_scene_parser& parser)
{
  
  if (filename.size() > 0) {
    vcl_FILE* xmlFile = vcl_fopen(filename.c_str(), "r");
    if (!xmlFile){
    vcl_cerr << filename.c_str() << " error on opening" << vcl_endl;
    return 0;
    }
    if (!parser.parseFile(xmlFile)) {
      vcl_cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
               << parser.XML_GetCurrentLineNumber() << '\n';
      vcl_fclose(xmlFile);
      return false;
    }
    vcl_fclose(xmlFile);
  }
  else
    vcl_cerr << " Filename is empty" << vcl_endl;

  parser.paths(scene_path_, block_pref_);
  app_model_ = boxm_apm_types::str_to_enum(parser.app_model().data());
  multi_bin_ = parser.multi_bin();
  parser.levels(max_tree_level_, init_tree_level_);
  filename_ = filename;
  return true;
}
