#include "vpgl_nitf_RSM_camera.h"
#include "vpgl_nitf_rational_camera.h"
#include <vpgl/vpgl_affine_camera.h>
#include <vil/vil_load.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <fstream>
bool
vpgl_nitf_RSM_camera::init(vil_nitf2_image * nitf_image, bool verbose)
{
  std::vector<vil_nitf2_image_subheader *> headers = nitf_image->get_image_headers();
   vil_nitf2_image_subheader * hdr = headers[1];//fix
   if(!hdr->get_property("IXSHD", isxhd_tres_)){
     std::cout << "IXSHD Property failed in vil_nitf2_image_subheader\n";
     return false;
   }
   // Get common metadata from the nitf2_image and image subheader
   if(!hdr->get_property("IID2", rsm_meta_.image_name_)){
     std::cout << "IID2 Property failed in vil_nitf2_image_subheader\n";
   }else rsm_meta_.image_name_valid = true;
   //trim to standard length
   rsm_meta_.image_name_ = rsm_meta_.image_name_.substr(0, 39);
   

   rsm_meta_.effective_bits_per_pixel_ = hdr->get_number_of_bits_per_pixel();
   rsm_meta_.effective_bits_per_pixel_valid = true;

   rsm_meta_.platform_name_ = hdr->get_image_source();
   rsm_meta_.platform_name_valid = true;

   if(!hdr->get_property("IGEOLO", image_igeolo_)){
       std::cout << "IGEOLO Property failed in vil_nitf2_image_subheader\n";
   }else igeolo_valid_ = true;

   // elevation is degrees above tangent plane
   // azimuth is clockwise from North
   double sun_elev, sun_azimuth;
   if(!hdr->get_sun_params(sun_elev, sun_azimuth)){
       std::cout << "get_sun_params failed in vil_nitf2_image_subheader\n";
   }else rsm_meta_.sun_angles_valid = true;

   if (rsm_meta_.sun_angles_valid)
     rsm_meta_.sun_angles_.set(sun_azimuth, sun_elev);

   image_time t;
   if( !hdr->get_date_time(t.year, t.month, t.day, t.hour, t.min, t.sec)){
     std::cout << "get_date_time failed in vil_nitf2_image_subheader\n";
   }else rsm_meta_.acquisition_time_valid = true;
   if(rsm_meta_.acquisition_time_valid)
     rsm_meta_.acqusisition_time_ = t;

   // offset from ICHIPB (translation)
   // or optionally STDIDC or STDIDB Block(START_COLUMN, START_ROW)
   double u_off, v_off;
   if( !hdr->get_image_offset(u_off, v_off)){
     std::cout << "get_image offset failed in vil_nitf2_image_subheader\n";
   }else rsm_meta_.image_offset_valid = true;
   if(rsm_meta_.image_offset_valid)
     rsm_meta_.image_offset_.set(u_off, v_off);

   if( !hdr->get_ichipb_info(ichipb_.translation_, ichipb_.F_grid_points_,
                              ichipb_.O_grid_points_,ichipb_.scale_factor_,
                              ichipb_.anamorphic_corr_)){
     std::cout << "ichipb_info not present in vil_nitf2_image_subheader\n";
   }else ichipb_.ichipb_data_valid_ = true;

   // extract corner coordinates from image_geolo field
  // example 324158N1171117W324506N1171031W324428N1170648W324120N1170734W
   if(igeolo_valid_){
     double ULlat, ULlon;
     double URlat, URlon;
     double LLlat, LLlon;
     double LRlat, LRlon;

     vpgl_nitf_rational_camera::geostr_to_latlon(image_igeolo_.c_str(), &ULlat, &ULlon);
     vpgl_nitf_rational_camera::geostr_to_latlon(image_igeolo_.c_str() + 15, &URlat, &URlon);
     vpgl_nitf_rational_camera::geostr_to_latlon(image_igeolo_.c_str() + 30, &LRlat, &LRlon);
     vpgl_nitf_rational_camera::geostr_to_latlon(image_igeolo_.c_str() + 45, &LLlat, &LLlon);
     // the rational camera tres have, e.g. ul(lat, lon) however
     // if the point is interpreted as x, y then the order
     // should be ul(lon, lat), so
     unsigned LON = 0, LAT = 1;
     ul_[LAT] = ULlat;
     ul_[LON] = ULlon;
     ur_[LAT] = URlat;
     ur_[LON] = URlon;
     ll_[LAT] = LLlat;
     ll_[LON] = LLlon;
     lr_[LAT] = LRlat;
     lr_[LON] = LRlon;
   }
   return true;
}
vpgl_nitf_RSM_camera::vpgl_nitf_RSM_camera(std::string const & nitf_image_path, bool verbose)
{
  // first open the nitf image
  vil_image_resource_sptr image = vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "Image load failed in vpgl_nitf_RSM_camera_constructor\n";
    return;
  }
  std::string format = image->file_format();
  std::string prefix = format.substr(0, 4);
  if (prefix != "nitf")
  {
    std::cout << "not a nitf image in vpgl_nitf_RSM_camera_constructor\n";
    return;
  }
  // cast to an nitf2_image
  auto * nitf_image = (vil_nitf2_image *)image.ptr();

  // read information
  this->init(nitf_image, verbose);
}

vpgl_nitf_RSM_camera::vpgl_nitf_RSM_camera(vil_nitf2_image * nitf_image, bool verbose)
{
  this->init(nitf_image, verbose);
}

bool vpgl_nitf_RSM_camera::raw_tres(std::ostream& tre_str, bool verbose ) const
{
  // Now get the sub-header TRE parameters
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  // Check through the TREs to find ""

  if (!tre_str){
      std::cout << "bad stream" << std::endl;
      return false;
  }
  bool v = verbose;

  for (tres_itr = isxhd_tres_.begin(); tres_itr != isxhd_tres_.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();

    if (type == "RSMIDA") // looking for "RSMIDA..."
    {
      if (!tre_str)
        std::cout << "bad stream" << std::endl;
      //Start TRE section =====================
      nitf_tre<std::string> st("RSMIDA", tre_str);
      //=======================================
      // RSMIDA TREs
      nitf_tre<std::string> nt0("IID", false, true);
      nt0.get_append(tres_itr, tre_str, v);

      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);

      nitf_tre<std::string> nt2("ISID", false, true);
      nt2.get_append(tres_itr, tre_str, v);
      
      nitf_tre<std::string> nt3("SID", false, true);
      nt3.get_append(tres_itr, tre_str, v);
      
      nitf_tre<std::string> nt4("STID", false, true);
      nt4.get_append(tres_itr, tre_str, v);
      
      nitf_tre<int> nt5("YEAR", false, true);
      nt5.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt6("MONTH", false, true);
      nt6.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt7("DAY", false, true);
      nt7.get_append(tres_itr, tre_str, v);
      
      nitf_tre<int> nt8("HOUR", false, true);
      nt8.get_append(tres_itr, tre_str, v);
      
      nitf_tre<int> nt9("MINUTE", false, true);
      nt9.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt10("SECOND", false, true);
      nt10.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt11("NRG", false, true);
      nt11.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt12("NCG", false, true);
      nt12.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt13("TRG", false, true);
      nt13.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt14("TCG", false, true);
      nt14.get_append(tres_itr, tre_str, v);

      nitf_tre<std::string> nt15("NDD", false, false);
      nt15.get_append(tres_itr, tre_str, v);

      bool opt = nt15.value_ == "G";
      
      nitf_tre<double> nt16("XUOR", opt, false);
      nt16.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt17("YUOR", opt, false);
      nt17.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt18("ZUOR", opt, false);
      nt18.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt19("XUXR", opt, false);
      nt19.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt20("XUYR", opt, false);
      nt20.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt21("XUZR", opt, false);
      nt21.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt22("YUXR", opt, false);
      nt22.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt23("YUYR", opt, false);
      nt23.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt24("YUZR", opt, false);
      nt24.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt25("ZUXR", opt, false);
      nt25.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt26("ZUYR", opt, false);
      nt26.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt27("ZUZR", opt, false);
      nt27.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt28("V1X", false, false);
      nt28.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt29("V1Y", false, false);
      nt29.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt30("V1Z", false, false);
      nt30.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt31("V2X", false, false);
      nt31.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt32("V2Y", false, false);
      nt32.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt33("V2Z", false, false);
      nt33.get_append(tres_itr, tre_str, v);
      
      nitf_tre<double> nt34("V3X", false, false);
      nt34.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt35("V3Y", false, false);
      nt35.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt36("V3Z", false, false);
      nt36.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt37("V4X", false, false);
      nt37.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt38("V4Y", false, false);
      nt38.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt39("V4Z", false, false);
      nt39.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt40("V5X", false, false);
      nt40.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt41("V5Y", false, false);
      nt41.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt42("V5Z", false, false);
      nt42.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt43("V6X", false, false);
      nt43.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt44("V6Y", false, false);
      nt44.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt45("V6Z", false, false);
      nt45.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt46("V7X", false, false);
      nt46.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt47("V7Y", false, false);
      nt47.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt48("V7Z", false, false);
      nt48.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt49("V8X", false, false);
      nt49.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt50("V8Y", false, false);
      nt50.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt51("V8Z", false, false);
      nt51.get_append(tres_itr, tre_str, v);
      opt = true;

      nitf_tre<double> nt52("GRPX", opt, false);
      nt52.get_append(tres_itr, tre_str, v);
      
      nitf_tre<double> nt53("GRPY", opt, false);
      nt53.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt54("GRPZ", opt, false);
      nt54.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt55("FULLR", false, true);
      nt55.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt56("FULLC", false, true);
      nt56.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt57("MINR", false, false);
      nt57.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt58("MAXR", false, false);
      nt58.get_append(tres_itr, tre_str, v);
      

      nitf_tre<int> nt59("MINC", false, false);
      nt59.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt60("MAXC", false, false);
      nt60.get_append(tres_itr, tre_str, v);

      opt = true;
      nitf_tre<double> nt61("IE0", opt, false);
      nt61.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt62("IER", opt, false);
      nt62.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt63("IEC", opt, false);
      nt63.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt64("IERR", opt, false);
      nt64.get_append(tres_itr, tre_str, v);
      
      nitf_tre<double> nt65("IERC", opt, false);
      nt65.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt66("IECC", opt, false);
      nt66.get_append(tres_itr, tre_str, v);
   
      nitf_tre<double> nt67("IA0", opt, false);
      nt67.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt68("IAR", opt, false);
      nt68.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt69("IAC", opt, false);
      nt69.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt70("IARR", opt, false);
      nt70.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt71("IARC", opt, false);
      nt71.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt72("IACC", opt, false);
      nt72.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt73("SPX", opt, false);
      nt73.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt74("SVX", opt, false);
      nt74.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt75("SAX", opt, false);
      nt75.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt76("SPY", opt, false);
      nt76.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt77("SVY", opt, false);
      nt77.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt78("SAY", opt, false);
      nt78.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt79("SPZ", opt, false);
      nt79.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt80("SVZ", opt, false);
      nt80.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt81("SAZ", opt, false);
      nt81.get_append(tres_itr, tre_str, v);
    }  
    if (type == "RSMPCA"){ // looking for "RSMPCA..."
      // =======================================
      nitf_tre<std::string> nt82("RSMPCA", tre_str);
      // =========================================
      bool opt = false;
      nitf_tre<std::string> nt83("IID", true, false);
      nt83.get_append(tres_itr, tre_str, v);

      nitf_tre<std::string> nt84("EDITION", false, false);
      nt84.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt85("RSN", false, false);
      nt85.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt86("CSN", false, false);
      nt86.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt87("RFEP",true , false);
      nt87.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt88("CFEP", true, false);
      nt88.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt89("RNRMO", false, false);
      nt89.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt90("CNRMO", false, false);
      nt90.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt91("XNRMO", false, false);
      nt91.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt92("YNRMO", false, false);
      nt92.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt93("ZNRMO", false, false);
      nt93.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt94("RNRMSF", false, false);
      nt94.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt95("CNRMSF", false, false);
      nt95.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt96("XNRMSF", false, false);
      nt96.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt97("YNRMSF", false, false);
      nt97.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt98("ZNRMSF", false, false);
      nt98.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt99("RNPWRX", false, false);
      nt99.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt100("RNPWRY", false, false);
      nt100.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt101("RNPWRZ", false, false);
      nt101.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt102("RNTRMS", false, false);
      nt102.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt103("RNPCF","vector", false, false);
      nt103.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt104("RDPWRX", false, false);
      nt104.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt105("RDPWRY", false, false);
      nt105.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt106("RDPWRZ", false, false);
      nt106.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt107("RDTRMS", false, false);
      nt107.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt108("RDPCF","vector", false, false);
      nt108.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt109("CNPWRX", false, false);
      nt109.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt110("CNPWRY", false, false);
      nt110.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt111("CNPWRZ", false, false);
      nt111.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt112("CNTRMS", false, false);
      nt112.get_append(tres_itr, tre_str, v);
      
      nitf_tre<double> nt113("CNPCF","vector", false, false);
      nt113.get_append(tres_itr, tre_str, v);
      
      nitf_tre<int> nt114("CDPWRX", false, false);
      nt114.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt115("CDPWRY", false, false);
      nt115.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt116("CDPWRZ", false, false);
      nt116.get_append(tres_itr, tre_str, v);

      nitf_tre<int> nt117("CDTRMS", false, false);
      nt117.get_append(tres_itr, tre_str, v);

      nitf_tre<double> nt118("CDPCF","vector", false, false);
      nt118.get_append(tres_itr, tre_str, v);
    }
  }
  bool RSMPIA = false, RSMGIA = false, RSMECA = false, RSMECB = false,
  RSMDCA = false, RSMDCB = false; bool RSMAPA = false, RSMAPB = false, RSMGGA = false; 
  for (tres_itr = isxhd_tres_.begin(); tres_itr != isxhd_tres_.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();

    if (type == "RSMPIA"){ // looking for "RSMPIA..."
      // =======================================
      nitf_tre<std::string> nt("RSMPIA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMPIA = true;
    }


    if (type == "RSMGIA"){ // looking for "RSMGIA..."
      // =======================================
      nitf_tre<std::string> nt("RSMGIA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMGIA = true;
    }
    if (type == "RSMDCA"){ // looking for "RSMDCA..."
      // =======================================
      nitf_tre<std::string> nt("RSMDCA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMDCA = true;
    }

    if (type == "RSMDCB"){ // looking for "RSMDCB..."
      // =======================================
      nitf_tre<std::string> nt("RSMDCB", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMDCB = true;
    }
    if (type == "RSMECA"){ // looking for "RSMECA..."
      // =======================================
      nitf_tre<std::string> nt("RSMECA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMECA = true;
    }

    if (type == "RSMECB"){ // looking for "RSMDCB..."
      // =======================================
      nitf_tre<std::string> nt("RSMECB", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMECB = true;
    }

    if (type == "RSMAPA"){ // looking for "RSMAPA..."
      // =======================================
      nitf_tre<std::string> nt("RSMAPA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMAPA = true;
    }

    if (type == "RSMAPB"){ // looking for "RSMAPB..."
      // =======================================
      nitf_tre<std::string> nt("RSMAPB", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMAPB = true;
    }
        
    if (type == "RSMGGA"){ // looking for "RSMGGA..."
      // =======================================
      nitf_tre<std::string> nt("RSMGGA", tre_str);
      // =========================================
      nitf_tre<std::string> nt1("EDITION", false, false);
      nt1.get_append(tres_itr, tre_str, v);
      RSMGGA = true;
    }
  }
  tre_str << "\n===========  TREs not present in NITF2.1 Image Header ===========" << std::endl;
  if(!RSMPIA) tre_str << "RSMPIA" << std::endl;
  if(!RSMGIA) tre_str << "RSMGIA" << std::endl;
  if(!RSMDCA) tre_str << "RSMDCA" << std::endl;
  if(!RSMDCB) tre_str << "RSMDCB" << std::endl;
  if(!RSMECA) tre_str << "RSMECA" << std::endl;
  if(!RSMECB) tre_str << "RSMECB" << std::endl;
  if(!RSMAPA) tre_str << "RSMAPA" << std::endl;
  if(!RSMAPB) tre_str << "RSMAPB" << std::endl;
  if(!RSMGGA) tre_str << "RSMGGA" << std::endl;
  return true;
}
bool vpgl_nitf_RSM_camera::set_RSM_camera_params()
{
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  std::vector<std::vector<int> > powers;
  std::vector<std::vector<double> > coeffs;
  std::vector<vpgl_scale_offset<double> > scale_offsets;

  double x_scale, x_off;
  double y_scale, y_off;
  double z_scale, z_off;
  double u_scale, u_off;
  double v_scale, v_off;
  int x_pow, y_pow, z_pow;
  bool good = false;
  for (tres_itr = isxhd_tres_.begin(); tres_itr != isxhd_tres_.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();
    if (type == "RSMPCA"){
      nitf_tre<double> nt0("RNRMO");
      good = nt0.get(tres_itr, v_off);
      if(!good) return false;

      nitf_tre<double> nt1("CNRMO");
      good = nt1.get(tres_itr, u_off);
      if(!good) return false;
      
      nitf_tre<double> nt2("XNRMO");
      good = nt2.get(tres_itr, x_off);
      if(!good) return false;

      nitf_tre<double> nt3("YNRMO");
      good = nt3.get(tres_itr, y_off);
      if(!good) return false;

      nitf_tre<double> nt4("ZNRMO");
      good = nt4.get(tres_itr, z_off);
      if(!good) return false;
      
      nitf_tre<double> nt5("RNRMSF");
      good = nt5.get(tres_itr, v_scale);
      if(!good) return false;

      nitf_tre<double> nt6("CNRMSF");
      good = nt6.get(tres_itr, u_scale);
      if(!good) return false;

      nitf_tre<double> nt7("XNRMSF");
      good = nt7.get(tres_itr, x_scale);
      if(!good) return false;
      
      nitf_tre<double> nt8("YNRMSF");
      good = nt8.get(tres_itr, y_scale);
      if(!good) return false;

      nitf_tre<double> nt9("ZNRMSF");
      good = nt9.get(tres_itr, z_scale);
      if(!good) return false;

      scale_offsets.emplace_back(x_scale, x_off);
      scale_offsets.emplace_back(y_scale, y_off);
      scale_offsets.emplace_back(z_scale, z_off);
      scale_offsets.emplace_back(u_scale, u_off);
      scale_offsets.emplace_back(v_scale, v_off);

      int rn_nterms;
      std::vector<int> rnpows;
      nitf_tre<int> nt10("RNPWRX");
      good = nt10.get(tres_itr, x_pow);
      if(!good) return false;

      nitf_tre<int> nt11("RNPWRY");
      good = nt11.get(tres_itr, y_pow);
      if(!good) return false;
      
      nitf_tre<int> nt12("RNPWRZ");
      good = nt12.get(tres_itr, z_pow);
      if(!good) return false;

      nitf_tre<int> nt13("RNTRMS");
      good = nt13.get(tres_itr, rn_nterms);
      if(!good) return false;

      rnpows.push_back(x_pow); rnpows.push_back(y_pow); rnpows.push_back(z_pow);

      std::vector<double> rnpcf;
      nitf_tre<double> nt14("RNPCF");
      good = nt14.get(tres_itr, rnpcf);
      if(!good) return false;

      int rd_nterms;
      std::vector<int> rdpows;
      nitf_tre<int> nt15("RDPWRX");
      good = nt15.get(tres_itr, x_pow);
      if(!good) return false;

      nitf_tre<int> nt16("RDPWRY");
      good = nt16.get(tres_itr, y_pow);
      if(!good) return false;
      
      nitf_tre<int> nt17("RDPWRZ");
      good = nt17.get(tres_itr, z_pow);
      if(!good) return false;

      nitf_tre<int> nt18("RDTRMS");
      good = nt18.get(tres_itr, rd_nterms);
      if(!good) return false;

      rdpows.push_back(x_pow);rdpows.push_back(y_pow);rdpows.push_back(z_pow);

      std::vector<double> rdpcf;
      nitf_tre<double> nt19("RDPCF");
      good = nt19.get(tres_itr, rdpcf);
      if (!good) return false;
      int cn_nterms;
      std::vector<int> cnpows;
      nitf_tre<int> nt20("CNPWRX");
      good = nt20.get(tres_itr, x_pow);
      if(!good) return false;

      nitf_tre<int> nt21("CNPWRY");
      good = nt21.get(tres_itr, y_pow);
      if(!good) return false;
      
      nitf_tre<int> nt22("CNPWRZ");
      good = nt22.get(tres_itr, z_pow);
      if(!good) return false;

      nitf_tre<int> nt23("CNTRMS");
      good = nt23.get(tres_itr, cn_nterms);
      if(!good) return false;

      cnpows.push_back(x_pow);cnpows.push_back(y_pow);cnpows.push_back(z_pow);

      std::vector<double> cnpcf;
      nitf_tre<double> nt24("CNPCF");
      good = nt24.get(tres_itr, cnpcf);
      if(!good) return false;

      int cd_nterms;
      std::vector<int> cdpows;
      nitf_tre<int> nt25("CDPWRX");
      good = nt25.get(tres_itr, x_pow);
      if(!good) return false;

      nitf_tre<int> nt26("CDPWRY");
      good = nt26.get(tres_itr, y_pow);
      if(!good) return false;
      
      nitf_tre<int> nt27("CDPWRZ");
      good = nt27.get(tres_itr, z_pow);
      if(!good) return false;

      nitf_tre<int> nt28("CDTRMS");
      good = nt28.get(tres_itr, cd_nterms);
      if(!good) return false;

      cdpows.push_back(x_pow);cdpows.push_back(y_pow);cdpows.push_back(z_pow);

      std::vector<double> cdpcf;
      nitf_tre<double> nt29("CDPCF");
      good = nt29.get(tres_itr, cdpcf);
      if(!good) return false;
      
      powers.push_back(cnpows); powers.push_back(cdpows);
      powers.push_back(rnpows); powers.push_back(rdpows);
      coeffs.push_back(cnpcf);  coeffs.push_back(cdpcf);
      coeffs.push_back(rnpcf);  coeffs.push_back(rdpcf);
      good = (cnpcf.size() == cn_nterms) && (cdpcf.size() == cd_nterms) &&
        (rnpcf.size() == rn_nterms) && (rdpcf.size() == rd_nterms);
      vpgl_RSM_camera<double>::set_powers(powers);
      vpgl_RSM_camera<double>::set_coefficients(coeffs);
      vpgl_RSM_camera<double>::set_scale_offsets(scale_offsets);
    }
    bool aux_good = true;
    if (type == "RSMIDA"){
      nitf_tre<std::string> nt("STID", false, true);
      good = nt.get(tres_itr, rsm_meta_.platform_name_);
      rsm_meta_.platform_name_valid = aux_good;
      // boundary metadata
      //  Obtain elevation bounds from the ground domain vertices
      //  The primary purpose of the RSM ground domain is to define
      //  the region of validity of the RSM TRE Set representation in ground space.
      double min_lon = std::numeric_limits<double>::max(), min_lat = min_lon;
      double max_lon = -min_lon, max_lat = -min_lat;
      double min_z, max_z;
      nitf_tre<double> nt1("V1Z", false, false);
      bool min_good = nt1.get(tres_itr, min_z);
      nitf_tre<double> nt2("V8Z", false, false);
      bool max_good = nt2.get(tres_itr, max_z);
 
      rsm_meta_.upper_left_.set(ul_[0], ul_[1], min_z);
      rsm_meta_.upper_right_.set(ur_[0], ur_[1], min_z);
      rsm_meta_.lower_left_.set(ll_[0], ll_[1], max_z);
      rsm_meta_.lower_right_.set(lr_[0], lr_[1], max_z);

      // bounding box
      // longitude bounds
      if(ul_[0] < min_lon) min_lon = ul_[0];
      if(ul_[0] > max_lon) max_lon = ul_[0];
      if(ur_[0] < min_lon) min_lon = ur_[0];
      if(ur_[0] > max_lon) max_lon = ur_[0];
      if(ll_[0] < min_lon) min_lon = ll_[0];
      if(ll_[0] > max_lon) max_lon = ll_[0];
      if(lr_[0] < min_lon) min_lon = lr_[0];
      if(lr_[0] > max_lon) max_lon = lr_[0];
      // latitude bounds
      if(ul_[1] < min_lat) min_lat = ul_[1];
      if(ul_[1] > max_lat) max_lat = ul_[1];
      if(ur_[1] < min_lat) min_lat = ur_[1];
      if(ur_[1] > max_lat) max_lat = ur_[1];
      if(ll_[1] < min_lat) min_lat = ll_[1];
      if(ll_[1] > max_lat) max_lat = ll_[1];
      if(lr_[1] < min_lat) min_lat = lr_[1];
      if(lr_[1] > max_lat) max_lat = lr_[0];

      vgl_point_3d<double> min_pt(min_lon, min_lat, min_z);
      vgl_point_3d<double> max_pt(max_lon, max_lat, max_z);
      rsm_meta_.bounding_box_= vgl_box_3d<double>(min_pt, max_pt);

      // footprint
      vgl_point_2d<double> ll(ll_[0],ll_[1]), lr(lr_[0],lr_[1]);
      vgl_point_2d<double> ur(ur_[0],ur_[1]), ul(ul_[0],ul_[1]);
      std::vector<vgl_point_2d<double> > sheet;
      sheet.push_back(ll); sheet.push_back(lr);
      sheet.push_back(ur); sheet.push_back(ul);
      rsm_meta_.footprint_ = vgl_polygon<double>(sheet);
      rsm_meta_.corners_valid = min_good && max_good && igeolo_valid_;
      // view direction
      // convert to local coordinates
      // cent ===> lon, lat, elev
      vgl_point_3d<double> cent = rsm_meta_.bounding_box_.centroid();
      vpgl_lvcs lvcs(cent.y(), cent.x(), cent.z(), vpgl_lvcs::wgs84,
                     vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      // Define bounding box for randomly selecting 3-d points to be
      // projected using the RSM polynomial coefficients defined in
      // the TREs.
      double lx, ly, lz;
      lvcs.global_to_local(cent.x(), cent.y(), cent.z(), vpgl_lvcs::wgs84, lx, ly, lz);
      vgl_point_3d<double> loc_org(lx, ly, lz);//should be (0.0, 0.0, 0.0)
      vgl_point_3d<double> loc_min_pt = loc_org + vgl_vector_3d<double>(-500.0, -500.0, -500.0);
      vgl_point_3d<double> loc_max_pt = loc_org + vgl_vector_3d<double>(500.0, 500.0, 500.0);
      vgl_box_3d<double> roi(loc_min_pt, loc_max_pt);

      // Cast the nitf camera to the base camera
      vpgl_RSM_camera* rsm_cam_ptr = reinterpret_cast<vpgl_RSM_camera<double>*>(this);

      // Fit the affine camera model to the set of random (XYZ -> UV) pairs
      vpgl_affine_camera<double> acam;
      vpgl_affine_camera_convert::convert(*rsm_cam_ptr, lvcs, roi, acam);

      // The Cartesian vector looking towards the sensor requires a minus sign
      //                              v
      vgl_vector_3d<double> ray_dir = -acam.ray_dir();

      // Convert the local Cartesian vector to
      // geodetic coordinates according to IMD convention
      //  degrees above horizon
      double view_elevation = std::asin(ray_dir.z()) * vnl_math::deg_per_rad;
      //  degrees east of north 
       double view_azimuth = std::atan2(ray_dir.x(), ray_dir.y()) * vnl_math::deg_per_rad;;
      if (view_azimuth < 0)
        view_azimuth += 360;

      rsm_meta_.view_angles_.set(view_azimuth, view_elevation);

      // gsd at center of image
      int min_r, max_r, min_c, max_c;
      nitf_tre<int> nt3("MINR", false, false);
      bool row_good_min = nt3.get(tres_itr, min_r);
      nitf_tre<int> nt4("MAXR", false, false);
      bool row_good_max = nt4.get(tres_itr, max_r);
      nitf_tre<int> nt5("MINC", false, false);
      bool col_good_min = nt5.get(tres_itr, min_c);
      nitf_tre<int> nt6("MAXC", false, false);
      bool col_good_max = nt6.get(tres_itr, max_c);
      bool cent_good = row_good_min && row_good_max && col_good_min && col_good_max;
      if(cent_good){
        double u_cent = (max_r - min_r)/2;
        double v_cent = (max_c - min_c)/2;
        double u_centp = u_cent + vnl_math::sqrt1_2;
        double v_centp = v_cent + vnl_math::sqrt1_2;
        vgl_plane_3d<double> pl(0.0, 0.0, 1.0, -cent.z());
        vgl_point_3d<double> ipt(0.0, 0.0, 0.0), wrld_ptc, wrld_ptp;
        vpgl_camera<double>* acam_ptr = reinterpret_cast<vpgl_camera<double>*>(&acam);
        bool gsd_good_c =
          vpgl_backproject::bproj_plane(acam_ptr, vgl_point_2d<double>(u_cent, v_cent),
                                        pl, ipt, wrld_ptc);
        bool gsd_good_cp =
          vpgl_backproject::bproj_plane(acam_ptr, vgl_point_2d<double>(u_centp, v_centp),
                                        pl, ipt, wrld_ptp);
        rsm_meta_.gsd_ = (wrld_ptp - wrld_ptc).length();
        bool good_gsd =gsd_good_c && gsd_good_cp;
        rsm_meta_.gsd_valid = good_gsd;
      }
    }
  }
  return good;
}

    
