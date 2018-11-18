#include "bwm_shape_file.h"
#include "vul/vul_file.h"
#include <vsol/vsol_point_3d.h>

bool bwm_shape_file::load(std::string filename)
{
  std::string ext = vul_file::extension(filename);
  if ((!ext.compare(".shp") ) == 0) {
    std::cerr << filename << "is not a .shp file\n";
    return false;
  }

  handle_ = SHPOpen(filename.c_str(), "rb");
  if (handle_) {
    //nEntities_ = new int();
    //nShapeType_ = new int();
    SHPGetInfo(handle_, &nEntities_, &nShapeType_, padfMinBound, padfMaxBound );
    bb_.add_point(padfMinBound[0], padfMinBound[1], padfMinBound[2]);
    bb_.add_point(padfMaxBound[0], padfMaxBound[1], padfMaxBound[2]);

    std::vector<vsol_point_3d_sptr> vlist;
    for (int i=0; i<nEntities_; i++) {
      SHPObject* obj = SHPReadObject( handle_, i );
      if (obj->nShapeId != -1) {  // undefined shape types are meaningless
        vlist.clear();
        //double xmin = obj->dfXMin;
        //double ymin = obj->dfYMin;
        //double zmin = obj->dfZMin;
        //double xmax = obj->dfXMax;
        //double ymax = obj->dfYMax;
        //double zmax = obj->dfZMax;

        for (int j=0; j<obj->nVertices; j++) {
          double x = obj->padfX[j];
          double y = obj->padfY[j];
          double z = obj->padfZ[j];
          vsol_point_3d_sptr v = new vsol_point_3d(x,y,z);
          vlist.push_back(v);
        }
      }
      vertices_.push_back(vlist);
    }
  }
  return true;
}

void bwm_shape_file::print()
{
  if (handle_) {
    std::cout << "SHAPE TYPE:" << nShapeType_ << '\n'
             << "NUMBER OF OBJECTS:" << nEntities_ << '\n'
             << "BOUNDING BOX:" << bb_ << std::endl;

    for (int i=0; i<nEntities_; i++) {
      SHPObject* obj = SHPReadObject( handle_, i );
      std::cout << i << "th Object -----" << '\n'
               << "SHAPE TYPE:" << obj->nSHPType << '\n'
               << "SHAPE ID:" << obj->nShapeId << '\n'
               << "NUM PARTS:" << obj->nParts << '\n'
               << "NUM Vertices:" << obj->nVertices << std::endl;
      for (int j=0; j<obj->nVertices; j++) {
        double x = obj->padfX[j];
        double y = obj->padfY[j];
        double z = obj->padfZ[j];
        std::cout << "   Vertex[" << j << "]=(" << x << ',' << y << ',' << z << ')' << std::endl;
      }
    }
  }
}
