// This is gel/gkll/gkll_kl.cxx
#include "gkll_kl.h"

#include <vxl_config.h>
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_image_as.h>
#include <gkll/gkll_multi_view_data.h>
#include <vtol/vtol_vertex_2d.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_movie.h>

#include <vcl_iostream.h>


gkll_kl::gkll_kl(const gkll_kl_params & params) : params_(params)
{
}

gkll_kl::~gkll_kl()
{
}

void gkll_kl::match_sequence(vcl_vector<vil1_image> &image_list,gkll_multi_view_data_vertex_sptr matches)
{
  // Uses the KL tracker to track points through an image
  int nFeatures = params_.numpoints;
  int nFrames = image_list.size();
  // If there are no frames in this movie, then skip
  if (nFrames < 1) return;

  // Set up the context
  KLT_TrackingContext tc = KLTCreateTrackingContext();
  KLT_FeatureList     fl = KLTCreateFeatureList(nFeatures);
  KLT_FeatureTable    ft = KLTCreateFeatureTable(nFrames, nFeatures);

  // Apply the defaults
  set_tracking_context (tc);
  tc->sequentialMode = TRUE;

  int width=image_list[0].width();
  int height=image_list[0].height();

  // Now, get the imagery into a linear buffer
  KLT_PixelType* img1=convert_to_gs_image(image_list[0]);

  // Get some features from the first image
  KLTSelectGoodFeatures(tc, img1, width, height, fl);
  KLTStoreFeatureList(fl, ft, 0);

  for (int i=1; i<nFrames; i++)
  {
    KLT_PixelType* img2=convert_to_gs_image(image_list[i]);

    // Track the points
    KLTTrackFeatures(tc, img1, img2, width, height, fl);

    // Restore lost features
    if (params_.replaceLostPoints)
      KLTReplaceLostFeatures(tc, img2, width, height, fl);

    // Store the values
    KLTStoreFeatureList(fl, ft, i);
  }
  // Go through the feature table and store them
  int matchnum = -1;
  int pointnum, viewnum;
  //matches.set_params(ft->nFrames,ft->nFeatures);

  for (pointnum=0; pointnum<ft->nFeatures; pointnum++)
    for (viewnum=0; viewnum<ft->nFrames; viewnum++)
    {
      // Get the current feature
      KLT_Feature feat = ft->feature[pointnum][viewnum];

      // Get the components of this feature
      float x = feat->x;
      float y = feat->y;
      //int val = feat->val;

      // Test to see if this is the continuation of a sequence
      // - then put in the table
      if (feat->val == 0)
      {
        vtol_vertex_2d_sptr vertex=new vtol_vertex_2d(x,y);
        matches->set(viewnum, matchnum, vertex);
      }

      // Otherwise, this is the start of a sequence
      if (feat->val > 0)
      {
        // Must test to see if the next value is zero
        // - otherwise this is a 1-frame sequence
        if (viewnum < ft->nFrames-1 &&
            ft->feature[pointnum][viewnum+1]->val == 0)
        {
          // This is a new match
          matchnum++;

          // Store it
          vtol_vertex_2d_sptr vertex=new vtol_vertex_2d(x,y);
          matches->set (viewnum, matchnum, vertex);
        }
      }
    }
  // Finally, renumber the matches
//  matches.renumber();
}


void gkll_kl::match_sequence(vidl_vil1_movie_sptr movie,gkll_multi_view_data_vertex_sptr matches)
{
  vcl_vector<vil1_image> image_list;
  for (vidl_vil1_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last();
       ++pframe)
  {
    vil1_image im = vil1_image(pframe->get_image());
    image_list.push_back(im);
  }
  match_sequence(image_list,matches);
}

vcl_vector<vtol_vertex_2d_sptr>* gkll_kl::extract_points(vil1_image & image)
{
  int width=image.width();
  int height=image.height();
  vcl_cerr << "Beginning points extraction\n";

  KLT_PixelType* img1=convert_to_gs_image(image);

  // Now, run the extractor
  int nFeatures = params_.numpoints;

  vcl_cerr << "Setting up the context...\n";
  // Set up the context
  KLT_TrackingContext tc = KLTCreateTrackingContext();

  // Set the default values
  set_tracking_context (tc);

  // KLTPrintTrackingContext(tc);

  // Set up structure to hold the features.
  vcl_cerr << "Setting up structure to hold the features...\n";
  KLT_FeatureList fl = KLTCreateFeatureList(nFeatures);

  // Extract the features
  vcl_cerr << "Extracting the features...\n";
  KLTSelectGoodFeatures(tc, img1, width, height, fl);

  // Make an IUPointGroup to hold the values
  vcl_vector<vtol_vertex_2d_sptr> *grp = new vcl_vector<vtol_vertex_2d_sptr>();

  for (int i=0 ; i< fl->nFeatures ; i++)
  {
    // Change the point into an IUPoint - offset by ilow, jlow
    float x = fl->feature[i]->x;
    float y = fl->feature[i]->y;
    //HomgPoint2D *point = new HomgPoint2D (x, y,1.0)
    vtol_vertex_2d_sptr point=new vtol_vertex_2d(x,y);
    // Put the point in the backup list
    grp->push_back(point);
  }

  // !!
  // We probably need to delete the feature list

  // Return the group
  return grp;
}

//Convert a vil1_image to an array of grey scale
KLT_PixelType* gkll_kl::convert_to_gs_image(vil1_image &image)
{
  vcl_cerr << "Converting image to grey scale...\n";
  if (vil1_pixel_format(image)==VIL1_RGB_BYTE)
  {
    int w=image.width();
    int h=image.height();
    KLT_PixelType* tab_mono=new KLT_PixelType[w*h];
    vcl_cerr << "width: " <<w<< "  height"<<h<<  vcl_endl;

    vil1_memory_image_of<vxl_byte> ima_mono;
    ima_mono.resize(w,h);

    vil1_image_as_byte(image).get_section(ima_mono.get_buffer(), 0, 0, w, h);
    vxl_byte* p=ima_mono.get_buffer();

    for (int i=0;i<w;i++)
      for (int j=0;j<h;j++)
      {
        tab_mono[i*h+j]=(KLT_PixelType)p[i*h+j];
      }
    return tab_mono;
  } else return NULL;
}

void gkll_kl::set_tracking_context( KLT_TrackingContext tc)
{
  /* Set values to values derived from the parameters */
  tc->mindist               = params_.mindist;
  tc->window_width          = params_.window_width;
  tc->window_height         = params_.window_height;
  tc->sequentialMode        = params_.sequentialMode;
  tc->smoothBeforeSelecting = params_.smoothBeforeSelecting;
  tc->writeInternalImages   = params_.writeInternalImages;
  tc->min_eigenvalue        = params_.min_eigenvalue;
  tc->min_determinant       = params_.min_determinant;
  tc->max_iterations        = params_.max_iterations;
  tc->min_displacement      = params_.min_displacement;
  tc->max_residue           = params_.max_residue;
  tc->grad_sigma            = params_.grad_sigma;
  tc->smooth_sigma_fact     = params_.smooth_sigma_fact;
  tc->pyramid_sigma_fact    = params_.pyramid_sigma_fact;
  tc->nSkippedPixels        = params_.nSkippedPixels;

  // klt functions to complete the setup
  KLTChangeTCPyramid (tc, params_.search_range); //set nPyramidLevels and subsampling
  KLTUpdateTCBorder  (tc); //set borderx and bordery
}
