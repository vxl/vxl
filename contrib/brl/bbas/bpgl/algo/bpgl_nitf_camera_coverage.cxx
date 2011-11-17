#include "bpgl_nitf_camera_coverage.h"

#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vul/vul_awk.h>

#include <vgl/vgl_polygon.h>

bool bpgl_nitf_camera_coverage::coverage_list(vcl_vector<vgl_point_2d<double> > geo_pts,
                                              vcl_string img_list,
                                              vcl_string img_coverage_list)
{
  vcl_ifstream ifs( img_list.c_str() );
  vcl_ofstream ofs( img_coverage_list.c_str() );

  if (!ifs)
  {
    vcl_cerr << "Error in bpgl_nitf_camera_coverage::coverage_list: Failed to open image list file\n";
    return false;
  }

  if (!ofs)
  {
    vcl_cerr << "Error in bpgl_nitf_camera_coverage::coverage_list: Failed to create output file\n";
    return false;
  }

  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    vcl_string img_file = awk.line();

    //load rational camera from image file
    vpgl_nitf_rational_camera *nitf_cam = new vpgl_nitf_rational_camera(img_file);

    if (!nitf_cam)
    {
      vcl_cerr << "Error in bpgl_nitf_camera_coverage::coverage_list: Failed to load NITF camera\n";
      return false;
    }

    //create a the coverage polygon-only one sheet.
    vgl_polygon<double> poly_region(1);
    poly_region.push_back(nitf_cam->upper_right()[0],nitf_cam->upper_right()[1]);
    poly_region.push_back(nitf_cam->upper_left()[0],nitf_cam->upper_left()[1]);
    poly_region.push_back(nitf_cam->lower_left()[0],nitf_cam->lower_left()[1]);
    poly_region.push_back(nitf_cam->lower_right()[0],nitf_cam->lower_right()[1]);

    bool contain = true;

    for ( unsigned i = 0; i<geo_pts.size(); i++)
    {
      contain = contain && poly_region.contains( vgl_point_2d<double>(
        geo_pts[i].x(), geo_pts[i].y() ) );
    }

    //If all the points aren't contained within the image region, continue to next image
    //otherwise add th image to the list file
    if (!contain) continue;

    ofs << img_file << '\n';
  }

  return true;
}

//Not implemented yet
bool bpgl_nitf_camera_coverage::compute_coverage_region(vcl_string /*camera_list*/, vcl_string /*out_imfile*/)
{
  return true;
}
