#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include "sdet_curvelet.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: copy constructor
sdet_curvelet::sdet_curvelet(const sdet_curvelet& other)
{
  //the edgels have to copied as links because curvelets are just groupings of the edgels
  ref_edgel = other.ref_edgel;
  edgel_chain = other.edgel_chain;

  //but the curve model has to be deep copied
  switch(other.curve_model->type)
  {
    case sdet_curve_model::LINEAR:
      curve_model = new sdet_linear_curve_model(*(sdet_linear_curve_model*)other.curve_model);
      break;
    case sdet_curve_model::CC:
      curve_model = new sdet_CC_curve_model(*(sdet_CC_curve_model*)other.curve_model);
    case sdet_curve_model::CC2:
      curve_model = new sdet_CC_curve_model_new(*(sdet_CC_curve_model_new*)other.curve_model);
    case sdet_curve_model::CC3d:
      curve_model = new sdet_CC_curve_model_3d(*(sdet_CC_curve_model_3d*)other.curve_model);
    case sdet_curve_model::ES:
      curve_model = new sdet_ES_curve_model(*(sdet_ES_curve_model*)other.curve_model);
    default:
      curve_model=nullptr; //TO DO
  }

  forward = other.forward;
  length = other.length;
  quality = other.quality;
  used = other.used;
}

//: copy constructor with provisions for copying a different curve bundle
sdet_curvelet::sdet_curvelet(const sdet_curvelet& other, sdet_curve_model* cm)
{
  //the edgels have to copied as links because curvelets are just groupings of the edgels
  ref_edgel = other.ref_edgel;
  edgel_chain = other.edgel_chain;

  //do not copy the CB, just assign the ne passed to it
  curve_model = cm;

  forward = other.forward;
  length = other.length;
  quality = other.quality;
  used = other.used;
}


//: destructor
sdet_curvelet::~sdet_curvelet()
{
  //delete the curve model
  if (curve_model)
    delete curve_model;

  edgel_chain.clear();
}

// weighting constants for the heuristic
#define alpha3 1.0
#define alpha4 1.0

//: compute properties of this curvelet once formed
void sdet_curvelet::compute_properties(double R, double token_len)
{
  //find out the # of edgels before and after the reference edgel
  //also find out the length before and after the reference edgel
  int num_before=0, num_after=0;
  double Lm=0, Lp=0;

  bool before_ref = true;
  for (unsigned i=0; i<edgel_chain.size()-1; i++){
    if (before_ref) { Lm += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt); num_before++; }
    else            { Lp += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt); num_after++; }

    if (edgel_chain[i+1]==ref_edgel)
      before_ref = false;
  }

  //compute the length of the curvelet (extrinsic length)
  length = Lm+Lp;

  //also compute the LG ratio and store as quality
  //quality = (num_before+num_after)*token_len/length;

  //new quality measure (1/cost of the compatibility heauristic)
  quality = 2/(alpha3*R/length + alpha4*length/token_len/edgel_chain.size());
}

//: print info to file
void sdet_curvelet::print(std::ostream& os)
{
  //first output the edgel chain
  os << "[";
  for (auto & i : edgel_chain){
    os << i->id << " ";
  }
  os << "] ";

  //forward/backward tag
  os << "(";
  if (forward) os << "F";
  else         os << "B";
  os << ") ";

  //next output the curve model
  curve_model->print(os);

  //then output the other properties
  os << " " << length << " " << quality << std::endl;
}

std::list<sdet_edgel*> sdet_curvelet::child_chain()
{
    bool flag=false;
    std::list<sdet_edgel*> return_chain;
    for(auto & i : edgel_chain)
    {
        if(flag)
            return_chain.push_back(i);

        if(i->id==ref_edgel->id)
            flag=true;

    }
    return return_chain;
}


std::list<sdet_edgel*> sdet_curvelet::parent_chain()
{
    bool flag=true;
    std::list<sdet_edgel*> return_chain;
    for(auto & i : edgel_chain)
    {

        if(i->id==ref_edgel->id)
            flag=false;
        if(flag)
            return_chain.push_front(i);
    }
    return return_chain;
}
