//:
// \file
// \author Tim Cootes
// \brief Example program using F&H method to locate matches on a pair of images

#include <vul/vul_arg.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_crop.h>
#include <vil/algo/vil_corners.h>
#include <vil/algo/vil_find_peaks.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_fill.h>
#include <vil/vil_crop.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_bilin.h>
#include <mbl/mbl_index_sort.h>
#include <vnl/vnl_math.h>
#include <fhs/fhs_searcher.h>
#include <vimt/algo/vimt_normalised_correlation_2d.h>
#include <mbl/mbl_minimum_spanning_tree.h>
#include <mbl/mbl_draw_line.h>

void print_usage()
{
  vcl_cout<<"find_matches -i1 image1.jpg -i2 image2.jpg -Llo 0 Lhi 2"<<vcl_endl
          <<"Loads in image1 and image2."<<vcl_endl
          <<"Locates a set of interesting features (corners) on levels of image1."<<vcl_endl
          <<"Constructs a tree model of their relative positions."<<vcl_endl
          <<"Points at level L are linked to the nearest points at level L+1"<<vcl_endl
          <<"Uses normalised correllation and this model to locate"<<vcl_endl
          <<"equivalent points on the second image."<<vcl_endl;
  vul_arg_display_usage_and_exit();
}

//: Write tree into the image
// Draw disks at each point, and lines between linked points
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

//: Apply corner operator to image and select strongest corners
void get_strongest_corners(const vimt_image& image,
                           vcl_vector<vgl_point_2d<double> >& pts,
                           unsigned max_n)
{
  const vimt_image_2d_of<vxl_byte>& byte_im =
               static_cast<const vimt_image_2d_of<vxl_byte>&>(image);
  vimt_image_2d_of<float> corner_im;
  corner_im.set_world2im(byte_im.world2im());
  vil_corners(byte_im.image(),corner_im.image());

  vcl_vector<unsigned> pi,pj;
  float threshold = 4.0f;
  vil_find_peaks_3x3(pi,pj,corner_im.image(),threshold);

  // Evaluate corner strength at each point (pi[i],pj[i])
  unsigned n = pi.size();
  vcl_vector<float> corner_str(n);
  for (unsigned i=0;i<n;++i)
    corner_str[i] = corner_im.image()(pi[i],pj[i]);

  // Sort and generate a list of image points and equivalent world points
  vcl_vector<unsigned> index;
  mbl_index_sort(corner_str,index);

  unsigned n_c = vcl_min(max_n,n);
  vcl_vector<vgl_point_2d<int> > im_pts(n_c);
  pts.resize(n_c);
  vimt_transform_2d im2w = byte_im.world2im().inverse();
  for (unsigned i=0;i<n_c;++i)
  {
    im_pts[i] = vgl_point_2d<int>(pi[index[n-1-i]],pj[index[n-1-i]]);
    pts[i]  = im2w(pi[index[n-1-i]],pj[index[n-1-i]]);
  }
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

//: Return index of closest point in pts[lo,hi]
unsigned closest_pt_index(const vcl_vector<vgl_point_2d<double> >& pts,
                          unsigned lo, unsigned hi,
                          vgl_point_2d<double> p)
{
  unsigned c=lo;
  double min_d = (pts[lo]-p).sqr_length();
  for (unsigned i=lo+1;i<=hi;++i)
  {
    double d = (pts[i]-p).sqr_length();
    if (d<min_d) {min_d=d; c=i;}
  }
  return c;
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
  vul_arg<vcl_string> image1_path("-i1","Input image 1");
  vul_arg<vcl_string> image2_path("-i2","Input image 2");
  vul_arg<vcl_string> output_image1_path("-o1","Output image 1","output1.png");
  vul_arg<vcl_string> output_image2_path("-o2","Output image 1","output2.png");
  vul_arg<int> level_lo("-Llo","Image pyramid level to work on",0);
  vul_arg<int> level_hi("-Lhi","Image pyramid level to work on",2);
  vul_arg<unsigned> nc("-n","Number of points to select at coarse level",5);
  vul_arg<unsigned> w("-w","Half width of filters",7);
  vul_arg<double> f("-f","Relative strength of intensity vs shape",0.1);

  vul_arg_parse(argc, argv);

  if (image1_path()=="" || image2_path()=="")
  {
    print_usage();
    return 0;
  }

  // ============================================
  // Attempt to load in images
  // ============================================

  vimt_image_2d_of<vxl_byte> image1,image2;
  image1.image() = vil_load(image1_path().c_str());
  if (image1.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<image1_path()<<vcl_endl;
    return 1;
  }
  image2.image() = vil_load(image2_path().c_str());
  if (image2.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<image2_path()<<vcl_endl;
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
  if (level_lo()<0  || level_hi()>max_L  || level_hi()<level_lo())
  {
    vcl_cerr<<"Levels must be in range [0,"<<max_L<<"], with lo<=hi\n";
    return 1;
  }

  // ====================================================================
  // Create model, consisting of patches and a tree of relative positions
  // ====================================================================

  vcl_vector<vgl_point_2d<double> > pts;
  vcl_vector<vil_image_view<float> > patch;
  vcl_vector<vgl_point_2d<double> > patch_ref;  // Reference point
  vcl_vector<vcl_pair<int,int> > pairs;
  vcl_vector<unsigned> im_level;  // Image level for each point

  // Search the coarsest image for corners
  get_strongest_corners(image_pyr1(level_hi()),pts,nc());

  // Construct tree structure for points
  mbl_minimum_spanning_tree(pts,pairs);

  // Extract patches for each point, pushing back onto patch,patch_ref
  extract_normalised_patches(image_pyr1(level_hi()),pts,w(),patch,patch_ref);

  for (unsigned i=0;i<pts.size();++i) im_level.push_back(level_hi());

  vcl_cout<<"Found "<<pts.size()<<" points at level "<<level_hi()<<vcl_endl;

  // Generate more points at each level
  int max_pts = nc();
  unsigned i0 = 0;          // Record index of first point at level above
  unsigned i1 = pts.size()-1; // Record index of last point at level above
  for (int L=level_hi()-1;L>=level_lo();--L)
  {
    max_pts*=3;
    vcl_vector<vgl_point_2d<double> > L_pts;

    // Search the coarsest image for corners
    get_strongest_corners(image_pyr1(L),L_pts,max_pts);

    // Extract patches for each point, pushing back onto patch,patch_ref
    extract_normalised_patches(image_pyr1(L),L_pts,w(),patch,patch_ref);

    // For each new point, find the closest point in the levels above
    // Change 0,i1 to i0,i1 to consider only level above
    for (unsigned i=0;i<L_pts.size();++i)
    {
      pts.push_back(L_pts[i]);
      vcl_pair<int,int> pair_i(pts.size()-1,
                               closest_pt_index(pts,0,i1,L_pts[i]));
      pairs.push_back(pair_i);
      im_level.push_back(L);
    }

    vcl_cout<<"Found "<<L_pts.size()<<" points at level "<<L<<vcl_endl;

    i0=i1+1;
    i1=pts.size()-1;
  }


  // Draw tree into image for display purposes
  draw_tree(image1.image(),pts,pairs);

  if (vil_save(image1.image(),output_image1_path().c_str()))
  {
    vcl_cout<<"Saved output image 1 to "<<output_image1_path()<<vcl_endl;
  }

  // =================================================
  // Construct the arc model from the points and pairs
  // =================================================

  vcl_vector<fhs_arc> arcs(pairs.size());
  int root_node = pairs[0].first;
  for (unsigned i=0;i<pairs.size();++i)
  {
    int i1 = pairs[i].first;
    int i2 = pairs[i].second;
    vgl_vector_2d<double> dp = pts[i2]-pts[i1];
    double sd_x = vcl_max(vcl_pow(2.0,im_level[i]),0.2*dp.length());
    double sd_y = vcl_max(vcl_pow(2.0,im_level[i]),0.2*dp.length());
    arcs[i]=fhs_arc(i1,i2,dp.x(),dp.y(),sd_x*sd_x,sd_y*sd_y);
  }

  // =================================================
  // Apply filters to image2 (initially to whole image)
  // =================================================
  vcl_vector<vimt_image_2d_of<float> > feature_response(pts.size());
  for (unsigned i=0;i<pts.size();++i)
  {
    const vimt_image_2d_of<vxl_byte>& byte_im =
       static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr2(im_level[i]));

    // Compute region over which to search (20% of image, centered on point)
    int ni=byte_im.image().ni();
    int xw = ni/10+1+w();
    int nj=byte_im.image().nj();
    int yw = nj/10+1+w();
    vgl_point_2d<double> im_p = byte_im.world2im()(pts[i]);
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
    if (im_level[i]!=level_lo())
    {
      // Resample the feature response at resolution of image level_lo()
      vimt_image_2d_of<float> fr;
      fr.deep_copy(feature_response[i]);
      vimt_resample(fr,im_level[i], feature_response[i],level_lo());
    }
    // Need good values to be small, not large, so apply -ve factor
    vil_math_scale_values(feature_response[i].image(),-f());
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
  draw_tree(image2.image(),pts2,pairs);

  if (vil_save(image2.image(),output_image2_path().c_str()))
  {
    vcl_cout<<"Saved output image 2 to "<<output_image2_path()<<vcl_endl;
  }


  return 0;
}
