// This is gel/gkll/gkll_kl_params.h
#ifndef gkll_kl_params_h_
#define gkll_kl_params_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Interface to set Kanade-Lucas parameters
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
#include <vgel/kl/klt.h>

class gkll_kl_params
{
 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Parameters set to defaults
  //---------------------------------------------------------------------------
  gkll_kl_params();

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  gkll_kl_params(const gkll_kl_params & params);

  //---------------------------------------------------------------------------
  //: Destructor.
  //---------------------------------------------------------------------------
  virtual ~gkll_kl_params();

  //---------------------------------------------------------------------------
  //: Set the parameters to default values
  //---------------------------------------------------------------------------
  void set_defaults();

  //---------------------------------------------------------------------------
  //: Parameters
  //---------------------------------------------------------------------------

  int numpoints;
  int search_range;

  int mindist;
  int window_width, window_height;
  bool sequentialMode;
  bool smoothBeforeSelecting;
  bool writeInternalImages;
  int min_eigenvalue;
  float min_determinant;
  float min_displacement;
  int max_iterations;
  float max_residue;
  float grad_sigma;
  float smooth_sigma_fact;
  float pyramid_sigma_fact;
  bool replaceLostPoints;
  int nSkippedPixels;
  int borderx;
  int bordery;
  int nPyramidLevels;
  int subsampling;
  bool verbose;
};

#endif // gkll_kl_params
