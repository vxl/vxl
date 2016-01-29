#include "bwm_utils.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil/vil_config.h>

#include <vgui/vgui_dialog.h>
#include <bgui/bgui_image_utils.h>

#include <vil/vil_load.h>
#include <vil/vil_property.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/file_formats/vil_nitf2_image.h>
#if HAS_J2K
#  include <vil/file_formats/vil_j2k_nitf2_pyramid_image_resource.h>
#  include <vil/file_formats/vil_j2k_pyramid_image_resource.h>
#endif // HAS_J2K

#include <vul/vul_file.h>
#if 0 // ony used inside commented-out section
#  include <vgl/vgl_point_3d.h>
#endif

vcl_string bwm_utils::select_file()
{
  vgui_dialog params("File Open");
  vcl_string  ext, file, empty = "";

  params.file("Open...", ext, file);
  if( !params.ask() )
    {
    return empty;
    }

  if( file == "" )
    {
    vgui_dialog error("Error");
    error.message("Please specify an input file (prefix)." );
    error.ask();
    return empty;
    }
  return file;
}

void bwm_utils::load_from_txt(vcl_string filename,
                              vcl_string & tab_type,
                              vcl_string & tab_name,
                              vcl_string & img_path,
                              vcl_string & cam_path,
                              int & cam_type)
{
  if( filename.empty() )
    {
    vcl_cout << "Bad filename\n";
    return;
    }

  vcl_ifstream is(filename.data() );
  if( !is.is_open() )
    {
    vcl_cout << "Can't open file\n";
    return;
    }

  vcl_string type, str;
  while( !is.eof() )
    {
    is >> type;

    // IMAGE TABLEAU
    if( type == "IMAGE_TAB:" )
      {
      is >> tab_name;
      is >> str;
      if( str != "IMAGE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> img_path;
      // bwm_tableau_mgr::instance()->create_img_tableau(name, image_path);
      }

    // CAMERA TABLEAU
    if( type == "CAM_TAB:" )
      {
      is >> tab_name >> str;
      if( str != "IMAGE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> img_path;
      is >> str;
      if( str != "CAMERA_TYPE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }
      vcl_string camera_type;
      is >> camera_type;
      is >> str;
      if( str != "CAMERA_PATH:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> cam_path;
#if 0
      int cam_type;
      if( camera_type == "projective" )
        {
        cam_type = 0;
        }
      else if( camera_type == "rational" )
        {
        cam_type = 1;
        }
      else
        {
        cam_type = -1;
        }
      create_cam_tableau(name, image_path, camera_path, cam_type);
#endif
      }
    // COIN3D Tableau
    else if( type == "COIN3D_TAB:" )
      {
      is >> tab_name;
      is >> str;
      if( str != "CAMERA_TYPE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }
      vcl_string camera_type;
      is >> camera_type;
      is >> cam_type;
      is >> str;
      if( str != "CAMERA_PATH:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> cam_path;
      if( camera_type == "projective" )
        {
        cam_type = 0;
        }
      if( camera_type == "rational" )
        {
        cam_type = 1;
        }
      // create_coin3d_tableau(name, camera_path, cam_type);

      // PROJ2D TABLEAU
      }
    else if( type == "PROJ2D_TAB:" )
      {
      is >> tab_name;
      is >> str;
      if( str != "TYPE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      vcl_string type;
      is >> type;

      vcl_string coin3d_name = "";
      if( type == "simple" )
        {
        is >> str;
        if( str != "COIN3D:" )
          {
          vcl_cout << "Bad file parse\n";
          return;
          }
        is >> coin3d_name;
        }

      is >> str;
      if( str != "IMAGE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> img_path;
      is >> str;
      if( str != "CAMERA_TYPE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      vcl_string camera_type;
      is >> camera_type;
      is >> str;
      if( str != "CAMERA_PATH:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }

      is >> cam_path;
      if( camera_type == "projective" )
        {
        cam_type = 0;
        }
      if( camera_type == "rational" )
        {
        cam_type = 1;
        }
      // create_proj2d_tableau(name, type, coin3d_name, image_path, camera_path, cam_type);
      }
    else if( type == "LIDAR_TAB:" )
      {
      is >> tab_name;
      is >> str;
      if( str != "FIRST RESPONSE IMAGE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }
      vcl_string first_ret = str;
      is >> str;
      if( str != "SECOND RESPONSE IMAGE:" )
        {
        vcl_cout << "Bad file parse\n";
        return;
        }
      vcl_string second_ret = str;
      is >> img_path;
      // create_lidar_tableau(name, first_ret, second_ret);
      }
#if 0
    else if( type == "CORRESPONDENCES:" )
      {
      int num, c_num;
      is >> c_num;

      vcl_string mode;
      is >> str;
      if( str != "CORR_MODE:" )
        {
        vcl_cerr << "Correspondence mode is missing\n";
        return;
        }
      is >> mode;
      if( (mode != "IMAGE_TO_IMAGE") && (mode != "WORLD_TO_IMAGE") )
        {
        vcl_cerr << "Invalid correspondence mode\n";
        return;
        }
      // read the correspondences
      for( int i = 0; i < c_num; i++ )
        {
        is >> str;
        if( str != "C:" )
          {
          vcl_cerr << "Invalid correspondence format\n";
          }
        else
          {
          is >> num;
          bwm_corr_sptr corr = new bwm_corr();
          if( mode == "WORLD_TO_IMAGE" )
            {
            is >> str;

            if( str != "WORLD_POINT:" )
              {
              vcl_cerr << "wrong corr format\n";
              return;
              }

            double wx, wy, wz;
            is >> wx;
            is >> wy;
            is >> wz;
            corr->set_mode(false);
            corr->set_world_pt(vgl_point_3d<double>(wx, wy, wz) );
            }
          vcl_string tab_name;
          double     X, Y;
          for( int j = 0; j < num; j++ )
            {
            is >> tab_name;
            is >> X;
            is >> Y;
            vgui_tableau_sptr tab = this->find_tableau(tab_name);
            if( tab )
              {
              if( (tab->type_name().compare("bwm_tableau_proj_cam") == 0)
                  || (tab->type_name().compare("bwm_tableau_rat_cam") == 0) )
                {
                bwm_tableau_cam*  tab_cam = static_cast<bwm_tableau_cam *>(tab.as_pointer() );
                bwm_observer_cam* obs = tab_cam->observer();
                if( obs )
                  {
                  corr->set_match(obs, X, Y);
                  obs->add_cross(X, Y, 3);
                  }
                }
              }
            }
          bwm_observer_mgr::instance()->set_corr(corr);
          }
        }
      }
#endif // 0
    else if( type == "END" )
      {
      return;
      }
    }
}

vil_image_resource_sptr
bwm_utils::load_image(vcl_string& filename, vgui_range_map_params_sptr& rmps)
{
  vil_image_resource_sptr res;

  // if filename is a directory, assume pyramid image
  if( vul_file::is_directory(filename) )
    {
    // if filename is a directory, assume pyramid image  if (vul_file::is_directory(filename)) {
    vil_pyramid_image_resource_sptr pyr = vil_load_pyramid_resource(filename.c_str() );

    if( pyr )
      {
      res = pyr.ptr();
      }
    else
      {
      vcl_cerr << "error loading image pyramid " << filename << '\n';
      return 0;
      }
    }
  else
    {
    res = vil_load_image_resource(filename.c_str() );
#if HAS_J2K
    // determine if the image can be made into a J2K-nitf pyramid
    char const* fmtp = res->file_format();
    vcl_string  file_fmt = "";
    if( fmtp )
      {
      file_fmt = fmtp;        // fmtp can be 0 for undefined formats
      }
    if( file_fmt == "nitf21" )
      {
      vil_nitf2_image* nitf_resc = static_cast<vil_nitf2_image *>(res.ptr() );
      if( nitf_resc->is_jpeg_2000_compressed() )
        {
        vil_j2k_nitf2_pyramid_image_resource* j2k_nitf =
          new vil_j2k_nitf2_pyramid_image_resource(res);
        res = j2k_nitf;
        }
      }
    else if( file_fmt == "j2k" )
      {
      vil_j2k_pyramid_image_resource* j2k_pyr =
        new vil_j2k_pyramid_image_resource(res);
      res = j2k_pyr;
      }
#endif // HAS_J2K
    }
  if( !res ) {return 0; }
  float gamma = 1.0f;
  bool  invert = false;
  bool  gl_map = false;
  bool  cache = true;
  bool  is_pyr = res->get_property(vil_property_pyramid, 0);
  if( is_pyr )
      { gl_map = true; cache = true; }

  unsigned ni = res->ni(), nj = res->nj();
  unsigned area = ni * nj;
  if( area > 2500000 ) {gl_map = true; }

  bgui_image_utils biu(res);

  if( biu.range_map_from_hist(gamma, invert, gl_map, cache, rmps) )
    {
    return res;
    }
  if( biu.default_range_map(rmps, gamma, invert, gl_map, cache) )
    {
    return res;
    }
  return 0;
}

void bwm_utils::show_error(vcl_string msg)
{
  vgui_dialog err("ERROR occurred");

  err.message(msg.c_str() );
  err.ask();
}
