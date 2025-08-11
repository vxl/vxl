#include "vpgl_replacement_sensor_model_tres.h"
#include <vil/file_formats/vil_nitf2_tagged_record_definition.h>
#include <vil/file_formats/vil_nitf2_tagged_record.h>
#include <vil/file_formats/vil_nitf2_field_functor.h>
#include <vil/file_formats/vil_nitf2_field_definition.h>
#include <vil/file_formats/vil_nitf2_typed_field_formatter.h>
#include <string>

//=================================================
// Custom Field Functors Needed to Parse RSM TREs
//=================================================

// covariance matrix number of elements functor
// input: integer tag  output value: (n+1)*n/2
template <class T>
class vil_nitf2_field_covar_size : public vil_nitf2_field_functor<T>
{
public:
  vil_nitf2_field_covar_size(std::string tag)
    : tag(std::move(tag))
  {}

  vil_nitf2_field_functor<int> *
  copy() const override
  {
    return new vil_nitf2_field_covar_size(tag);
  }

  bool
  operator()(vil_nitf2_field_sequence * record, const vil_nitf2_index_vector & indexes, T & value) override
  {
    bool success = record->get_value(tag, indexes, value, true);
    if (!success)
      return false;
    // number of elements in the upper diagonal section
    value = (value + 1) * value / 2;
    return success;
  }

private:
  std::string tag;
};

// produce the product of values at each of two tags
// inputs: T v1,v2 at tag1, tag2  output value: T v1*v2
template <class T>
class vil_nitf2_field_product_size : public vil_nitf2_field_functor<T>
{
public:
  vil_nitf2_field_product_size(std::string tag1, std::string tag2)
    : tag1(std::move(tag1))
    , tag2(std::move(tag2))
  {}

  vil_nitf2_field_functor<int> *
  copy() const override
  {
    return new vil_nitf2_field_product_size(tag1, tag2);
  }

  bool
  operator()(vil_nitf2_field_sequence * record, const vil_nitf2_index_vector & indexes, T & value) override
  {
    int value1, value2;
    bool success = record->get_value(tag1, indexes, value1, true);
    success = success && record->get_value(tag2, indexes, value2, true);
    
    if (!success)
      return false;
    // number of elements in the upper diagonal section
    value = value1 * value2;
    return success;
  }

private:
  std::string tag1;
  std::string tag2;
};

// repeat count functor
// input: int v at tag output: result = v
template <class T>
class vil_nitf2_repeat_count : public vil_nitf2_field_functor<int>
{
public:
  vil_nitf2_repeat_count(std::string tag)
    : tag(std::move(tag))
  {}

  vil_nitf2_field_functor<int> *
  copy() const override
  {
    return new vil_nitf2_repeat_count(tag);
  }

  bool
  operator()(vil_nitf2_field_sequence * record, const vil_nitf2_index_vector & indexes, T & value) override
  {
    value = T(0);
    bool success = record->get_value(tag, indexes, value, true);
    return true;
  }

private:
  std::string tag;
};

// conditional repeat with negation flag to repeat if "N"
// input: string s in {"Y", "N"}, bool negation  value:1 if "Y" && false || "N" && true
template <class T>
class vil_nitf2_conditional_repeat : public vil_nitf2_field_functor<int>
{
public:
  vil_nitf2_conditional_repeat(std::string tag, bool negation)
    : tag(std::move(tag)), negation(std::move(negation))
  {}

  vil_nitf2_field_functor<int> *
  copy() const override
  {
    return new vil_nitf2_conditional_repeat(tag, negation);
  }

  bool
  operator()(vil_nitf2_field_sequence * record, const vil_nitf2_index_vector & indexes, T & value) override
  {
    value = T(0);
    std::string condition;
    bool success = record->get_value(tag, indexes, condition, true);
    if (condition == "Y" && negation)
        value = T(0);
    else if(condition == "Y" && !negation)
        value = T(1);
    else if(condition == "N" && negation)
        value = T(1);
    else if(condition == "N" && !negation)
        value = T(0);
    return success;
  }

private:
  bool negation;
  std::string tag;
};
//:
// Functor defines a predicate that sets its result parameter to true iff
// the value of each of two specified tags equal their specified values
//
template <typename T>
class vil_nitf2_field_value_both_equal : public vil_nitf2_field_functor<bool>
{
public:
  /// Constructor to specify a std::vector of acceptable values
  vil_nitf2_field_value_both_equal(T tag1, T val1,
                                   T tag2, T val2)
    : tag1(std::move(tag1)), val1(std::move(val1)),
      tag2(std::move(tag2)), val2(std::move(val2))
  {}
  // clone the functor
  vil_nitf2_field_functor<bool>* copy() const override
  {
      return new vil_nitf2_field_value_both_equal(tag1, val1, tag2, val2);
  }
  // determines if both tags have the specified value
  bool
  operator()(vil_nitf2_field_sequence * record, const vil_nitf2_index_vector & indexes, bool & result) override
  {
    result = false;
    T tval1, tval2;
    bool good1 = record->get_value(tag1, indexes, tval1, true);
    bool good2 = record->get_value(tag2, indexes, tval2, true);
    if(good1 && good2)
    {
        bool eq1 = tval1 == val1;
        bool eq2 = tval2 == val2;
        result = eq1 && eq2;
        return true;
    }
    // failed to get one or both of the tag values
    return false;
  }
protected:
  T tag1;
  T val1;
  T tag2;
  T val2;
};
void
vpgl_replacement_sensor_model_tres::define_RSMIDA()
{

  vil_nitf2_tagged_record_definition * tri = vil_nitf2_tagged_record_definition::find("RSMIDA");
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
      .field("DAY", "Day of Acquisition", NITF_INT(2), true)
      .field("HOUR", "Hour of Acquisition", NITF_INT(2), true)
      .field("MINUTE", "Minute of Acquisition", NITF_INT(2), true)
      .field("SECOND", "Second of Acquisition", NITF_DBL(9, 6, false), true)
      .field("NRG", "Number of Simulatanous Rows", NITF_INT(8, false), true)
      .field("NCG", "Number of Simulatanous Cols", NITF_INT(8, false), true)
      .field("TRG", "Time Between Rows", NITF_EXP(14, 2), true)
      .field("TCG", "Time Between Cols", NITF_EXP(14, 2), true)
      .field("GRNDD", "Ground Domain", NITF_STR_BCSA(1))
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
      .field("V1X", "Vertex 1 Ground Domain X Coord.", NITF_EXP(14, 2))
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
void
vpgl_replacement_sensor_model_tres::define_RSMPCA()
{
  vil_nitf2_tagged_record_definition * trp = vil_nitf2_tagged_record_definition::find("RSMPCA");
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
      .repeat("RNTRMS",
              vil_nitf2_field_definitions().field("RNPCF", "Row Neumerator Polynomial Coefficients", NITF_EXP(14, 2)))
      .field("RDPWRX", "Row Denominator max power of X", NITF_INT(1, false))
      .field("RDPWRY", "Row Denominator max power of Y", NITF_INT(1, false))
      .field("RDPWRZ", "Row Denominator max power of Z", NITF_INT(1, false))
      .field("RDTRMS", "Row Denominator polynomial number of terms", NITF_INT(3, false))
      .repeat("RDTRMS",
              vil_nitf2_field_definitions().field("RDPCF", "Row Denominator Polynomial Coefficients", NITF_EXP(14, 2)))
      .field("CNPWRX", "Col. Numerator max power of X", NITF_INT(1, false))
      .field("CNPWRY", "Col. Numerator max power of Y", NITF_INT(1, false))
      .field("CNPWRZ", "Col. Numerator max power of Z", NITF_INT(1, false))
      .field("CNTRMS", "Col. Numerator polynomial number of terms", NITF_INT(3, false))
      .repeat("CNTRMS",
              vil_nitf2_field_definitions().field("CNPCF", "Col. Neumerator Polynomial Coefficients", NITF_EXP(14, 2)))
      .field("CDPWRX", "Col. Denominator max power of X", NITF_INT(1, false))
      .field("CDPWRY", "Col. Denominator max power of Y", NITF_INT(1, false))
      .field("CDPWRZ", "Col. Denominator max power of Z", NITF_INT(1, false))
      .field("CDTRMS", "Col. Denominator polynomial number of terms", NITF_INT(3, false))
      .repeat("CDTRMS",
              vil_nitf2_field_definitions().field("CDPCF", "Col. Denominator Polynomial Coefficients", NITF_EXP(14, 2)))
      .end(); // of RPC TRE
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMPIA()
{
  // check for multiple polynomials
  vil_nitf2_tagged_record_definition * trpi = vil_nitf2_tagged_record_definition::find("RSMPIA");
  if (!trpi)
  {
    vil_nitf2_tagged_record_definition::define("RSMPIA", "Multiple Section Polynomials")
      .field("IID", "Image Identifier", NITF_STR(80), true)
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40), false)
      .field("R0", "Constant Coef. Row", NITF_EXP(14, 2), false)
      .field("RX", "X Coef. Row", NITF_EXP(14, 2), false)
      .field("RY", "Y Coef. Row", NITF_EXP(14, 2), false)
      .field("RZ", "Z Coef. Row", NITF_EXP(14, 2), false)
      .field("RXX", "X^2 Coef. Row", NITF_EXP(14, 2), false)
      .field("RXY", "X*Y Coef. Row", NITF_EXP(14, 2), false)
      .field("RXZ", "X*Z Coef. Row", NITF_EXP(14, 2), false)
      .field("RYY", "Y^2 Coef. Row", NITF_EXP(14, 2), false)
      .field("RYZ", "Y*Z Coef. Row", NITF_EXP(14, 2), false)
      .field("RZZ", "Z^2 Coef. Row", NITF_EXP(14, 2), false)
      .field("C0", "Constant Coef. Col", NITF_EXP(14, 2), false)
      .field("CX", "X Coef. Col", NITF_EXP(14, 2), false)
      .field("CY", "Y Coef. Col", NITF_EXP(14, 2), false)
      .field("CZ", "Z Coef. Col", NITF_EXP(14, 2), false)
      .field("CXX", "X^2 Coef. Col", NITF_EXP(14, 2), false)
      .field("CXY", "X*Y Coef. Col", NITF_EXP(14, 2), false)
      .field("CXZ", "X*Z Coef. Col", NITF_EXP(14, 2), false)
      .field("CYY", "Y^2 Coef. Col", NITF_EXP(14, 2), false)
      .field("CYZ", "Y*Z Coef. Col", NITF_EXP(14, 2), false)
      .field("CZZ", "Z^2 Coef. Col", NITF_EXP(14, 2), false)
      .field("RNIS", "Number of Image Sections, Row", NITF_STR_BCS(3), false)
      .field("CNIS", "Number of Image Sections, Col", NITF_STR_BCS(3), false)
      .field("TNIS", "Number of Image Sections, Total", NITF_STR_BCS(3), false)
      .field("RSSIZ", "Section Size, Image Rows", NITF_EXP(14, 2), false)
      .field("CSSIZ", "Section Size, Image Cols", NITF_EXP(14, 2), false)
      .end();
  }
}

// condition functors
#define INCLIC_Y new vil_nitf2_field_value_one_of<std::string>("INCLIC", "Y")
#define INCLUC_Y new vil_nitf2_field_value_one_of<std::string>("INCLUC", "Y")

void
vpgl_replacement_sensor_model_tres::define_RSMECA()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMECA");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMECA", "Indirect Error Covariance")
      .field("IID", "Image Identifier", NITF_STR_BCSA(80))
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .field("TID", "Triangulation ID", NITF_STR_BCSA(40))
      .field("INCLIC", "Indirect Covariance Flag", NITF_STR_BCS(1))
      .field("INCLUC", "Unmodeled Error Covariance Flag", NITF_STR_BCS(1))

      .condition(INCLIC_Y, vil_nitf2_field_definitions()
        .field("NPAR", "Number Original Adjustable Params", NITF_INT(2, false))
        .field("NPARO", "Number Original Adjustable Params", NITF_INT(2, false))
        .field("IGN", "Number Independent Subgroups", NITF_INT(2, false))
        .field("CVDATE", "Version Date", NITF_STR_BCSA(8), true)
        .field("XUOL", "Local Coordinate Origin (XUOL)", NITF_STR_BCS(21))
        .field("YUOL", "Local Coordinate Origin (YUOL)", NITF_STR_BCS(21))
        .field("ZUOL", "Local Coordinate Origin (ZUOL)", NITF_STR_BCS(21))
        .field("XUXL", "Local Coordinate Unit Vector (XUXL)", NITF_STR_BCS(21))
        .field("XUYL", "Local Coordinate Unit Vector (XUYL)", NITF_STR_BCS(21))
        .field("XUZL", "Local Coordinate Unit Vector (XUZL)", NITF_STR_BCS(21))
        .field("YUXL", "Local Coordinate Unit Vector (YUXL)", NITF_STR_BCS(21))
        .field("YUYL", "Local Coordinate Unit Vector (YUYL)", NITF_STR_BCS(21))
        .field("YUZL", "Local Coordinate Unit Vector (YUZL)", NITF_STR_BCS(21))
        .field("ZUXL", "Local Coordinate Unit Vector (ZUXL)", NITF_STR_BCS(21))
        .field("ZUYL", "Local Coordinate Unit Vector (ZUYL)", NITF_STR_BCS(21))
        .field("ZUZL", "Local Coordinate Unit Vector (ZUZL)", NITF_STR_BCS(21))
        .field("IRO", "Image AdjP Row Const. Index", NITF_STR_BCS(2), true)
        .field("IRX", "Image AdjP Row X Index", NITF_STR_BCS(2), true)
        .field("IRY", "Image AdjP Row Y Index", NITF_STR_BCS(2), true)
        .field("IRZ", "Image AdjP Row Z Index", NITF_STR_BCS(2), true)
        .field("IRXX", "Image AdjP Row X^2 Index", NITF_STR_BCS(2), true)
        .field("IRXY", "Image AdjP Row XY Index", NITF_STR_BCS(2), true)
        .field("IRXZ", "Image AdjP Row XZ Index", NITF_STR_BCS(2), true)
        .field("IRYY", "Image AdjP Row Y^2 Index", NITF_STR_BCS(2), true)
        .field("IRYZ", "Image AdjP Row YZ Index", NITF_STR_BCS(2), true)
        .field("IRZZ", "Image AdjP Row Z^2 Index", NITF_STR_BCS(2), true)
        .field("ICO", "Image AdjP Row Const. Index", NITF_STR_BCS(2), true)
        .field("ICX", "Image AdjP Row X Index", NITF_STR_BCS(2), true)
        .field("ICY", "Image AdjP Row Y Index", NITF_STR_BCS(2), true)
        .field("ICZ", "Image AdjP Row Z Index", NITF_STR_BCS(2), true)
        .field("ICXX", "Image AdjP Row X^2 Index", NITF_STR_BCS(2), true)
        .field("ICXY", "Image AdjP Row XY Index", NITF_STR_BCS(2), true)
        .field("ICXZ", "Image AdjP Row XZ Index", NITF_STR_BCS(2), true)
        .field("ICYY", "Image AdjP Row Y^2 Index", NITF_STR_BCS(2), true)
        .field("ICYZ", "Image AdjP Row YZ Index", NITF_STR_BCS(2), true)
        .field("ICZZ", "Image AdjP Row Z^2 Index", NITF_STR_BCS(2), true)
        .field("GXO", "Ground AdjP X Const. Index", NITF_STR_BCS(2), true)
        .field("GYO", "Ground AdjP Y Const. Index", NITF_STR_BCS(2), true)
        .field("GZO", "Ground AdjP Z Const. Index", NITF_STR_BCS(2), true)
        .field("GXR", "X Ground Rotation Index", NITF_STR_BCS(2), true)
        .field("GYR", "Y Ground Rotation Index", NITF_STR_BCS(2), true)
        .field("GZR", "Z Ground Rotation Index", NITF_STR_BCS(2), true)
        .field("GS", " Ground Scale Index", NITF_STR_BCS(2), true)
        .field("GXX", "Ground X Adj.Prop. X Index", NITF_STR_BCS(2), true)
        .field("GXY", "Ground X Adj.Prop. Y Index", NITF_STR_BCS(2), true)
        .field("GXZ", "Ground X Adj.Prop. Z Index", NITF_STR_BCS(2), true)
        .field("GYX", "Ground Y Adj.Prop. X Index", NITF_STR_BCS(2), true)
        .field("GYY", "Ground Y Adj.Prop. Y Index", NITF_STR_BCS(2), true)
        .field("GYZ", "Ground Y Adj.Prop. Z Index", NITF_STR_BCS(2), true)
        .field("GZX", "Ground Z Adj.Prop. X Index", NITF_STR_BCS(2), true)
        .field("GZY", "Ground Z Adj.Prop. Y Index", NITF_STR_BCS(2), true)
        .field("GZZ", "Ground Z Adj.Prop. Z Index", NITF_STR_BCS(2), true)
        .repeat("IGN", vil_nitf2_field_definitions()
          .field("NUMOPG", "Number of Original Adj. Parameters", NITF_INT(2, false))
          .repeat(new vil_nitf2_field_covar_size<int>("NUMOPG"), vil_nitf2_field_definitions()
            .field("ERRCVG", "Error Covariance", NITF_EXP(14, 2))
          )
          .field("TCDF", "Time correlation domain flag", NITF_INT(1, false))
          .field("NCSEG", "Number of Correlation Segments", NITF_INT(1, false))
          .repeat("NCSEG", vil_nitf2_field_definitions()
            .field("CORSEG", "Segment Correlation Value", NITF_EXP(14, 2))
            .field("TAUSEG", "Segment Tau Value", NITF_EXP(14, 2))
          )
        )
        .repeat(new vil_nitf2_field_product_size<int>("NPAR", "NPARO"), vil_nitf2_field_definitions()
          .field("MAP", "Mapping Matrix Element", NITF_EXP(14, 2))
        )
      ) // end INCLIC_Y

      .condition(INCLUC_Y, vil_nitf2_field_definitions()
        .field("URR", "Unmodeled. Row Var.", NITF_STR_BCS(21))
        .field("URC", "Unmodeled. Row-Col Var.", NITF_STR_BCS(21))
        .field("UCC", "Unmodeled. Col-Col Var.", NITF_STR_BCS(21))
        .field("UNCSR", "Number of Corr. Segments Row", NITF_INT(1, false))
        .repeat("UNCSR", vil_nitf2_field_definitions()
          .field("UCORSR", "Segment Correlation", NITF_STR_BCS(21))
          .field("UTAUSR", "Segment Correlation", NITF_STR_BCS(21))
        )
        .field("UNCSC", "Number of Corr. Segments Col", NITF_INT(1, false))
        .repeat("UNCSC", vil_nitf2_field_definitions()
          .field("UCORSC", "Segment Correlation", NITF_STR_BCS(21))
          .field("UTAUSC", "Segment Correlation", NITF_STR_BCS(21))
        )
      ) // end INCLUC_Y

      .end();
  }
}

// condition functors
#define LOCTYP_R new vil_nitf2_field_value_one_of<std::string>("LOCTYP", "R")
#define APTYP_I new vil_nitf2_field_value_one_of<std::string>("APTYP", "I")
#define APTYP_G new vil_nitf2_field_value_one_of<std::string>("APTYP", "G")
#define APBASE_Y new vil_nitf2_field_value_one_of<std::string>("APBASE", "Y")
#define ACSMC_N new vil_nitf2_field_value_one_of<std::string>("ACSMC", "N")
#define ACSMC_Y new vil_nitf2_field_value_one_of<std::string>("ACSMC", "Y")
#define UACSMC_N new vil_nitf2_field_value_one_of<std::string>("UACSMC", "N")
#define UACSMC_Y new vil_nitf2_field_value_one_of<std::string>("UACSMC", "Y")

void
vpgl_replacement_sensor_model_tres::define_RSMECB()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMECB");
  if (!trgi)
  {

    vil_nitf2_tagged_record_definition::define("RSMECB", "Extended Indirect Error Covariance")
      .field("IID", "Image Identifier", NITF_STR_BCSA(80))
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .field("TID", "Triangulation ID", NITF_STR_BCSA(40))
      .field("INCLIC", "Indirect Covariance Flag", NITF_STR_BCS(1))
      .field("INCLUC", "Unmodeled Error Covariance Flag", NITF_STR_BCS(1))

      .condition(INCLIC_Y, vil_nitf2_field_definitions()
        .field("NPARO", "Number Original Adjustable Params", NITF_INT(2, false))
        .field("IGN", "Number Independent Subgroups", NITF_INT(2, false))
        .field("CVDATE", "Version Date", NITF_STR_BCSA(8), true)
        .field("NPAR", "Number Active Adjustable Params", NITF_INT(2, false))
        .field("APTYP", "Adjustable Parameter Type", NITF_STR_BCS(1))
        .field("LOCTYP", "Local Coordinate System Id", NITF_STR_BCS(1))
        .field("NSFX", "Normalization Scale Factor for X", NITF_STR_BCS(21))
        .field("NSFY", "Normalization Scale Factor for Y", NITF_STR_BCS(21))
        .field("NSFZ", "Normalization Scale Factor for Z", NITF_STR_BCS(21))
        .field("NOFFX", "Normalization Offset for X", NITF_STR_BCS(21))
        .field("NOFFY", "Normalization Offset for Y", NITF_STR_BCS(21))
        .field("NOFFZ", "Normalization Offset for Z", NITF_STR_BCS(21))
        .condition(LOCTYP_R, vil_nitf2_field_definitions()
          .field("XUOL", "Local Coordinate Origin (XUOL)", NITF_STR_BCS(21))
          .field("YUOL", "Local Coordinate Origin (YUOL)", NITF_STR_BCS(21))
          .field("ZUOL", "Local Coordinate Origin (ZUOL)", NITF_STR_BCS(21))
          .field("XUXL", "Local Coordinate Unit Vector (XUXL)", NITF_STR_BCS(21))
          .field("XUYL", "Local Coordinate Unit Vector (XUYL)", NITF_STR_BCS(21))
          .field("XUZL", "Local Coordinate Unit Vector (XUZL)", NITF_STR_BCS(21))
          .field("YUXL", "Local Coordinate Unit Vector (YUXL)", NITF_STR_BCS(21))
          .field("YUYL", "Local Coordinate Unit Vector (YUYL)", NITF_STR_BCS(21))
          .field("YUZL", "Local Coordinate Unit Vector (YUZL)", NITF_STR_BCS(21))
          .field("ZUXL", "Local Coordinate Unit Vector (ZUXL)", NITF_STR_BCS(21))
          .field("ZUYL", "Local Coordinate Unit Vector (ZUYL)", NITF_STR_BCS(21))
          .field("ZUZL", "Local Coordinate Unit Vector (ZUZL)", NITF_STR_BCS(21))
        )
        .field("APBASE", "Basis Option", NITF_STR_BCS(1))
        .condition(APTYP_I, vil_nitf2_field_definitions()
          .field("NISAP", "Number Img Space APs", NITF_INT(2))
          .field("NISAPR", "Number Row Img Space APs", NITF_INT(2))
          .repeat("NISAPR", vil_nitf2_field_definitions()
            .field("XPWRR", "Row Parameter Power of X", NITF_INT(1))
            .field("YPWRR", "Row Parameter Power of Y", NITF_INT(1))
            .field("ZPWRR", "Row Parameter Power of Z", NITF_INT(1))
          )
          .field("NISAPC", "Number Col. Img Space APs", NITF_INT(2))
          .repeat("NISAPC",  vil_nitf2_field_definitions()
            .field("XPWRC", "Col Parameter Power of X", NITF_INT(1))
            .field("YPWRC", "Col Parameter Power of Y", NITF_INT(1))
            .field("ZPWRC", "Col Parameter Power of Z", NITF_INT(1))
          )
        )
        .condition(APTYP_G, vil_nitf2_field_definitions()
          .field("NGSAP", "Number Gnd Space APs", NITF_INT(2, false))
          .repeat("NGSAP", vil_nitf2_field_definitions()
            .field("GSAPID", "Gnd Space Param. id", NITF_INT(4, false))
          )
        )
        .condition(APBASE_Y, vil_nitf2_field_definitions()
          .field("NBASIS", "Number Basis APs", NITF_INT(2, false))
          .repeat(new vil_nitf2_field_product_size<int>("NPAR", "NBASIS"), vil_nitf2_field_definitions()
            .field("AEL", "Matrix A Element", NITF_STR_BCS(21))
          )
        )
        .repeat("IGN", vil_nitf2_field_definitions()
          .field("NUMOPG", "Number of Corr. Segments", NITF_INT(2))
          .repeat(new vil_nitf2_field_covar_size<int>("NUMOPG"), vil_nitf2_field_definitions()
            .field("ERRCVG", "Error Covariance", NITF_STR_BCS(21))
          )
          .field("TCDF", "Correlation Flag", NITF_INT(1))
          .field("ACSMC", "Correlation Option", NITF_STR_BCS(1))
          .condition(ACSMC_N, vil_nitf2_field_definitions()
            .field("NCSEG", "Number of Corr. Segments", NITF_INT(1))
            .repeat("NCSEG",vil_nitf2_field_definitions()
              .field("CORSEG", "Correlation ", NITF_STR_BCS(21))
              .field("TAUSEG", "Covar. Time", NITF_STR_BCS(21))
            )
          )
          .condition(ACSMC_Y, vil_nitf2_field_definitions()
            .field("AC", "Correlation A", NITF_STR_BCS(21))
            .field("ALPC", "Correlation Alpha", NITF_STR_BCS(21))
            .field("BETC", "Correlation Beta", NITF_STR_BCS(21))
            .field("TC", "Correlation T", NITF_STR_BCS(21))
          )
        )
        .repeat(new vil_nitf2_field_product_size<int>("NPAR", "NPARO"), vil_nitf2_field_definitions()
          .field("MAP", "Mapping Matrix", NITF_STR_BCS(21))
        )
      ) // end INCLIC_Y

      .condition(INCLUC_Y, vil_nitf2_field_definitions()
        .field("URR", "Unmodeled. Row Var.", NITF_STR_BCS(21))
        .field("URC", "Unmodeled. Row-Col Var.", NITF_STR_BCS(21))
        .field("UCC", "Unmodeled. Col-Col Var.", NITF_STR_BCS(21))
        .field("UACSMC", "Unmodeled CSM Option", NITF_STR_BCSA(1))
        .condition(UACSMC_N, vil_nitf2_field_definitions()
          .field("UNCSR", "Number of row corr segments", NITF_INT(1))
          .repeat("UNCSR", vil_nitf2_field_definitions()
            .field("UCORSR", "row corr value for seg", NITF_STR_BCS(21))
            .field("UTAUSR", "row tau value for seg", NITF_STR_BCS(21))
          )
          .field("UNCSC", "Number of col corr segments", NITF_INT(1))
          .repeat("UNCSC", vil_nitf2_field_definitions()
            .field("UCORSC", "col corr value for seg", NITF_STR_BCS(21))
            .field("UTAUSC", "col tau value for seg", NITF_STR_BCS(21))
          )
        )
        .condition(UACSMC_Y, vil_nitf2_field_definitions()
          .field("UACR",   "A for row corr function", NITF_STR_BCS(21))
          .field("UALPCR", "alpha for row corr function", NITF_STR_BCS(21))
          .field("UBETCR", "beta for row corr function", NITF_STR_BCS(21))
          .field("UTCR",   "T for row corr function", NITF_STR_BCS(21))
          .field("UACC",   "A for col corr function", NITF_STR_BCS(21))
          .field("UALPCC", "alpha for col corr function", NITF_STR_BCS(21))
          .field("UBETCC", "beta for col corr function", NITF_STR_BCS(21))
          .field("UTCC",   "T for row corr function", NITF_STR_BCS(21))
        )
      ) // end INCLUC_Y

      .end();
  }
}

// define TREs for cases not currently handled by vpgl_RSM_camera
// even if present only 40 bytes are read to recover the EDITION field
void
vpgl_replacement_sensor_model_tres::define_RSMDCA()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMDCA");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMDCA", "Direct Error Covariance")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMDCB()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMDCB");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMDCB", "Extended Direct Error Covariance")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMAPA()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMAPA");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMAPA", "Adjustable Parameters")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMAPB()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMAPB");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMAPB", "Extended Adjustable Parameters")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMGIA()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMGIA");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMGIA", "Multi-section Grids")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}
void
vpgl_replacement_sensor_model_tres::define_RSMGGA()
{
  vil_nitf2_tagged_record_definition * trgi = vil_nitf2_tagged_record_definition::find("RSMGGA");
  if (!trgi)
  {
    vil_nitf2_tagged_record_definition::define("RSMGGA", "Ground to Image Grid")
      .field("EDITION", "Association with Image", NITF_STR_BCSA(40))
      .end();
  }
}

#if 0
      .field("UNCSR", "Number of Corr. Segments Row", NITF_INT(1, false), false, nullptr, IND_UACSMC_N)
      .repeat(new vil_nitf2_repeat_count<int>("UNCSR"),
           vil_nitf2_field_definitions().field("UCORSR", "Segment Correlation", NITF_STR_BCS(21), false, nullptr, IND_UACSMC_N)
           .field("UTAUSR", "Segment Covar Time", NITF_STR_BCS(21), false, nullptr, IND_UACSMC_N))
      .field("UNCSC", "Number of Corr. Segments Col", NITF_INT(1, false), false, nullptr, IND_UACSMC_N)
      .repeat(new vil_nitf2_repeat_count<int>("UNCSC"),
           vil_nitf2_field_definitions().field("UCORSC", "Segment Correlation", NITF_STR_BCS(21), false, nullptr, IND_UACSMC_N)
           .field("UTAUSC", "Segment Covar. Time",   NITF_STR_BCS(21), false, nullptr, IND_UACSMC_N))
      .field("UACR",   "Unmodeled CSM Row A param.",NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UALPCR", "Unmodeled CSM Row Alpha",   NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UBETCR", "Unmodeled CSM Row Beta",    NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UTCR",   "Unmodeled CSM Row T",       NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UACC",   "Unmodeled CSM Col A param.",NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UALPCC", "Unmodeled CSM Col Alpha",   NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UBETCC", "Unmodeled CSM Col Beta",    NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
      .field("UTCC",   "Unmodeled CSM Col T",       NITF_STR_BCS(21), false, nullptr, IND_UACSMC_Y)
#endif
