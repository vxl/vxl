//:
// \file
#include <vul/vul_timer.h>

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>   // for vcl_abs(int)
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h> // for vcl_max()

#include <gevd/gevd_contour.h>
#include <gevd/gevd_region_edge.h>
#include <gevd/gevd_intensity_face.h>
#include <gevd/gevd_bufferxy.h>

#include <vil/vil_byte.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_memory_image_of.h>
#include <vnl/vnl_math.h>     // for sqrt()

#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_cycle_processor.h>

//#include <vdgl/vdgl_curve_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>

#include <gevd/gevd_detector.h>
#include <gevd/gevd_intensity_face.h>
#include "gevd_edgel_regions.h"


#define bytePixel(buf,x,y)   (*((unsigned char*)buf->GetElementAddr(x,y)))
#define shortPixel(buf,x,y)  (*((short*)buf->GetElementAddr(x,y)))

#if 0 // not ported to vxl
//----------------------------------------------------------------
//: cache debug info
void edgel_regions::cache_bad_edges(CoolArrayP<Edge*>& bad_edges)
{
  vcl_vector<Edge*> corrupt_edges;
  for (CoolArrayP<Edge*>::iterator eit = bad_edges.begin();
      eit != bad_edges.end(); eit++)
    corrupt_edges.push_back(*eit);
  corrupt_edges_.push_back(corrupt_edges);
}
void edgel_regions::cache_bad_edges(vcl_vector<Edge*>& bad_edges)
{
  vcl_vector<Edge*> corrupt_edges;
  for (vcl_vector<Edge*>::iterator eit = bad_edges.begin();
      eit != bad_edges.end(); eit++)
    corrupt_edges.push_back(*eit);
  corrupt_edges_.push_back(corrupt_edges);
}
void edgel_regions::cache_bad_verts(CoolArrayP<Vertex*>& bad_verts)
{
  vcl_vector<Vertex*> corrupt_verts;
  for (CoolArrayP<Vertex*>::iterator vit = bad_verts.begin();
      vit != bad_verts.end(); vit++)
    corrupt_verts.push_back(*vit);
  corrupt_vertices_.push_back(corrupt_verts);
}
#endif

//--------------------------------------------------------
//: compare function to be used with CoolList<float>::sort() [increasing]
static int increasing_compare(unsigned int const&a, unsigned int const&b)
{
  if (a > b) return 1;
  if (a < b) return -1;
  return 0;
}
//Print the region label array
void gevd_edgel_regions::print_region_array()
{
  vcl_cout << vcl_endl << vcl_endl;
  for (unsigned int y = _yo; y<=(_yend); y++)
    {
    //      vcl_cout << setw(2);
      for (unsigned int x = _xo; x<=(_xend); x++)
        if (_region_label_array[Y(y)][X(x)]==EDGE
           //&&_edge_boundary_array[Y(y)][X(x)]->IsVertex()
           )
          vcl_cout << "* " ;
        else
          vcl_cout << _region_label_array[Y(y)][X(x)] << " " ;
      vcl_cout << vcl_endl;
    }
  vcl_cout << vcl_endl << vcl_endl;
}
//Print the contents of the forward eqivalence index
void gevd_edgel_regions::print_region_equivalence()
{
  vcl_cout << vcl_endl << "Label Equivalence:\n"
           << "----------------\n";
  vcl_map<unsigned int, vcl_vector<unsigned int>*>::iterator rpf_iterator;
  for (rpf_iterator= _region_pairs_forward.begin(); rpf_iterator!=_region_pairs_forward.end(); rpf_iterator++)
    {
      vcl_cout << (*rpf_iterator).first << " == "
               << (*rpf_iterator).second << vcl_endl;
    }
  vcl_cout << vcl_endl;
}
//Print the contents of the reverse eqivalence index
void gevd_edgel_regions::print_reverse_region_equivalence()
{
  vcl_cout << vcl_endl << "Reverse Label Equivalence:\n"
           << "----------------\n";
  vcl_map<unsigned int, vcl_vector<unsigned int>*>::iterator rpf_iterator;
  for (rpf_iterator= _region_pairs_reverse.begin(); rpf_iterator!=_region_pairs_reverse.end(); rpf_iterator++)
    {
      vcl_cout << (*rpf_iterator).first << " == "
               << (*rpf_iterator).second << vcl_endl;
    }
  vcl_cout << vcl_endl;
}
//Print the reduced equivalence relation
void gevd_edgel_regions::print_base_equivalence()
{
  vcl_cout << vcl_endl << "Base Label Equivalence:\n"
           << "----------------\n";

  for (unsigned int i = _min_region_label; i<_max_region_label; i++)
    vcl_cout << i << " == "
             << this->BaseLabel(i) << vcl_endl;
}

//Print the fitted intensity data for all faces
void gevd_edgel_regions::print_intensity_data()
{
  for (vcl_vector<gevd_intensity_face*>::iterator fit =_faces->begin(); fit != _faces->end(); fit++)
    {
#if 0
    gevd_intensity_face* f = *fit;
      vcl_cout << "IntFaceAt(" << f->Xo() << " " << f->Yo() << "):\n";
      f->PrintFit();
#endif
    }
}

//--------------------------------------------------------------
//: The region array can be offset from the corner of the ROI.
//    The following two methods transform from the ROI coordinate
//    system to the region label array coordinate system
unsigned int gevd_edgel_regions::X(unsigned int x)
{
  return (x-_xo);
}
unsigned int gevd_edgel_regions::Y(unsigned int y)
{
  return (y-_yo);
}
//---------------------------------------------------------
//: Casts the float x location of a point to an unsigned-int location
unsigned int gevd_edgel_regions::Xf(float x)
{
  unsigned int xu = ((unsigned int)(x));
  return xu;
}
//---------------------------------------------------------
//: Casts the float x location of a point to an unsigned-int location
unsigned int gevd_edgel_regions::Yf(float y)
{
  unsigned int yu = ((unsigned int)(y));
  return yu;
}
//----------------------------------------------------------
//: Default constructor
gevd_edgel_regions::gevd_edgel_regions(bool debug)
{
  _verbose = false;
  _debug = debug;
#if 0
  if (_debug)
    _debug_data = new topo_debug_data;
  else
    _debug_data = 0;
#endif
    _image = NULL;
  _image_source = false;
  _buf_source = false;
  _buf = NULL;
  _edge_boundary_array = NULL;
  _region_label_array = NULL;
  _xo=0;
  _yo=0;
  _xend = 0;
  _yend = 0;
  _min_region_label = LABEL;
  _max_region_label = LABEL;
  _faces = new vcl_vector<gevd_intensity_face*>;
  _face_edge_index = NULL;
  _intensity_face_index = NULL;
  _failed_insertions = new vcl_vector<vtol_edge_2d_sptr>;
  _ubuf = NULL;
  _sbuf = NULL;
}

//----------------------------------------------------------
//: Default destructor
gevd_edgel_regions::~gevd_edgel_regions()
{
  if (_image) {
#if 0
    _image->unref();
#endif
    _image = NULL;
  }

  unsigned int y;
  if (_region_label_array)
    for (y=_yo; y<=(_yend); y++)
      delete [] _region_label_array[Y(y)];
  delete [] _region_label_array;

#if 0
  for (_region_pairs_reverse.reset(); _region_pairs_reverse.next();)
    delete _region_pairs_reverse.value();

  for (_region_pairs_forward.reset(); _region_pairs_forward.next();)
    delete _region_pairs_forward.value();
  for (_equivalence_set.reset(); _equivalence_set.next();)
    delete _equivalence_set.value();

  for (_region_edges.reset(); _region_edges.next();)
    {
      _region_edges.value()->UnProtect();
    }

   if (_edge_boundary_array)
     for (y = _yo; y<=_yend; y++)
       {
         for (x = _xo; x<=_xend; x++)
           if (_edge_boundary_array[Y(y)][X(x)])
             _edge_boundary_array[Y(y)][X(x)]->UnProtect();
         delete [] _edge_boundary_array[Y(y)];
       }
   delete [] _edge_boundary_array;


  for (_region_edge_adjacency.reset(); _region_edge_adjacency.next();)
    delete _region_edge_adjacency.value();

  for (_faces->reset(); _faces->next();)
    _faces->value()->UnProtect();
  delete _faces;

  if (_intensity_face_index)
    {
      for (unsigned int i = 0; i<_max_region_label; i++)
        if (_intensity_face_index[i])
          _intensity_face_index[i]->UnProtect();
      delete [] _intensity_face_index;
    }
#endif


  if (_face_edge_index)
    {
      for (unsigned int i = 0; i<_max_region_label; i++)
        delete _face_edge_index[i];
      delete [] _face_edge_index;
    }
  delete _failed_insertions;
  delete [] _ubuf;
  delete [] _sbuf;
}

bool gevd_edgel_regions::compute_edgel_regions(gevd_bufferxy* buf,
                                               vcl_vector<vsol_spatial_object_2d*>& sgrp,
                                               vcl_vector<gevd_intensity_face*>& faces)
{
  _buf = buf;
  _image= NULL;
  _buf_source=true;
  _image_source=false;
  return compute_edgel_regions(sgrp, faces);
}
//-----------------------------------------------------------------
//: The key process loop.
//  Carries out the steps:
//   1) Connected components 2)Edge-label assignment 3)Collect region
//   boundaries 4) Construct gevd_intensity_faces 5)Calculate intensity fit
bool
gevd_edgel_regions::compute_edgel_regions(vil_image* image, vcl_vector<vsol_spatial_object_2d *>& sgrp,
                                     vcl_vector<gevd_intensity_face*>& faces)
{
  _image = image;
  //  _image->ref();
  _buf = NULL;
  _image_source=true;
  _buf_source = false;
  return compute_edgel_regions(sgrp, faces);
}
bool gevd_edgel_regions::compute_edgel_regions(vcl_vector<vsol_spatial_object_2d *>& sgrp,
                                               vcl_vector<gevd_intensity_face*>& faces)
{
//    corrupt_edges_.clear();
//    corrupt_vertices_.clear();
#if 0
  if (_debug_data)
    {
      _debug_data->clear();
      if (_buf_source)
        _debug_data->set_buf(_buf);
    }
#endif
  vul_timer t;
  if (!sgrp.size())
    return false;

  //Set up the region label array with edge boundaries
  this->InitRegionArray(sgrp);
  if (_verbose)
    this->print_region_array();
  //Propagate connected components
  unsigned int y, x;
  for (y=_yo; y<_yend; y++)
    for (x=_xo; x<_xend; x++)
      this->UpdateConnectedNeighborhood(X(x), Y(y));
  if (_verbose)
    this->print_region_array();
  //Resolve region label equivalence
  this->GrowEquivalenceClasses();
  this->PropagateEquivalence();
  if (_verbose)
    this->print_base_equivalence();
  this->ApplyRegionEquivalence();
  if (_verbose)
    this->print_region_array();

  //Associate Edge(s) with region labels
  for (y=_yo; y<_yend; y++)
    for (x=_xo; x<_xend; x++)
      this->AssignEdgeLabels(X(x), Y(y));

  //Collect Edge(s) bounding each region
  this->CollectEdges();
  vcl_cout << "Propagate Regions and Collect Edges("
           << _max_region_label-_min_region_label << ") in "
           << t.real() << " msecs.\n";

  //Collect Face-Edge associations
  this->CollectFaceEdges();

  //Construct gevd_intensity_faces
  this->ConstructFaces();
  if (!_faces||!_faces->size())
    return false;

  //Collect intensity data for each region
  this->InsertFaceData();

  if (_verbose)
    this->print_intensity_data();
  //Output the result
  faces.clear();
  for (vcl_vector<gevd_intensity_face*>::iterator fit = _faces->begin(); fit != _faces->end(); fit++)
      {
      faces.push_back(*fit);
        //        _faces->value()->Protect(); //This caused a big leak
      }
  vcl_cout << "Compute Edgel Regions Total(" << sgrp.size() << ") in "
           << t.real() << " msecs.\n";
  return true;
}


//----------------------------------------------------------
//: assign eqivalence of region label b to region label a
//
bool gevd_edgel_regions::InsertRegionEquivalence(unsigned int label_b,
                                                 unsigned int label_a)
{
  bool result = true;
  result = result && this->add_to_forward(label_b, label_a);
  result = result && this->add_to_reverse(label_a, label_b);
  return result;
}

//--------------------------------------------------------------
//: Get the most basic label equivalent to a given label.
//    If the _label_map is not defined, this function uses the
//    forward label hash table.  Otherwise 0 is returned.
unsigned int gevd_edgel_regions::BaseLabel(unsigned int label)
{
 vcl_map<unsigned int, unsigned int >::iterator lmtest;
 lmtest = _label_map.find(label);
 if ( lmtest != _label_map.end() )
   return lmtest->second;
 else
   return 0;
}

//------------------------------------------------------------
//:
//  Paint the edgels into the region label array and then
//    output an image where the value is 255 if the pixel is an
//    edge, 0 otherwise
vil_image* gevd_edgel_regions::GetEdgeImage(vcl_vector<vsol_spatial_object_2d *>& sg)
{
  if (!this->InitRegionArray(sg)) return NULL;
  int sizex = this->GetXSize(), sizey = this->GetYSize();
  unsigned char no_edge = 0, edge = 255;
#if 0
  ImageTemplate temp(sizex, sizey, 8, 32, 32);
  MemoryImage* image = new MemoryImage(&temp);
  for (int y = 0; y<sizey; y++)
    for (int x = 0; x<sizex; x++)
      if (_region_label_array[y][x] == EDGE)
        image->PutPixel(&edge, x, y);
      else
        image->PutPixel(&no_edge, x, y);
#endif
  vil_memory_image_of<vil_byte> * image = new vil_memory_image_of<vil_byte>(sizex,sizey);

  for (int y = 0; y<sizey; y++)
    for (int x = 0; x<sizex; x++)
      if (_region_label_array[y][x] == EDGE)
        (*image)[y][x] = edge;
      else
        (*image)[y][x] = no_edge;
  return image;
}
//-----------------------------------------------------------
//: Populate the _label_map to reflect the equivalences between labels.
//
void gevd_edgel_regions::PropagateEquivalence()
{
  vcl_map<unsigned int, vcl_vector<unsigned int>* >::iterator  esi;

  for (esi = _equivalence_set.begin(); esi != _equivalence_set.end(); esi++)
    {
      unsigned int base = esi->first;
      vcl_vector<unsigned int>* labels = esi->second;
      if (!labels) continue;
      int len = labels->size();
      if (!len) continue;
      for (int i = 0; i<len; i++)
        _label_map[(*labels)[i]] = base;
    }
  for (unsigned int i = _min_region_label; i<_max_region_label; i++)
    if (_label_map.find(i) == _label_map.end())
      _label_map[i] = i;
}
//---------------------------------------------------------------------
//:
//  Find the set of labels equivalent to label from a given hash table
//    and merge into the appropriate equivalence set. cur_label is the
//    equivalence set being formed. label is the table key of equivalences
//    to be merged.
bool gevd_edgel_regions::
merge_equivalence(vcl_map<unsigned int, vcl_vector<unsigned int>* >& tab,
                  unsigned int cur_label,
                  unsigned int label)
{
  vcl_map<unsigned int, vcl_vector<unsigned int>* >::iterator hashi;

  hashi = tab.find(label);
  if (hashi == tab.end()) {
  return false;
  }
  vcl_vector<unsigned int>* labels = hashi->second;

  int len = labels->size();
  if (!len) {
  return false;
  }
  vcl_vector<unsigned int>* array = NULL;

  hashi = _equivalence_set.find(cur_label);
  if ( hashi == _equivalence_set.end())
    {
      array = new vcl_vector<unsigned int>;
      _equivalence_set[cur_label] = array;
    }
  else
    {
    array = hashi->second;
    }
  bool found = false;
  for (int i = 0; i<len; i++)
    {
    unsigned int l = (*labels)[i];
    found = false;
    for (unsigned int j=0 ; j < array->size() ; j++)
      {
      if ((*array)[j] == l) found = true;
      }
    if (!found)
      {
      array->push_back(l);
      }
    }

  return true;
}
//----------------------------------------------------------------
//: Find the next label not accounted for in the current equivalence set.
//    The set of labels is searched to find a label larger than label, but
//    not in the set, labels.
bool gevd_edgel_regions::get_next_label(vcl_vector<unsigned int>* labels,
                                        unsigned int& label)
{
  unsigned int tmp = label+1;
  if (!labels)
    if (tmp<_max_region_label)
      {
        label = tmp;
        return true;
      }
  for (unsigned int i = tmp; i<_max_region_label; i++)
    {
    bool found = false;
    for ( unsigned int j = 0 ; j < labels->size() ; j++ )
      {
      if ((*labels)[j] == i)
        {
        found = true;
        }
      }
    if (!found)
      {
        label = i;
        return true;
      }
    }
  return false;
}
//----------------------------------------------------------------
//: Form equivalence classes by transitive closure on each label.
//    The idea is to add labels to the set, cur_set, by equivalence until no
//    new equivalence groups can be found.  The current equivalence class,
//    cur_set is repeatedly scanned whan new labels are added to pick up
//    new equivalence groups.  Old equivalence table entries are removed
//    as they are used.  When the equivalence class corresponding to cur_lable
//    is completely closed, then a new label not in a previously formed
//    equivalence class is used to seed a new equivalence class.
void gevd_edgel_regions::GrowEquivalenceClasses()
{
  if ((_max_region_label-_min_region_label) < 2)
    return;
  unsigned int cur_label = _min_region_label;
  vcl_map<unsigned int, vcl_vector<unsigned int>* >::iterator mei;
  while (true)
    {
      bool merging = true;
      vcl_vector<unsigned int>* cur_set = NULL;
      unsigned int i = cur_label;
      int len = 0;
      int old_len = 0;
      while (merging)
      {
        old_len = len;
        merging = false;
        bool find_forward =
          this->merge_equivalence(_region_pairs_forward, cur_label, i);
        if (find_forward)
          _region_pairs_forward.erase(i);
        bool find_reverse =
          this->merge_equivalence(_region_pairs_reverse, cur_label, i);
        if (find_reverse)
          _region_pairs_reverse.erase(i);
        mei = _equivalence_set.find(cur_label);
        if (mei == _equivalence_set.end()) continue;
        cur_set = _equivalence_set[cur_label];
        if (!cur_set) continue;
        len = cur_set->size();
        if (!len) continue;
        if (len > old_len)
        {
          i = cur_label;
          vcl_sort(cur_set->begin(), cur_set->end(), &increasing_compare);
#if 0
          cur_set->sort(increasing_compare);
          //-tpk- need to convert
#endif
        }

        for (int j = 0; j<len&&!merging; j++)
          if ((*cur_set)[j]>i)
          {
            i = (*cur_set)[j];
            merging = true;
          }
      }
      if (!get_next_label(cur_set, cur_label)) return;
    }
}
//------------------------------------------------------------
//: Check if the SpatialGroup contains Edge(s)
//
bool gevd_edgel_regions::GroupContainsEdges(vcl_vector<vsol_spatial_object_2d*>& sg)
{
#if 0
  CoolString type(sg.GetSpatialGroupName());
  return (type == CoolString("EdgelGroup")||
          type == CoolString("FittedEdgeGroup"));
#endif
  return true; // TODO
}
//Advance along a line and generate continguous pixels on the line.
//This function assures that the spatial digitization of the Edge(s)
//produces completely connected boundaries.
static bool line_gen(float xs, float ys, float xe, float ye,
                     unsigned int& x, unsigned int& y)
{
  assert(xs > 0.0f); assert(ys > 0.0f);
  const float pix_edge = 1.0f; //We are working at scale = 1.0
  static float xi, yi;
  static bool init = true;
  static bool done = false;
  if (init)
    {
      xi = xs;
      yi = ys;
      x = (unsigned int)(xi/pix_edge);
      y = (unsigned int)(yi/pix_edge);
      init = false;
      return true;
    }
  if (done) return false;
  float dx = xe-xs;
  float dy = ye-ys;
  float mag = vcl_sqrt(dx*dx + dy*dy);
  if (mag<pix_edge)//Can't reach the next pixel under any circumstances
    {             //so just output the target, xe, ye.
      x = (unsigned int)xe; y = (unsigned int)ye;
      done = true;
      return true;
    }
  float delta = (0.5f*pix_edge)/mag; //move in 1/2 pixel increments
  //Previous pixel location
  int xp = int(xi/pix_edge);
  int yp = int(yi/pix_edge);
  //Increment along the line until the motion is greater than one pixel
  for (int i = 0; i<3; i++)
    {
      xi += dx*delta;
      yi += dy*delta;
      //Check for end of segment, make sure we emit the end of the segment
      if ((xe>=xs&&xi>xe)||(xe<=xs&&xi<xe)||(ye>=ys&&yi>ye)||(ye<=ys&&yi<ye))
        {
          x = (unsigned int)xe; y = (unsigned int)ye;
          done = true;
          return true;
        }
      //Check if we have advanced by more than .5 pixels
      x = (unsigned int)(xi/pix_edge);
      y = (unsigned int)(yi/pix_edge);
      if (vcl_abs(int(x)-xp)>(.5*pix_edge)||vcl_abs(int(y)-yp)>(.5*pix_edge))
        return true;
    }
  vcl_cout << "in gevd_edgel_regions line_gen: - shouldn't happen\n";
  return false;
}
//----------------------------------------------------------
//: A utility for inserting an edgel into the _region_label_array.
//    An edgel and a previous edgel in the chain are used to interpolate
//    intermediate edgels to take account of pixel quantization
bool gevd_edgel_regions::insert_edgel(float pre_x, float pre_y,
                                      float xedgel, float yedgel, gevd_region_edge* re)
{
  unsigned int xinterp, yinterp;
  bool edge_insert = false;
  while (line_gen(pre_x, pre_y, xedgel, yedgel, xinterp, yinterp))
    {
      if (out_of_bounds(X(xinterp), Y(yinterp)))
        {
          vcl_cout << "In gevd_edgel_regions::insert_edgel - out of bounds "
                   << "at (" << xinterp << " " << yinterp << ")\n";
          continue;
        }

      _region_label_array[Y(yinterp)][X(xinterp)] = EDGE;

      if (!re) continue;
      gevd_region_edge* old_re = _edge_boundary_array[Y(yinterp)][X(xinterp)];
      if (old_re&&old_re->get_edge())
        {
        //        old_re->UnProtect();
          _edge_boundary_array[Y(yinterp)][X(xinterp)] = re;
          //      re->Protect();
          edge_insert = true;
        }
      if (!old_re)
        {
          _edge_boundary_array[Y(yinterp)][X(xinterp)] = re;
          //      re->Protect();
          edge_insert = true;
        }
    }
  return edge_insert;
}

int gevd_edgel_regions::bytes_per_pix()
{
  int bypp = 1;
  if (_image_source)
    bypp = (_image->components() / _image->bits_per_component());

  if (_buf_source)
    bypp = _buf->GetBytesPixel();
  return bypp;
}
//-----------------------------------------------------------
//: Initialize the region label array.
//  There are three types of region label symbols:
//  1) UNLABELED - no label has yet been assigned,
//  2) EDGE, the existence of an edgel boundary pixel.
//  3) An unsigned integer which represents an existing region label.

bool gevd_edgel_regions::InitRegionArray(vcl_vector< vsol_spatial_object_2d *>& sg)
{
  if (!this->GroupContainsEdges(sg))
    return false;

  double xmin;
  double ymin;
  double xmax;
  double ymax;
  vsol_box_2d *b;
  vcl_vector<vsol_spatial_object_2d *>::iterator i;

  for (i=sg.begin();i!=sg.end();++i)
    {
      b=(*i)->get_bounding_box();
      if (i==sg.begin())
        {
          xmin=b->get_min_x();
          ymin=b->get_min_y();
          xmax=b->get_max_x();
          ymax=b->get_max_y();
        }
      else
        {
          if (b->get_min_x()<xmin)
            xmin=b->get_min_x();
          if (b->get_min_y()<ymin)
            ymin=b->get_min_y();
          if (b->get_max_x()>xmax)
            xmax=b->get_max_x();
          if (b->get_max_y()>ymax)
            ymax=b->get_max_y();
        }
      delete b;
    }

  //Get the size of the arrays
  _xo = (unsigned int)xmin;
  _yo = (unsigned int)ymin;

  _xend = (unsigned int)xmax;
  _yend = (unsigned int)ymax;

  unsigned int sizex = _xend - _xo + 1;
  unsigned int sizey = _yend - _yo + 1;
  //Construct the arrays
  _edge_boundary_array = new gevd_region_edge**[sizey];
  unsigned int y;
  for (y = _yo; y<=(_yend); y++)
    _edge_boundary_array[Y(y)] = new gevd_region_edge*[sizex];

  _region_label_array = new unsigned int*[sizey];
  for (y = _yo; y<=(_yend); y++)
    _region_label_array[Y(y)] = new unsigned int[sizex];

  switch(this->bytes_per_pix())
    {
    case 1:     // Grey scale image with one byte per pixel
      _ubuf = new unsigned char[sizex];
      break;
    case 2:     // Grey scale image with an unsigned short per pixel
      {
        _sbuf = new unsigned short[sizex];
        break;
      }
    default:
      vcl_cout<<"In gevd_intensity_face::get_intensity(): bytes/pixel not 1 or 2\n";
    }

  //Intialize the arrays
  unsigned int x;
  if (_ubuf)
    for (x = _xo; x<=(_xend); x++)
      _ubuf[X(x)] = 0;

  if (_sbuf)
    for (x = _xo; x<=(_xend); x++)
      _sbuf[X(x)] = 0;

  for (y = _yo; y<=(_yend); y++)
    for (x = _xo; x<=(_xend); x++)
      {
        _region_label_array[Y(y)][X(x)] = UNLABELED;
        _edge_boundary_array[Y(y)][X(x)] = NULL;
      }
  //Insert edgels into arrays.

  int counter=0;
  for (vcl_vector<vsol_spatial_object_2d *>::iterator sgit = sg.begin(); sgit != sg.end(); sgit++)
    {
    vtol_edge_2d* e = (*sgit)->cast_to_topology_object()->cast_to_edge()->cast_to_edge_2d();
      if (!e)
        continue;
      e->set_id(counter++);
      vdgl_digital_curve* dc = e->cast_to_edge_2d()->curve()->cast_to_digital_curve();
      if (!dc)
        continue;
      //The gevd_region_edge enables the link between a region label and
      //an Edge from the input segmentation
      gevd_region_edge* re = new gevd_region_edge(e);
      _region_edges[e->get_id()] = re;
      //      e->Protect(); re->Protect();
      //      float * ex = dc->GetX();
      //      float * ey = dc->GetY();
      vdgl_edgel_chain * xy= dc->get_interpolator()->get_edgel_chain().ptr();
      int n_edgels = xy->size();

      //Insert the vertices at the ends of the edge
      //If there is a gap between the vertex locations and the
      //nearest edgels, the insert_edgel function will generate
      //edge insertions to prevent any gaps in the region boundary
      //There shouldn't be DigitalCurve(s) with this defect but it
      //does seem to occur.
      gevd_region_edge* vre = new gevd_region_edge(NULL);
      float pex1, pey1, pex2, pey2;
      if (n_edgels>0)
        {
          pex1 = (*xy)[0].x(); pey1 = (*xy)[0].y();
          pex2 = (*xy)[n_edgels-1].x(); pey2 = (*xy)[n_edgels-1].y();
          this->insert_edgel(pex1, pey1,
                             e->v1()->cast_to_vertex_2d()->x(), e->v1()->cast_to_vertex_2d()->y(), vre);
          this->insert_edgel(pex2, pey2,
                             e->v2()->cast_to_vertex_2d()->x(), e->v2()->cast_to_vertex_2d()->y(), vre);
        }
      else
        {
          pex1 = e->v1()->cast_to_vertex_2d()->x(); pey1 = e->v1()->cast_to_vertex_2d()->y();
          pex2 = e->v2()->cast_to_vertex_2d()->x(); pey2 = e->v2()->cast_to_vertex_2d()->y();
          this->insert_edgel(pex1, pey1, pex1, pey1, vre);
          this->insert_edgel(pex1, pey1, pex2, pey2, vre);
        }
      //Insert the edgels between the vertices
      //An edgel cannot be inserted on top of an existing vertex
      //insertion.  A vertex is distinguished by having a NULL Edge.
      //That is, for vertices it is ambiguous what Edge instance
      //exists at that location. Edge insertion fails if no edgels
      //of the Edge can be sucessfully inserted.
      bool edge_insert = false;
      for (int k =0; k < n_edgels; k++)
        {
          bool inserted = this->insert_edgel(pex1, pey1, (*xy)[k].x(), (*xy)[k].y(), re);
          edge_insert = edge_insert || inserted;
          pex1 = (*xy)[k].x();
          pey1 = (*xy)[k].y();
        }
      if (!edge_insert)
        {
          vcl_cout << "Edge Insert Failed for (" << e->v1() << " "
                   << e->v2() << ")N: "<< n_edgels << vcl_endl;
          _failed_insertions->push_back(e);
        }
    }
  return true;
}

//---------------------------------------------------------------
//: Get code for a given label.
//
unsigned char gevd_edgel_regions::label_code(unsigned int label)
{
  unsigned char result = 0;
  if (label<_min_region_label)
    result = label;
  else
    result = LABEL;
  return result;
}
//----------------------------------------------------------------
//: Add a new pair to the forward equivalence list.
//    That is, a ==> b. Note that there can be multiple equivalences which are
//    stored in a vcl_vector
bool gevd_edgel_regions::add_to_forward(unsigned int key, unsigned int value)
{
  bool result = true;
  vcl_map<unsigned int, vcl_vector<unsigned int>* >::iterator rpfi;
  rpfi = _region_pairs_forward.find(key);

  if (rpfi !=_region_pairs_forward.end())
    {
    vcl_vector<unsigned int> * vec = _region_pairs_forward[key];
    bool found = false;
    for (unsigned int i =0 ; i < vec->size() ; i++)
      {
      if ((*vec)[i] == value)
        found = true;
      }
    if (!found)
      {
      vec->push_back(value);
      }
    }
  else
    {
      vcl_vector<unsigned int>* larray = new vcl_vector<unsigned int>;
      larray->push_back(value);
       _region_pairs_forward[key]=larray;
    }
  return result;
}

//----------------------------------------------------------------
//: Add a new pair to the reverse equivalence list.
//    That is, b==>a. Note that there can be multiple equivalences which are
//    stored in a vcl_vector
//
bool gevd_edgel_regions::add_to_reverse(unsigned int key, unsigned int value)
{
  bool result = true;
  vcl_map<unsigned int, vcl_vector<unsigned int>* >::iterator rpfi;
  rpfi = _region_pairs_reverse.find(key);

  if (rpfi !=_region_pairs_reverse.end())
    {
    vcl_vector<unsigned int> * vec = _region_pairs_reverse[key];
    bool found = false;
    for (unsigned int i =0 ; i < vec->size() ; i++)
      {
      if ((*vec)[i] == value)
        found = true;
      }
    if (!found)
      {
      vec->push_back(value);
      }
    }
  else
    {
      vcl_vector<unsigned int>* larray = new vcl_vector<unsigned int>;
      larray->push_back(value);
       _region_pairs_reverse[key]=larray;
    }
  return result;
}
//-------------------------------------------------------------------
//: Encode a 2x2 neighbor hood with the state of the region array for a given location.
//    The Neighborhood    The states are:
//         ul ur          UNLABELED, EDGE, LABEL
//         ll lr              0       1      2
//    The encoding maps the 2x2 pattern to an unsigned char.  The layout
//    of the uchar is:  [ul|ur|ll|lr] with 2 bits for the state of each
//    position.
unsigned char
gevd_edgel_regions::EncodeNeighborhood(unsigned int ul, unsigned int ur,
                                       unsigned int ll, unsigned int lr)
{
  unsigned char nhood = 0, nul = 0, nur = 0, nll = 0, nlr =0;

  nul = label_code(ul);
  nul = nul<<6;
  nhood |= nul;

  nur = label_code(ur);
  nur = nur<<4;
  nhood |= nur;

  nll = label_code(ll);
  nll = nll << 2;
  nhood |= nll;

  nlr = label_code(lr);
  nhood |= nlr;

  return nhood;
}
//----------------------------------------------------------------------
// --This is the fundamental assignment of label equivalence
//
void gevd_edgel_regions::insert_equivalence(unsigned int ll, unsigned int ur, unsigned int& lr)
{
  if (ll!=ur)//Is a=b?
    {//No. We want the smallest label to be the equivalence base
      //Also we don't want to loose earlier equivalences
      unsigned int smaller_label, larger_label;
      //Get the ordering right
      if (ll>ur)
        {smaller_label = ur; larger_label = ll;}
      else
        {smaller_label = ll; larger_label = ur;}
      this->add_to_forward(larger_label, smaller_label);
      this->add_to_reverse(smaller_label, larger_label);
      lr = smaller_label;
    }
  else //yes, a=b the simple case
    lr = ur;
}
//-------------------------------------------------------------------
//: Propagate connected components.
//    Uses an unsigned char encoding
//    a 2x2 neighborhood to propagate region labels. For example:
//    aa ->   aa
//    xx ->   aa
//    Here the lower two labels are set to the upper label.
//    This method operates directly on the _region_label_array.
//
//    Note that the 2x2 neighborhood is encoded as uchar = [ul|ur|ll|lr]
//                                                          2  2  2  2  bits
void gevd_edgel_regions::UpdateConnectedNeighborhood(unsigned int x, unsigned int y)

{
  unsigned int ul = _region_label_array[y][x];
  unsigned int ur = _region_label_array[y][x+1];
  unsigned int& ll = _region_label_array[y+1][x];
  unsigned int& lr = _region_label_array[y+1][x+1];

  unsigned char nhood = this->EncodeNeighborhood(ul, ur, ll, lr);
  switch(int(nhood))
    {
    case 0:
      //xx xx
      //xx xx
      return;
    case 1:
      //xx xx
      //xe xe
      return;
    case 5:
      //xx xx
      //ee ee
      return;

    case 17:
      //xe xe
      //xe xe
      return;

    case 20:
      //xe xe
      //ex ea
      lr = _max_region_label++;
      return;

    case 21:
      //xe xe
      //ee ee
      return;

    case 22:
      //xe xe
      //ea ea
      return;

    case 68:
      //ex ex
      //ex ex
      return;

    case 72:
      //ex ex
      //ax ax
      return;

    case 133:
      //ax aa
      //ee ee
      return;

    case 136:
      //ax ax
      //ax ax
      return;

    case 160:
      //aa aa
      //xx aa
      ll = ul;
      lr = ul;
      return;

    case 161:
      //aa aa
      //xe ae
      ll = ul;
      return;

    case 168:
      //aa aa
      //ax aa
      insert_equivalence(ll, ur, lr);
      return;

    case 169:
      //aa aa
      //ae ae
      return;

    case 164:
      //aa aa
      //ex ea
      lr = ul;
      return;

    case 165:
      //aa aa
      //ee ee
      return;

    case 144:
      //ae ae
      //xx aa
      ll = ul;
      lr = ul;
      return;

    case 145:
      //ae ae
      //xe ae
      ll = ul;
      return;

    case 152:
      //ae ae
      //ax aa
      lr = ul;
      return;

    case 153:
      //ae ae
      //ae ae
      return;

    case 148:
      //ae ae
      //ex eb
      lr = _max_region_label++;
      return;

    case 149:
      //ae ae
      //ee ee
      return;

    case 96:
      //ea ea
      //xx aa
      ll = ur;
      lr = ur;
      return;

    case 97:
      //ea ea
      //xe be
      ll = _max_region_label++;
      return;

    case 104:
      //ea ea
      //bx aa
      insert_equivalence(ll, ur, lr);
      return;

    case 105:
      //ea ea
      //ae ae
      return;

    case 100:
      //ea ea
      //ex ea
      lr = ur;
      return;

    case 101:
      //ea ea
      //ee ee
      return;

    case 80:
        //ee ee
        //xx bb
      ll = _max_region_label++;
      lr = ll;
      return;

    case 81:
      //ee ee
      //xe be
      ll = _max_region_label++;
      return;

    case 88:
      //ee ee
      //ax aa
      lr = ll;
      return;

    case 89:
      //ee ee
      //ae ae
      return;

    case 84:
      //ee ee
      //ex eb
      lr = _max_region_label++;
      return;

    case 85:
        //ee ee
        //ee ee
        return;
    default:
      vcl_cout << "In gevd_edgel_regions::UpdateNeigborhood(..)"
               << "impossible pattern = " << (int)nhood
               << vcl_endl;
    }
}
static bool reg_edges_neq(gevd_region_edge* r1, gevd_region_edge* r2)
{
  if (!r1||!r2) return false;
  bool v1 = r1->is_vertex();
  bool v2 = r2->is_vertex();
  if (v1||v2) return false;
  return (r1->get_edge()!=r2->get_edge());
}
//A collision is defined by the condition where an region is bounded
//by two different edges at adjacent pixels without crossing a vertex.
//This can happen since boundary positions are sub-pixel and region
//definition is at pixel granularity.  The edge collison causes a needed
//edge to be superseeded.
void gevd_edgel_regions::print_edge_colis(unsigned int x, unsigned int y,
                                    gevd_region_edge* r1, gevd_region_edge* r2)
{
  if (reg_edges_neq(r1, r2))
    if (_verbose)
      vcl_cout << "Collision at (" << x+_xo << " " << y+_yo << ")\n";
}

//----------------------------------------------------------
//:
//    In a correct boundary, each vertex must appear twice, i.e.,
//    shared by two edges.  The only exception is a closed loop with
//    one vertex, but in this case, one edge shares the same vertex twice.
//    This routine tests this constraint.
bool gevd_edgel_regions::
corrupt_boundary(vcl_vector<vtol_edge_2d_sptr>& edges,
                 vcl_vector<vtol_vertex_sptr>& bad_vertices)
{
  bool bad = false;
  //Intialize Markers
  vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      v1->set_id(0);
      v2->set_id(0);
    }
  eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      int id1 = v1->get_id();
      v1->set_id(++id1);
      int id2 = v2->get_id();
      v2->set_id(++id2);
    }
  eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      if ((v1!=v2)&&*v1==*v2)
        vcl_cout << "Improper Loop(" << *v1 << *v2 << ")\n\n";
      int id1 = v1->get_id(), id2 = v2->get_id();
      bool bad1 = id1==1, bad2 = id2==1;
      if (bad1)
        {
#if 0
        bad_vertices.push_back_new(v1);
        //-tpk- assuming puch_bask is close enough
#endif
          bad_vertices.push_back(v1);
          bad = true;
        }
      if (bad2)
        {
#if 0
          bad_vertices.push_back_new(v2);
#endif
          bad_vertices.push_back(v2);
          bad = true;
        }
      if (_verbose&&(bad1||bad2))
        vcl_cout << "Id1 = " << id1 << "  Id2 = " << id2 << vcl_endl;            }
  return bad;
}
//------------------------------------------------------
//:
//  the top of the T is embedded in a boundary.  This routine tests
//  a dangling edge, called "bar" to see if the "T" vertex, "v", joins
//  the contour in a connected chain.  That is, the top of the "T" is
//  part of a chain.  This condition is called "embedded".  This routine
//  is used to remove "hairs" which are extra edges attached to a
//  closed contour by the pixel-level granularity of the region growing
//  process.
static bool embedded_T(vtol_vertex_sptr v, vtol_edge_2d_sptr bar, vcl_vector<vtol_edge_2d_sptr>& real_edges)
{
  bool embedded = true;
  vcl_vector<vtol_edge*>* edges = v->compute_edges();
  int tedges = 0;
  for (vcl_vector<vtol_edge*>::iterator eit = edges->begin();
      eit != edges->end(); eit++)
    {
      if (vcl_find(real_edges.begin(), real_edges.end(),(*eit)->cast_to_edge_2d()) == real_edges.end())
        continue;

      if ((*eit)->cast_to_edge_2d()==bar)
        {
          tedges++;
          continue;
        }
      int end_count = (*eit)->other_endpoint(*v)->get_id();
      if (end_count>1)
        {
          tedges++;
          continue;
        }
    }
  delete edges;
  embedded = (tedges>=3);
  return embedded;
}
//--------------------------------------------------------------------
//: Remove hairs from region boundary.
//    This condition can occur because
//    the region labels are not sub-pixel.  A "hair" is an extra edge joined
//    at a vertex which is part of a continuous chain. Unattached vertices
//    are detected by incrementing a count at each vertex for each
//    attached edge in the input array, "edges".  Such hairs are removed from
//    the input array.
bool gevd_edgel_regions::remove_hairs(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  vcl_vector<vtol_edge_2d_sptr> hairs;
  vcl_vector<vtol_edge_2d_sptr> temp;
  //Intialize Markers
  vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      v1->set_id(0);
      v2->set_id(0);
      temp.push_back(*eit);
    }
  //Use the vertex id as a counter for the number of edges incident
  eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      int id1 = v1->get_id();
      v1->set_id(++id1);
      int id2 = v2->get_id();
      v2->set_id(++id2);
    }
  eit = edges.begin();
  for (;eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1();
      vtol_vertex_sptr v2 = (*eit)->v2();
      int id1 = v1->get_id(), id2 = v2->get_id();
      //The definition of a "hair" a dangling edge meeting at a "T"
      if ((id1==1&&id2>2)||(id1>2&&id2==1))
        {
          //Make sure the top of the "T" is connected
          if (id1>2&&embedded_T(v1, *eit, temp))
            hairs.push_back(*eit);
          else
            if (id2>2&&embedded_T(v2, *eit, temp))
              hairs.push_back(*eit);
        }
    }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator hit = hairs.begin();
      hit != hairs.end(); hit++)
    edges.erase(vcl_find(edges.begin(),edges.end(),*hit));

  return hairs.size();
}
//-----------------------------------------------------------
// --Fix up corrupt boundaries by connecting dangling vertices
//   bad_verts has a list of vertices which are not incident
//   on at least two edges. This routine attempts to connect
//   each vertex to another in the list.  A missing connection is
//   constructed only if there already exists an edge between the
//   vertices in the input array, "edges".
//
bool gevd_edgel_regions::connect_ends(vcl_vector<vtol_edge_2d_sptr>& edges,
                                vcl_vector<vtol_vertex_sptr>& bad_verts)
{
  bool all_ends_connected = true;
  if (!bad_verts.size())
    return all_ends_connected;
  //Clear the bad vertex flags
  vcl_vector<vtol_vertex_sptr> temp;//temporary bad_verts array
  vcl_vector<vtol_vertex_sptr>::iterator vit = bad_verts.begin();
  for (; vit != bad_verts.end(); vit++)
    {
      (*vit)->unset_user_flag(VSOL_FLAG1);
      temp.push_back(*vit);
    }
  //Collect the vertices from edges
  //VSOL_FLAG1 defines the state of a vertex in the search for a connecting edge
  //FLAG2 defines the state of a vertex in forming the set edge_verts,
  //that is there should be no duplicate vertices
  vcl_vector<vtol_vertex_sptr> edge_verts;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
      eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1(), v2 = (*eit)->v2();

      v1->unset_user_flag(VSOL_FLAG1); v2->unset_user_flag(VSOL_FLAG1);
      v1->unset_user_flag(VSOL_FLAG2); v2->unset_user_flag(VSOL_FLAG2);
    }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
      eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1(), v2 = (*eit)->v2();
      if (!v1->get_user_flag(VSOL_FLAG2))
        {
        edge_verts.push_back(v1.ptr());
        v1->set_user_flag(VSOL_FLAG2);
        }
      if (!v2->get_user_flag(VSOL_FLAG2))
        {
        edge_verts.push_back(v2.ptr());
        v2->set_user_flag(VSOL_FLAG2);}
    }

  vcl_vector<vtol_vertex_sptr> repaired_verts;
  for (vit=bad_verts.begin(); vit != bad_verts.end(); vit++)
    {
      if ((*vit)->get_user_flag(VSOL_FLAG1))//skip used vertices
        continue;
      bool found_edge = false;
      vcl_vector<vtol_edge*>* vedges = (*vit)->compute_edges();
      for (vcl_vector<vtol_edge*>::iterator eit = vedges->begin();
          (eit != vedges->end())&&!found_edge; eit++)
        {
          vtol_vertex_sptr v = (*eit)->other_endpoint(**vit);
          //Continue if:
          //  1)the vertex v has been used;
          //  2)v can't be found in bad_verts;
          //  3)v can't be found in edge_verts;
          //  4)(*eit) is already in the input edge set.
          if (v->get_user_flag(VSOL_FLAG1))
            continue; //1)
          bool found_in_bad_verts = false;
          if (vcl_find (temp.begin(), temp.end(), v) != temp.end())
            found_in_bad_verts = true;
          //already iterating
          // in bad_verts, so use temp
          bool found_in_edge_verts = false;
          if (!found_in_bad_verts) //2)
            {
            if (vcl_find(edge_verts.begin(), edge_verts.end(),v) != edge_verts.end())
              {
              found_in_edge_verts = true;
              }
            }
          if (!(found_in_bad_verts||found_in_edge_verts)) //3)
            continue;
          if (vcl_find(edges.begin(), edges.end(), (*eit)->cast_to_edge_2d()) != edges.end())
            continue; //4)
          //Found a connecting edge.
          edges.push_back((*eit)->cast_to_edge_2d());
          found_edge = true;
          v->set_user_flag(VSOL_FLAG1);
          (*vit)->set_user_flag(VSOL_FLAG1);
          repaired_verts.push_back(*vit);
        }
      delete vedges;
      all_ends_connected =
        all_ends_connected&&(*vit)->get_user_flag(VSOL_FLAG1);
    }
  for (vit = repaired_verts.begin();
      vit != repaired_verts.end(); vit++)
    bad_verts.erase(vit);
  return all_ends_connected;
}
//-------------------------------------------------------------------
//:
//    There can be some gaps in the region boundary due to missing
//    edges caused by a high local density of vertices.  Take each
//    un-attached vertex and look for a match in the failed edge insertion
//    array. If an edge can be attached, do so.
void gevd_edgel_regions::repair_failed_insertions(vcl_vector<vtol_edge_2d_sptr>& edges,
                                            vcl_vector<vtol_vertex_sptr>& bad_verts)
{
  vcl_vector<vtol_vertex_sptr> temp1, temp2;
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = bad_verts.begin();
      vit != bad_verts.end(); vit++)
    for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = _failed_insertions->begin();
        eit != _failed_insertions->end(); eit++)
      if ((*vit)==(*eit)->v1())
        {
          edges.push_back(*eit);
          temp1.push_back(*vit);
          temp2.push_back((*eit)->v2());
        }
      else if ((*vit)==(*eit)->v2())
        {
          edges.push_back(*eit);
          temp1.push_back(*vit);
          temp2.push_back((*eit)->v1());
        }
  for (vcl_vector<vtol_vertex_sptr>::iterator wit = temp1.begin();
      wit != temp1.end(); wit++)
    bad_verts.erase(wit);

  for (vcl_vector<vtol_vertex_sptr>::iterator vvit = temp2.begin();
      vvit != temp2.end(); vvit++)
    bad_verts.push_back(*vvit);
}

//-------------------------------------------------------------------
//:
//    After connected components have been generated pass over the
//    array and assign region labels to the gevd_region_edge(s).
//    As in ::UpdateConnectedNeighborhood, the algorithm uses a 2x2 neigborhood
//    E.G.,
//    ee But the purpose is to assign labels. No updating of the
//    aa _region_label_array is carried out.
//
//    Note that the 2x2 neighborhood is encoded as uchar = [ul|ur|ll|lr]
//                                                          2  2  2  2  bits
void gevd_edgel_regions::AssignEdgeLabels(unsigned int x, unsigned int y)

{
  unsigned int ul = _region_label_array[y][x];
  unsigned int ur = _region_label_array[y][x+1];
  unsigned int& ll = _region_label_array[y+1][x];
  unsigned int& lr = _region_label_array[y+1][x+1];
  gevd_region_edge* rul = _edge_boundary_array[y][x];
  gevd_region_edge* rur = _edge_boundary_array[y][x+1];
  gevd_region_edge* rll = _edge_boundary_array[y+1][x];
  gevd_region_edge* rlr = _edge_boundary_array[y+1][x+1];

  unsigned char nhood = this->EncodeNeighborhood(ul, ur, ll, lr);
  switch(int(nhood))
    {
    case 170:
      //aa
      //aa
      return;

    case 169:
      //aa
      //ae
      rlr->Prop(rlr, ul);
      return;

    case 166:
      //aa
      //ea
      rll->Prop(rll, ul);
      return;

    case 165:
      //aa
      //ee
      rll->Prop(rll, ul);
      print_edge_colis(x, y, rll, rlr);
      return;

    case 154:
      //ae
      //aa
      rur->Prop(rur, ul);
      return;

    case 153:
      //ae
      //ae
      rlr->Prop(rur, ul);
      print_edge_colis(x, y, rur, rlr);
      return;

    case 150:
      //ae
      //ea
      rur->Prop(rll, ul);
      rur->Prop(rll, lr);
      print_edge_colis(x, y, rur, rll);
      return;

    case 149:
      //ae
      //ee
      rll->Prop(rll, ul);
      rur->Prop(rlr, ul);
      if (!(rlr->is_vertex()))
        {
          print_edge_colis(x, y, rur, rlr);
          print_edge_colis(x, y, rll, rur);
          print_edge_colis(x, y, rll, rlr);
        }
      return;

    case 106:
      //ea
      //aa
      rul->Prop(rul, ll);
      return;

    case 105:
      //ea
      //ae
      rlr->Prop(rul, ll);
      rlr->Prop(rul, ur);
      print_edge_colis(x, y, rul, rlr);
      return;

    case 102:
      //ea
      //ea
      rll->Prop(rul, ur);
      print_edge_colis(x, y, rul, rll);
      return;

    case 101:
      //ea
      //ee
    if (!(rll->is_vertex()))
      {
        print_edge_colis(x, y, rul, rll);
        print_edge_colis(x, y, rul, rlr);
        print_edge_colis(x, y, rll, rlr);
      }
      return;

    case 90:
      //ee
      //aa
      rul->Prop(rul,ll);
      print_edge_colis(x, y, rul, rur);
      return;

    case 89:
      //ee
      //ae
         rul->Prop(rul, ll);
         rlr->Prop(rur, ll);
         if (!(rur->is_vertex()))
           {
             print_edge_colis(x, y, rul, rur);
             print_edge_colis(x, y, rul, rlr);
             print_edge_colis(x, y, rur, rlr);
           }
      return;

    case 86:
      //ee
      //ea
      rul->Prop(rul, lr);
      if (!(rul->is_vertex()))
      {
        print_edge_colis(x, y, rul, rur);
        print_edge_colis(x, y, rul, rll);
        print_edge_colis(x, y, rur, rll);
      }
      return;
    case 85:
        //ee
        //ee
      if (!((rul->is_vertex())||(rur->is_vertex())||
         (rll->is_vertex())||(rlr->is_vertex())))
        {
          print_edge_colis(x, y, rul ,rur);
          print_edge_colis(x, y, rul ,rll);
          print_edge_colis(x, y, rul ,rlr);
          print_edge_colis(x, y, rur ,rll);
          print_edge_colis(x, y, rur ,rlr);
          print_edge_colis(x, y, rll ,rlr);
        }
      return;
    default:
      vcl_cout << "In gevd_edgel_regions::UpdateNeigborhood(..)"
               << "impossible pattern = " << (int)nhood
               << vcl_endl;
    }
}
//---------------------------------------------------------------------
//: Scan the _region_label_array and apply the region equivalence map.
//    The result is that all equivalences are reconciled with the smallest
//    labels.
void gevd_edgel_regions::ApplyRegionEquivalence()
{
  unsigned int x, y;
  for (y = _yo; y<=_yend; y++)
    for (x = _xo; x<=_xend; x++)
      {
        //Update the region label array
        unsigned int label = _region_label_array[Y(y)][X(x)];
        if (label<_min_region_label)
          continue;
        unsigned int base = this->BaseLabel(label);
        _region_label_array[Y(y)][X(x)] = base;
      }
}
//-------------------------------------------------------------------
//: Bounds check on _region_label_array
bool gevd_edgel_regions::out_of_bounds(unsigned int x, unsigned int y)
{
  bool out = x>(_xend-_xo)||y>(_yend-_yo);
  return out;
}

//: Get the a region label for an edge used to construct the boundaries.
//   A return corresponding to UNLABELED means the domain outside the ROI
//   or nr is larger than the number of adjacent regions.
unsigned int gevd_edgel_regions::GetLabel(vtol_edge_2d_sptr e, unsigned int nr)
{
  vcl_map<int,gevd_region_edge *>::iterator reit = _region_edges.find(e->get_id());
  if ( reit == _region_edges.end())
    return UNLABELED;
  return reit->second->GetLabel(nr);
}

//--------------------------------------------------------------------
//: Insert an Edge into the adjacency list for a region
//
void gevd_edgel_regions::insert_adjacency(unsigned int r, vtol_edge_2d_sptr e)
{
  if (!e) return;
  //  e->Protect();
  vcl_map<unsigned int,vcl_vector<vtol_edge_2d_sptr> *>::iterator reit =_region_edge_adjacency.find(r);
  if (reit == _region_edge_adjacency.end())
    {
      vcl_vector<vtol_edge_2d_sptr>* array = new vcl_vector<vtol_edge_2d_sptr>;
      array->push_back(e);
      _region_edge_adjacency[r] = array;
    }
  else
    reit->second->push_back(e);
  //    _region_edge_adjacency.value()->push(e);
}
//------------------------------------------------------------------
//: Get the edges adjacent to each region
//
void gevd_edgel_regions::CollectEdges()
{
  // for (_region_edges.reset(); _region_edges.next();)
  for ( vcl_map<int, gevd_region_edge*>::iterator reit= _region_edges.begin(); reit == _region_edges.end(); reit++)
    {
      gevd_region_edge* re = reit->second;
      vtol_edge_2d_sptr e = re->get_edge()->cast_to_edge_2d();
      if (_verbose)
        vcl_cout << "\nEdge:" << e << "(" << e->v1() <<  " " << e->v2() <<"):(";
      for (unsigned int i = 0; i<re->NumLabels(); i++)
        {
          unsigned int l = re->GetLabel(i);
          if (_verbose)
            vcl_cout << "l[" << i << "]:" << l << " ";
          if (l!=0)
            insert_adjacency(l, e);
        }
      if (_verbose)
        vcl_cout << ")\n";
    }
}
//---------------------------------------------------------------------
//: Trace through the topology network keeping regions on the left.
//    At this point, we have a list of edges for each region. We will
//    trace out the list and form OneCycle(s).  Then the set of OneCycle(s)
//    will form the boundary of the face corresponding to the region.
void gevd_edgel_regions::CollectFaceEdges()
{
  vul_timer t;
  unsigned int i;
  vcl_cout<<"Constructing Face-Edges:";

  _face_edge_index = new vcl_vector<vtol_edge_2d_sptr>*[_max_region_label];
  for (i=0; i<_max_region_label; i++)
    _face_edge_index[i] = NULL;

  for (i =_min_region_label; i<_max_region_label; i++)
    {
    vcl_map<unsigned int, vcl_vector<vtol_edge_2d_sptr>* >::iterator  reait;
    reait = _region_edge_adjacency.find(i);
      if (reait == _region_edge_adjacency.end())
        continue;

      vcl_vector<vtol_edge_2d_sptr>* edges = reait->second;

      int len = edges->size();
      if (!len)
        continue;

      this->remove_hairs(*edges);
      vcl_vector<vtol_vertex_sptr> bad_verts;
      //If the input edge list is corrupt, then attempt to fix it.
      if (this->corrupt_boundary(*edges, bad_verts))
        {
          this->repair_failed_insertions(*edges, bad_verts);
          if (!this->connect_ends(*edges, bad_verts))
            {
              if (_verbose)
                {
                  vcl_cout << "Region [" << i << "] is corrupt \n";
                  vcl_cout << "Bad Vertices \n";
                  for (vcl_vector<vtol_vertex_sptr>::iterator vit =
                        bad_verts.begin(); vit != bad_verts.end();
                      vit++)
                    if (!(*vit)->get_user_flag(VSOL_FLAG1))
                      vcl_cout << *(*vit);
                  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges->begin();
                      eit != edges->end(); eit++)
                    vcl_cout << "\nEdge(\n" << *((*eit)->v1()) << *((*eit)->v2()) <<")\n";
                }
#if 0
              if (_debug_data)
                {
                  _debug_data->set_verts(bad_verts);
                  _debug_data->set_edges(*edges);
                }
#endif
            }
        }
      if (_verbose)
        vcl_cout << " Building Region [" << i << "]\n";
      len = edges->size();

      vcl_vector<vtol_edge_2d_sptr> EdgeSet;
      for (int j =0; j<len; j++)
        {
          vtol_edge_2d_sptr e = (*edges)[j];
          if (_verbose)
            vcl_cout << "Edge(" << e->v1() <<  " " << e->v2() << vcl_endl;
          EdgeSet.push_back ( e );
        }
      vcl_vector<vtol_edge_2d_sptr>* edge_list = new vcl_vector<vtol_edge_2d_sptr>;
      for (vcl_vector<vtol_edge_2d_sptr>::iterator esit = EdgeSet.begin(); esit != EdgeSet.end() ; esit++)
        {
        edge_list->push_back (*esit );
        }
      _face_edge_index[i] = edge_list;
    }

  vcl_cout << vcl_endl;
  vcl_cout << "Constructed Face-Edges(" << _max_region_label - _min_region_label
           << ") in " << t.real() << " msecs.\n";
}
//----------------------------------------------------------------
//: Construct Face(s) from Edge(s) in the _face_edge_index array.
//    This method has been made virtual so that sub-classes of
//    gevd_intensity_face can be constructed by sub-classes of gevd_edgel_regions.
void gevd_edgel_regions::ConstructFaces()
{
  vul_timer t;
  unsigned int i;
  vcl_cout<<"Constructing Faces:";
  //Initialize the _intensity_face_index
  _intensity_face_index = new gevd_intensity_face*[_max_region_label];
  for (i=0; i<_max_region_label; i++)
    _intensity_face_index[i] = NULL;

  for (i =_min_region_label; i<_max_region_label; i++)
    {
      //Retrieve the face boundary edges
      vcl_vector<vtol_edge_2d_sptr>* edge_list = _face_edge_index[i];
      if (!edge_list||!edge_list->size())
        continue;
      //Make a new gevd_intensity_face
      vtol_cycle_processor cp(*edge_list);
      vcl_vector<vtol_one_chain_sptr> one_chains;
      cp.nested_one_cycles(one_chains, 0.5);
#if 0
      if (!one_chains.size()&&_debug_data)
        {
          _debug_data->set_edges(*edge_list);
          continue;
        }
#endif
      gevd_intensity_face* face = new gevd_intensity_face(one_chains);
      //gevd_intensity_face* face = new gevd_intensity_face(edge_list);
      //Check if the Face has valid Edges, since the Face
      //constructor can fail
      //looks like an expensive call
      vcl_vector<vtol_edge*>* face_edges = face->compute_edges();
      if (face_edges->size())
        {
        _faces->push_back(face);
        //  face->Protect();
        _intensity_face_index[i] = face;
        //  face->Protect();
        }
      //      else
      //        face->UnProtect();
      delete face_edges;
    }
  vcl_cout << vcl_endl;
  vcl_cout << "Constructed Faces(" << _max_region_label - _min_region_label
           << ") in " << t.real() << " msecs.\n";
}
//--------------------------------------------------------------
//: get a row from a BufferXY
//
void gevd_edgel_regions::get_buffer_row(unsigned int row)
{
  if (!_buf_source)
    return;
  int x0 = (int)_xo, y0 = (int)row, xsize = (int)(_xend-_xo + 1);
  for (int x = x0; x<xsize; x++)
    switch(this->bytes_per_pix())
      {
      case 1:   // Grey scale image with one byte per pixel
        _ubuf[x]= bytePixel(_buf, x, y0);
        break;
      case 2:   // Grey scale image with an unsigned short per pixel
        _sbuf[x] = shortPixel(_buf, x, y0);
        break;

      default:
        vcl_cout<<"In gevd_edgel_rgions::get_row(): bytes/pixel not 1 or 2\n";
        return;
      }
}
//These routines were added to speed up the intensity face data fill
//------------------------------------------------
//: Get an image row
void gevd_edgel_regions::get_image_row(unsigned int row)
{
  if (!_image_source)
    return;
  int x0 = (int)_xo, y0 = (int)row, xsize = (int)(_xend-_xo + 1);

  switch (this->bytes_per_pix())
    {
    case 1:     // Grey scale image with one byte per pixel
      _image->get_section(_ubuf, x0, y0, xsize, 1);
      break;
    case 2:     // Grey scale image with an unsigned short per pixel
      _image->get_section(_sbuf, x0, y0, xsize, 1);
      break;

    default:
      vcl_cout<<"In gevd_edgel_regions::get_row(): bytes/pixel not 1 or 2\n";
    }
}
//---------------------------------------------------------------
//: Get the intensity of a single pixel
unsigned short gevd_edgel_regions::get_intensity(unsigned int x)
{
  unsigned int intensity = 0;
  switch (this->bytes_per_pix())
    {
    case 1:     // Grey scale image with one byte per pixel
      intensity = (unsigned short)_ubuf[X(x)];
      break;
    case 2:     // Grey scale image with an unsigned short per pixel
      {
        intensity = _sbuf[X(x)];
        break;
      }
    default:
      vcl_cout<<"In gevd_edgel_regions::get_intensity(): bytes/pixel not 1 or 2\n";
    }
  return intensity;
}
//---------------------------------------------------------------------
//: Accumulate intensity statistics from each region and update the gevd_intensity_face parameters
void gevd_edgel_regions::AccumulateMeans()
{
  vul_timer t;
  unsigned int i =0;
  //Initialize the intensity face means
  for (i=_min_region_label; i<_max_region_label; i++)
    if (_intensity_face_index[i])
      _intensity_face_index[i]->ResetPixelData();

  int Npixels = 0;
  for (unsigned int y=_yo; y<=_yend; y++)
    {
      if (_image_source)
        this->get_image_row(y);
      if (_buf_source)
        this->get_buffer_row(y);
      for (unsigned int x=_xo; x<=_xend; x++)
        {
          unsigned int label = _region_label_array[Y(y)][X(x)];
          Npixels++;
          if (_intensity_face_index[label])
            {
              unsigned short intensity = this->get_intensity(x);
              double Ximg = double(x)+.5;//coordinates are at the pixel center
              double Yimg = double(y)+.5;
              _intensity_face_index[label]->
                IncrementMeans(Ximg, Yimg, intensity);
            }
        }
    }
  int Nregions = _max_region_label - _min_region_label;
  vcl_cout << "Accumulate Region Means(" << Nregions << ") in "
           << t.real() << " msecs.\n";
  vcl_cout << "Normalized Time = " << (10000.0*t.real())/Npixels << vcl_endl;
}

//---------------------------------------------------------------------
//: Insert region pixels into the gevd_intensity_face arrays
//
void gevd_edgel_regions::AccumulateRegionData()
{
  vul_timer t;
  //Initialize the intensity face pixel arrays
  for (unsigned int i = _min_region_label; i<_max_region_label; i++)
    if (_intensity_face_index[i])
      (_intensity_face_index[i])->InitPixelArrays();
  //Scan the image and insert intensities
  for (unsigned int y=_yo; y<=_yend; y++)
    {
      if (_image_source)
        this->get_image_row(y);
      if (_buf_source)
        this->get_buffer_row(y);
      for (unsigned int x=_xo; x<=_xend; x++)
      {
        unsigned int label = _region_label_array[Y(y)][X(x)];
        if (_intensity_face_index[label])
          {
            unsigned short intensity= this->get_intensity(x);
            //Set face pixel arrays
            gevd_intensity_face* f = _intensity_face_index[label];
            double Ximg = double(x)+.5;
            double Yimg = double(y)+.5;
            f->InsertInPixelArrays(Ximg, Yimg, intensity);
          }
      }
    }
  vcl_cout << "Accumulate Region Data(" << _max_region_label - _min_region_label
           << ") in " << t.real() << " msecs.\n";
}
//---------------------------------------------------------------------
//:
//    Do both a scatter matrix update and insertion into the region pixel
//    arrays of each intensity face.  AccumulateScatterData is done first
//    so that the number of pixels can be determined.
//
void gevd_edgel_regions::InsertFaceData()
{
  this->AccumulateMeans();
  this->AccumulateRegionData();
}


