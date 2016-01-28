// This is brl/bbas/volm/pro/processes/volm_generate_class_map_process.cxx
#include <bprb/bprb_func_process.h>
// :
// \file
//         Take a color classification map generated by sdet classifier and generate corresponding volm land id map
//
//
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <volm/volm_category_io.h>
#include <vul/vul_file.h>

// :
//  Take a colored segmentation output and map it to volm labels
bool volm_generate_class_map_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // classification image
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // output unsigned short image with volm_land_type ids
  return pro.set_input_types(input_types)
         && pro.set_output_types(output_types);
}

// : Execute the process
bool volm_generate_class_map_process(bprb_func_process& pro)
{
  if( pro.n_inputs() < 1 )
    {
    vcl_cout << "volm_map_osm_process: The number of inputs should be 1" << vcl_endl;
    return false;
    }

  // get the inputs
  vil_image_view_base_sptr           img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view<vil_rgb<vxl_byte> > img(img_sptr);
  vil_image_view<unsigned short>     out_img(img_sptr->ni(), img_sptr->nj(), 1);
  out_img.fill(0);

  vcl_map<vcl_pair<vxl_byte, vcl_pair<vxl_byte, vxl_byte> >, unsigned short> sdet_color_map;
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (52, vcl_pair < vxl_byte, vxl_byte > (226, 127) )] = 15;                                 // volm_category_io::volm_land_table - "building";
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (254, vcl_pair < vxl_byte, vxl_byte > (17, 199) )] = 242;                                 // "palm_tree";
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (185, vcl_pair < vxl_byte, vxl_byte > (242, 86) )] = 0;                                 // "invalid";  // actually park/open in sdet but we're mapping to invalid in volm
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (20, vcl_pair < vxl_byte, vxl_byte > (166, 41) )] = 31;                                 // "road"; // actually parking_lot in sdet
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (191, vcl_pair < vxl_byte, vxl_byte > (184, 98) )] = 31;                                 // "road";
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (225, vcl_pair < vxl_byte, vxl_byte > (36, 147) )] = 0;                                 // "invalid"; // actually shadow in sdet
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (116, vcl_pair < vxl_byte, vxl_byte > (45, 119) )] = 31;                                 // "road"; // actually street in sdet
  sdet_color_map[vcl_pair < vxl_byte, vcl_pair < vxl_byte, vxl_byte > > (41, vcl_pair < vxl_byte, vxl_byte > (234, 166) )] = 243;                                 // "tree";
  for( unsigned i = 0; i < img.ni(); i++ )
    {
    for( unsigned j = 0; j < img.nj(); j++ )
      {
      vcl_pair<vxl_byte, vcl_pair<vxl_byte, vxl_byte> > pp(img(i,
                                                               j).r, vcl_pair<vxl_byte, vxl_byte>(img(
                                                                                                    i,
                                                                                                    j).g, img(
                                                                                                    i, j).b) );
      vcl_map<vcl_pair<vxl_byte, vcl_pair<vxl_byte, vxl_byte> >,
              unsigned short>::iterator iter = sdet_color_map.find(pp);
      if( iter != sdet_color_map.end() )
        {
        out_img(i, j) = iter->second;
        }
      else
        {
        out_img(i, j) = 0;  // map to invalid if undefined
        }
      }
    }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<unsigned short>(out_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return true;
}

// :
// a process generating color class map from volm id class map
bool volm_generate_color_class_map_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // classification image
  input_types.push_back("vcl_string");                // id to color txt
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // output unsigned short image with volm_land_type ids
  return pro.set_input_types(input_types)
         && pro.set_output_types(output_types);
}

bool volm_generate_color_class_map_process(bprb_func_process& pro)
{
  if( pro.n_inputs() < 1 )
    {
    vcl_cout << "volm_map_osm_process: The number of inputs should be 1" << vcl_endl;
    return false;
    }

  // get the inputs
  vil_image_view_base_sptr           img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vcl_string                         id_to_color_txt = pro.get_input<vcl_string>(1);
  vil_image_view<unsigned char>      img(img_sptr);
  vil_image_view<vil_rgb<vxl_byte> > out_img(img_sptr->ni(), img_sptr->nj(), 1);
  out_img.fill(vil_rgb<vxl_byte>(0, 0, 0) );

  vcl_map<unsigned char, vil_rgb<vxl_byte> > sdet_color_map;
  if( !vul_file::exists(id_to_color_txt) )
    {
    for( vcl_map<unsigned int, volm_land_layer>::iterator mit = volm_osm_category_io::volm_land_table.begin();
         mit != volm_osm_category_io::volm_land_table.end();  ++mit )
      {
      sdet_color_map.insert(vcl_pair<unsigned char, vil_rgb<vxl_byte> >(mit->second.id_, mit->second.color_) );
      }
    }
  else
    {
    vcl_ifstream ifs(id_to_color_txt.c_str() );
    vcl_string   cat_name; int id; int r, g, b;
    ifs >> cat_name;
    while( !ifs.eof() )
      {
      ifs >> id;  ifs >> r;  ifs >> g;  ifs >> b;
      sdet_color_map.insert(vcl_pair<unsigned char, vil_rgb<vxl_byte> >( (unsigned char)id,
                                                                         vil_rgb<vxl_byte>(r, g, b) ) );
      vcl_cout << "\t\t" << cat_name << " color: " << sdet_color_map[id] << '\n';
      ifs >> cat_name;
      }
    }
  for( unsigned i = 0; i < img.ni(); i++ )
    {
    for( unsigned j = 0; j < img.nj(); j++ )
      {
      vcl_map<unsigned char, vil_rgb<vxl_byte> >::iterator mit = sdet_color_map.find(img(i, j) );
      if( mit != sdet_color_map.end() )
        {
        out_img(i, j) = mit->second;
        }
      }
    }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return true;
}

// :
// a process that uses a source class map to update current class map.
// Note that the new source map will overwrite current class map
bool volm_update_class_map_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types_;
  input_types_.push_back("vil_image_view_base_sptr");  // current classification map
  input_types_.push_back("vil_image_view_base_sptr");  // source classification map used to update the class map
  vcl_vector<vcl_string> output_types_;
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_update_class_map_process(bprb_func_process& pro)
{
  if( pro.n_inputs() < 2 )
    {
    vcl_cout << pro.name() << ": The number of inputs should be 2" << vcl_endl;
    return false;
    }

  // get the inputs
  vil_image_view_base_sptr curr_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr srce_img_sptr = pro.get_input<vil_image_view_base_sptr>(1);

  vil_image_view<unsigned char>* curr_img = dynamic_cast<vil_image_view<unsigned char> *>(curr_img_sptr.ptr() );
  vil_image_view<unsigned char>* srce_img = dynamic_cast<vil_image_view<unsigned char> *>(srce_img_sptr.ptr() );
  unsigned                       ni = curr_img->ni();
  unsigned                       nj = curr_img->nj();
  if( srce_img->ni() != ni || srce_img->nj() != nj )
    {
    vcl_cout << pro.name() << ": The class map and source class map have difference in size (" << ni << 'x' << nj
             << "), and (" << srce_img->ni() << 'x' << srce_img->nj() << ')' << vcl_endl;
    return false;
    }
  for( unsigned i = 0; i < ni; i++ )
    {
    for( unsigned j = 0; j < nj; j++ )
      {
      if( (*srce_img)(i, j) == volm_osm_category_io::volm_land_table_name["invalid"].id_ )
        {
        continue;
        }
      // overwrite the class map with new source
      (*curr_img)(i, j) = (*srce_img)(i, j);
      }
    }
  return true;
}
