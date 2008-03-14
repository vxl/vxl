#ifndef _bvam_image_schedule_cxx_
#define _bvam_image_schedule_cxx_

#include <vcl_iostream.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_awk.h>
#include <vil/vil_load.h>
#include "bvam_image_schedule.h"



//---------------------------------------------------
bvam_image_schedule::bvam_image_schedule()
{

};


//---------------------------------------------------
void 
bvam_image_schedule::push_back( 
  const bvam_image_metadata& new_img )
{ 
  list_.push_back( new_img );
  schedule_.push_back( schedule_.size() );
}

//---------------------------------------------------
void 
bvam_image_schedule::load(
  vcl_string image_dir,
  vcl_string camera_file,
  vcl_string light_file,
  bool read_date )
{
  // Read all images from the directory.
  vcl_vector< vil_image_view_base_sptr > image_list;
  vcl_vector<double> date_list;
  vcl_vector<vcl_string> name_list;
  if( image_dir != "NONE" ){
    image_dir += "/*.*";
    for( vul_file_iterator fit = image_dir; fit; ++fit ){
      if( vul_file::is_directory(fit()) )
        continue;
      vcl_string image_name = fit();
      vcl_string extension = vul_file::extension( image_name ); 
      if( ( extension != ".jpg" ) &&
          ( extension != ".png" ) &&
          ( extension != ".tif" ) &&
          ( extension != ".JPG" ) &&
          ( extension != ".PNG" ) &&
          ( extension != ".TIF" ) ){
        continue;
      }
      image_list.push_back( vil_load( image_name.c_str() ) );
      name_list.push_back( vul_file::strip_directory( vul_file::strip_extension( image_name ) ) );
      if( read_date )
        date_list.push_back( get_date( image_name ) );
      else
        date_list.push_back(-1.0);
    }
  }

  // Read the cameras.
  vcl_vector< vpgl_proj_camera<double> > camera_list;
  if( camera_file != "NONE" ){
    vcl_ifstream camera_stream( camera_file.c_str() );
    char line_buffer[256];

    if( !(camera_stream.good()) ){
      return;
    }
    while( camera_stream.eof() == 0 ){
      char nc = camera_stream.peek();
      if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
          nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
        vnl_matrix_fixed<double,3,4> new_camera_matrix;
        camera_stream >> new_camera_matrix;
        camera_list.push_back( vpgl_proj_camera<double>( new_camera_matrix) );
      }
      else
        camera_stream.getline(line_buffer,256);
    }
  }

  // Read the lights.
  vcl_vector< vnl_vector<double> > light_list;
  if( light_file != "NONE" ){
    vcl_ifstream light_stream( light_file.c_str() );
    for( vul_awk lawk( light_stream ); lawk; ++lawk ){
      if( lawk.NF() == 0 ) continue;
      vnl_vector<double> new_light( lawk.NF() );
      for( int i = 0; i < lawk.NF(); i++ )
        new_light(i) = (double)atof( lawk[i] );
      light_list.push_back( new_light );
    }
  }

  // Check that all images/camera/lights have same size.
  unsigned max_size = image_list.size();
  if( camera_list.size() > max_size ) max_size = camera_list.size();
  if( light_list.size() > max_size ) max_size = light_list.size();

  for( unsigned i = 0; i < max_size; i++ ){
    list_.push_back( bvam_image_metadata() );
    if( i < image_list.size() ){
      list_[list_.size()-1].img = image_list[i];
      list_[list_.size()-1].name = name_list[i];
      list_[list_.size()-1].timestamp = date_list[i];
    }
    if( i < camera_list.size() )
      list_[list_.size()-1].camera = &(camera_list[i]);
    if( i < light_list.size() )
      list_[list_.size()-1].light = light_list[i];
  }
    
  schedule_all();
};


//---------------------------------------------------
void 
bvam_image_schedule::schedule_all()
{
  schedule_.clear();
  for( unsigned i = 0; i < list_.size(); i++ )
    schedule_.push_back( i );
};


//---------------------------------------------------
void 
bvam_image_schedule::order_by_date()
{
  schedule_.clear();
  vcl_vector<bool> added;
  for( unsigned i = 0; i < list_.size(); i++ ) added[i] = false;

  // Brute force n^2 sorting.
  double smallest_date;
  unsigned smallest_index;
  for( unsigned i = 0; i < list_.size(); i++ ){
    smallest_date = 1.0;
    for( unsigned j = 0; j < list_.size(); j++ ){
      if( list_[j].timestamp < smallest_date && !added[j] ){ 
        smallest_date = list_[j].timestamp; smallest_index = j; 
      }
    }
    schedule_.push_back( smallest_index );
    added[smallest_index] = true;
  }

};


//---------------------------------------------------
void 
bvam_image_schedule::schedule_subset(
  unsigned start_index,
  unsigned end_index,
  unsigned inc )
{
  vcl_vector<unsigned> new_schedule;
  for( unsigned i = start_index; i < end_index; i += inc ){
    new_schedule.push_back( schedule_[i] );
  }
  schedule_ = new_schedule;
};


//---------------------------------------------------
void 
bvam_image_schedule::shuffle()
{
  vcl_cerr << "SHUFFLE: NOT YET IMPLEMENTED";
};


//---------------------------------------------------
double 
bvam_image_schedule::get_date( 
  vcl_string file_name )
{
  vcl_string f = vul_file::strip_directory( file_name );
  vcl_string year; year += f[0]; year += f[1];
  vcl_string month; month += f[2]; month += f[3]; month += f[4]; 
  vcl_string day; day += f[5]; day += f[6];
  double date = atoi(year.c_str())/100.0;
  if( month == "JAN" ) date += .0001;
  else if( month == "FEB" ) date += .0002;
  else if( month == "MAR" ) date += .0003;
  else if( month == "APR" ) date += .0004;
  else if( month == "MAY" ) date += .0005;
  else if( month == "JUN" ) date += .0006;
  else if( month == "JUL" ) date += .0007;
  else if( month == "AUG" ) date += .0008;
  else if( month == "SEP" ) date += .0009;
  else if( month == "OCT" ) date += .0010;
  else if( month == "NOV" ) date += .0011;
  else if( month == "DEC" ) date += .0012;
  else vcl_cerr << "ERROR: UNKNOWN DATE: " << month << '\n';
  date += atoi(day.c_str())/1000000.0;
  return date;
};

#endif // _bvam_image_schedule_cxx_
