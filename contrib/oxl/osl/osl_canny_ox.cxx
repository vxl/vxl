//:
//  \file

#include "osl_canny_ox.h"
#include <osl/osl_canny_port.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_kernel.h>
#include <osl/osl_canny_smooth.h>
#include <osl/osl_canny_gradient.h>
#include <osl/osl_canny_nms.h>

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>

//#define proddy(n) { delete [] new char [n]; }
//#define prod_heap { for (int n=1; n <= 0x100000; n *= 2) proddy(n); }

//-----------------------------------------------------------------------------

osl_canny_ox::osl_canny_ox(osl_canny_ox_params const &params)
  : osl_canny_base(params.sigma, params.low, params.high, params.verbose)
{
  // Set the maximum allowable convolution kernel
  gauss_tail_ = params.gauss_tail;
  max_width_OX_ = params.max_width;
  kernel_ = new float[max_width_OX_];
  sub_area_OX_ = new float[max_width_OX_];

  edge_min_OX_ = params.edge_min;
  min_length_OX_ = params.min_length;
  join_flag_OX_ = params.join_flag;
  border_size_OX_ = params.border_size;
  border_value_OX_ = params.border_value;
  scale_OX_ = params.scale;
  follow_strategy_OX_ = params.follow_strategy;
  junction_option_OX_ = params.junction_option;

  xjunc_ = new vcl_list<int>;
  yjunc_ = new vcl_list<int>;
  vlist_ = new vcl_list<osl_Vertex*>;

  jval_ = 2000.0;
}

//-----------------------------------------------------------------------------

osl_canny_ox::~osl_canny_ox()
{
  osl_canny_base_free_raw_image(smooth_);
  osl_canny_base_free_raw_image(dx_);
  osl_canny_base_free_raw_image(dy_);
  osl_canny_base_free_raw_image(grad_);
  osl_canny_base_free_raw_image(thick_);
  osl_canny_base_free_raw_image(thin_);
  osl_canny_base_free_raw_image(theta_);

  osl_canny_base_free_raw_image(junction_);
  osl_canny_base_free_raw_image(jx_);
  osl_canny_base_free_raw_image(jy_);

  fsm_delete_array kernel_;
  fsm_delete_array sub_area_OX_;
  //no point vlist_->clear();
  fsm_delete vlist_;
  fsm_delete xjunc_;
  fsm_delete yjunc_;
}

//-----------------------------------------------------------------------------

void osl_canny_ox::detect_edges(vil1_image const &image_in, vcl_list<osl_edge*> *edges)
{
  assert(edges!=0);

  // Get the image size
  xsize_ = image_in.height();
  ysize_ = image_in.width();
  xstart_ = 0;
  ystart_ = 0;

  if (verbose)
    vcl_cerr << "Doing Canny on image region "
             << xsize_ << " by " << ysize_ << vcl_endl
             << "Sigma               = " << sigma_ << vcl_endl
             << "Gaussian tail       = " << gauss_tail_ << vcl_endl
             << "Max kernel size     = " << max_width_OX_ << vcl_endl
             << "Upper threshold     = " << high_ << vcl_endl
             << "Lower threshold     = " << low_ << vcl_endl
             << "Min edgel intensity = " << edge_min_OX_ << vcl_endl
             << "Min edge length     = " << min_length_OX_ << vcl_endl
             << "Image border size   = " << border_size_OX_ << vcl_endl << vcl_endl;

  // Allocate internal bitmaps ..
  smooth_ = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  dx_     = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  dy_     = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  grad_   = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  thick_  = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  thin_   = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  theta_  = osl_canny_base_make_raw_image(xsize_, ysize_, (float*)0);
  junction_ = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)0);
  jx_       = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)0);
  jy_       = osl_canny_base_make_raw_image(xsize_, ysize_, (int*)0);
  //image_ = 0;

  // .. and initialize them.
  osl_canny_base_fill_raw_image(grad_, xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(thick_, xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(thin_, xsize_, ysize_, 0.0f);
  osl_canny_base_fill_raw_image(theta_, xsize_, ysize_, 10000.0f);
  osl_canny_base_fill_raw_image(junction_, xsize_, ysize_, 0);
  osl_canny_base_fill_raw_image(jx_, xsize_, ysize_, 0);
  osl_canny_base_fill_raw_image(jy_, xsize_, ysize_, 0);

  // Do the traditional Canny parts

  if (verbose) vcl_cerr << "setting convolution kernel and zeroing images\n";
  osl_kernel_DOG(kernel_, sub_area_OX_, k_size_,
                 sigma_, gauss_tail_,
                 max_width_OX_, width_);
  if (verbose) vcl_cerr << "Kernel size     = " << k_size_ << vcl_endl;


  if (verbose) vcl_cerr << "smoothing the image\n";
  osl_canny_smooth(image_in,
                   kernel_, width_, sub_area_OX_,
                   smooth_);

  if (verbose)
    vcl_cerr << "computing x derivatives, y derivatives and norm of gradient\n";
  osl_canny_gradient(xsize_, ysize_, smooth_, dx_, dy_, grad_);

  if (verbose) vcl_cerr << "doing non-maximal suppression\n";
  unsigned int n_edgels_NMS = osl_canny_nms(xsize_, ysize_, dx_, dy_, grad_, thick_, theta_);
  if (verbose) vcl_cerr << "Number of edgels after NMS = " << n_edgels_NMS << vcl_endl;


  // (x_,y_) holds the pixel location (and not the sub pixel accuracy)
  // of the edges.
  // Needed in Get_hysteresis_edgelsOX() to fill the thin_ image
  // at the edgels locations
  // (x_,y_)'s are initialised in Get_NMS_edgelsOX()
  int *x_ = new int[n_edgels_NMS];
  int *y_ = new int[n_edgels_NMS];


  // Copy edgels from thick_ image into an osl_edgel_chain class.
  // edgels after NMS are not connected, but still are stored in osl_edgel_chain.
  // edgels_NMS is the input to Hysteresis
  osl_edgel_chain *edgels_NMS = Get_NMS_edgelsOX(n_edgels_NMS, x_, y_);


  if (verbose) vcl_cerr << "doing hysteresis\n";
  int *status = new int[n_edgels_NMS];
  int n_edgels_Hysteresis = HysteresisOX(edgels_NMS, status);
  if (verbose) vcl_cerr << "Number of edgels after Hysteresis = " << n_edgels_Hysteresis << vcl_endl;

  osl_edgel_chain *edgels_Hysteresis = new osl_edgel_chain(n_edgels_Hysteresis);
  Get_hysteresis_edgelsOX(edgels_NMS,status,edgels_Hysteresis, x_, y_);

  // delete the edgels that are output of Non_maximal_suppression
  fsm_delete_array status;
  fsm_delete edgels_NMS; edgels_NMS = 0;

  // delete *x_ and *y_; they are not needed anymore
  fsm_delete_array x_;
  fsm_delete_array y_;

  if (follow_strategy_OX_ == 0)  {  //Don't do the follow stage of canny

    edges->push_front( NO_FollowerOX(edgels_Hysteresis) );
    // delete the edgels that are output of Hysteresis
    fsm_delete edgels_Hysteresis;
    return;
  }

  // delete the edgels that are output of Hysteresis
  fsm_delete edgels_Hysteresis;

  // Multiply thin_ image by scaleOX_. Values that are above 255 are set to 255
  Scale_imageOX(thin_, scale_OX_);
  // Set image border to border_valueOX_ (default = 0) so follow can't overrun
  Set_image_borderOX(thin_, border_size_OX_, border_value_OX_);

  if (junction_option_OX_)
  {
    // Locate junctions in the edge image
    if (verbose) vcl_cerr << "locating junctions in the edge image - ";
    Find_junctionsOX();
    if (verbose) vcl_cerr << xjunc_->size() << " junctions found\n";
    Find_junction_clustersOX();
    if (verbose)  vcl_cerr << vlist_->size() << " junction clusters found\n";
  }

  // Finally do edge following to extract the edge data from the thin_ image
  if (verbose) vcl_cerr << "doing final edge following\n";
  FollowerOX(edges);
  if (verbose) vcl_cerr << "finished osl_canny_ox\n";
}


//-----------------------------------------------------------------------------
//
//: Copy edgels from thick_ image. These edgels are the result of NMS.
// Although the edgels are stored in osl_edgel_chain edgels_NMS, they are
// not actually connected.
// Also initialises x_ and y_: the pixel locations of the edgels. These
// are needed later in filling the image thin_ with the edgels after
// hysteresis.
osl_edgel_chain *osl_canny_ox::Get_NMS_edgelsOX(int n_edgels_NMS, int *x_, int *y_)
{
  // the number of edges must be given in advance!
  osl_edgel_chain *edgels_NMS = new osl_edgel_chain(n_edgels_NMS);

  int i = 0;

  for (unsigned int y=1; y+1<ysize_; ++y)
    for (unsigned int x=1; x+1<xsize_; ++x)
      if ( thick_[x][y] != 0 /*&& i < n_edgels_NMS*/) {
        assert(i < n_edgels_NMS);
        // fill edgels_NMS
        edgels_NMS->SetX(dx_[x][y],i);
        edgels_NMS->SetY(dy_[x][y],i);
        edgels_NMS->SetGrad(thick_[x][y],i);
        edgels_NMS->SetTheta(theta_[x][y],i);
        // remember the pixel locations (x,y) of the edgels
        x_[i] = x; y_[i] = y;
        i++;
      }

  return edgels_NMS;
}


//-----------------------------------------------------------------------------
//
//: Hysteresis follows edgels that lie above the low threshold and have at least one edgel above the high threshold.
//
int osl_canny_ox::HysteresisOX(osl_edgel_chain *&edgels_NMS,
                               int *&status)
{
  unsigned int n_edgels_NMS = edgels_NMS->size();

  // Allocate arrays ..
  vcl_vector<unsigned> rows(ysize_+1); // rows[i] will contain the index in 'edgels_NMS' of the first
  //                                      edgel after start of row i. Thus, the edgels in row i are exactly
  //                                      those with indices j in the range rows[i] <= j < rows[i+1].
  vcl_vector<unsigned> row(n_edgels_NMS);  // (row[i], col[i]) will be the position of the ith
  vcl_vector<unsigned> col(n_edgels_NMS);  // edgel in 'edgels_NMS'.
  vcl_vector<osl_LINK *> links(n_edgels_NMS);  //
  // .. initialize arrays.
  for (unsigned int i=0; i<n_edgels_NMS; ++i) {
    links[i]  = 0; // null pointer
    row[i]    = (int) edgels_NMS->GetY(i);
    col[i]    = (int) edgels_NMS->GetX(i);
    status[i] = 0;
  }
  for (unsigned int i=0,j=0; i<=ysize_; ++i) { // Note: rows[ysize_] is one more than last edgel index
    while (j<n_edgels_NMS && row[j]<i)
      ++j;
    rows[i]=j;   // index of first edgel after start of row i
  }


  // Create a list of links for each edgel.
  Link_edgelsOX(col, rows, &links[0]); //vector<>::iterator


  // Perform Hysteresis part of canny.
  double low  = (32.0/vcl_log(2.0)) * vcl_log(low_/100+1.0);     // compute lower threshold
  double high = (32.0/vcl_log(2.0)) * vcl_log(high_/100+1.0);    // compute upper threshold
  //formerly "Do_hysteresisOX(edgels_NMS,links,status,low,high);"
  for (unsigned int i=0; i<n_edgels_NMS; ++i)
    if (!status[i] && edgels_NMS->GetGrad(i)>high) {
      status[i]=1;
      for (osl_LINK *lptr=links[i]; lptr; lptr=lptr->nextl)
        Initial_followOX(lptr->to, i, edgels_NMS, &links[0], status, (float)low); //vector<>::iterator
    }


  // and this?
  int n_edgels_Hysteresis = Get_n_edgels_hysteresisOX(edgels_NMS,status);

  // delete the osl_LINK * in the array 'links' :
  for (unsigned int i=0; i<n_edgels_NMS; ++i) {
    for (osl_LINK *link1 = links[i]; link1; ) {
      osl_LINK *link2 = link1->nextl;
      fsm_delete link1;
      link1 = link2;
    }
  }

  return n_edgels_Hysteresis;
}


//-------------------------------------------------------------------------
//
//: Initial follow. Used for the hysteresis part of canny.
void osl_canny_ox::Initial_followOX(int to,
                                    int from,
                                    osl_edgel_chain *&edgels_NMS,
                                    osl_LINK *links[],
                                    int *&status,
                                    float low)
{
  if (!status[to] && edgels_NMS->GetGrad(to) > low)
    status[to]=1;
  else
    return;

  for (osl_LINK *lptr=links[to]; lptr; lptr=lptr->nextl)
    if (lptr->to!=from)
      Initial_followOX(lptr->to,to,edgels_NMS,links,status,low);
}


//-------------------------------------------------------------------------
//
//: Add link. 'edgel' and 'to' are the indices of the two edges to be linked.
// A link with value 'to'    becomes the new head of the link-list for 'edgel'.
// A link with value 'edgel' becomes the new head of the link-list for 'to'.
void osl_canny_ox::Add_linkOX(int edgel,
                              int to,
                              osl_LINK *links[])
{
  osl_LINK *lptr1=new osl_LINK;
  lptr1->to=to;
  lptr1->nextl=links[edgel];
  links[edgel]=lptr1;

  osl_LINK *lptr2=new osl_LINK;
  lptr2->to=edgel;
  lptr2->nextl=links[to];
  links[to]=lptr2;
}


//-------------------------------------------------------------------------
//
//: Link edgels.
// First try pixels at distance 1 (direct neighbours),
// then at sqrt(2) (diagonal), then at 2 (horizontal or vertical), then
// at sqrt(5) (chess horse). I.e. in the following order:
// \verbatim
//         4 3 4
//       4 2 1 2 4
//       3 1 0 1 3
//       4 2 1 2 4
//         4 3 4
// \endverbatim
void osl_canny_ox::Link_edgelsOX(vcl_vector<unsigned> const &col,
                                 vcl_vector<unsigned> const &rows,
                                 osl_LINK *links[])
  // Rewritten and inline-documented by Peter Vanroose, 30 Dec. 1999.
{
  for (unsigned int i=0; i<ysize_; ++i) {// for each image row
    for (unsigned j=rows[i]; j<rows[i+1]; ++j) // for each edgel in this row
    {
      bool e=false; // set to true if next edgel is (horiz) neighbour
      bool s=false; // set to true if edgels are (vertical) neighbours

      // First link horizontal, direct neighbours:

      if (j+1<rows[i+1] && col[j]+1==col[j+1])  {  // next edgel is (horiz) neighbour
        Add_linkOX(j, j+1, links); e=true; }

      bool w=(j>rows[i] && col[j]==col[j-1]+1);// previous edgel was (horiz) neighbour

      // Don't go on (except for distance 2 neighbour) if there is certainly
      // no vertical neighbour edgel:
      if (rows[i+1] == rows[ysize_]) {
        if (e) continue;
        // Verify that there was no diagonal north-east link:
        if (i > 0) {
          unsigned int k=rows[i-1]; // candidate diagonal neighbour edgel
          while (k < rows[i] && col[k] < col[j]+1) ++k;
          if (k < rows[i] && col[k] == col[j]+1) continue; // found n-e link
        }
        // Now verify the east-2 link:
        if (j+1<rows[i+1] && col[j]+2==col[j+1])
          Add_linkOX(j, j+1, links);
        continue; // go on with the next value of j
      }

      // Now try to link vertically:

      unsigned int k=rows[i+1]; // candidate vertical (/ | or \) neighbour edgel

      while (k < rows[i+2] && col[k]+1 < col[j]) ++k; // skip the early ones

      // Note that rows[i+2] makes sense when rows[i+1]<rows[ysize_]
      if (k == rows[i+2]) continue; // no vertical or diagonal neighbour

      if (k+1 < rows[i+2] && col[k+1] == col[j]) ++k;
      if (col[j]==col[k])  {  // j and k are (vertical) neighbours
        Add_linkOX(j, k, links); s = true; }

      // Diagonal neighbours (distance sqrt(2)):

      bool se = false;
      if (!e && !s && col[j]+1==col[k])  {  // j and k are diagonal \ neigbours
        Add_linkOX(j,k,links); se = true; }
      if (!w && !s && col[j]==col[k]+1)  {  // j and k are diagonal / neigbours
        Add_linkOX(j,k,links); s = w = true; }
      if (se) s = e = true;

      // Verify if there was a diagonal north-east or north-west link:
      if (i > 0) {
        k=rows[i-1];
        while (k < rows[i] && col[k]+1 < col[j]) ++k;
        if (k < rows[i] && col[k]+1 == col[j])  w = true;
        while (k < rows[i] && col[k] < col[j]+1) ++k;
        if (k < rows[i] && col[k] == col[j]+1)  e = true;
      }

      if (e && w && s) continue;

      // Horizontal neighbours at distance 2:

      if (!e && j+1<rows[i+1] && col[j]+2==col[j+1]) {
        Add_linkOX(j, j+1, links); e = true; }

      if (j>rows[i] && col[j]==col[j-1]+2)
        w = true;

      // Vertical neighbour at distance 2:

      k=rows[i+2];
      if (!s && k < rows[ysize_]) {
        while (k < rows[i+3] && col[k] < col[j]) ++k;
        // Note that rows[i+3] makes sense if rows[i+2]<rows[ysize_]
        if (k < rows[i+3] && col[j] == col[k]) {
          Add_linkOX(j, k, links); s = true; }
      }

      if (e && w && s) continue;

      // Neighbours at distance sqrt(5):

      // First find the e-s-e and w-s-w neighbours:
      k=rows[i+1];
      while (k < rows[i+2] && col[k]+2 < col[j]) ++k;
      if (!w && k < rows[i+2] && col[j] == col[k]+2) {
        Add_linkOX(j, k, links); s = w = true; }
      while (k < rows[i+2] && col[k] < col[j]+2) ++k;
      if (!e && k < rows[i+2] && col[j]+2 == col[k]) {
        Add_linkOX(j, k, links); s = e = true; }

      if (s) continue;

      // And finally the s-s-e and s-s-w neighbours:
      k=rows[i+2]; if (k == rows[ysize_]) continue;
      while (k < rows[i+3] && col[k]+1 < col[j]) ++k;
      if (k < rows[i+3] && (col[j] == col[k]+1 || col[j]+1 == col[k]))
        Add_linkOX(j, k, links);

    } // end for j
  } // end for i
}


//-------------------------------------------------------------------------
//
//: Returns the number of edgels after hysteresis.
//
int osl_canny_ox::Get_n_edgels_hysteresisOX(osl_edgel_chain *&edgels_NMS,
                                            int *&status)
{
  int n_edgels_Hysteresis = 0;
  for (int i=edgels_NMS->size()-1; i>=0; --i)
    if (status[i])
      ++n_edgels_Hysteresis;
  return n_edgels_Hysteresis;
}


//-------------------------------------------------------------------------
//
//: Returns the edgels after hysteresis.
// Also fill the image thin_ with the edgels after hysteresis for
// further processing by the FollowerOX() part.
//
void osl_canny_ox::Get_hysteresis_edgelsOX(osl_edgel_chain *& edgels_NMS,
                                           int *&status,
                                           osl_edgel_chain *& edgels_Hysteresis,
                                           int *x_, int *y_)
{
  // Initialise thin_ to zero's
  osl_canny_base_fill_raw_image(thin_, xsize_, ysize_, 0.0f);

  unsigned int n_edgels_NMS = edgels_NMS->size();

  for (unsigned int i=0,j=0; i<n_edgels_NMS; ++i) {
    if (status[i]) {
      // Fill edgels_Hysteresis
      edgels_Hysteresis->SetX(edgels_NMS->GetX(i),j);
      edgels_Hysteresis->SetY(edgels_NMS->GetY(i),j);
      edgels_Hysteresis->SetGrad(edgels_NMS->GetGrad(i),j);
      edgels_Hysteresis->SetTheta(edgels_NMS->GetTheta(i),j);
      // Re-fill image thin_ for further processing
      thin_[x_[i]][y_[i]] = edgels_NMS->GetGrad(i);
      j++;
    } // end if
  } // end for i
}

//----------------------------------------------------------------------------
//
//:
// In the case of follow_strategy_OX_ = 0, this function
// returns an osl_edge * filled from osl_edgel_chain *edgels_Hysteresis.
// i.e., the result of the Hysteresis part of Canny is returned in the osl_edge *.
// The Follow part (FollowerOX) is not executed.
osl_edge *osl_canny_ox::NO_FollowerOX(osl_edgel_chain *edgels_Hysteresis)
{
  int n_edgels_Hysteresis = edgels_Hysteresis->size();

  // Define a digital curve to store the edgels data
  //  dc only stores the list of edgels after the Hysteresis stage of canny.
  //  dc is used later in this function to define an osl_edge *edge
  osl_edge *dc = new osl_edge(n_edgels_Hysteresis,
                              new osl_Vertex(edgels_Hysteresis->GetX(0)+xstart_,
                                             edgels_Hysteresis->GetY(0)+ystart_),
                              new osl_Vertex(edgels_Hysteresis->GetX(n_edgels_Hysteresis-1)+xstart_,
                                             edgels_Hysteresis->GetY(n_edgels_Hysteresis-1)+ystart_));

  float* px = dc->GetX();    float* py = dc->GetY();
  float* pg = dc->GetGrad(); float* pt = dc->GetTheta();

  for (int i=0; i<n_edgels_Hysteresis; ++i) {
    *(px++) = edgels_Hysteresis->GetX(i) + xstart_;
    *(py++) = edgels_Hysteresis->GetY(i) + ystart_;
    *(pg++) = edgels_Hysteresis->GetGrad(i);
    *(pt++) = edgels_Hysteresis->GetTheta(i);
  }

  return dc; //new osl_edge(dc);
}


//----------------------------------------------------------------------------
//
//: Returns the first osl_Vertex* in l which matches (i.e. compares equal to) v.
// returns 0 if none found.
osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, osl_Vertex const &v)
{
  for (vcl_list<osl_Vertex*>::const_iterator i=l->begin(); i!=l->end(); ++i)
    if (v == *(*i))
      return *i;
  return 0;
}

osl_Vertex *osl_find(vcl_list<osl_Vertex*> const *l, float x, float y)
{
  for (vcl_list<osl_Vertex*>::const_iterator i=l->begin(); i!=l->end(); ++i)
    if ((*i)->x == x && (*i)->y == y)
      return *i;
  return 0;
}


//----------------------------------------------------------------------------
//
//:
// Go through every point in the image and for every one which is above a
// threshold:   follow from the point, in one direction and then the other.
//
void osl_canny_ox::FollowerOX(vcl_list<osl_edge*> *edges)
{
  if (junction_option_OX_)
    chain_no_ = 10;    // Must be set to a number >= 1

  // temporaries used in loop
  vcl_list<int> xcoords, ycoords;
  vcl_list<float> grad;

  edges->clear();
  for (unsigned int x=border_size_OX_; x<xsize_-border_size_OX_; ++x) {
    for (unsigned int y=border_size_OX_; y<ysize_-border_size_OX_; ++y) {
      // Due to Initial_hysteresis we can follow everything > edge_min_OX_
      if ( (thin_[x][y] < edge_min_OX_) || junction_[x][y] )
        continue;

      if (junction_option_OX_)
        chain_no_++;

      // clear lists before accumulating edgels.
      xcoords.clear();
      ycoords.clear();
      grad.clear();

      // forward follow [? fsm]
      Final_followOX(x,y, &xcoords, &ycoords, &grad,0);

      // We may have picked up the edgel chain somewhere
      // away from its ends. Therefore, reverse the list
      // and try to follow again.
      xcoords.reverse();
      ycoords.reverse();
      grad.reverse();
      Final_followOX(x,y,&xcoords,&ycoords,&grad,1);

      int count=xcoords.size();
      if (count < min_length_OX_ || count < 1)
        // vcl_cerr << "short list found in Final_followOX\n";
        continue;

      // Create an osl_edgel_chain and add to the list
      osl_edgel_chain * dc = new osl_edgel_chain(count);
      float *px = dc->GetX();
      float *py = dc->GetY();
      float *pg = dc->GetGrad();
      float *pt = dc->GetTheta();

      // Write the points to the osl_edgel_chain and the end points to the Curve
      //dc->SetStart(xcoords.front()+xstart_, ycoords.front()+ystart_);
      int tmpx=0,tmpy=0;// dummy initialization, as count is always > 0.
      while (count--) {
        tmpx = xcoords.front(); xcoords.pop_front();
        tmpy = ycoords.front(); ycoords.pop_front();
        float val = grad.front(); grad.pop_front();
        // If we are not at a junction use sub-pixel value
        if ( val != jval_ ) {
          *(px++) = dx_[tmpx][tmpy] + xstart_;
          *(py++) = dy_[tmpx][tmpy] + ystart_;
          *(pg++) = val;
        }
        else {
          *(px++) = float(tmpx + xstart_);
          *(py++) = float(tmpy + ystart_);
          *(pg++) = 0.0f; // Mark the gradient as zero at a junction
        }
        *(pt++) = theta_[tmpx][tmpy];
      }
      //dc->SetEndX(tmpx+xstart_, tmpy+ystart_);

      // Just check whether we have created a trivial edgechain
      // can happen if min_length_OX_ = 2
      if ( (dc->size()==2) &&
           (dc->GetX(0)==dc->GetX(1)) &&
           (dc->GetY(0)==dc->GetY(1)) ) {
        //vcl_cerr << "trivial edgechain\n";
        delete dc;
        dc = 0;
        continue;
      }
      else if ( dc->size() > 1 ) {
        // Create an edge for the image topology
        osl_Vertex *v1 = new osl_Vertex(dc->GetX(0),dc->GetY(0));
        osl_Vertex *v2 = new osl_Vertex(dc->GetX(dc->size()-1),dc->GetY(dc->size()-1));

        if (junction_option_OX_) {
          // Check whether each vertex is a junction
          osl_Vertex *V1 = osl_find(vlist_, *v1);
          osl_Vertex *V2 = osl_find(vlist_, *v2);

          // If neither are junctions we may have formed a single
          // isolated chain that should have common vertex endpoints.
          bool single_chain = false;
          if ( !V1 && !V2 ) {
            float dx = dc->GetX(0) - dc->GetX(dc->size()-1);
            float dy = dc->GetY(0) - dc->GetY(dc->size()-1);
            if (dc->size() < 1 || dx*dx+dy*dy < 4) {// ie. dist < 2 pixels it is closed
              //edge = new osl_edge(v1,v1);
              delete v2; // osl_IUDelete(v2);
              v2 = v1;
              single_chain = true;
            }
          }

          if (!single_chain) {
            if (V1) { delete v1; v1 = V1; }//was: { osl_IUDelete(v1); v1 = V1; }
            if (V2) { delete v2; v2 = V2; }//was: { osl_IUDelete(v2); v2 = V2; }
            //edge = new osl_edge(v1,v2);
          }
        }
        else {
          // We may have formed a single isolated
          // chain that should have common vertex endpoints.
          float dx = dc->GetX(0) - dc->GetX(dc->size()-1);
          float dy = dc->GetY(0) - dc->GetY(dc->size()-1);
          if ((dx*dx+dy*dy) < 4.0) {// ie. dist < 2 pixels it is closed
            //edge = new osl_edge(v1,v1);
            delete v2; // osl_IUDelete(v2);
            v2 = v1;
          }
          //else
          //  edge = new osl_edge(v1,v2);
        }

        // Note that the edge can start and end in the same osl_Vertex.
        // However, if this is so the DigitalCurve has positive length
        //dc->SetStart(dc->GetX(0), dc->GetY(0));
        //dc->SetEnd(dc->GetX(dc->size()-1),dc->GetY(dc->size()-1));

        //vcl_cerr << __FILE__ ": push\n";
        edges->push_front(new osl_edge(*dc, v1, v2));
        delete dc;
      }
    }
  }
  //vcl_cerr << "edges->size() : " << edges->size() << vcl_endl;
}


//-----------------------------------------------------------------------------
//
//:
// Adds point (x, y) to the current curve, sets its value in the image to
// zero (any point may be included at most once in at most one curve)
// then searches for adjacent pixels (in an order intended to make closed
// curves ordered in a clockwise direction).
// If none are found, and we are not at a junction Join_dotsOX() is called to
// search for non-adjacent pixels.
// If a new point is found, a recursive call is made to Final_followOX() with
// that point.
//
// Two strategies can be followed according to follow_strategy_OX_
// if follow_strategy_OX_ = 1 then neighbours are checked in the following
//    order (Charlie Rothwell's way)
// \verbatim
//         8  7  6
//         1  x  5
//         2  3  4
// \endverbatim
// but if follow_strategy_OX_ = 2 then neighbours are checked in the following
//    order (Nic Pillow's way)
// \verbatim
//         8  6  7
//         1  x  4
//         3  2  5
// \endverbatim
//
void osl_canny_ox::Final_followOX(int x,
                                  int y,
                                  vcl_list<int> *xc,
                                  vcl_list<int> *yc,
                                  vcl_list<float> *grad,
                                  int reverse)
{
  // Make sure that we do not overrun the border of the image
  assert ( x>0 && y>0 );
  assert ( (unsigned)x+1<xsize_ );
  assert ( (unsigned)y+1<ysize_ );

  // Add the current point to the coordinate lists, and delete from
  // the edge image
  if (!reverse) {
    xc->push_front(x);
    yc->push_front(y);
    grad->push_front(thin_[x][y]);
  }
  thin_[x][y] = 0.0;

  bool junction_or_jump = false;

  switch (follow_strategy_OX_) {
  case 1: // charlie rothwell way
    // Find one adjacent pixel;  for a closed curve, the method `guarantees'
    // a clockwise ordering of the points in the image sense
    if (false) { }
#define smoo(a, b) \
    else if ( (thin_[a][b] >= edge_min_OX_) && (!junction_[a][b]) ) \
      Final_followOX(a,b,xc,yc,grad,0);
    smoo(x-1, y  )
    smoo(x-1, y+1)
    smoo(x  , y+1)
    smoo(x+1, y+1)
    smoo(x+1, y  )
    smoo(x+1, y-1)
    smoo(x  , y-1)
    smoo(x-1, y-1)
#undef smoo
    else
      junction_or_jump = true;
    break;

  case 2: default: // nic pillow way
    if (false) { }
#define smoo(a, b) \
    else if ( (thin_[a][b] >= edge_min_OX_) && (!junction_[a][b]) ) Final_followOX(a,b ,xc,yc,grad,0);
    smoo(x-1, y  )
    smoo(x  , y+1)
    smoo(x-1, y+1)
    smoo(x+1, y  )
    smoo(x+1, y+1)
    smoo(x  , y-1)
    smoo(x+1, y-1)
    smoo(x-1, y-1)
#undef smoo
    else
      junction_or_jump = true;
    break;
  } // end switch

  if (!junction_or_jump)
    return;

  // Else see if there is a junction nearby, and record it. The chain_no_
  // variable is used to prevent the same junction being inserted at both
  // ends of the edgel chains when reversel occurs next to the junction
  // (in that case there will only be two stored points: the edge and the junction)
  if (false) { }
#define smoo(a, b) \
  else if ( junction_[a][b] && ((xc->size()>2) || (junction_[a][b]!=chain_no_)) ) { \
    xc->push_front(jx_[a][b]); \
    yc->push_front(jy_[a][b]); \
    grad->push_front(jval_); \
    junction_[a][b] = chain_no_; \
  }
  smoo(x  , y-1)
  smoo(x-1, y  )
  smoo(x  , y+1)
  smoo(x+1, y  )
  smoo(x+1, y-1)
  smoo(x-1, y-1)
  smoo(x-1, y+1)
  smoo(x+1, y+1)
#undef smoo
  else if ( join_flag_OX_ && (xc->size() > 1) )  {
    // Try to find a pixel to jump to,
    //  and if successful, follow from it
    int x_c = xc->front(); xc->pop_front();
    int x_p = xc->front(); xc->pop_front();
    int y_c = yc->front(); yc->pop_front();
    int y_p = yc->front(); yc->pop_front();
    int dx = x_c - x_p;
    int dy = y_c - y_p;
    xc->push_front(x_p);  xc->push_front(x_c);
    yc->push_front(y_p);  yc->push_front(y_c);
    int xNew, yNew;
    if (Join_dotsOX(x, y, dx, dy, xNew, yNew))
      Final_followOX(xNew,yNew,xc,yc,grad,0);
  }
}


//-----------------------------------------------------------------------------

#if 0
void Set_intsOX(int& int1, int& int2, int val1, int val2)
{
  int1 = val1;
  int2 = val2;
}
#endif
#define Set_intsOX(d1, d2, v1, v2) ((d1)=(v1), (d2)=(v2))


//----------------------------------------------------------------------------
//
//:
// The point (x, y) is the current curve point;
// (dx, dy) is the increment in position to (x, y) from the preceding point.
// Based on dx and dy, it looks ahead in the same direction (or failing
// that,  similar directions) for another point.
// On success, xNew and yNew are set to the coordinates of the point found
// and true is returned;  otherwise false is returned.
//
int osl_canny_ox::Join_dotsOX(int x, int y, int dx, int dy, int& xNew, int& yNew)
{
  if ((vcl_abs(dx) > 1) || (vcl_abs(dy) > 1)) {
    //  If dx or dy is too large (> 1), meaning the last
    //   point was found by jumping, then jumping again
    //   will be too unreliable
    return false;
  }

  // Make sure that we do not overrun the border of the image
  assert( x>1 && y>1 );
  assert( (unsigned int)x+2<xsize_ );
  assert( (unsigned int)y+2<ysize_ );

  if (!dx && (vcl_abs(dy) == 1)) {
    if      (thin_[x  ][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x  , y+2*dy);
    else if (thin_[x+1][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+1, y+2*dy);
    else if (thin_[x-1][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x-1, y+2*dy);
    else if (thin_[x+2][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2, y+2*dy);
    else if (thin_[x-2][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x-2, y+2*dy);
    else if (thin_[x+2][y+  dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2, y+dy  );
    else if (thin_[x-2][y+  dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x-2, y+dy  );
    else if (thin_[x+2][y     ] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2, y     );
    else if (thin_[x-2][y     ] >= edge_min_OX_) Set_intsOX(xNew, yNew, x-2, y     );
    else return false;
  }
  else if ((vcl_abs(dx) == 1) && !dy) {
    if      (thin_[x+2*dx][y  ] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y  );
    else if (thin_[x+2*dx][y+1] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y+1);
    else if (thin_[x+2*dx][y-1] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y-1);
    else if (thin_[x+2*dx][y+2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y+2);
    else if (thin_[x+2*dx][y-2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y-2);
    else if (thin_[x+  dx][y+2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+  dx, y+2);
    else if (thin_[x+  dx][y-2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+  dx, y-2);
    else if (thin_[x     ][y+2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x     , y+2);
    else if (thin_[x     ][y-2] >= edge_min_OX_) Set_intsOX(xNew, yNew, x     , y-2);
    else return false;
  }
  else if (vcl_abs(dx*dy) == 1) {
    if      (thin_[x+2*dx][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y+2*dy);
    else if (thin_[x+2*dx][y+  dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y+  dy);
    else if (thin_[x+  dx][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+  dx, y+2*dy);
    else if (thin_[x+2*dx][y     ] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y     );
    else if (thin_[x     ][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x     , y+2*dy);
    else if (thin_[x+2*dx][y-  dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x+2*dx, y-  dy);
    else if (thin_[x-  dx][y+2*dy] >= edge_min_OX_) Set_intsOX(xNew, yNew, x-  dx, y+2*dy);
    else return false;
  }
  else
    return false;             // Should never be reached, but just in case...

  return true;
}


//-----------------------------------------------------------------------------
//
//: Multiply image by scale, and clip at 255.
//
void osl_canny_ox::Scale_imageOX(float **image, float scale)
{
  for (unsigned int x=0; x<xsize_; ++x)
    for (unsigned int y=0; y<ysize_; ++y)
      image[x][y] = vnl_math_min( image[x][y]*scale, 255.0f );
}


//-----------------------------------------------------------------------------
//
//: Set size of pixels around image (border) to value, so follow can't overrun.
//
void osl_canny_ox::Set_image_borderOX(float **image, int border_size, float value)
{
  assert(border_size >= 0);
  assert((unsigned int)border_size <= xsize_);
  assert((unsigned int)border_size <= ysize_);

  for (int i=0; i<border_size; ++i) {
    for (unsigned int x=0; x<xsize_; ++x)
      image[x][i] = image[x][ysize_-1-i] = value;
    for (unsigned int y=0; y<ysize_; ++y)
      image[i][y] = image[xsize_-1-i][y] = value;
  }
}


//-----------------------------------------------------------------------------
//
//: Searches for the junctions in the image.
//
void osl_canny_ox::Find_junctionsOX()
{
  // Reset the junction variables
  xjunc_->clear();     yjunc_->clear();
  osl_canny_base_fill_raw_image(junction_, xsize_, ysize_, 0);

  for (unsigned int x=border_size_OX_; x+border_size_OX_<xsize_; ++x)
    for (unsigned int y=border_size_OX_; y+border_size_OX_<ysize_; ++y) {
      if ( thin_[x][y] < edge_min_OX_ )
        continue;

      int a = ( thin_[x-1][y-1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x  ][y-1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x+1][y-1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x+1][y  ] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x+1][y+1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x  ][y+1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x-1][y+1] >= edge_min_OX_ ) ? 1 : 0;
      a +=    ( thin_[x-1][y  ] >= edge_min_OX_ ) ? 1 : 0;

      if ( a > 2 )  {
        xjunc_->push_front(x);  yjunc_->push_front(y);
        junction_[x][y] = 1;
      }
    }
}


//-----------------------------------------------------------------------------
//
//: Locate junction clusters using the following method of hysteresis.
//
void osl_canny_ox::Find_junction_clustersOX()
{
  vcl_list<int> xvertices,yvertices,xjunc,yjunc;

  // Find a junction and follow
  xvertices.clear();  yvertices.clear();
  xjunc.clear();      yjunc.clear();
  for (unsigned int x=border_size_OX_; x+border_size_OX_<xsize_; ++x)
    for (unsigned int y=border_size_OX_; y+border_size_OX_<ysize_; ++y)
      if ( junction_[x][y] ) {
        // Each cluster is written to (xcoords,ycooords)
        vcl_list<int> xcoords,ycoords;
        Follow_junctions(junction_, x,y,&xcoords,&ycoords);

        // Find the `centre' of the cluster. This is defined as the
        // junction closest to the centre of gravity of the cluster
        int x0,y0;
        Cluster_centre_of_gravity(jx_, jy_, xcoords,ycoords,x0,y0);

        // Add both the junctions and the new cluster centre to
        // the main lists
        xvertices.push_front(x0);
        yvertices.push_front(y0);
        xjunc.insert(xjunc.begin(), xcoords.begin(), xcoords.end()); //xjunc.prepend(xcoords);
        yjunc.insert(yjunc.begin(), ycoords.begin(), ycoords.end()); //yjunc.prepend(ycoords);
      }

  // Reset the junction image - this is order dependent because
  // the cluster centres appear in both lists
  // xjunc.reset();  yjunc.reset();
  while ( xjunc.size() ) {
    junction_[xjunc.front()][yjunc.front()] = 1;
    xjunc.pop_front();
    yjunc.pop_front();
  }

  // Construct the list of junction cluster centres
  typedef vcl_list<int>::iterator it;
  for (it i=xvertices.begin(), j=yvertices.begin(); i!=xvertices.end() && j!=yvertices.end(); ++i, ++j) {
    osl_Vertex *v = new osl_Vertex( float((*i)+xstart_), float((*j)+ystart_));
    vlist_->push_front(v);
    junction_[*i][*j] = 2;
  }
}
