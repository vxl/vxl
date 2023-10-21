#include "vpgl_nitf_RSM_camera.h"
#include <vil/vil_load.h>
bool
vpgl_nitf_RSM_camera::init(vil_nitf2_image * nitf_image, bool verbose)
{
  std::vector<vil_nitf2_image_subheader *> headers = nitf_image->get_image_headers();
   vil_nitf2_image_subheader * hdr = headers[1];//fix
   hdr->get_property("IXSHD", isxhd_tres_);
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

// print all camera information
void
vpgl_nitf_RSM_camera::print(std::ostream & ostr) const{
}
bool vpgl_nitf_RSM_camera::test_rsm_params() const{
    // Now get the sub-header TRE parameters
    //vil_nitf2_tagged_record_sequence isxhd_tres;
    vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
    //hdr_.get_property("IXSHD", isxhd_tres);
  // Check through the TREs to find ""
    bool success;
    std::string cetag, id, iid, edition, isid, sid, stid, nrg,ncg,ndd;
    int rsn, csn, year, month, day , hour , minute, nrgi, ncgi,
      fullr, fullc, minr, maxr, minc, maxc;
    double trg, tcg, second, v1x, v1y, v1z,v2x, v2y, v2z,v3x, v3y, v3z,v4x, v4y, v4z,v5x, v5y, v5z,v6x, v6y, v6z,v7x, v7y, v7z, v8x, v8y, v8z;
    double xuor, yuor, zuor, xuxr, xuyr, xuzr, yuxr, yuyr, yuzr,
      zuxr, zuyr, zuzr, grpx, grpy, grpz, ie0, ier, iec, ierr, ierc, iecc,
      ia0, iar, iac, iarr, iarc, iacc, spx, svx, sax, spy, svy, say, spz, svz, saz;
    bool not_opt = false;
    for (tres_itr = isxhd_tres_.begin(); tres_itr != isxhd_tres_.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();

    if (type == "RSMIDA") // looking for "RSMIDA..."
    {
      success = (*tres_itr)->get_value("IID", iid);
      if (!success)
      {
        std::cout << "IID Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("EDITION", edition);
      if (!success)
      {
        std::cout << "EDITION Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ISID", isid);
      if (!success)
        {
        std::cout << "ISID Property failed in vil_nitf2_image_subheader\n";
        return false;
        }
      success = (*tres_itr)->get_value("SID", sid);
      if (!success)
        {
        std::cout << "SID Property failed in vil_nitf2_image_subheader\n";
        return false;
        }
      
      success = (*tres_itr)->get_value("STID", stid);
      if (!success)
      {
        std::cout << "STID Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YEAR", year);
      if (!success)
      {
        std::cout << "YEAR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MONTH", month);
      if (!success)
      {
        std::cout << "MONTH Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("DAY", day);
      if (!success)
      {
        std::cout << "DAY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("HOUR", hour);
      if (!success)
      {
        std::cout << "HOUR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MINUTE", minute);
      if (!success)
      {
        std::cout << "MINUTE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SECOND", second);
      if (!success)
      {
        std::cout << "SECOND Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("NRG", nrgi);
      if (!success)
      {
        std::cout << "NRG Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("NCG", ncgi);
      if (!success)
      {
        std::cout << "NCG Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("TRG", trg);
      if (!success)
      {
        std::cout << "TRG Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("TCG", tcg);

      if (!success)
      {
        std::cout << "TCG Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("NDD", ndd);

      if (!success)
      {
        std::cout << "NDD Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      // if ground skip coordinate transforms
      bool mask = ndd != "G";

      success = (*tres_itr)->get_value("XUOR", xuor);
      if (mask&&!success)
      {
        std::cout << "XUOR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }

      success = (*tres_itr)->get_value("YUOR", yuor);
      if (mask&&!success)
      {
        std::cout << "YUOR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }

      success = (*tres_itr)->get_value("ZUOR", zuor);
      if (mask&&!success)
      {
        std::cout << "ZUOR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("XUXR", xuxr);
      if (mask&&!success)
      {
        std::cout << "XUXR Property failed in vil_nitf2_image_subheader\n";
        return false;
     }
      success = (*tres_itr)->get_value("XUYR", xuyr);
      if (mask&&!success)
      {
        std::cout << "XUYR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("XUZR", xuzr);
      if (mask&&!success)
      {
        std::cout << "XUZR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YUXR", yuxr);
      if (mask&&!success)
      {
        std::cout << "YUXR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YUYR", yuyr);
      if (mask&&!success)
      {
        std::cout << "YUYR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YUZR", yuzr);
      if (mask&&!success)
      {
        std::cout << "YUZR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ZUXR", zuxr);
      if (mask&&!success)
      {
        std::cout << "ZUXR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ZUYR", zuyr);
      if (mask&&!success)
      {
        std::cout << "ZUYR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ZUZR", zuzr);
      if (mask&&!success)
      {
        std::cout << "ZUZR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V1X", v1x);
      if (!success)
      {
        std::cout << "V1X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V1Y", v1y);
      if (!success)
      {
        std::cout << "V1Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V1Z", v1z);
      if (!success)
      {
        std::cout << "V1Z Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V2X", v2x);
      if (!success)
      {
        std::cout << "V2X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V2Y", v2y);
      if (!success)
      {
        std::cout << "V2Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V2Z", v2z);
      if (!success)
      {
        std::cout << "V1X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V3X", v3x);
      if (!success)
      {
        std::cout << "V1X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V3Y", v3y);
      if (!success)
      {
        std::cout << "V3Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V3Z", v3z);
      if (!success)
      {
        std::cout << "V1X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V4X", v4x);
      if (!success)
      {
        std::cout << "V4X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V4Y", v4y);
      if (!success)
      {
        std::cout << "V4Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V4Z", v4z);
      if (!success)
      {
        std::cout << "V4Z Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V5X", v5x);
      if (!success)
      {
        std::cout << "V5X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V5Y", v5y);
      if (!success)
      {
        std::cout << "V5Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V5Z", v5z);
      if (!success)
      {
        std::cout << "V6X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V6X", v6x);
      if (!success)
      {
        std::cout << "V6X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V6Y", v6y);
      if (!success)
      {
        std::cout << "V6Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V6Z", v6z);
      if (!success)
      {
        std::cout << "V6Z Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V7X", v7x);
      if (!success)
      {
        std::cout << "V7X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V7Y", v7y);
      if (!success)
      {
        std::cout << "V7Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V7Z", v7z);
      if (!success)
      {
        std::cout << "V7Z Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V8X", v8x);
      if (!success)
      {
        std::cout << "V8X Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V8Y", v8y);
      if (!success)
      {
        std::cout << "V8Y Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("V8Z", v8z);
      if (!success)
      {
        std::cout << "V8Z Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
     
      success = (*tres_itr)->get_value("GRPX", grpx);
      if (not_opt&&!success)
      {
        std::cout << "GRPX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("GRPY", grpy);
      if (not_opt&&!success)
      {
        std::cout << "GRPY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("GRPZ", grpz);
      if (not_opt&&!success)
      {
        std::cout << "GRPZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("FULLR", fullr);
      if (not_opt&&!success)
      {
        std::cout << "FULLR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("FULLC", fullc);
      if (not_opt&&!success)
      {
        std::cout << "FULLC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MINR", minr);
      if (!success)
      {
        std::cout << "MINR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MAXR", maxr);
      if (!success)
      {
        std::cout << "MAXR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MINC", minc);
      if (!success)
      {
        std::cout << "MINC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("MAXC", maxc);
      if (!success)
      {
        std::cout << "MAXC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IE0", ie0);
      if (not_opt&&!success)
      {
        std::cout << "IE0 Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IE0", ie0);
      if (not_opt&&!success)
      { 
       std::cout << "IE0 Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IER", ier);
      if (not_opt&&!success)
      {
        std::cout << "IER Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IEC", iec);
      if (not_opt&&!success)
      {
        std::cout << "IEC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IERR", ierr);
      if (not_opt&&!success)
      {
        std::cout << "IERR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IERC", ierc);
      if (not_opt&&!success)
      {
        std::cout << "IERC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IECC", iecc);
      if (not_opt&&!success)
      {
        std::cout << "IECC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IA0", ia0);
      if (not_opt&&!success)
      { 
       std::cout << "IA0 Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IAR", iar);
      if (not_opt&&!success)
      {
        std::cout << "IAR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IAC", iac);
      if (not_opt&&!success)
      {
        std::cout << "IAC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IARR", iarr);
      if (not_opt&&!success)
      {
        std::cout << "IARR Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IARC", iarc);
      if (not_opt&&!success)
      {
        std::cout << "IARC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("IACC", iacc);
      if (not_opt&&!success)
      {
        std::cout << "IACC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SPX", spx);
      if (not_opt&&!success)
      {
        std::cout << "SPX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SVX", svx);
      if (not_opt&&!success)
      {
        std::cout << "SVX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SAX", sax);
      if (not_opt&&!success)
      {
        std::cout << "SAX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
            success = (*tres_itr)->get_value("SPY", spy);
      if (not_opt&&!success)
      {
        std::cout << "SPY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SVY", svy);
      if (not_opt&&!success)
      {
        std::cout << "SVY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SAY", say);
      if (not_opt&&!success)
      {
        std::cout << "SAY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SPZ", spz);
      if (not_opt&&!success)
      {
        std::cout << "SPZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SVZ", svz);
      if (not_opt&&!success)
      {
        std::cout << "SVZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("SAZ", saz);
      if (not_opt&&!success)
      {
        std::cout << "SAZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
    }
    if (type == "RSMPCA"){ // looking for "RSMPCA..."
      std::string iid, edition;
      int rsn, csn, rnpwrx, rnpwry,rnpwrz,rntrms,rdpwrx, rdpwry,rdpwrz,rdtrms,
        cnpwrx, cnpwry,cnpwrz,cntrms,cdpwrx, cdpwry,cdpwrz,cdtrms;
      double rfep, cfep, rnrmo,cnrmo, xnrmo, ynrmo, znrmo, rnrmsf, cnrmsf,xnrmsf, ynrmsf, znrmsf;
      success = (*tres_itr)->get_value("IID", iid);
      if (not_opt&&!success)
      {
        std::cout << "IID Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("EDITION", edition);
      if (!success)
      {
        std::cout << "EDITION Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RSN", rsn);
      if (!success)
      {
        std::cout << "RSN Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CSN", csn);
      if (!success)
      {
        std::cout << "RSN Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RFEP", rfep);
      if (not_opt&&!success)
      {
        std::cout << "RFEP Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CFEP", cfep);
      if (not_opt&&!success)
      {
        std::cout << "CFEP Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNRMO", rnrmo);
      if (!success)
      {
        std::cout << "RNORMO Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CNRMO", cnrmo);
      if (!success)
      {
        std::cout << "CNORMO Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("XNRMO", xnrmo);
      if (!success)
      {
        std::cout << "XNORMO Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YNRMO", ynrmo);
      if (!success)
      {
        std::cout << "YNORMO Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ZNRMO", znrmo);
      if (!success)
      {
        std::cout << "ZNORMO Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNRMSF", rnrmsf);
      if (!success)
      {
        std::cout << "RNRMSF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CNRMSF", cnrmsf);
      if (!success)
      {
        std::cout << "CNRMSF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("XNRMSF", xnrmsf);
      if (!success)
      {
        std::cout << "XNRMSF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("YNRMSF", ynrmsf);
      if (!success)
      {
        std::cout << "YNRMSF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("ZNRMSF", znrmsf);
      if (!success)
      {
        std::cout << "ZNRMSF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNPWRX", rnpwrx);
      if (!success)
      {
        std::cout << "RNPWRX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNPWRY", rnpwry);
      if (!success)
      {
        std::cout << "RNPWRY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNPWRZ", rnpwrz);
      if (!success)
      {
        std::cout << "RNPWRZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RNTRMS", rntrms);
      if (!success)
      {
        std::cout << "RNTRMS Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      std::vector<double> rnpcf;
      success = (*tres_itr)->get_values("RNPCF", rnpcf);
      if (!success)
      {
          std::cout << "RNPCF Property failed in vil_nitf2_image_subheader\n";
          return false;
      }
      success = (*tres_itr)->get_value("RDPWRX", rdpwrx);
      if (!success)
      {
        std::cout << "RDPWRX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RDPWRY", rdpwry);
      if (!success)
      {
        std::cout << "RDPWRY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RDPWRZ", rdpwrz);
      if (!success)
      {
        std::cout << "RDPWRZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("RDTRMS", rdtrms);
      if (!success)
      {
        std::cout << "RDTRMS Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      std::vector<double> rdpcf;
      success = (*tres_itr)->get_values("RDPCF", rdpcf);
      if (!success)
      {
          std::cout << "RDPCF Property failed in vil_nitf2_image_subheader\n";
          return false;
      }
      success = (*tres_itr)->get_value("CNPWRX", cnpwrx);
      if (!success)
      {
        std::cout << "CNPWRX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CNPWRY", cnpwry);
      if (!success)
      {
        std::cout << "CNPWRY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CNPWRZ", cnpwrz);
      if (!success)
      {
        std::cout << "CNPWRZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CNTRMS", cntrms);
      if (!success)
      {
        std::cout << "CNTRMS Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      std::vector<double> cnpcf;
      success = (*tres_itr)->get_values("CNPCF", cnpcf);
      if (!success)
      {
          std::cout << "CNPCF Property failed in vil_nitf2_image_subheader\n";
          return false;
      }
      success = (*tres_itr)->get_value("CDPWRX", cdpwrx);
      if (!success)
      {
        std::cout << "CDPWRX Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CDPWRY", cdpwry);
      if (!success)
      {
        std::cout << "CDPWRY Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CDPWRZ", cdpwrz);
      if (!success)
      {
        std::cout << "CDPWRZ Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      success = (*tres_itr)->get_value("CDTRMS", cdtrms);
      if (!success)
      {
        std::cout << "CDTRMS Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      std::vector<double> cdpcf;
      success = (*tres_itr)->get_values("CDPCF", cdpcf);
      if (!success)
      {
          std::cout << "CDPCF Property failed in vil_nitf2_image_subheader\n";
          return false;
      }
    }
    if (type == "RSMPIA"){ // looking for "RSMPCA..."
      std::cout << "CAN'T HANDLE MULIPL SECTIONS" << std::endl; 
      return false;
    }
  }
  
  return true;
}
bool vpgl_nitf_RSM_camera::
get_rsm_camera_params(std::vector<std::vector<int> >& powers,
    std::vector<std::vector<double> >& coeffs,
    std::vector<vpgl_scale_offset<double> >& scale_offsets){
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  powers.clear();
  coeffs.clear();
  scale_offsets.clear();
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
      good = (*tres_itr)->get_value("RNRMO", v_off);
      if(!good) return false;
      good = (*tres_itr)->get_value("CNRMO", u_off);
      if(!good) return false;
      good = (*tres_itr)->get_value("XNRMO", x_off);
      if(!good) return false;
      good = (*tres_itr)->get_value("YNRMO", y_off);
      if(!good) return false;
      good = (*tres_itr)->get_value("ZNRMO", z_off);
      if(!good) return false;
      good = (*tres_itr)->get_value("RNRMSF", v_scale);
      if(!good) return false;
      good = (*tres_itr)->get_value("CNRMSF", u_scale);
      if(!good) return false;
      good = (*tres_itr)->get_value("XNRMSF", x_scale);
      if(!good) return false;
      good = (*tres_itr)->get_value("YNRMSF", y_scale);
      if(!good) return false;
      good = (*tres_itr)->get_value("ZNRMSF", z_scale);
      if(!good) return false;

      scale_offsets.emplace_back(x_scale, x_off);
      scale_offsets.emplace_back(y_scale, y_off);
      scale_offsets.emplace_back(z_scale, z_off);
      scale_offsets.emplace_back(u_scale, u_off);
      scale_offsets.emplace_back(v_scale, v_off);

      int rn_nterms, rd_nterms;
      std::vector<int> rnpows, rdpows;
      good = (*tres_itr)->get_value("RNPWRX", x_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RNPWRY", y_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RNPWRZ", z_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RNTRMS", rn_nterms);
      if(!good) return false;
      rnpows.push_back(x_pow);rnpows.push_back(y_pow);rnpows.push_back(z_pow);

      std::vector<double> rnpcf;
      good = (*tres_itr)->get_values("RNPCF", rnpcf);
      if(!good) return false;
      
      good = (*tres_itr)->get_value("RDPWRX", x_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RDPWRY", y_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RDPWRZ", z_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("RDTRMS", rd_nterms);
      if(!good) return false;
      rdpows.push_back(x_pow);rdpows.push_back(y_pow);rdpows.push_back(z_pow);

      std::vector<double> rdpcf;
      good = (*tres_itr)->get_values("RDPCF", rdpcf);
      if(!good) return false;

      int cn_nterms, cd_nterms;
      std::vector<int> cnpows, cdpows;
      good = (*tres_itr)->get_value("CNPWRX", x_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CNPWRY", y_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CNPWRZ", z_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CNTRMS", cn_nterms);
      if(!good) return false;
      cnpows.push_back(x_pow);cnpows.push_back(y_pow);cnpows.push_back(z_pow);

      std::vector<double> cnpcf;
      good = (*tres_itr)->get_values("CNPCF", cnpcf);
      if(!good) return false;

      good = (*tres_itr)->get_value("CDPWRX", x_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CDPWRY", y_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CDPWRZ", z_pow);
      if(!good) return false;
      good = (*tres_itr)->get_value("CDTRMS", cd_nterms);
      if(!good) return false;
      cdpows.push_back(x_pow);cdpows.push_back(y_pow);cdpows.push_back(z_pow);

      std::vector<double> cdpcf;
      good = (*tres_itr)->get_values("CDPCF", cdpcf);
      if(!good) return false;

      powers.push_back(cnpows); powers.push_back(cdpows); powers.push_back(rnpows); powers.push_back(rdpows);
      coeffs.push_back(cnpcf);  coeffs.push_back(cdpcf);   coeffs.push_back(rnpcf);   coeffs.push_back(rdpcf);
      good = (cnpcf.size() == cn_nterms) && (cdpcf.size() == cd_nterms) && (rnpcf.size() == rn_nterms) && (rdpcf.size() == rd_nterms);
      return good;      
    }
  }
  return good;
}
    
