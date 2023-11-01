#include "vpgl_replacement_sensor_model_tres.h"
#include <vil/file_formats/vil_nitf2_tagged_record_definition.h>
#include <vil/file_formats/vil_nitf2_field_functor.h>
#include <vil/file_formats/vil_nitf2_field_definition.h>
#include <vil/file_formats/vil_nitf2_typed_field_formatter.h>
void vpgl_replacement_sensor_model_tres::define_RSMIDA(){

    vil_nitf2_tagged_record_definition* tri = vil_nitf2_tagged_record_definition::find("RSMIDA");
    if (!tri)
    {
        vil_nitf2_tagged_record_definition::define("RSMIDA", "Replacement Sensor Model Identification")

            .field("IID", "Image Identifier", NITF_STR_BCSA(80))
            .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
            .field("ISID", "Sensor Identifier", NITF_STR_BCSA(40))
            .field("SID", "Sensor Identifier", NITF_STR_BCSA(40))
            .field("STID", "Sensor Type Identifier", NITF_STR_BCSA(40))
            .field("YEAR", "Year of Acquisition", NITF_INT(4), true)
            .field("MONTH", "Month of Acquisition", NITF_INT(2), true)
            .field("DAY", "Month of Acquisition", NITF_INT(2), true)
            .field("HOUR", "Hour of Acquisition", NITF_INT(2), true)
            .field("MINUTE", "Minute of Acquisition", NITF_INT(2), true)
            .field("SECOND", "Second of Acquisition", NITF_DBL(9, 6, false), true)
            .field("NRG", "Number of Simulatanous Rows", NITF_INT(8, false), true)
            .field("NCG", "Number of Simulatanous Cols", NITF_INT(8, false), true)
            .field("TRG", "Time Between Rows", NITF_EXP(14, 2), true)
            .field("TCG", "Time Between Cols", NITF_EXP(14, 2), true)
            .field("NDD", "Ground Domain", NITF_STR_BCSA(1))
            .field("XUOR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("YUOR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("ZUOR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("XUXR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("XUYR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("XUZR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("YUXR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("YUYR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("YUZR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("ZUXR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("ZUYR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("ZUZR", "Rectangular Coord. Unit Vector", NITF_EXP(14, 2), true)
            .field("V1X", "Vertex 1 Ground Domain X Coord.", NITF_EXP(14, 2), true)
            .field("V1Y", "Vertex 1 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V1Z", "Vertex 1 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V2X", "Vertex 2 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V2Y", "Vertex 2 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V2Z", "Vertex 2 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V3X", "Vertex 3 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V3Y", "Vertex 3 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V3Z", "Vertex 3 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V4X", "Vertex 4 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V4Y", "Vertex 4 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V4Z", "Vertex 4 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V5X", "Vertex 5 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V5Y", "Vertex 5 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V5Z", "Vertex 5 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V6X", "Vertex 6 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V6Y", "Vertex 6 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V6Z", "Vertex 6 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V7X", "Vertex 7 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V7Y", "Vertex 7 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V7Z", "Vertex 7 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("V8X", "Vertex 8 Ground Domain X Coord.", NITF_EXP(14, 2))
            .field("V8Y", "Vertex 8 Ground Domain Y Coord.", NITF_EXP(14, 2))
            .field("V8Z", "Vertex 8 Ground Domain Z Coord.", NITF_EXP(14, 2))
            .field("GRPX", "Ground Reference Pt. X Coord", NITF_EXP(14, 2), true)
            .field("GRPY", "Ground Reference Pt. Y Coord", NITF_EXP(14, 2), true)
            .field("GRPZ", "Ground Reference Pt. Z Coord", NITF_EXP(14, 2), true)
            .field("FULLR", "Number of Rows Full Image", NITF_INT(8, false), true)
            .field("FULLC", "Number of Cols. Full Image", NITF_INT(8, false), true)
            .field("MINR", "Min Number of Rows", NITF_INT(8, false))
            .field("MAXR", "Max Number of Rows", NITF_INT(8, false))
            .field("MINC", "Min Number of Cols.", NITF_INT(8, false))
            .field("MAXC", "Max Number of Cols.", NITF_INT(8, false))
            .field("IE0", "Illumination elevation angle at 0", NITF_EXP(14, 2), true)
            .field("IER", "Elevation angle change per Row", NITF_EXP(14, 2), true)
            .field("IEC", "Elevation angle change per Col.", NITF_EXP(14, 2), true)
            .field("IERR", "Elevation angle 2nd order per Row^2.", NITF_EXP(14, 2), true)
            .field("IERC", "Elevation angle 2nd order per Row,Col.", NITF_EXP(14, 2), true)
            .field("IECC", "Elevation angle 2nd order per Col.^2.", NITF_EXP(14, 2), true)
            .field("IA0", "Illumination azimuth angle at 0", NITF_EXP(14, 2), true)
            .field("IAR", "Azimuth angle change per Row", NITF_EXP(14, 2), true)
            .field("IAC", "Azimuth angle change per Col.", NITF_EXP(14, 2), true)
            .field("IARR", "Azimuth angle 2nd order per Row^2.", NITF_EXP(14, 2), true)
            .field("IARC", "Azimuth angle 2nd order per Row,Col.", NITF_EXP(14, 2), true)
            .field("IACC", "Azimuth angle 2nd order per Col.^2.", NITF_EXP(14, 2), true)
            .field("SPX", "Sensor X position", NITF_EXP(14, 2), true)
            .field("SVX", "Sensor X velocity", NITF_EXP(14, 2), true)
            .field("SAX", "Sensor X acceleration", NITF_EXP(14, 2), true)
            .field("SPY", "Sensor Y position", NITF_EXP(14, 2), true)
            .field("SVY", "Sensor Y velocity", NITF_EXP(14, 2), true)
            .field("SAY", "Sensor Y acceleration", NITF_EXP(14, 2), true)
            .field("SPZ", "Sensor Z position", NITF_EXP(14, 2), true)
            .field("SVZ", "Sensor Z velocity", NITF_EXP(14, 2), true)
            .field("SAZ", "Sensor Z acceleration", NITF_EXP(14, 2), true)
            .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMPCA(){
    vil_nitf2_tagged_record_definition* trp = vil_nitf2_tagged_record_definition::find("RSMPCA");
    if (!trp)
    {
        vil_nitf2_tagged_record_definition::define("RSMPCA", "Replacement Sensor Model Polynomial Coefficients")
            .field("IID", "Image Identifier", NITF_STR(80), true)
            .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
            .field("RSN", "Row Section Number", NITF_INT(3, false))
            .field("CSN", "Column Section Number", NITF_INT(3, false))
            .field("RFEP", "Row Fitting Error", NITF_EXP(14, 2), true)
            .field("CFEP", "Col. Fitting Error", NITF_EXP(14, 2), true)
            .field("RNRMO", "Row Normalization Offset", NITF_EXP(14, 2))
            .field("CNRMO", "Col. Normalization Offset", NITF_EXP(14, 2))
            .field("XNRMO", "X Normalization Offset", NITF_EXP(14, 2))
            .field("YNRMO", "Y. Normalization Offset", NITF_EXP(14, 2))
            .field("ZNRMO", "Z. Normalization Offset", NITF_EXP(14, 2))
            .field("RNRMSF", "Row. Normalization Scale Factor", NITF_EXP(14, 2))
            .field("CNRMSF", "Col. Normalization Scale Factor", NITF_EXP(14, 2))
            .field("XNRMSF", "X. Normalization Scale Factor", NITF_EXP(14, 2))
            .field("YNRMSF", "Y. Normalization Scale Factor", NITF_EXP(14, 2))
            .field("ZNRMSF", "Z. Normalization Scale Factor", NITF_EXP(14, 2))
            .field("RNPWRX", "Row Numerator max power of X", NITF_INT(1, false))
            .field("RNPWRY", "Row Numerator max power of Y", NITF_INT(1, false))
            .field("RNPWRZ", "Row Numerator max power of Z", NITF_INT(1, false))
            .field("RNTRMS", "Row Numerator polynomial number of terms", NITF_INT(3, false))
            .repeat("RNTRMS", vil_nitf2_field_definitions().field("RNPCF", "Row Neumerator Polynomial Coefficients", NITF_EXP(14, 2)))
            .field("RDPWRX", "Row Denominator max power of X", NITF_INT(1, false))
            .field("RDPWRY", "Row Denominator max power of Y", NITF_INT(1, false))
            .field("RDPWRZ", "Row Denominator max power of Z", NITF_INT(1, false))
            .field("RDTRMS", "Row Denominator polynomial number of terms", NITF_INT(3, false))
            .repeat("RDTRMS", vil_nitf2_field_definitions().field("RDPCF", "Row Denominator Polynomial Coefficients", NITF_EXP(14, 2)))
            .field("CNPWRX", "Col. Numerator max power of X", NITF_INT(1, false))
            .field("CNPWRY", "Col. Numerator max power of Y", NITF_INT(1, false))
            .field("CNPWRZ", "Col. Numerator max power of Z", NITF_INT(1, false))
            .field("CNTRMS", "Col. Numerator polynomial number of terms", NITF_INT(3, false))
            .repeat("CNTRMS", vil_nitf2_field_definitions().field("CNPCF", "Col. Neumerator Polynomial Coefficients", NITF_EXP(14, 2)))
            .field("CDPWRX", "Col. Denominator max power of X", NITF_INT(1, false))
            .field("CDPWRY", "Col. Denominator max power of Y", NITF_INT(1, false))
            .field("CDPWRZ", "Col. Denominator max power of Z", NITF_INT(1, false))
            .field("CDTRMS", "Col. Denominator polynomial number of terms", NITF_INT(3, false))
            .repeat("CDTRMS", vil_nitf2_field_definitions().field("CDPCF", "Col. Denominator Polynomial Coefficients", NITF_EXP(14, 2)))
            .end(); // of RPC TRE
    }
}
// define TREs for cases not currently handled by vpgl_RSM_camera
void vpgl_replacement_sensor_model_tres::define_RSMPIA(){
// check for multiple polynomials
    vil_nitf2_tagged_record_definition* trpi = vil_nitf2_tagged_record_definition::find("RSMPIA");
    if (!trpi)
    {
      vil_nitf2_tagged_record_definition::define("RSMPIA", "Multiple Section Polynomials")
        .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
        .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMECA(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMECA");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMECA", "Indirect Error Covariance")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMECB() {
    // check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMECB");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMECB", "Extended Indirect Error Covariance")
            .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
            .end();
    }
}
    
void vpgl_replacement_sensor_model_tres::define_RSMDCA(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMDCA");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMDCA", "Direct Error Covariance")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMDCB(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMDCB");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMDCB", "Extended Direct Error Covariance")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMAPA(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMAPA");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMAPA", "Adjustable Parameters")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMAPB(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMAPB");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMAPB", "Extended Adjustable Parameters")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMGIA(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMPIA");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMGIA", "Multi-section Grids")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
void vpgl_replacement_sensor_model_tres::define_RSMGGA(){
// check for refinement grid
    vil_nitf2_tagged_record_definition* trgi = vil_nitf2_tagged_record_definition::find("RSMGGA");
    if (!trgi)
    {
        vil_nitf2_tagged_record_definition::define("RSMGGA", "Ground to Image Grid")
          .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
          .end();
    }
}
