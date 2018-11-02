#include <cstdlib>
#include <iostream>
#include "vil_geotiff_header.h"
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <geo_tiffp.h>
#include <geotiffio.h>
#include <geovalues.h>

vil_geotiff_header::vil_geotiff_header(TIFF* tif) : tif_(tif)
{
  if (tif) {
    gtif_ = GTIFNew(tif);
    if (gtif_) {
      GTIFPrint(gtif_, nullptr, nullptr);

      // read the header of the GeoDirectoryKey Tag
      int version[3];
      GTIFDirectoryInfo(gtif_, version, &number_of_geokeys_);
      key_directory_version_ = (unsigned short)version[0];
      key_revision_ =          (unsigned short)version[1];
      minor_revision_ =        (unsigned short)version[2];
    }
  }
}

bool vil_geotiff_header::gtif_tiepoints(std::vector<std::vector<double> > &tiepoints)
{
  double* points=nullptr;
  short count;
  if (TIFFGetField(tif_, GTIFF_TIEPOINTS, &count, &points) < 0)
    return false;

  // tiepoints are stored as 3d points (I,J,K)->(X,Y,Z)
  // where the point at location (I,J) at raster space with pixel value K
  // and (X,Y,Z) is a vector in model space

  // the number of values should be K*6
  assert((count % 6) == 0);
  for (unsigned short i=0; i<count; ) {
    std::vector<double> tiepoint(6);
    tiepoint[0] = points[i++];
    tiepoint[1] = points[i++];
    tiepoint[2] = points[i++];
    tiepoint[3] = points[i++];
    tiepoint[4] = points[i++];
    tiepoint[5] = points[i++];
    tiepoints.push_back(tiepoint);
  }
  return true;
}

bool vil_geotiff_header::gtif_pixelscale(double &scale_x, double &scale_y, double &scale_z)
{
  double *data;
  short count;
  if (TIFFGetField(tif_, GTIFF_PIXELSCALE, &count, &data )) {
    assert (count == 3);
    scale_x = data[0];
    scale_y = data[1];
    scale_z = data[2];
    return true;
  }
  else
    return false;
}

bool vil_geotiff_header::gtif_trans_matrix (double* &trans_matrix)
{
  short count;
  if (TIFFGetField(tif_, GTIFF_TRANSMATRIX, &count, &trans_matrix )) {
    assert (count == 16);
    return true;
  }
  else
    return false;
}

bool vil_geotiff_header::gtif_modeltype (modeltype_t& type)
{
  geocode_t model;
  if (!GTIFKeyGet(gtif_, GTModelTypeGeoKey, &model, 0, 1))  {
    std::cerr << "NO Model Type defined!!!!\n";
    return false;
  }
  else {
    type = static_cast<modeltype_t> (model);
    return true;
  }
}

bool vil_geotiff_header::gtif_rastertype (rastertype_t& type)
{
  geocode_t raster;
  if (!GTIFKeyGet(gtif_, GTRasterTypeGeoKey, &raster, 0, 1)) {
    std::cerr << "NO Raster Type, failure!!!!\n";
    return false;
  }
  else {
    type = static_cast<rastertype_t> (raster);
    return true;
  }
}

bool vil_geotiff_header::geounits (geounits_t& units)
{
  short nGeogUOMLinear;
  if (!GTIFKeyGet(gtif_, GeogLinearUnitsGeoKey, &nGeogUOMLinear, 0, 1 )) {
    std::cerr << "NO GEOUNITS, failure!!!!\n";
    return false;
  }
  else {
    units = static_cast<geounits_t> (nGeogUOMLinear);
    return true;
  }
}

bool vil_geotiff_header::PCS_WGS84_UTM_zone(int &zone, GTIF_HEMISPH &hemisph) // hemisph is O for N, 1 for S
{
  modeltype_t type;
  if (gtif_modeltype(type) && type == ModelTypeProjected)
  {
    void *value;
    int size;
    int length;
    tagtype_t ttype;
    bool status = get_key_value(ProjectedCSTypeGeoKey, &value, size, length, ttype);
    if (!status) {
      std::cerr << "Missing ProjectedCSTypeGeoKey (" << ProjectedCSTypeGeoKey << ") key!\n";
      return false;
    }

    // WGS84 / UTM northern hemisphere:  326zz where zz is UTM zone number
    // WGS84 / UTM southern hemisphere:  327zz where zz is UTM zone number
    // we are looking for a short value, only one
    if(length != 1 || ttype != TYPE_SHORT) {
      std::cerr << "Expected a single value with type int16 (short)!\n";
      return false;
    }

    auto *val = static_cast<short*> (value);
    if ((*val < PCS_WGS84_UTM_zone_1N ) || ((*val > PCS_WGS84_UTM_zone_60S ))) {
      return false;
    }

#if 0
     int zone;
     int hemisph; // O for N, 1 for S
#endif // 0
    if ((*val >= PCS_WGS84_UTM_zone_1N) && (*val <= PCS_WGS84_UTM_zone_60N)) {
      zone = *val - 32600;
      hemisph = NORTH;
    }
    else if ((*val >= PCS_WGS84_UTM_zone_1S) && (*val <= PCS_WGS84_UTM_zone_60S)) {
      zone = *val - 32700;
      hemisph = SOUTH;
    }
    return true;
  }
  else {
    hemisph = UNDEF;
    return false;
  }
}

//: returns true if in geographic coords, linear units are in meters and angular units are in degrees
bool vil_geotiff_header::GCS_WGS84_MET_DEG()
{
  modeltype_t type;
  if (gtif_modeltype(type) && type == ModelTypeGeographic) {
    void *value; int size; int length; tagtype_t ttype;

    bool status = get_key_value(GeogLinearUnitsGeoKey, &value, size, length, ttype);
    if (!status) {
      std::cerr << "Missing GeogLinearUnitsGeoKey (" << GeogLinearUnitsGeoKey << ") key!\n";
      return false;
    }
    if(length != 1 || ttype != TYPE_SHORT) {
      std::cerr << "Expected a single value with type int16 (short)!\n";
      return false;
    }
    auto *val = static_cast<short*> (value);

    if (*val != Linear_Meter) {
      std::cerr << "Linear units are not in Meters!\n";
      return false;
    }

    status = get_key_value(GeogAngularUnitsGeoKey, &value, size, length, ttype);
    if (!status) {
      std::cerr << "Missing GeogAngularUnitsGeoKey (" << GeogAngularUnitsGeoKey << ") key!\n";
      return false;
    }
    if(length != 1 || ttype != TYPE_SHORT) {
      std::cerr << "Expected a single value with type int16 (short)!\n";
      return false;
    }
    val = static_cast<short*> (value);

    if (*val != Angular_Degree) {
      std::cerr << "Angular units are not in Degrees!\n";
      return false;
    }

    return true;
  }
  return false;
}

//: returns the Zone and the Hemisphere (0 for N, 1 for S);
bool vil_geotiff_header::PCS_NAD83_UTM_zone(int &zone, GTIF_HEMISPH &hemisph)
{
  modeltype_t type;
  if (gtif_modeltype(type) && type == ModelTypeProjected)
  {
    void *value;
    int size;
    int length;
    tagtype_t ttype;
    bool status = get_key_value(ProjectedCSTypeGeoKey, &value, size, length, ttype);
    if (!status) {
      std::cerr << "Missing ProjectedCSTypeGeoKey (" << ProjectedCSTypeGeoKey << ") key!\n";
      return false;
    }
    if(length != 1 || ttype != TYPE_SHORT) {
      std::cerr << "Expected a single value with type int16 (short)!\n";
      return false;
    }

    auto *val = static_cast<short*> (value);
    if ((*val < PCS_NAD83_UTM_zone_3N ) || ((*val > PCS_NAD83_Missouri_West ))) {
      std::cerr << "NOT in RANGE PCS_NAD83_UTM_zone_3N and PCS_NAD83_Missouri_West!\n";
      return false;
    }
    zone = *val - 26900;
    hemisph = NORTH;

    return true;
  }
  else {
    hemisph = UNDEF;
    return false;
  }
}


bool vil_geotiff_header::get_key_value(geokey_t key, void** value,
                                       int& size, int& length, tagtype_t& type)
{
  length = GTIFKeyInfo(gtif_, key, &size, &type);

  // check if it is a valid key id
  if (length == 0) {
    // key is not defined
    std::cerr << "KeyID=" << (short int)key << " is not valid\n";
    return false;
  }
  else {
    *value = std::malloc(size*length);
    GTIFKeyGet(gtif_, key, *value, 0, length);
    return true;
  }
}
