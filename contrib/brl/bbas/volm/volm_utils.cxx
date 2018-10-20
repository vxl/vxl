#include <iostream>
#include "volm_utils.h"
#include <cstdlib>
#include <vcl_compiler.h>
#include <vcl_where_root_dir.h>

//returns path to opencl src (cl files)
std::string volm_utils::volm_src_root()
{
    std::string volm_src_dir;
    char* volm_dir_env = getenv("VOLM_DIR");
    if (volm_dir_env != nullptr) {
      volm_src_dir = std::string(volm_dir_env) + "/";
    }
//    std::cout<<"VOLM dir "<<volm_src_dir<<std::endl;
//    if ( volm_src_dir == "" )
//      volm_src_dir = VOLM_SOURCE_DIR ;
    if ( volm_src_dir == "" )
      volm_src_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/";
    return volm_src_dir;
}
