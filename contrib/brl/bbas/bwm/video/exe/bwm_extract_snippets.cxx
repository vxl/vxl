#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>

static vcl_string image_sequence_path(vcl_string const& dir_path,
                                      vcl_string const& sname,
                                      unsigned index,
                                      vcl_string const& format)
{
  vcl_stringstream str;
  str << vcl_setw(5) << vcl_setfill('0') << index;
  vcl_string path = dir_path + "\\" + sname +str.str();
  vcl_string save_path = path + '.' + format;
  return save_path;
}

static bool extract_snippets(vcl_string const& spatial_object_path,
                             vcl_string const& image_path,
                             vcl_string const& snippet_output_dir)
{
  if (spatial_object_path == ""||
      image_path == ""||
      snippet_output_dir == "")
    return false;

  vsl_b_ifstream istr(spatial_object_path);
  if (!istr) {
    vcl_cerr << "Failed to open spatial object input stream "
             << spatial_object_path << vcl_endl;
    return false;
  }
  vcl_vector<vsol_spatial_object_2d_sptr> sos;

  vsl_b_read(istr, sos);

  if (!sos.size())
  {
    vcl_cerr << "no spatial objects in file "
             << spatial_object_path << vcl_endl;
    return false;
  }

  vil_image_resource_sptr image = vil_load_image_resource(image_path.c_str());
  if (!image)
  {
    vcl_cerr << "image failed to load "
             << image_path << vcl_endl;
    return false;
  }

  if (!vul_file::exists(snippet_output_dir))
    if (!vul_file::make_directory_path(snippet_output_dir))
      return false;

  unsigned ni = 0, nj = 0;
  for (vcl_vector<vsol_spatial_object_2d_sptr>::iterator soit = sos.begin();
       soit != sos.end(); ++soit)
  {
    vsol_spatial_object_2d_sptr so = (*soit);
    if (!so)
      continue;
    vsol_box_2d_sptr bb = so->get_bounding_box();
    if (!bb)
      continue;
    double w = bb->width(), h = bb->height();
    unsigned iw = static_cast<unsigned>(w), ih = static_cast<unsigned>(h);
    if (iw>ni) ni = iw;
    if (ih>nj) nj = ih;
  }
  if (!ni || !nj)
  {
    vcl_cerr << "degenerate snippet\n";
    return false;
  }

  unsigned index = 0;
  for (vcl_vector<vsol_spatial_object_2d_sptr>::iterator soit = sos.begin();
       soit != sos.end(); ++soit, ++index)
  {
    vsol_spatial_object_2d_sptr so = (*soit);
    if (!so)
      continue;
    vsol_box_2d_sptr bb = so->get_bounding_box();
    if (!bb)
      continue;
    double xmin = bb->get_min_x(), xmax = bb->get_max_x();
    double ymin = bb->get_min_y(), ymax = bb->get_max_y();
    double cx = (xmax+xmin)/2, cy = (ymax+ymin)/2;
    unsigned i0 = static_cast<unsigned>(cx-ni/2);
    unsigned j0 = static_cast<unsigned>(cy-nj/2);
    vil_image_view_base_sptr view = image->get_view(i0, ni, j0, nj);
    // create the output snippet file path
    vcl_string path = image_sequence_path(snippet_output_dir,
                                          "snippet", index,
                                          "tiff");
    if (!vil_save(*view, path.c_str()))
    {
      vcl_cerr << "snippet not saved to " << path << vcl_endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  vul_arg_info_list arglist;
  vul_arg<vcl_string> spatial_object_path(arglist, "-spatial_object_path",
                                          "spatial object path", "");
  vul_arg<vcl_string> image_path(arglist, "-image_path", "image path", "");
  vul_arg<vcl_string> snippet_output_dir(arglist, "-snippet_output_dir",
                                         "snippet output file directory", "");
  arglist.parse(argc, argv, true);

  if (!extract_snippets(spatial_object_path(), image_path(),
                        snippet_output_dir()))
    return -1;
  return 0;
}
