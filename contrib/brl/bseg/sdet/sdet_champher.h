// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef _SpatialBorgefors_h
#define _SpatialBorgefors_h

//-----------------------------------------------------------------------------
//
// Class : SpatialBorgefors
//
// .SECTION Description:
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
// Man page information:
//
// .NAME SpatialBorgefors - Computes 3-4 distance transform
// .LIBRARY SpatialBasics
// .HEADER SpatialObjects package
// .INCLUDE SpatialBasics/SpatialBorgefors.h
// .FILE SpatialBorgefors.C
//
// .SECTION Author:
//             Charlie Rothwell - 4/5/95
//             INRIA, Sophia Antipolis
//
//-----------------------------------------------------------------------------

#ifndef CoolListPh
template <class T> class CoolListP;
#endif

class BufferXY;
class Edge;
#include <Basics/RefCntTimeStampMixin.h>

class SpatialBorgefors : public RefCntTimeStampMixin
{

  // PUBLIC INTERFACE----------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-------------------------------------
  
  SpatialBorgefors(int,int,int,int,CoolListP<Edge*>&);
  ~SpatialBorgefors();

  // Data Access---------------------------------------------------------------

  inline float Distance(int x, int y) {
	int i = x - _xstart;
	int j = y - _ystart;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( (float) _distance[i][j] );
	else
	    return(HUGE);
    }

  inline float Orientation(int x, int y) {
	int i = x - _xstart;
	int j = y - _ystart;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( (float) _orientation[i][j] );
	else
	    return(-1.0);
    }

  inline Edge *ImageEdge(int x, int y) {
	int i = x - _xstart;
	int j = y - _ystart;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( _edges[i][j] );
	else
	    return(NULL);
    }

  inline int CurveIndex(int x, int y) {
	int i = x - _xstart;
	int j = y - _ystart;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( _index[i][j] );
	else
	    return(-1);
    }

  // Data Control--------------------------------------------------------------

  // Utility Methods-----------------------------------------------------------

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
  BufferXY* GetBufImage(){return _buf_image;}

protected:

  // Data Members--------------------------------------------------------------

private:

  // Various pieces of image info
  int _xsize,_ysize,_xstart,_ystart;

  // The distance and orientation images
  unsigned char **_orientation,**_distance;

  // Pointers to the nearest Edge for each pixel;
  Edge ***_edges;

  // The index of the digital curve element at a given pixel
  int **_index;

  // Store the image in this buffer
  BufferXY* _buf_image;
};

#endif
