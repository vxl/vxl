// This is gel/mrc/carmen_interface/carmen_interface.cpp
#include <iostream>
#include <fstream>
#include "carmen_interface.h"
//:
//  \file
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//--------------------------------------------
// Carmen Includes
#define DEFINED_MAX_MIN
#undef REALLOC
#undef NEW
#undef Status
#include <Carmen/Carmen.h>
#include <CarmenModels/Perspective/pp.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//: Constructor
carmen_interface::carmen_interface()
{
  PerspectiveCamera::Enregister();
  _carmen = new Carmen;
}

//: Destructor
carmen_interface::~carmen_interface()
{
  delete _carmen;
}

//-----------------------------------------------------------------------
//: initialize carmen
void carmen_interface::set_carmen_camera(int view_no)
{
  if (!_carmen)
    return;
  bool stat = (bool)_carmen->create_camera(view_no, "PerspectiveCamera");
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return;
    }
}
//----------------------------------------------------------------------
//: add a full 3d correspondence to Carmen
//
bool carmen_interface::add_full_correspondence(int view_no, int point_id,
                                               double x, double y, double z,
                                               float u, float v)
{
  double default_3d_point_sdev = 0.0;
  double default_2d_point_sdev = 1.0;
  bool stat=false;

  //Set up the primary point3D.
  stat = (bool)_carmen->set_primary_parameter(point_id, "X", x,
                                              default_3d_point_sdev, FIX);
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return stat;
    }

  stat = (bool)_carmen->set_primary_parameter(point_id, "Y", y,
                                              default_3d_point_sdev, FIX);
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return stat;
    }
  stat = (bool)_carmen->set_primary_parameter(point_id, "Z", z,
                                              default_3d_point_sdev, FIX);
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return stat;
    }
  //define the corresponding 2d secondary
  stat = _carmen->define_control_point(view_no, point_id, u, v,
                                       default_2d_point_sdev);
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return stat;
    }
  return stat;
}
//-------------------------------------------------------------------------
//: load a correspondence file
//
bool carmen_interface::load_correspondence_file(const string& file_path)
{
  std::ifstream instr(file_path.c_str());
  if (!instr)
    {
      std::cout <<"In carmen_interface::load_correspondence_file()"
               <<" - can't open file " << file_path.c_str() << std::endl;
      return false;
    }
  char buf[100];
  int npts;
  instr >> buf ;
  string keyword=buf;

  if (keyword=="NUMPOINTS:"&&instr)
    instr >> npts;
  else
    {
      std::cout << "bad file\n";
      return false;
    }
  int view_no=0, point_id=0;
  double x=0, y=0, z=0;
  float u=0, v=0;
  for (int i=0; i<npts&&instr; i++)
    {
      instr >> buf;
      keyword = buf;
      if (keyword!="CORRESP:")
        {
          std::cout << "wrong number of correspondences\n";
          return false;
        }
      instr >> view_no >> point_id >> x >> y >> z >> u >> v;
      std::cout << "Corr: " <<  view_no << " " << point_id << " " << x << " " <<  y
               << " " << z << " " <<  u << " " << v << std::endl;
      bool stat = add_full_correspondence(view_no, point_id, x, y, z, u, v);
      if (!stat)
        return false;
    }
  _carmen->DoneDefiningProblem();
}

//-----------------------------------------------------------------------
//: solve for a perspective camera
//
void carmen_interface::solve()
{
  if (!_carmen)
    return;
  bool stat = (bool)_carmen->init_cameras();
  if (!stat)
    {
      std::cerr << _carmen->geterror() << std::endl;
      return;
    }
  stat = (bool)_carmen->solve_for_everything();
  if (!stat)
    std::cerr << _carmen->geterror() << std::endl;
}

//--------------------------------------------------------------------
//: save camera
//
void carmen_interface::save_camera(const string& file_path)
{
}

//--------------------------------------------------------------------
//: Print Carmen results
//
void carmen_interface::print_results()
{
  _carmen->print_camera_parameters(stdout);
  _carmen->print_primary_parameters(stdout);
  _carmen->print_secondary_parameters(stdout);
  _carmen->print_measurements(stdout);
}
