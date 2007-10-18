#include "bwm_observer_lidar.h"
#include "bwm_observer_vgui.h"
#include "bwm_tableau_mgr.h"
#include "algo/bwm_utm_lat_lon.h"
#include "algo/bwm_lidar_algo.h"
#include "algo/bwm_algo.h"

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/file_formats/vil_tiff.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
bwm_observer_lidar::bwm_observer_lidar(bgui_image_tableau_sptr const& img,
                                       vil_image_resource_sptr res1, 
                                       vil_image_resource_sptr res2)
:bwm_observer_cam(img), first_ret_(res1), second_ret_(res2)
{
  // create transformation matrix and LVCS from the geotiff keys
  //img_res_ =  img->get_image_resource();

  // the file should be a geotiff
  vcl_cout << "FORMAT=" << first_ret_->file_format();
  if (strcmp(first_ret_->file_format(), "tiff") != 0) 
    return;

  vil_tiff_image* tiff_img = static_cast<vil_tiff_image*> (first_ret_.as_pointer());
  
  // check if the tiff file is geotiff
  if (!tiff_img->is_GEOTIFF()) {
    return;
  }

  vil_geotiff_header* gtif = tiff_img->get_geotiff_header();
  int utm_zone;
  vil_geotiff_header::GTIF_HEMISPH h;

  // convert given tiepoint to world coordinates (lat, long) 
  // based on the model defined in GEOTIFF

  // is this a PCS_WGS84_UTM?
  if (gtif->PCS_WGS84_UTM_zone(utm_zone, h))
  {
    gtif->gtif_tiepoints(tiepoints);
    bool south_flag = false;
    if (h == 1)
      south_flag = true;

    // transform each tiepoint
    bwm_utm_lat_lon utm;
    double lat, lon, elev ;
    for (unsigned i=0; i< tiepoints.size(); i++) {
      vcl_vector<double> tiepoint = tiepoints[i];
      assert (tiepoint.size() == 6);
      double I = tiepoint[0];
      double J = tiepoint[1];
      double K = tiepoint[2];
      double X = tiepoint[3];
      double Y = tiepoint[4];
      double Z = tiepoint[5];
      
      //double lat1, lon1, elev1;
      utm.transform(utm_zone, X, Y, Z, lat, lon, elev, south_flag );
      
      scale_ = 1; 
      // now, we have a mapping (I,J,K)->(X,Y,Z)
    }


    // create a transformation matrix
    // if there is a transormation matrix in GEOTIFF, use that
    set_trans_matrix(gtif, tiepoints);

    //use one of the tiepoints to create an LVCS for starter, user may change it 
    //later by defining a new LVCS origin
    lvcs_ = new bgeo_lvcs(lat, lon, elev);//, bgeo_lvcs::wgs84);

    // create image view
    vil_tiff_image* tiff_img = static_cast<vil_tiff_image*> (first_ret_.as_pointer());
    img_view_ = *(tiff_img->get_view());
  } else {
      vcl_cout << "Only ProjectedCSTypeGeoKey=PCS_WGS84_UTM_zoneXX_X is defined rigth now, please define yours!!" << vcl_endl;
  }
}

void bwm_observer_lidar::set_lvcs(double lat, double lon, double elev)
{
//  if (lvcs_ != 0)
 //    delete lvcs_;
  lvcs_ = new bgeo_lvcs(lat, lon, elev);
}

void bwm_observer_lidar::set_trans_matrix(vil_geotiff_header* gtif,
                                      vcl_vector<vcl_vector<double> > tiepoints)
{
//return;//JLM Kludge
//#if 0
  double sx, sy, sz, sx1, sy1, sz1;
  double* trans_matrix_values;
  if (gtif->gtif_trans_matrix(trans_matrix_values)){
     vcl_cout << "Transfer matrix is given, using that...." << vcl_endl;
     trans_matrix_.copy_in(trans_matrix_values);
     vcl_cout << trans_matrix_ << vcl_endl;
  } else if (gtif->gtif_pixelscale(sx1, sy1, sz1)) {

    // use tiepoints and sclae values to create a transformation matrix
    // for now use the first tiepoint if there are more than one
    assert (tiepoints.size() > 0);
    vcl_vector<double> tiepoint = tiepoints[0];
    assert (tiepoint.size() == 6);
    double I = tiepoint[0];
    double J = tiepoint[1];
    double K = tiepoint[2];
    double X = tiepoint[3];
    double Y = tiepoint[4];
    double Z = tiepoint[5];

    // Define a transformation matrix as follows:
    // 
    //      |-                         -| 
    //      |   Sx    0.0   0.0   Tx    | 
    //      |                           |      Tx = X - I*Sx
    //      |   0.0  -Sy    0.0   Ty    |      Ty = Y + J*Sy
    //      |                           |      Tz = Z - K*Sz
    //      |   0.0   0.0   Sz    Tz    | 
    //      |                           | 
    //      |   0.0   0.0   0.0   1.0   | 
    //      |-                         -| 
    
    sx = scale_; sy=scale_; sz=scale_;
    double Tx = X - I*sx;
    double Ty = Y + J*sy;
    double Tz = Z - K*sz;

    vnl_matrix<double> m(4,4);
    m.fill(0.0);
    m[0][0] = sx;
    m[1][1] = -1*sy;
    m[2][2] = sz;
    m[3][3] = 1.0;
    m[0][3] = Tx;
    m[1][3] = Ty;
    m[2][3] = Tz;
    trans_matrix_ = m;
    vcl_cout << trans_matrix_ << vcl_endl;
  } else {
    vcl_cerr << "Transform matrix cannot be formed.. " << vcl_endl;
  }
//#endif
}
//: transforms a given 3d world point to image plane
void bwm_observer_lidar::proj_point(vsol_point_3d_sptr p3d, 
                                     vsol_point_2d_sptr& p2d)
{
  vnl_vector<double> v(4), res(4);
  double lat, lon, gz;
  lvcs_->local_to_global(p3d->x(), p3d->y(), p3d->z(), bgeo_lvcs::wgs84, lon, lat, gz);
  bwm_utm_lat_lon utm;
  //double lat, lon, elev;
  double x, y;
  int utm_zone;
  utm.transform(lat, lon, x, y, utm_zone);
  v[0] = x;
  v[1] = y;
  v[2] = 0;
  v[3] = 1;
  trans_matrix_[2][2] = 1;
  //vcl_cout << trans_matrix_ << vcl_endl;
  vnl_matrix<double> trans_matrix_inv = vnl_inverse(trans_matrix_);
  res = trans_matrix_inv*v;
  //vcl_cout << res[0] << " " << res[1] << vcl_endl;
  p2d = new vsol_point_2d (res[0], res[1]);
}
   
//: transforms a given 3d world point to image plane
void bwm_observer_lidar::backproj_point(vsol_point_2d_sptr p2d, 
                                         vsol_point_3d_sptr &p3d)
{
  vnl_vector<double> v(4), res(4);
  v[0] = tiepoints[0][3] + p2d->x();
  v[1] = tiepoints[0][4] - p2d->y();
  v[2] = 0;
  v[3] = 1;

  //vcl_cout << "Northing=" << v[0] << " Easting=" << v[1];

  //find the UTM values 
  bwm_utm_lat_lon utm;
  double lat, lon, elev;

  utm.transform(38, v[0], v[1], v[2], lat, lon, elev); 

  p3d = new vsol_point_3d (lon, lat, elev);
  p3d->set_z(img_view_(p2d->x(), p2d->y()));

}
void bwm_observer_lidar::proj_poly(vsol_polygon_3d_sptr poly3d, 
                                    vsol_polygon_2d_sptr& poly2d)
{
  vcl_vector<vsol_point_2d_sptr> vertices;
  for (unsigned i=0; i<poly3d->size(); i++) {
    vsol_point_3d_sptr p3d = poly3d->vertex(i);
    // transfer the point to geographic coord
    vsol_point_2d_sptr p2d;
    //vcl_cout << *p3d << vcl_endl;
    proj_point(p3d, p2d);
    //vcl_cout << *p2d << vcl_endl;
    // now find a 3d coordinates by bgeo_lvcs
    vertices.push_back(p2d);
  }

  poly2d = new vsol_polygon_2d (vertices);
}

void bwm_observer_lidar::backproj_poly(vsol_polygon_2d_sptr poly2d, 
                                        vsol_polygon_3d_sptr& poly3d)
{
  vcl_vector<vsol_point_3d_sptr> vertices;
  for (unsigned i=0; i<poly2d->size(); i++) {
    vsol_point_2d_sptr p2d = poly2d->vertex(i);
   
    // transfer the point to geographic coord
    vsol_point_3d_sptr p3d;
    //vcl_cout << vcl_endl << "-----------------------" << vcl_endl 
    //  << *p2d << vcl_endl;
    backproj_point(p2d, p3d);
    //vcl_cout << *p3d << vcl_endl;
    //set the elevation from the pixel intensity
    
    p3d->set_z(img_view_(p2d->x(), p2d->y()));

    // now find a 3d coordinates by bgeo_lvcs
    double lx, ly, lz;
    lvcs_->global_to_local(p3d->x(), p3d->y(), p3d->z(), bgeo_lvcs::wgs84,
      lx, ly, lz, bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    vsol_point_3d_sptr p = new vsol_point_3d(lx, ly, lz);
    vertices.push_back(p);
  }

  // make sure that the points are on the same plane, if not make them
  poly3d = bwm_algo::move_points_to_plane(vertices); //new vsol_polygon_3d (vertices);
}

void bwm_observer_lidar::set_ground_plane(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  // find the image pixel values of the given points to calculate the elevation
  vil_pixel_format pf = first_ret_->pixel_format();
  vcl_cout << pf << vcl_endl;

  if (pf == VIL_PIXEL_FORMAT_FLOAT) {
    vil_tiff_image* tiff_img = static_cast<vil_tiff_image*> (first_ret_.as_pointer());
    vil_image_view<float> view = *(tiff_img->get_view());
    unsigned int ni = view.ni();
    unsigned int nj = view.nj();
    assert((x1 < ni) && (x2 < ni) && (y1 < nj) && (y2 < nj));
    float i1 = view(x1,y1);
    float i2 = view(x2,y2);
    ground_elev_ = (i1 + i2) / 2.;
  } else 
    vcl_cout << "bwm_observer_lidar::set_ground_plane() -- Pixel Format [" << 
    pf << "] is not supported yet" << vcl_cout;
}

void bwm_observer_lidar::get_img_to_wgs(const unsigned i, const unsigned j,
                                     double& lon, double& lat, double& elev)
{
  this->img_to_wgs(i, j, lon, lat);
  if (i < img_view_.ni() && j < img_view_.nj())
    elev = img_view_(i, j);
}

void bwm_observer_lidar::img_to_wgs(const unsigned i, const unsigned j,
                                     double& lon, double& lat)
{
  vnl_vector<double> v(4), res(4);
  v[0] = tiepoints[0][3] + i;
  v[1] = tiepoints[0][4] - j;
  v[2] = 0;
  v[3] = 1;
  //find the UTM values 
  bwm_utm_lat_lon utm;
  double elev;
  utm.transform(38, v[0], v[1], v[2], lat, lon, elev); 
}

bool bwm_observer_lidar::get_point_cloud(const float x1, const float y1,
                                          const float x2, const float y2,
                                          bgeo_lvcs& lvcs,
                                          vcl_vector<vsol_point_3d_sptr>& points)
{
  if(!img_view_)
    return false;
  unsigned ni = img_view_.ni(), nj = img_view_.nj();
  if(x1<0||x2<0||y1<0||y2<0||x1>=ni||x2>=ni||y1>=nj||y2>=nj)
    return false;
  points.clear();
  unsigned ix1 = static_cast<unsigned>(x1);
  unsigned ix2 = static_cast<unsigned>(x2);
  if(x2<x1)
    {
      ix1 = static_cast<unsigned>(x2);
      ix2 = static_cast<unsigned>(x1);
    }
  unsigned iy1 = static_cast<unsigned>(y1);
  unsigned iy2 = static_cast<unsigned>(y2);
  if(y2<y1)    
    {
      iy1 = static_cast<unsigned>(y2);
      iy2 = static_cast<unsigned>(y1);
    }
  double lon, lat;
  //define the lvcs in the middle of the box
  unsigned im = (ix2+ix1)/2, jm = (iy2+iy1)/2;
  this->img_to_wgs(im, jm, lon, lat);
  double elev = img_view_(im, jm);
  lvcs = bgeo_lvcs(lat, lon, elev);
  for(unsigned j = iy1; j<=iy2; ++j)
    for(unsigned i = ix1; i<=ix2; ++i)
      {
        this->img_to_wgs(i, j, lon, lat);
        elev = img_view_(i, j);
        double lx, ly, lz;
        lvcs.global_to_local(lon, lat, elev, bgeo_lvcs::wgs84,
                             lx, ly, lz, bgeo_lvcs::DEG,bgeo_lvcs::METERS);
        vsol_point_3d_sptr p = new vsol_point_3d(lx, ly, lz);
        points.push_back(p);
      }
  return true;
}

bool bwm_observer_lidar::save_meshed_point_cloud(const float x1, const float y1, const float x2, const float y2, vcl_string file)
{

  vcl_ofstream os(file.c_str());  
  if(!os.is_open())
  {
    vcl_cout << "Bad file path\n";
    return false;
  }

  vcl_vector<vsol_point_3d_sptr> points;
  //observer_left_->get_point_cloud(x1, y1, x2, y2, lvcs, points);

  //vil_image_view<float> img_view = (vil_image_view<float>)observer_left_->get_image_tableau()->get_image_view();
  //vil_image_resource_sptr img_res = observer_left_->get_image_tableau()->get_image_resource();

  if(!img_view_)
    return false;

  unsigned ni = img_view_.ni(), nj = img_view_.nj();
  if(x1<0||x2<0||y1<0||y2<0||x1>=ni||x2>=ni||y1>=nj||y2>=nj)
    return false;

  unsigned ix1 = static_cast<unsigned>(x1);
  unsigned ix2 = static_cast<unsigned>(x2);
  if(x2<x1)
  {
    ix1 = static_cast<unsigned>(x2);
    ix2 = static_cast<unsigned>(x1);
  }
  unsigned iy1 = static_cast<unsigned>(y1);
  unsigned iy2 = static_cast<unsigned>(y2);
  if(y2<y1)    
  {
    iy1 = static_cast<unsigned>(y2);
    iy2 = static_cast<unsigned>(y1);
  }
  double lon, lat;
  //define the lvcs in the middle of the box
  //unsigned im = (ix2+ix1)/2, jm = (iy2+iy1)/2;
  //this->img_to_wgs(im, jm, lon, lat);
  //double elev = img_view_(im, jm);
  //lvcs = bgeo_lvcs(lat, lon, elev);

  vcl_vector<vgl_point_3d<double> > vertices;

  for(unsigned j = iy1; j<=iy2; ++j) {
    for(unsigned i = ix1; i<=ix2; ++i) {
      this->img_to_wgs(i, j, lon, lat);
      float elev = img_view_(i, j);
      vgl_point_3d<double> p(lon,lat,elev);
      vertices.push_back(p);
    }
  }

  int sel_height = iy2 - iy1 + 1;
  int sel_width = ix2 - ix1 + 1;

  // use vgl_point_3d to hold triangle vertex indices
  vcl_vector<vgl_point_3d<int> > faces;

  for (int j=0; j < sel_height - 1; ++j) {
    for (int i=0; i < sel_width - 1; ++i) {
      int idx = j*sel_width + i;
      vgl_point_3d<int> f1(idx,idx+sel_width+1,idx+sel_width);
      faces.push_back(f1);
      vgl_point_3d<int> f2(idx,idx+1,idx+1+sel_width);
      faces.push_back(f2);
    }
  }

  // write out ply2 file

  unsigned nverts = vertices.size();
  unsigned nfaces = faces.size();
  os << nverts << vcl_endl;
  os << nfaces << vcl_endl;
  os.precision(12);
  for (unsigned i=0; i < nverts; ++i) {
    vgl_point_3d<double> vert = vertices[i];
    os << vert.x() << " " << vert.y() << " " << vert.z() << vcl_endl;
  }
  for (unsigned i=0; i < nfaces; ++i) {
    vgl_point_3d<int> face = faces[i];
    os << "3 " << face.x() << " " << face.y() << " " << face.z() << vcl_endl;
  }

  return true;
}

void bwm_observer_lidar::label_lidar(lidar_labeling_params params)
{
  
  if (!second_ret_) {
    vgui_dialog error ("Error");
    error.message ("Second Return is not set, cannot do labeling!" );
    error.ask();
    return;
  }

  vil_image_view<float> first_ret = first_ret_->get_view();
  vil_image_view<float> second_ret = second_ret_->get_view();
  vil_image_view<vxl_byte> labeled(first_ret.ni(), first_ret.nj(), 3);
  bwm_lidar_algo::label_lidar(first_ret, second_ret, params, labeled);

  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->set_image_view(labeled);
  img->center_pixels();

  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(img);
  bwm_tableau_mgr* mgr = bwm_tableau_mgr::instance();
//  mgr->add_tableau(viewer, "lidar_color");
}
