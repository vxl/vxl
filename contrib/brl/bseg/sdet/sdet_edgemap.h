// This is brcv/seg/sdet/edge/sdet_edgemap.h
#ifndef sdet_edgemap_h
#define sdet_edgemap_h
//:
//\file
//\brief The edge map class
//\author Amir Tamrakar
//\date 09/09/06
//
//\verbatim
//  Modifications
//\endverbatim

#include <vector>
#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_point_2d.h>
#include <sdet/sdet_edgel.h>
#include <sdet/sdet_sel_utils.h>

//: very light class to store edgels
class sdet_edge
{
public:
  vgl_point_2d<double> pt; ///< the location of the edgel
  double tangent;          ///< the orientation of the edgel
  double strength;         ///< the strength of the edgel (typically gradient magnitude)
  int id;                  ///< unique id

  sdet_edge(vgl_point_2d<double> new_pt, double tan, double edge_strength=10.0):
    pt(new_pt), tangent(tan), strength(edge_strength) {}
  ~sdet_edge()= default;
};

typedef vbl_array_2d<std::vector<sdet_edgel*> >::iterator sdet_edgemap_iter;
typedef vbl_array_2d<std::vector<sdet_edgel*> >::const_iterator sdet_edgemap_const_iter;

//: A bucketing structure to hold the edgel tokens. Currently this is implemented
//  simply as a 2d array of tokens the same size as the image that gave rise to it.
//  For expediting linking from this data structure, I am using the sdet_edgel class
//  to store the edgel tokens instead of the sdet_edge class which is considerably lighter
//
class sdet_edgemap : public vbl_ref_count
{
public:

  //: retinotopic map of edgels
  vbl_array_2d<std::vector<sdet_edgel*> > edge_cells;

  //: local list of edgels for easier traversal
  std::vector<sdet_edgel*> edgels;

  //: edgel occupancy map (redundant structure)
  vbl_array_2d<bool> occupancy;

  //: constructor
  sdet_edgemap(int width, int height) : edgels(0) { edge_cells.resize(height, width); }

  //: constructor2
  sdet_edgemap(int width, int height, std::vector<sdet_edgel*>& edgels) : edgels(0)
  {
    edge_cells.resize(height, width);
    for (auto & edgel : edgels)
      insert(edgel);
  }

  //: destructor
  ~sdet_edgemap() override
  {
    //go over each cell and delete the edgels
    sdet_edgemap_const_iter it = edge_cells.begin();
    for (; it!=edge_cells.end(); it++)
      for (auto j : (*it))
        delete j;

    edge_cells.clear();

    //also clear the list of edgels
    edgels.clear();
  }

  //Access functions
  unsigned width() const { return edge_cells.cols(); }
  unsigned height() const { return edge_cells.rows(); }
  unsigned ncols() const  { return edge_cells.cols(); }
  unsigned nrows() const  { return edge_cells.rows(); }
  unsigned num_edgels() const { return edgels.size(); } ///< number of edgels in the edgemap

  //: read only access
  const std::vector<sdet_edgel*>& cell(int x, int y){ return edge_cells(y, x); }

  //: put an edgel into the edgemap at the prescribed cell
  void insert(sdet_edgel* e, int xx, int yy)
  {
    edge_cells(yy, xx).push_back(e);
    e->id = edgels.size(); //assign unique id
    e->gpt = vgl_point_2d<int>(xx, yy); //record grid location

    edgels.push_back(e);
  }

  //: put an edgel into the edgemap using subpixel coordinates
  void insert(sdet_edgel* e)
  {
    //determine appropriate cell to put this token into
    unsigned xx = sdet_round(e->pt.x());
    if(xx==0)
        xx++;
    if(xx>=width())
        xx=width()-1;
    unsigned yy = sdet_round(e->pt.y());
    if(yy==0)
        yy++;
    if(yy>=height())
        yy=height()-1;
    insert(e, xx, yy);
  }
};


#endif // sdet_edgemap_h
