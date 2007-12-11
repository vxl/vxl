//:
// \file
// \author Tim Cootes
// \brief Program using F&H method to locate matches on a pair of images given tree on one

// Example parameter file:
#if 0
  image1_path: HandImages/Hand_1.jpg
  image2_path: HandImages/Hand_9.jpg
  points_path: hand_b.pts
  arcs_path: hand_b.arcs
  output_image1_path: output1.png
  output_image2_path: output_b9.png
  L_lo: 2
  L_hi: 2
  half_width: 7
  response_scale: 0.5
#endif // 0

#include <vul/vul_arg.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_crop.h>
//#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vil/vil_load.h>
#include <vul/vul_string.h>
#include <vcl_algorithm.h>
#include <vil/vil_save.h>
#include <vil/vil_fill.h>
#include <vil/vil_crop.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_math.h>
#include <fhs/fhs_searcher.h>
#include <vimt/algo/vimt_normalised_correlation_2d.h>
#include <mbl/mbl_draw_line.h>

void print_usage()
{
  vcl_cout<<"fhs_match_tree_model -p param_file\n"
          <<"Loads in parameter file, which defines images,\n"
          <<"points, the arcs defining a tree and related details.\n"
          <<"Constructs a tree model from the first image.\n"
          <<"Uses normalised correllation and this model to locate\n"
          <<"equivalent points on the second image."<<vcl_endl;
  vul_arg_display_usage_and_exit();
}

class fhs_model_params
{
 public:
  vcl_string image1_path;
  vcl_string image2_path;
  vcl_string points_path;
  vcl_string arcs_path;
  vcl_string output_image1_path;
  vcl_string output_image2_path;
  int L_lo,L_hi;
  int half_width;
  double response_scale;
};

bool parse_param_file(const vcl_string& param_path,
                      fhs_model_params& params)
{
  // ---------------------------------------------------------------
  // Load the parameters
  // ---------------------------------------------------------------
  vcl_ifstream ifs(param_path.c_str());
  if (!ifs)
  {
    vcl_cerr<<"Failed to open parameter file: "<<param_path<<vcl_endl;
    return false;
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  if (props.find("image1_path")!=props.end())
    params.image1_path = props["image1_path"];
  else {vcl_cerr<<"No image1_path: specified.\n"; return false; }

  if (props.find("image2_path")!=props.end())
    params.image2_path = props["image2_path"];
  else {vcl_cerr<<"No image2_path: specified.\n"; return false; }

  if (props.find("output_image1_path")!=props.end())
    params.output_image1_path = props["output_image1_path"];
  else {vcl_cerr<<"No output_image1_path: specified.\n"; return false; }

  if (props.find("output_image2_path")!=props.end())
    params.output_image2_path = props["output_image2_path"];
  else {vcl_cerr<<"No output_image2_path: specified.\n"; return false; }

  if (props.find("points_path")!=props.end())
    params.points_path = props["points_path"];
  else {vcl_cerr<<"No points_path: specified.\n"; return false; }

  if (props.find("arcs_path")!=props.end())
    params.arcs_path = props["arcs_path"];
  else {vcl_cerr<<"No arcs2_path: specified.\n"; return false; }

  if (props.find("L_lo")!=props.end())
    params.L_lo = vul_string_atoi(props["L_lo"]);
  else {vcl_cerr<<"No L_lo: specified.\n"; return false; }

  if (props.find("half_width")!=props.end())
    params.half_width = vul_string_atoi(props["half_width"]);
  else {vcl_cerr<<"No half_width: specified.\n"; return false; }

  if (props.find("L_hi")!=props.end())
    params.L_hi = vul_string_atoi(props["L_hi"]);
  else {vcl_cerr<<"No L_hi: specified.\n"; return false; }

  params.response_scale=0.1;
  if (props.find("response_scale")!=props.end())
    params.response_scale = vul_string_atof(props["response_scale"]);

  return true;
}

//: Load in points in ISBE points format
//  eg:
//  n_points: 12
//  {
//    1 2
//    3 4
//    ....
//  }
bool fhs_load_points(const vcl_string& path,
                     vcl_vector<vgl_point_2d<double> >& pts)
{
  vcl_ifstream ifs( path.c_str(), vcl_ios_in );
  if (!ifs)
    { vcl_cerr<<"Failed to load in points from "<<path<<vcl_endl; return false; }
  vcl_string label;

    // Possible extra data - ignore it
  int image_nx, image_ny, image_nz;
  int n = -1;

  ifs >> label;
  while (!ifs.eof()  && label != "}")
  {
    if (label.size()>=2 && label[0]=='/' && label[1]=='/')
    {
      // Comment: Read to the end of the line
      char buf[256];
      ifs.get(buf,256);
    }
    else
    if (label=="version:")
      ifs>>label;
    else if (label=="image_size_x:")
      ifs>>image_nx;
    else if (label=="image_size_y:")
      ifs>>image_ny;
    else if (label=="image_size_z:")
      ifs>>image_nz;
    else if (label=="n_points:")
      ifs>>n;
    else if (label=="{")
    {
      pts.resize(n);
      for (unsigned i=0;i<n;++i)
      {
        double x,y;
        ifs>>x>>y;
        pts[i]=vgl_point_2d<double>(x,y);
      }
      ifs>>label;
      if (label!="}")
      {
        vcl_cerr<<"Expecting }, got "<<label<<vcl_endl;
        return false;
      }
    }
    else
    {
      vcl_cerr<<"Unexpected label: <"<<label<<">\n";
      return false;
    }
    ifs>>label;
  }
  return true;
}

//: Load in pairs of indices indicating connections between points
//  File format: Each line contains two integers
bool fhs_load_arcs(const vcl_string& path,
                   vcl_vector<vcl_pair<int,int> >& pairs)
{
  vcl_vector<int> v;
    vcl_ifstream ifs( path.c_str(), vcl_ios_in );
  if (!ifs)
    { vcl_cerr<<"Failed to load in arc links from "<<path<<vcl_endl; return false; }

  int x;
  ifs>>vcl_ws;
  while (!ifs.eof())
  {
    ifs>>x>>vcl_ws;
    v.push_back(x);
  }

  if (v.size()%2==1)
  {
    vcl_cerr<<"Odd number of indices supplied in "<<path<<vcl_endl;
    return false;
  }

  pairs.resize(v.size()/2);
  for (unsigned i=0;i<pairs.size();++i)
    pairs[i] = vcl_pair<int,int>(v[2*i],v[2*i+1]);
  return true;
}

//: Sample normalised patches around each point
//  Extract patches with given half-width around each pts[i]
//  When near an image edge, patch may be truncated
//  ref_pts defines position of original point relative to the patch origin
void extract_normalised_patches(const vimt_image& image,
                                const vcl_vector<vgl_point_2d<double> >& pts,
                                int half_width,
                                vcl_vector<vil_image_view<float> >& patch,
                                vcl_vector<vgl_point_2d<double> >& ref_pts)
{
  const vimt_image_2d_of<vxl_byte>& byte_im =
               static_cast<const vimt_image_2d_of<vxl_byte>&>(image);
  int ni = byte_im.image().ni();
  int nj = byte_im.image().nj();
  unsigned n=pts.size();
  for (unsigned i=0;i<n;++i)
  {
    vgl_point_2d<double> im_p = byte_im.world2im()(pts[i]);
    int px = vnl_math_rnd(im_p.x()+0.5);
    int py = vnl_math_rnd(im_p.y()+0.5);

    // Select region around point, allowing for image edges.
    int ilo = vcl_max(0,px-half_width);
    int ihi = vcl_min(ni-1,px+half_width);
    int jlo = vcl_max(0,py-half_width);
    int jhi = vcl_min(nj-1,py+half_width);

    // Compute position of reference point relative to corner
    int kx = px-ilo;
    int ky = py-jlo;
    ref_pts.push_back(vgl_point_2d<double>(kx,ky));
    vil_image_view<float> patch1;
    vil_convert_cast(vil_crop(byte_im.image(),ilo,1+ihi-ilo, jlo,1+jhi-jlo),
                     patch1);
    vil_math_normalise(patch1);
    patch.push_back(patch1);
  }
}

void draw_tree(vil_image_view<vxl_byte>& image,
               const vcl_vector<vgl_point_2d<double> >& pts,
               const vcl_vector<vcl_pair<int,int> >& pairs)
{
  // Draw tree into image for display purposes
  for (unsigned i=0;i<pairs.size();++i)
    mbl_draw_line(image,
                  pts[pairs[i].first],
                  pts[pairs[i].second],vxl_byte(255));

  // Write position of selected points into the original image
  // for display purposes.
  for (unsigned i=0;i<pts.size();++i)
  {
    vil_fill_disk(image,pts[i].x(),pts[i].y(),4,vxl_byte(255));
  }
}

//: Generate a new image with resolution suitable for dest_L
// Requires dest_L>src_L, else dest_im is a (shallow) copy of src_im
void vimt_resample(const vimt_image_2d_of<float>& src_im, int src_L,
                   vimt_image_2d_of<float>& dest_im, int dest_L)
{
  if (dest_L<=src_L)
  {
    dest_im=src_im;
    return;
  }
  unsigned ni = src_im.image().ni();
  unsigned nj = src_im.image().nj();
  unsigned s=1;
  for (int L=dest_L;L>=src_L;--L) s*=2;

  vil_resample_bilin(src_im.image(),dest_im.image(),1+s*(ni-1),1+s*(nj-1));
  vimt_transform_2d scale;
  scale.set_zoom_only(s,s,0.0,0.0);
  dest_im.set_world2im(scale*src_im.world2im());
}


int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> param_path("-p","Parameter file path");

  vul_arg_parse(argc, argv);

  if (param_path()=="")
  {
    print_usage();
    return 0;
  }

  fhs_model_params params;
  if (!parse_param_file(param_path(),params)) return 1;

  // ============================================
  // Attempt to load in images
  // ============================================

  vimt_image_2d_of<vxl_byte> image1,image2;
  image1.image() = vil_load(params.image1_path.c_str());
  if (image1.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<params.image1_path<<vcl_endl;
    return 1;
  }
  image2.image() = vil_load(params.image2_path.c_str());
  if (image2.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<params.image2_path<<vcl_endl;
    return 1;
  }

  // ============================================
  // Build image pyramids and select chosen level
  // ============================================
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder;
  vimt_image_pyramid image_pyr1,image_pyr2;
  pyr_builder.build(image_pyr1,image1);
  pyr_builder.build(image_pyr2,image2);

  int max_L = vcl_min(image_pyr1.hi(),image_pyr2.hi());
  if (params.L_lo<0  || params.L_lo>max_L  || params.L_hi<params.L_lo)
  {
    vcl_cerr<<"Levels must be in range [0,"<<max_L<<"], with lo<=hi\n";
    return 2;
  }

  // ============================================
  // Load in the points
  // ============================================

  vcl_vector<vgl_point_2d<double> > ref_pts;
  if (!fhs_load_points(params.points_path,ref_pts)) return 3;

  vcl_vector<unsigned> im_level(ref_pts.size());
  for (unsigned i=0;i<ref_pts.size();++i) im_level[i]=params.L_hi;

  // ============================================
  // Load in the arcs (links between pairs)
  // ============================================
  vcl_vector<vcl_pair<int,int> > pairs;
  if (!fhs_load_arcs(params.arcs_path,pairs)) return 4;

  // Check arc ends are all valid points
  for (unsigned i=0;i<pairs.size();++i)
  {
    if (pairs[i].first<0 || pairs[i].first>=ref_pts.size())
      { vcl_cerr<<"Invalid point index "<<pairs[i].first<<vcl_endl; return 5; }
    if (pairs[i].second<0 || pairs[i].second>=ref_pts.size())
      { vcl_cerr<<"Invalid point index "<<pairs[i].second<<vcl_endl; return 5; }
  }

  // ====================================================================
  // Create model, consisting of patches and a tree of relative positions
  // ====================================================================

  vcl_vector<vil_image_view<float> > patch;
  vcl_vector<vgl_point_2d<double> > patch_ref;  // Reference point

  // Extract patches for each point, pushing back onto patch,patch_ref
  extract_normalised_patches(image_pyr1(params.L_hi),ref_pts,
                             params.half_width,patch,patch_ref);


  // =================================================
  // Construct the arc model from the points and pairs
  // =================================================

  vcl_vector<fhs_arc> arcs(pairs.size());
  int root_node = pairs[0].first;
  for (unsigned i=0;i<pairs.size();++i)
  {
    int i1 = pairs[i].first;
    int i2 = pairs[i].second;
    vgl_vector_2d<double> dp = ref_pts[i2]-ref_pts[i1];
    double sd_x = vcl_max(vcl_pow(2.0,double(im_level[i])),0.2*dp.length());
    double sd_y = vcl_max(vcl_pow(2.0,double(im_level[i])),0.2*dp.length());
    arcs[i]=fhs_arc(i1,i2,dp.x(),dp.y(),sd_x*sd_x,sd_y*sd_y);
  }


  // =================================================
  // Apply filters to image2 (initially to whole image)
  // =================================================
  vcl_vector<vimt_image_2d_of<float> > feature_response(ref_pts.size());
  for (unsigned i=0;i<ref_pts.size();++i)
  {
    const vimt_image_2d_of<vxl_byte>& byte_im =
       static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr2(im_level[i]));

    // Compute region over which to search (20% of image, centered on point)
    int ni=byte_im.image().ni();
    int xw = ni/8+1+params.half_width;
    int nj=byte_im.image().nj();
    int yw = nj/8+1+params.half_width;
    vgl_point_2d<double> im_p = byte_im.world2im()(ref_pts[i]);
    int xc = int(0.5+im_p.x());
    int yc = int(0.5+im_p.y());
    int ilo = vcl_max(0,xc-xw);
    int ihi = vcl_min(ni-1,xc+xw);
    int jlo = vcl_max(0,yc-yw);
    int jhi = vcl_min(nj-1,yc+yw);
    vimt_image_2d_of<vxl_byte> cropped_im = vimt_crop(byte_im,
                                                      ilo, 1+ihi-ilo,
                                                      jlo, 1+jhi-jlo);

    // Apply to whole image in first instance
    // Ideally would crop a region around expected position
    vimt_normalised_correlation_2d(cropped_im,feature_response[i],
                                   patch[i],patch_ref[i],float());
    if (im_level[i]!=params.L_lo)
    {
      // Resample the feature response at resolution of image params.L_lo
      vimt_image_2d_of<float> fr;
      fr.deep_copy(feature_response[i]);
      vimt_resample(fr,im_level[i], feature_response[i],params.L_lo);
    }
    // Need good values to be small, not large, so apply -ve factor
    vil_math_scale_values(feature_response[i].image(),-params.response_scale);
  }

  // ======================================================
  // Use fhs_searcher to locate equivalent points on image2
  // ======================================================

  fhs_searcher searcher;
  searcher.set_tree(arcs,root_node);
  searcher.search(feature_response);
  vcl_vector<vgl_point_2d<double> > pts2;
  searcher.best_points(pts2);

  // Draw tree into image for display purposes
  draw_tree(image1.image(),ref_pts,pairs);

  if (vil_save(image1.image(),params.output_image1_path.c_str()))
  {
    vcl_cout<<"Saved output image 1 to "<<params.output_image1_path<<vcl_endl;
  }

  // Draw tree into image for display purposes
  draw_tree(image2.image(),pts2,pairs);

  if (vil_save(image2.image(),params.output_image2_path.c_str()))
  {
    vcl_cout<<"Saved output image 2 to "<<params.output_image2_path<<vcl_endl;
  }


  return 0;
}
