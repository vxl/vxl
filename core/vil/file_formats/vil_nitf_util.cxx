// This is core/vil/file_formats/vil_nitf_util.cxx

//:
// \file
#include "vil_nitf_util.h"
#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vcl_cmath.h> // for fabs()
#include <vcl_cctype.h> // for isdigit()
#include <vcl_cstddef.h> // for size_t

//: Function to delete characters c from s.
//  Options are:
//
//  \param s input string
//  \param c character to remove
//  \param optflag flag indicating how to remove characters
//    -  ALL:        delete all characters c.
//    -  TRAILING:   delete trailing characters c.
//    -  LEADING:    delete leading characters c.
//    -  HEAD/TAIL:  delete all leading && trailing characters c.
//
char* squeeze (char *s, int c, int optflag)
{
    int i,j;
    switch (optflag) {
     case 0:
      for (i=j=0; s[i] != '\0'; i++)
        if (s[i] != c)
          s[j++] = s[i];
      s[j] = '\0';
      break;
     case 1:
     case 3:
      i = vcl_strlen(s)-1;
      while (s[i] == c) {
        s[i--] = '\0';
        if (i < 0) return s;
      }
      if (optflag==1) break;
     // Case 2 *MUST* come immediately after case 1/3.
     case 2:
      while (s && c && *s==c)
      {
        char *p = s;
        while (*p++) *(p-1) = *p;
        *(p-1) = 0;
      }
      break;
    }  // end switch

    return s;
}

//
// Function to delete contents of vector of strings, then  clear vector.
//
void clear_string_vector(vcl_vector<vcl_string *> str_vector)
{
  if (str_vector.size() > 0) {
    for (vcl_size_t i = 0; i < str_vector.size(); ++i) {
      vcl_string * temp_str = str_vector[i];
      delete temp_str;
    }
    str_vector.clear();
  }
}

////////////////////////////////////////////////////////////////////
//
//: Display bytes as hex.
//
// \param buffer buffer containing raw bytes
// \param display_count number of pixels worth of bytes to display
// \param bytes_per_pixel number of bytes per pixel
// \param label optional string to display explaining meaning of byes.
//
////////////////////////////////////////////////////////////////////
void display_as_hex(
    const unsigned char * buffer,
    unsigned int display_count,
    unsigned int bytes_per_value,
    vcl_string label)
{
  static vcl_string method_name = "display_as_hex: ";

  unsigned short temp_short;

  vcl_cout << method_name;
  if (label.length() > 0) {
    vcl_cout << label << " - ";
  }
  vcl_cout << "display " << display_count
           << " pixels.  bytes_per_value = " << bytes_per_value
           << vcl_endl;

  unsigned int loop_count = display_count * bytes_per_value;

  vcl_cout << vcl_hex;
  for (unsigned count = 0; count < loop_count; ++count)
  {
//  if ((count > 0) && (count % 2 == 0))
    if (count > 0)
    {
      temp_short = (unsigned short) buffer[count];
      vcl_cout << ' ';
    }
    vcl_cout << temp_short;

    if ((count > 0) && (count % 32 == 0))
      vcl_cout << vcl_endl;
  }  // end for count

  vcl_cout << vcl_endl;

  // RESET OUTPUT FORMAT
  vcl_cout << vcl_dec << vcl_endl;
}  // end method

////////////////////////////////////////////////////////////////////
//: Display bytes as hex.
//
// \param buffer buffer containing raw bytes
// \param display_count number of pixels worth of bytes to display
// \param bytes_per_pixel number of bytes per pixel
//
////////////////////////////////////////////////////////////////////
void display_as_hex(const unsigned char value)
{
  static vcl_string method_name = "display_as_hex: ";

  unsigned short temp_short;
  temp_short = (unsigned short) value;

  vcl_cout << vcl_hex << temp_short << vcl_dec;
}  // end method

/////////////////////////////////////////////////////////////////
//: converts a latitude longitude string to two double values
//
//  \param str string containing latitude longitude
//  \param lat pointer to latitude value to be set
//  \param lon pointer to longitude value to be set
//
//  \return sum of lengths of latitude and longitude substrings
//
/////////////////////////////////////////////////////////////////
int geostr_to_latlon(const char* str, double* lat, double* lon)
{
  int latstrlen = 0;
  int lonstrlen = 0;

  if ((latstrlen = geostr_to_double (str, lat, LAT)) == 0) return 0;
  str += latstrlen;
  if ( (lonstrlen = geostr_to_double (str, lon, LON)) == 0) return 0;

  return latstrlen + lonstrlen;
}

/////////////////////////////////////////////////////////////////
// Helper function for geostr_to_double
/////////////////////////////////////////////////////////////////
static int to_int(const char* string,int size)
{
  int value = 0;
  while (size--)
    value = (value*10) + (*string++ - '0');
  return value;
}

/////////////////////////////////////////////////////////////////
//: convert one of latitude or longitude string to a double
//
//  \param string string containing latitude longitude
//  \param val pointer to value to be set
//  \param lat_lon_flag enum value indicating if should convert latitude
//      or longitude value
//
//  \return pointer to next value to extract.
//
/////////////////////////////////////////////////////////////////
int geostr_to_double(const char* string, double* val, geopt_coord lat_lon_flag)
{
  int length;
  int deg, min;
  double fsec;
  const char* orig = string;

  //here are lat/lon dependent variables
  char sposdir, cposdir, snegdir, cnegdir;
  int maxval;

  if (lat_lon_flag == LAT)
  {
      sposdir='n';
      cposdir='N';
      snegdir='s';
      cnegdir='S';
      maxval=90;
  }
  else
  {
      sposdir='e';
      cposdir='E';
      snegdir='w';
      cnegdir='W';
      maxval=180;
  }

  while ((*string == ' ') || (*string == '\t'))
      ++string;

  for (length=0; vcl_isdigit(*string) && length<15; ++string, ++length);
  if (length >14) return 0;

  //three different formats accepted
  //DDDMMSS.S[d] where [d]=nNsSeEwW
  if (length > 3)
  {
    if (length < 5)
      return 0;

    //get the minutes
    if ((min = to_int (string-4, 2)) >= 60 || min<0)
      return 0;

    //get the degrees
    if ((deg = to_int (string-length, length-4)) > maxval || deg<0)
      return 0;

    //get the seconds (float)
    string-=2;
    char* temp = 0;
    for (length=0;
        (*string=='.' || vcl_isdigit(*string)) && length<15;
         ++string, ++length)
      ;
    if (length>14) return 0;

    STRNCPY(temp,string-length,length);
    if ( (fsec = atof(temp)) >= 60. || fsec<0.)
      return 0;
    delete[] temp;

    //skip to the direction
    while ((*string == ' ') || (*string == '\t'))
      ++string;

    //calculate the value
    *val = deg;
    *val += ((double)(min))/(60.0);
    *val += ((double)(fsec))/(3600.0);

    //adjust for the direction
    if ( *string==sposdir || *string==cposdir) {}
    else if ( *string==snegdir || *string==cnegdir) {*val = -(*val);}
    else return 0;

    ++string;

    return string-orig;
  }  // end if (length > 3)
  else
  //DDDdMM'SS"[d]  where [d]=nNsSeEwW
  if (*string == 'd') {
    //get the degrees
    if (length > 3)
      return 0;
    if ((deg = to_int (string-length, length)) > maxval || deg<0)
      return 0;

    //go past 'd' and spaces
    ++string;
    while ((*string == ' ') || (*string == '\t'))
      ++string;

    //get the minutes
    for (length=0; vcl_isdigit(*string) && length<15; ++string, ++length);
    if (length>14) return 0;
    if (length > 2)
      return 0;
    if ((min = to_int (string-length, length)) >= 60 || min<0)
      return 0;

    //go past ''' and spaces
    ++string;
    while ((*string == ' ') || (*string == '\t'))
      ++string;

    //get the seconds (float)
    char* temp=0;
    for (length=0;
        (*string=='.' || vcl_isdigit(*string)) && length<15;
         ++string, ++length)
      ;
    if (length>14) return 0;

    STRNCPY(temp,string-length,length);
    if ( (fsec = atof(temp)) >= 60. || fsec<0.)
      return 0;
    delete temp;

    //go past '"' and any spaces to the direction
    ++string;
    while ((*string == ' ') || (*string == '\t'))
      ++string;

    //calculate value
    *val = deg;
    *val += ((double)(min))/(60.0);
    *val += ((double)(fsec))/(3600.0);

    //adjust for the direction
    if ( *string==sposdir || *string==cposdir) {}
    else if ( *string==snegdir || *string==cnegdir) {*val = -(*val);}
    else return 0;

    ++string;

    return string-orig;
  }  // if (*string == 'd')
  else
  // DDD.DDDD
  if (*string == ' ' || *string == '-' || *string == '+' ||
      *string == '.' || *string == '\0')
  {
    char* temp = 0;
    string = (char*) orig;

    //go past any spaces
    while ((*string == ' ') || (*string == '\t'))
        ++string;

    //  calculate length of float
    for (length = 0;
        (*string == '+' ||*string == '-' || *string == '.' ||
         vcl_isdigit(*string)) && length < 15;
         ++string, ++length);
    if (length > 14) return 0;

    // calculate value of float
    STRNCPY (temp, string-length, length);
    *val = atof (temp);
    if (vcl_fabs(*val) > float(maxval)) return 0;
    delete temp;

    ++string;

    return string - orig;
  }
  else {
    return 0;
  }
}  // end method geostr_to_double


// case insensitive string comparison
int
nitf_strcasecmp( const char* s1, const char* s2 )
{
  while( *s1 != '\0' &&
         *s2 != '\0' &&
         vcl_toupper( *s1 ) == vcl_toupper( *s2 ) ) {
    ++s1;
    ++s2;
  }
  int us1 = vcl_toupper( *s1 );
  int us2 = vcl_toupper( *s2 );
  if( us1 == us2 )
    return 0;
  else if( us1 < us2 )
    return -1;
  else
    return 1;
}
