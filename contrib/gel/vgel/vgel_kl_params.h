// This is gel/vgel/vgel_kl_params.h
#ifndef vgel_kl_params_h_
#define vgel_kl_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//--------------------------------------------------------------------------------
//:
// \file
// \brief Interface to set Kanade-Lucas parameters
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
#include <vgel/kl/klt.h>

class vgel_kl_params
{
 public:

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor. Parametres set to defaults
//---------------------------------------------------------------------------
  vgel_kl_params();

//---------------------------------------------------------------------------
//: Copy constructor
//---------------------------------------------------------------------------
  vgel_kl_params(const vgel_kl_params & params);

//---------------------------------------------------------------------------
//: Destructor.
//---------------------------------------------------------------------------
  virtual ~vgel_kl_params();

//---------------------------------------------------------------------------
//: Set the parametres to default values
//---------------------------------------------------------------------------
  void set_defaults();

//---------------------------------------------------------------------------
//: Parametres
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

#endif // vgel_kl_params
