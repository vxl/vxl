#include "osl_canny_base.h"
#include <osl/osl_canny_port.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_list.h>

//--------------------------------------------------------------

osl_canny_base::osl_canny_base(float sigma, float low, float high, bool v)
  : _xstart(0), _ystart(0)
  , _xsize(0), _ysize(0)

  , _smooth(0)
  , _dx(0)
  , _dy(0)
  , _grad(0)

  , _thick(0)
  , _thin(0)
  , _theta(0)

  , _junction(0)
  , _jx(0)
  , _jy(0)
  , _xjunc(0)
  , _yjunc(0)
  , _vlist(0)

  , _kernel(0)
{
  verbose =v;
  _sigma = sigma;
  _low = low;
  _high = high;
}

//: Destructor does nothing at all.
osl_canny_base::~osl_canny_base() {  }


//-----------------------------------------------------------------------------

//: Following routine looking for connectiveness of edgel chains
// and accounts for single pixel gaps in the chains.
void osl_canny_base::Initial_follow(float * const *thin, int xsize, int ysize, float low,
                                    int x, int y,
                                    vcl_list<int> *xc,
                                    vcl_list<int> *yc,
                                    vcl_list<float> *grad)
{
  // Make sure that we are not likely to overun the border of the image
  if ( (x<=0) || (x>=xsize-1) || (y<=0) || (y>=ysize-1) )
    return;

  // Add the current point to the coordinate lists, and delete from
  // the edge image
  xc->push_front(x);
  yc->push_front(y);
  grad->push_front(thin[x][y]);
  thin[x][y] = 0.0;

  // Now recursively look for connected eight-neighbours
  if ( thin[x][y-1] > low )
    Initial_follow(thin, xsize, ysize, low, x  ,y-1,xc,yc,grad);
  if ( thin[x-1][y  ] > low )
    Initial_follow(thin, xsize, ysize, low, x-1,y  ,xc,yc,grad);
  if ( thin[x  ][y+1] > low )
    Initial_follow(thin, xsize, ysize, low, x  ,y+1,xc,yc,grad);
  if ( thin[x+1][y  ] > low )
    Initial_follow(thin, xsize, ysize, low, x+1,y  ,xc,yc,grad);
  if ( thin[x+1][y-1] > low )
    Initial_follow(thin, xsize, ysize, low, x+1,y-1,xc,yc,grad);
  if ( thin[x-1][y-1] > low )
    Initial_follow(thin, xsize, ysize, low, x-1,y-1,xc,yc,grad);
  if ( thin[x-1][y+1] > low )
    Initial_follow(thin, xsize, ysize, low, x-1,y+1,xc,yc,grad);
  if ( thin[x+1][y+1] > low )
    Initial_follow(thin, xsize, ysize, low, x+1,y+1,xc,yc,grad);
}


//-----------------------------------------------------------------------------

//: Following routine looking for connectiveness of edgel chains
// and accounts for single pixel gaps in the chains.
void osl_canny_base::Final_follow(int x, int y,
                                  vcl_list<int> *xc,
                                  vcl_list<int> *yc,
                                  vcl_list<float> *grad,
                                  int reverse)
{
  // Make sure that we do not overun the border of the image
  if ( (x<=0) || (x>=_xsize-1) || (y<=0) || (y>=_ysize-1) )
    return;

  // Add the current point to the coordinate lists, and delete from
  // the edge image
  if (!reverse) {
    xc->push_front(x);
    yc->push_front(y);
    grad->push_front(_thin[x][y]);
  }
  _thin[x][y] = 0.0;

  // Now recursively look for connected eight-neighbours
  if      ( (_thin[x  ][y-1]>_low) && (_junction[x  ][y-1]==0) )
    Final_follow(x,y-1,xc,yc,grad,0);
  else if ( (_thin[x-1][y  ]>_low) && (_junction[x-1][y  ]==0) )
    Final_follow(x-1,y,xc,yc,grad,0);
  else if ( (_thin[x  ][y+1]>_low) && (_junction[x  ][y+1]==0) )
    Final_follow(x,y+1,xc,yc,grad,0);
  else if ( (_thin[x+1][y  ]>_low) && (_junction[x+1][y  ]==0) )
    Final_follow(x+1,y,xc,yc,grad,0);
  else if ( (_thin[x+1][y-1]>_low) && (_junction[x+1][y-1]==0) )
    Final_follow(x+1,y-1,xc,yc,grad,0);
  else if ( (_thin[x-1][y-1]>_low) && (_junction[x-1][y-1]==0) )
    Final_follow(x-1,y-1,xc,yc,grad,0);
  else if ( (_thin[x-1][y+1]>_low) && (_junction[x-1][y+1]==0) )
    Final_follow(x-1,y+1,xc,yc,grad,0);
  else if ( (_thin[x+1][y+1]>_low) && (_junction[x+1][y+1]==0) )
    Final_follow(x+1,y+1,xc,yc,grad,0);

  // Else see if there is a junction nearby, and record it. The _chain_no
  // variable is used to prevent the same junction being inserted at both
  // ends of the EdgelChains when reversel occurs next to the junction
  // (in that case there will only be two stored points: the edge and the junction)
  else if ( _junction[x  ][y-1] && ((xc->size()>2)||(_junction[x  ][y-1]!=_chain_no)) ) {
    xc->push_front(_jx[x  ][y-1]);  yc->push_front(_jy[x  ][y-1]);  grad->push_front(_jval);
    _junction[x  ][y-1] = _chain_no;
  }
  else if ( _junction[x-1][y  ] && ((xc->size()>2)||(_junction[x-1][y  ]!=_chain_no)) ) {
    xc->push_front(_jx[x-1][y  ]);  yc->push_front(_jy[x-1][y  ]);  grad->push_front(_jval);
    _junction[x-1][y  ] = _chain_no;
  }
  else if ( _junction[x  ][y+1] && ((xc->size()>2)||(_junction[x  ][y+1]!=_chain_no)) ) {
    xc->push_front(_jx[x  ][y+1]);  yc->push_front(_jy[x  ][y+1]);  grad->push_front(_jval);
    _junction[x  ][y+1] = _chain_no;
  }
  else if ( _junction[x+1][y  ] && ((xc->size()>2)||(_junction[x+1][y  ]!=_chain_no)) ) {
    xc->push_front(_jx[x+1][y  ]);  yc->push_front(_jy[x+1][y  ]);  grad->push_front(_jval);
    _junction[x+1][y  ] = _chain_no;
  }
  else if ( _junction[x+1][y-1] && ((xc->size()>2)||(_junction[x+1][y-1]!=_chain_no)) ) {
    xc->push_front(_jx[x+1][y-1]);  yc->push_front(_jy[x+1][y-1]);  grad->push_front(_jval);
    _junction[x+1][y-1] = _chain_no;
  }
  else if ( _junction[x-1][y-1] && ((xc->size()>2)||(_junction[x-1][y-1]!=_chain_no)) ) {
    xc->push_front(_jx[x-1][y-1]);  yc->push_front(_jy[x-1][y-1]);  grad->push_front(_jval);
    _junction[x-1][y-1] = _chain_no;
  }
  else if ( _junction[x-1][y+1] && ((xc->size()>2)||(_junction[x-1][y+1]!=_chain_no)) ) {
    xc->push_front(_jx[x-1][y+1]);  yc->push_front(_jy[x-1][y+1]);  grad->push_front(_jval);
    _junction[x-1][y+1] = _chain_no;
  }
  else if ( _junction[x+1][y+1] && ((xc->size()>2)||(_junction[x+1][y+1]!=_chain_no)) ) {
    xc->push_front(_jx[x+1][y+1]);  yc->push_front(_jy[x+1][y+1]);  grad->push_front(_jval);
    _junction[x+1][y+1] = _chain_no;
  }
}

//-----------------------------------------------------------------------------
//
//: Following routine looking for searching out junction clusters.
//
void osl_canny_base::Follow_junctions(int * const *junction,
                                      int x, int y,
                                      vcl_list<int> *xc,
                                      vcl_list<int> *yc)
{
  // Add the current junction to the coordinate lists, and delete from
  // the junction image
  xc->push_front(x);
  yc->push_front(y);
  junction[x][y] = 0;

  // Now recursively look for connected eight-neighbours
  if ( junction[x  ][y-1] )
    Follow_junctions(junction,x  ,y-1,xc,yc);
  if ( junction[x-1][y  ] )
    Follow_junctions(junction,x-1,y  ,xc,yc);
  if ( junction[x  ][y+1] )
    Follow_junctions(junction,x  ,y+1,xc,yc);
  if ( junction[x+1][y  ] )
    Follow_junctions(junction,x+1,y  ,xc,yc);
  if ( junction[x+1][y-1] )
    Follow_junctions(junction,x+1,y-1,xc,yc);
  if ( junction[x-1][y-1] )
    Follow_junctions(junction,x-1,y-1,xc,yc);
  if ( junction[x-1][y+1] )
    Follow_junctions(junction,x-1,y+1,xc,yc);
  if ( junction[x+1][y+1] )
    Follow_junctions(junction,x+1,y+1,xc,yc);
}


//-----------------------------------------------------------------------------

//: Finds which member of the lists lies closest to the centre
// of gravity of the lists.
void osl_canny_base::Cluster_centre_of_gravity(int * const *jx, int * const *jy,
                                               vcl_list<int> &xc,
                                               vcl_list<int> &yc,
                                               int &x0, int &y0)
{
  typedef vcl_list<int>::iterator it;

  if ( xc.empty() )
    return;

  // First find the CofG
  double x=0.0,y=0.0;
  for (it i=xc.begin(),j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i, ++j) {
    //for (xc.reset(),yc.reset(); xc.next(),yc.next(); )
    x += *i;//xc.value();
    y += *j;//yc.value();
  }
  x /= xc.size();  y /= yc.size();

  // Now find the point closest to the CofG
  float dist = -1; // an invalid number
  for (it i=xc.begin(),j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i, ++j) {
    //xc.reset(),yc.reset();xc.next(),yc.next();)
    //float newdist = hypot(x- *i/*xc.value()*/,y- *j/*yc.value()*/);
    float newdist;
    { float dx = x- *i/*xc.value()*/, dy = y- *j/*yc.value()*/; newdist = vcl_sqrt(dx*dx + dy*dy); }
    if ( dist<0 || newdist < dist ) {
      x0 = *i;//xc.value();
      y0 = *j;//yc.value();
      dist = newdist;
    }
  }

  // Set up the (jx,jy) arrays to point to the cluster centre
  for (it i=xc.begin(),j=yc.begin(); i!=xc.end() && j!=yc.end(); ++i,++j) {
    //xc.reset(),yc.reset();xc.next(),yc.next();)
    jx[*i/*xc.value()*/][*j/*yc.value()*/] = x0;
    jy[*i/*xc.value()*/][*j/*yc.value()*/] = y0;
  }
}


//-----------------------------------------------------------------------------
//
//: Determines whether the point (x,y) is a neighbour to a junction.
//
int osl_canny_base::Junction_neighbour(int const * const *junction, int x, int y) {
  // Find the neighbour of (x][y) in the image
  if ( junction[x-1][y-1] || junction[x  ][y-1] || junction[x+1][y-1] ||
       junction[x-1][y  ] ||                       junction[x+1][y  ] ||
       junction[x-1][y+1] || junction[x  ][y+1] || junction[x+1][y+1]
       )
    return 1;
  else
    return 0;
}


//-----------------------------------------------------------------------------

//: Returns an m*n array of Ts.
template <class T>
T **osl_canny_base_make_raw_image(int m, int n, T * /*dummy*/) {
  T *array = new T[m*n];
  T **data = new T* [m];
  for (int i =0; i < m; ++i)
    data[i] = &array[i*n];
  return data;
}

//: Initialise an m*n array of Ts with value
template <class T>
void osl_canny_base_fill_raw_image(T * * image, int sizex, int sizey, T value) {
  for (int x =0; x < sizex; ++x)
    for (int y =0; y < sizey; ++y)
      image[x][y] = value;
}

//: Frees an m*n array of Ts.
template <class T>
void osl_canny_base_free_raw_image(T **ptr) {
  T *array = ptr[0];
  fsm_delete_array array;
  fsm_delete_array ptr;
}

//: Copies image1 to image2.
template <class S, class T>
void osl_canny_base_copy_raw_image(S const * const *image1, T * const *image2, int m, int n) //
{
  for (int x=0; x<m; ++x)
    for (int y=0; y<n; ++y)
      image2[x][y] = image1[x][y];
}


#define inst(T) \
template T     **osl_canny_base_make_raw_image(int, int, T *dummy); \
template void    osl_canny_base_fill_raw_image(T **, int, int, T value); \
template void    osl_canny_base_free_raw_image(T **); \
template void osl_canny_base_copy_raw_image(T const * const *, T * const *, int, int);
inst(float);
inst(int);
#undef inst
