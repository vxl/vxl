#ifndef SpatialBorgefors_h_
#define SpatialBorgefors_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Computes 3-4 distance transform
// \author
//             Charlie Rothwell - 4/5/95
//             INRIA, Sophia Antipolis
//
// SpatialBorgefors is a class for managing parts of the verfication procedures which
// are required for recognition. At the basic level, this involves computing the
// distance transform image and the orientation image. This is done using the
// 3-4 chamfer distance transform. All indexing of distances and orientations
// is done in global image terms, and not at the level of the ROI (and hence
// we are using the real image origin). The distance and orientation images
// are stored as unsigned chars, though the return values are cast to floats.
// Information about the Edge and element of its associated curve are also
// stored in separate images.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_numeric_traits.h>
#include <vbl/vbl_ref_count.h>
#include <vul/vul_timestamp.h>
class BufferXY;
class Edge;

class SpatialBorgefors : public vbl_ref_count, public vul_timestamp
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  SpatialBorgefors(int,int,int,int,CoolListP<Edge*>&);
  ~SpatialBorgefors();

  // Data Access---------------------------------------------------------------

  inline float Distance(int x, int y) {
    int i = x - xstart_;
    int j = y - ystart_;
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return (float) distance_[i][j];
    else
        return vnl_numeric_traits<float>::maxval;
    }

  inline float Orientation(int x, int y) {
    int i = x - xstart_;
    int j = y - ystart_;
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return (float) orientation_[i][j];
    else
        return -1.0f;
    }

  inline Edge *ImageEdge(int x, int y) {
    int i = x - xstart_;
    int j = y - ystart_;
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return edges_[i][j];
    else
        return NULL;
    }

  inline int CurveIndex(int x, int y) {
    int i = x - xstart_;
    int j = y - ystart_;
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return index_[i][j];
    else
        return -1;
    }

  // INTERNALS-----------------------------------------------------------------

  unsigned char **MakeUnsignedCharImage(int,int);
  void FreeUnsignedCharImage(unsigned char**,int);
  int **MakeIntImage(int,int);
  void FreeIntImage(int**,int);
  Edge ***MakeEdgeImage(int,int);
  void FreeEdgeImage(Edge***,int);
  void InitialiseDistancesOrientationsEdges();
  void InitialiseImages(CoolListP<Edge*>&);

  void Chamfer34();
  int Minimum5(int,int,int,int,int);
  void ForwardChamfer();
  void BackwardChamfer();
  void ComputeRealDistances();
  void UpdateBufImage();
  BufferXY* GetBufImage(){return buf_image_;}

  // Data Members--------------------------------------------------------------

private:

  // Various pieces of image info
  int xsize_,ysize_,xstart_,ystart_;

  // The distance and orientation images
  unsigned char **orientation_,**distance_;

  // Pointers to the nearest Edge for each pixel;
  Edge ***edges_;

  // The index of the digital curve element at a given pixel
  int **index_;

  // Store the image in this buffer
  BufferXY* buf_image_;
};

#endif
