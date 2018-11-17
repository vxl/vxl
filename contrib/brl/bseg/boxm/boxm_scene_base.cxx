#include <iostream>
#include <cstdio>
#include "boxm_scene_base.h"
#include "boxm_scene_parser.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool boxm_scene_base::load_scene(const std::string& filename, boxm_scene_parser& parser)
{

  if (filename.size() > 0) {
    std::FILE* xmlFile = std::fopen(filename.c_str(), "r");
    if (!xmlFile){
    std::cerr << filename.c_str() << " error on opening" << std::endl;
    return false;
    }
    if (!parser.parseFile(xmlFile)) {
      std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
               << parser.XML_GetCurrentLineNumber() << '\n';
      std::fclose(xmlFile);
      return false;
    }
    std::fclose(xmlFile);
  }
  else
    std::cerr << " Filename is empty" << std::endl;

  parser.paths(scene_path_, block_pref_);
  app_model_ = boxm_apm_types::str_to_enum(parser.app_model().data());
  multi_bin_ = parser.multi_bin();
  parser.levels(max_tree_level_, init_tree_level_);
  filename_ = filename;
  return true;
}
