#ifndef vil_dicom_header_h_
#define vil_dicom_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief DICOM Header reader and writer.
// \author Chris Wolstenholme - Manchester
// Modified by Ian Scott to work with vil loader.

#include <vcl_ostream.h>
#include <vcl_string.h>
#include <vxl_config.h>
#include <vil/vil_stream.h>

//: DICOM types
//  Defines the type of the dicom file being loaded. A
//  dicom file can be a Part10 file a non-Part10 file or an unknown
//  file type (possibly not dicom)

enum vil_dicom_header_type
{
  VIL_DICOM_HEADER_DTUNKNOWN=0, /*!< Value for unknown dicom types */
  VIL_DICOM_HEADER_DTPART10,    /*!< Value for Part10 dicom files */
  VIL_DICOM_HEADER_DTNON_PART10 /*!< Value for non Part10 dicom files */
};

//: Endian types
enum vil_dicom_header_endian
{
  VIL_DICOM_HEADER_DEUNKNOWN=0,    /*!< If the endian is unknown */
  VIL_DICOM_HEADER_DELITTLEENDIAN, /*!< Value for little endian */
  VIL_DICOM_HEADER_DEBIGENDIAN     /*!< Value for big endian */
};

//: Encapsulated image types
enum vil_dicom_header_image_type
{
  VIL_DICOM_HEADER_DITUNKNOWN=0,     /*!< Value for unknown (or non-encapsulated) file types */
  VIL_DICOM_HEADER_DITJPEGBASE,      /*!< Value for encapsulated jpeg baseline files */
  VIL_DICOM_HEADER_DITJPEGEXTLOSSY,  /*!< Value for encapsulated jpeg, extended lossy files */
  VIL_DICOM_HEADER_DITJPEGSPECNH,    /*!< Value for encapsulated jpeg, spectral selection non-hierarchical files */
  VIL_DICOM_HEADER_DITJPEGFULLNH,    /*!< Value for encapsulated jpeg, full progression non-hierarchical files */
  VIL_DICOM_HEADER_DITJPEGLOSSLNH,   /*!< Value for encapsulated jpeg, lossless non-hierarchical files */
  VIL_DICOM_HEADER_DITJPEGEXTHIER,   /*!< Value for encapsulated jpeg, extended hierarchical files */
  VIL_DICOM_HEADER_DITJPEGSPECHIER,  /*!< Value for encapsulated jpeg, spectral selection, hierarchical files */
  VIL_DICOM_HEADER_DITJPEGFULLHIER,  /*!< Value for encapsulated jpeg, full progression, hierarchical files */
  VIL_DICOM_HEADER_DITJPEGLOSSLHIER, /*!< Value for encapsulated jpeg, lossless, hierarchical files */
  VIL_DICOM_HEADER_DITJPEGLOSSLDEF,  /*!< Value for encapsulated jpeg, default lossless files */
  VIL_DICOM_HEADER_DITRLE            /*!< Value for encapsulated RLE files */
};

// Max size (number of groups/elements)
const unsigned VIL_DICOM_HEADER_MAXHEADERSIZE             =100;

// Define the dicom groups
const vxl_uint_16 VIL_DICOM_HEADER_COMMANDGROUP             =0x0000;
const vxl_uint_16 VIL_DICOM_HEADER_METAFILEGROUP            =0x0002;
const vxl_uint_16 VIL_DICOM_HEADER_BASICDIRGROUP            =0x0004;
const vxl_uint_16 VIL_DICOM_HEADER_IDENTIFYINGGROUP         =0x0008;
const vxl_uint_16 VIL_DICOM_HEADER_PATIENTINFOGROUP         =0x0010;
const vxl_uint_16 VIL_DICOM_HEADER_ACQUISITIONGROUP         =0x0018;
const vxl_uint_16 VIL_DICOM_HEADER_RELATIONSHIPGROUP        =0x0020;
const vxl_uint_16 VIL_DICOM_HEADER_IMAGEGROUP               =0x0028;
const vxl_uint_16 VIL_DICOM_HEADER_STUDYGROUP               =0x0032;
const vxl_uint_16 VIL_DICOM_HEADER_VISITGROUP               =0x0038;
const vxl_uint_16 VIL_DICOM_HEADER_WAVEFORMGROUP            =0x003a;
const vxl_uint_16 VIL_DICOM_HEADER_PROCEDUREGROUP           =0x0040;
const vxl_uint_16 VIL_DICOM_HEADER_DEVICEGROUP              =0x0050;
const vxl_uint_16 VIL_DICOM_HEADER_NMIMAGEGROUP             =0x0054;
const vxl_uint_16 VIL_DICOM_HEADER_MEDIAGROUP               =0x0088;
const vxl_uint_16 VIL_DICOM_HEADER_BASICFILMSESSIONGROUP    =0x2000;
const vxl_uint_16 VIL_DICOM_HEADER_BASICFILMBOXGROUP        =0x2010;
const vxl_uint_16 VIL_DICOM_HEADER_BASICIMAGEBOXGROUP       =0x2020;
const vxl_uint_16 VIL_DICOM_HEADER_BASICANNOTATIONBOXGROUP  =0x2030;
const vxl_uint_16 VIL_DICOM_HEADER_BASICIMAGEOVERLAYBOXGROUP=0x2040;
const vxl_uint_16 VIL_DICOM_HEADER_PRINTJOBGROUP            =0x2100;
const vxl_uint_16 VIL_DICOM_HEADER_PRINTERGROUP             =0x2110;
const vxl_uint_16 VIL_DICOM_HEADER_TEXTGROUP                =0x4000;
const vxl_uint_16 VIL_DICOM_HEADER_RESULTSGROUP             =0x4008;
const vxl_uint_16 VIL_DICOM_HEADER_CURVEGROUP               =0x5000;
const vxl_uint_16 VIL_DICOM_HEADER_OVERLAYGROUP             =0x6000;
const vxl_uint_16 VIL_DICOM_HEADER_PIXELGROUP               =0x7fe0;
const vxl_uint_16 VIL_DICOM_HEADER_PADGROUP                 =0xfffc;
const vxl_uint_16 VIL_DICOM_HEADER_DELIMITERGROUP           =0xfffe;

// Useful elements of the Meta File group
const vxl_uint_16 VIL_DICOM_HEADER_MFGROUPLENGTH            =0x0000;
const vxl_uint_16 VIL_DICOM_HEADER_MFTRANSFERSYNTAX         =0x0010;

// Useful elements of the Identifying group
const vxl_uint_16 VIL_DICOM_HEADER_IDGROUPLENGTH            =0x0000; // UL
const vxl_uint_16 VIL_DICOM_HEADER_IDLENGTHTOEND            =0x0001; // RET
const vxl_uint_16 VIL_DICOM_HEADER_IDSPECIFICCHARACTER      =0x0005; // CS
const vxl_uint_16 VIL_DICOM_HEADER_IDIMAGETYPE              =0x0008; // CS
const vxl_uint_16 VIL_DICOM_HEADER_IDSOPCLASSID             =0x0016; // UI
const vxl_uint_16 VIL_DICOM_HEADER_IDSOPINSTANCEID          =0x0018; // UI
const vxl_uint_16 VIL_DICOM_HEADER_IDSTUDYDATE              =0x0020; // DA
const vxl_uint_16 VIL_DICOM_HEADER_IDSERIESDATE             =0x0021; // DA
const vxl_uint_16 VIL_DICOM_HEADER_IDACQUISITIONDATE        =0x0022; // DA
const vxl_uint_16 VIL_DICOM_HEADER_IDIMAGEDATE              =0x0023; // DA
const vxl_uint_16 VIL_DICOM_HEADER_IDSTUDYTIME              =0x0030; // TM
const vxl_uint_16 VIL_DICOM_HEADER_IDSERIESTIME             =0x0031; // TM
const vxl_uint_16 VIL_DICOM_HEADER_IDACQUISITIONTIME        =0x0032; // TM
const vxl_uint_16 VIL_DICOM_HEADER_IDIMAGETIME              =0x0033; // TM
const vxl_uint_16 VIL_DICOM_HEADER_IDACCESSIONNUMBER        =0x0050; // SH
const vxl_uint_16 VIL_DICOM_HEADER_IDMODALITY               =0x0060; // CS
const vxl_uint_16 VIL_DICOM_HEADER_IDMANUFACTURER           =0x0070; // LO
const vxl_uint_16 VIL_DICOM_HEADER_IDINSTITUTIONNAME        =0x0080; // LO
const vxl_uint_16 VIL_DICOM_HEADER_IDINSTITUTIONADDRESS     =0x0081; // ST
const vxl_uint_16 VIL_DICOM_HEADER_IDREFERRINGPHYSICIAN     =0x0090; // PN
const vxl_uint_16 VIL_DICOM_HEADER_IDSTATIONNAME            =0x1010; // SH
const vxl_uint_16 VIL_DICOM_HEADER_IDSTUDYDESCRIPTION       =0x1030; // LO
const vxl_uint_16 VIL_DICOM_HEADER_IDSERIESDESCRIPTION      =0x103E; // LO
const vxl_uint_16 VIL_DICOM_HEADER_IDATTENDINGPHYSICIAN     =0x1050; // PN
const vxl_uint_16 VIL_DICOM_HEADER_IDOPERATORNAME           =0x1070; // PN
const vxl_uint_16 VIL_DICOM_HEADER_IDMANUFACTURERMODEL      =0x1090; // LO

// Useful elements from the Patient Info group
const vxl_uint_16 VIL_DICOM_HEADER_PIGROUPLENGTH            =0x0000; // UL
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTNAME            =0x0010; // PN
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTID              =0x0020; // LO
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTBIRTHDATE       =0x0030; // DA
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTSEX             =0x0040; // CS
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTAGE             =0x1010; // AS
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTWEIGHT          =0x1030; // DS
const vxl_uint_16 VIL_DICOM_HEADER_PIPATIENTHISTORY         =0x21B0; // LT

// Useful elements from the Acquisition group
const vxl_uint_16 VIL_DICOM_HEADER_AQSCANNINGSEQUENCE       =0x0020; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQSEQUENCEVARIANT        =0x0021; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQSCANOPTIONS            =0x0022; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQMRACQUISITIONTYPE      =0x0023; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQSEQUENCENAME           =0x0024; // SH
const vxl_uint_16 VIL_DICOM_HEADER_AQANGIOFLAG              =0x0025; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQSLICETHICKNESS         =0x0050; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQREPETITIONTIME         =0x0080; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQECHOTIME               =0x0081; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQINVERSIONTIME          =0x0082; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQNUMBEROFAVERAGES       =0x0083; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQECHONUMBERS            =0x0086; // IS
const vxl_uint_16 VIL_DICOM_HEADER_AQMAGNETICFIELDSTRENGTH  =0x0087; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQSLICESPACING           =0x0088; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQECHOTRAINLENGTH        =0x0091; // IS
const vxl_uint_16 VIL_DICOM_HEADER_AQPIXELBANDWIDTH         =0x0095; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQSOFTWAREVERSION        =0x1020; // LO
const vxl_uint_16 VIL_DICOM_HEADER_AQPROTOCOLNAME           =0x1030; // LO
const vxl_uint_16 VIL_DICOM_HEADER_AQHEARTRATE              =0x1088; // IS
const vxl_uint_16 VIL_DICOM_HEADER_AQCARDIACNUMBEROFIMAGES  =0x1090; // IS
const vxl_uint_16 VIL_DICOM_HEADER_AQTRIGGERWINDOW          =0x1094; // IS
const vxl_uint_16 VIL_DICOM_HEADER_AQRECONTRUCTIONDIAMETER  =0x1100; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQRECEIVINGCOIL          =0x1250; // SH
const vxl_uint_16 VIL_DICOM_HEADER_AQPHASEENCODINGDIRECTION =0x1312; // CS
const vxl_uint_16 VIL_DICOM_HEADER_AQFLIPANGLE              =0x1314; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQSAR                    =0x1316; // DS
const vxl_uint_16 VIL_DICOM_HEADER_AQPATIENTPOSITION        =0x5100; // CS

// Useful elements from the Relationship group
const vxl_uint_16 VIL_DICOM_HEADER_RSSTUDYINSTANCEUID       =0x000D; // UI
const vxl_uint_16 VIL_DICOM_HEADER_RSSERIESINSTANCEUID      =0x000E; // UI
const vxl_uint_16 VIL_DICOM_HEADER_RSSTUDYID                =0x0010; // SH
const vxl_uint_16 VIL_DICOM_HEADER_RSSERIESNUMBER           =0x0011; // IS
const vxl_uint_16 VIL_DICOM_HEADER_RSAQUISITIONNUMBER       =0x0012; // IS
const vxl_uint_16 VIL_DICOM_HEADER_RSIMAGENUMBER            =0x0013; // IS
const vxl_uint_16 VIL_DICOM_HEADER_RSPATIENTORIENTATION     =0x0020; // CS
const vxl_uint_16 VIL_DICOM_HEADER_RSIMAGEPOSITION          =0x0032; // DS
const vxl_uint_16 VIL_DICOM_HEADER_RSIMAGEORIENTATION       =0x0037; // DS
const vxl_uint_16 VIL_DICOM_HEADER_RSFRAMEOFREFERENCEUID    =0x0052; // UI
const vxl_uint_16 VIL_DICOM_HEADER_RSIMAGESINACQUISITION    =0x1002; // IS
const vxl_uint_16 VIL_DICOM_HEADER_RSPOSITIONREFERENCE      =0x1040; // LO
const vxl_uint_16 VIL_DICOM_HEADER_RSSLICELOCATION          =0x1041; // DS

// Useful elements from the Image group
const vxl_uint_16 VIL_DICOM_HEADER_IMSAMPLESPERPIXEL        =0x0002; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMPHOTOMETRICINTERP      =0x0004; // CS
const vxl_uint_16 VIL_DICOM_HEADER_IMROWS                   =0x0010; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMCOLUMNS                =0x0011; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMPLANES                 =0x0012; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMPIXELSPACING           =0x0030; // DS
const vxl_uint_16 VIL_DICOM_HEADER_IMBITSALLOCATED          =0x0100; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMBITSSTORED             =0x0101; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMHIGHBIT                =0x0102; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMPIXELREPRESENTATION    =0x0103; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMSMALLIMPIXELVALUE      =0x0106; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMLARGEIMPIXELVALUE      =0x0107; // US
const vxl_uint_16 VIL_DICOM_HEADER_IMPIXELPADDINGVALUE      =0x0120; // SS
const vxl_uint_16 VIL_DICOM_HEADER_IMWINDOWCENTER           =0x1050; // DS
const vxl_uint_16 VIL_DICOM_HEADER_IMWINDOWWIDTH            =0x1051; // DS
const vxl_uint_16 VIL_DICOM_HEADER_IMRESCALEINTERCEPT       =0x1052; // DS
const vxl_uint_16 VIL_DICOM_HEADER_IMRESCALESLOPE           =0x1053; // DS

// Useful elements from the Pixel group
const vxl_uint_16 VIL_DICOM_HEADER_PXGROUPLENGTH            =0x0000; // UL
const vxl_uint_16 VIL_DICOM_HEADER_PXPIXELDATA              =0x0010; // OW

// Tags needed in the Delimeter group
const vxl_uint_16 VIL_DICOM_HEADER_DLITEM                   =0xe000;
const vxl_uint_16 VIL_DICOM_HEADER_DLITEMDELIMITATIONITEM   =0xe00d;
const vxl_uint_16 VIL_DICOM_HEADER_DLSEQDELIMITATIONITEM    =0xe0dd;


// Defines for the Value Representations for Part10 meta header
const char * const VIL_DICOM_HEADER_APPLICATIONENTRY          ="AE";
const char * const VIL_DICOM_HEADER_AGESTRING                 ="AS";
const char * const VIL_DICOM_HEADER_ATTRIBUTETAG              ="AT";
const char * const VIL_DICOM_HEADER_CODESTRING                ="CS";
const char * const VIL_DICOM_HEADER_DATE                      ="DA";
const char * const VIL_DICOM_HEADER_DECIMALSTRING             ="DS";
const char * const VIL_DICOM_HEADER_DATETIME                  ="DT";
const char * const VIL_DICOM_HEADER_FLOATINGPOINTDOUBLE       ="FD";
const char * const VIL_DICOM_HEADER_FLOATINGPOINTSINGLE       ="FL";
const char * const VIL_DICOM_HEADER_INTEGERSTRING             ="IS";
const char * const VIL_DICOM_HEADER_LONGSTRING                ="LO";
const char * const VIL_DICOM_HEADER_LONGTEXT                  ="LT";
const char * const VIL_DICOM_HEADER_OTHERBYTE                 ="OB";
const char * const VIL_DICOM_HEADER_OTHERWORD                 ="OW";
const char * const VIL_DICOM_HEADER_PERSONNAME                ="PN";
const char * const VIL_DICOM_HEADER_SHORTSTRING               ="SH";
const char * const VIL_DICOM_HEADER_SIGNEDLONG                ="SL";
const char * const VIL_DICOM_HEADER_SEQUENCE                  ="SQ";
const char * const VIL_DICOM_HEADER_SIGNEDSHORT               ="SS";
const char * const VIL_DICOM_HEADER_SHORTTEXT                 ="ST";
const char * const VIL_DICOM_HEADER_TIME                      ="TM";
const char * const VIL_DICOM_HEADER_UNIQUEIDENTIFIER          ="UI";
const char * const VIL_DICOM_HEADER_UNSIGNEDLONG              ="UL";
const char * const VIL_DICOM_HEADER_UNKNOWN                   ="UN";
const char * const VIL_DICOM_HEADER_UNSIGNEDSHORT             ="US";
const char * const VIL_DICOM_HEADER_UNLIMITEDTEXT             ="UT";

// Enumerations of all VR types
enum vil_dicom_header_vr_type
{
  vil_dicom_header_AE,
  vil_dicom_header_AS,
  vil_dicom_header_AT,
  vil_dicom_header_CS,
  vil_dicom_header_DA,
  vil_dicom_header_DS,
  vil_dicom_header_DT,
  vil_dicom_header_FD,
  vil_dicom_header_FL,
  vil_dicom_header_IS,
  vil_dicom_header_LO,
  vil_dicom_header_LT,
  vil_dicom_header_OB,
  vil_dicom_header_OW,
  vil_dicom_header_PN,
  vil_dicom_header_SH,
  vil_dicom_header_SL,
  vil_dicom_header_SQ,
  vil_dicom_header_SS,
  vil_dicom_header_ST,
  vil_dicom_header_TM,
  vil_dicom_header_UI,
  vil_dicom_header_UL,
  vil_dicom_header_UN,
  vil_dicom_header_US,
  vil_dicom_header_UT
};

// maps VR types to C++ types
template <int T /* enum vil_dicom_header_vr_type */>
struct vil_dicom_header_type_of
{
  typedef vcl_string type; // preferred C++ type for this VR (specialised below)
};

#define vr_macro( VR, CPPT ) \
  VCL_DEFINE_SPECIALIZATION struct  vil_dicom_header_type_of<VR> { typedef CPPT type; }

vr_macro( vil_dicom_header_AE, vcl_string );
vr_macro( vil_dicom_header_AS, vcl_string );
vr_macro( vil_dicom_header_AT, vcl_string );
vr_macro( vil_dicom_header_CS, vcl_string );
vr_macro( vil_dicom_header_DA, long );
vr_macro( vil_dicom_header_DS, float );
vr_macro( vil_dicom_header_FD, double );
vr_macro( vil_dicom_header_FL, float );
vr_macro( vil_dicom_header_IS, long );
vr_macro( vil_dicom_header_LO, vcl_string );
vr_macro( vil_dicom_header_LT, vcl_string );
vr_macro( vil_dicom_header_OB, vcl_string );
vr_macro( vil_dicom_header_OW, vcl_string );
vr_macro( vil_dicom_header_PN, vcl_string );
vr_macro( vil_dicom_header_SH, vcl_string );
vr_macro( vil_dicom_header_SL, vxl_sint_32 );
vr_macro( vil_dicom_header_SQ, vcl_string );
vr_macro( vil_dicom_header_SS, vxl_sint_16 );
vr_macro( vil_dicom_header_ST, vcl_string );
vr_macro( vil_dicom_header_TM, float );
vr_macro( vil_dicom_header_UI, vcl_string );
vr_macro( vil_dicom_header_UL, vxl_uint_32 );
vr_macro( vil_dicom_header_UN, vcl_string );
vr_macro( vil_dicom_header_US, vxl_uint_16 );
vr_macro( vil_dicom_header_UT, vcl_string );
#undef vr_macro


const vxl_uint_32 VIL_DICOM_HEADER_ALLSET                   = 0xffffffff;

// For determining the endian of the file or the transfer type (JPEG or RLE)
const char * const VIL_DICOM_HEADER_IMPLICITLITTLE            ="1.2.840.10008.1.2";
const char * const VIL_DICOM_HEADER_EXPLICITLITTLE            ="1.2.840.10008.1.2.1";
const char * const VIL_DICOM_HEADER_EXPLICITBIG               ="1.2.840.10008.1.2.2";
const char * const VIL_DICOM_HEADER_JPEGBASELINE_P1           ="1.2.840.10008.1.2.4.50";
const char * const VIL_DICOM_HEADER_JPEGDEFLOSSY_P2_4         ="1.2.840.10008.1.2.4.51";
const char * const VIL_DICOM_HEADER_JPEGEXTENDED_P3_5         ="1.2.840.10008.1.2.4.52";
const char * const VIL_DICOM_HEADER_JPEGSPECTRAL_P6_8         ="1.2.840.10008.1.2.4.53";
const char * const VIL_DICOM_HEADER_JPEGSPECTRAL_P7_9         ="1.2.840.10008.1.2.4.54";
const char * const VIL_DICOM_HEADER_JPEGFULLPROG_P10_12       ="1.2.840.10008.1.2.4.55";
const char * const VIL_DICOM_HEADER_JPEGFULLPROG_P11_13       ="1.2.840.10008.1.2.4.56";
const char * const VIL_DICOM_HEADER_JPEGLOSSLESS_P14          ="1.2.840.10008.1.2.4.57";
const char * const VIL_DICOM_HEADER_JPEGLOSSLESS_P15          ="1.2.840.10008.1.2.4.58";
const char * const VIL_DICOM_HEADER_JPEGEXTHIER_P16_18        ="1.2.840.10008.1.2.4.59";
const char * const VIL_DICOM_HEADER_JPEGEXTHIER_P17_19        ="1.2.840.10008.1.2.4.60";
const char * const VIL_DICOM_HEADER_JPEGSPECHIER_P20_22       ="1.2.840.10008.1.2.4.61";
const char * const VIL_DICOM_HEADER_JPEGSPECHIER_P21_23       ="1.2.840.10008.1.2.4.62";
const char * const VIL_DICOM_HEADER_JPEGFULLHIER_P24_26       ="1.2.840.10008.1.2.4.63";
const char * const VIL_DICOM_HEADER_JPEGFULLHIER_P25_27       ="1.2.840.10008.1.2.4.64";
const char * const VIL_DICOM_HEADER_JPEGLLESSHIER_P28         ="1.2.840.10008.1.2.4.65";
const char * const VIL_DICOM_HEADER_JPEGLLESSHIER_P29         ="1.2.840.10008.1.2.4.66";
const char * const VIL_DICOM_HEADER_JPEGLLESSDEF_P14_SV1      ="1.2.840.10008.1.2.4.70";
const char * const VIL_DICOM_HEADER_RLELOSSLESS               ="1.2.840.10008.1.2.5";

//: DICOM information read from the header
//
// This structure is filled when reading the header information from
// a dicom file. Each member relates to a field in the header part
// of the file.

struct vil_dicom_header_info
{
  // General info fields
  vil_dicom_header_type file_type_;        /*< The type of dicom file */
  vil_dicom_header_endian sys_endian_;     /*< The endian of the architecture */
  vil_dicom_header_endian file_endian_;    /*< The endian of the file being read */
  vil_dicom_header_image_type image_type_; /*< The encapsulated (or not) image type */

  // Identifying fields
  vil_dicom_header_type_of<vil_dicom_header_CS>::type image_id_type_;    /*< The image type from the dicom header */
  vil_dicom_header_type_of<vil_dicom_header_UI>::type sop_cl_uid_;       /*< The class unique id for the Service/Object Pair */
  vil_dicom_header_type_of<vil_dicom_header_UI>::type sop_in_uid_;       /*< The instance uid for the SOP */
  vil_dicom_header_type_of<vil_dicom_header_DA>::type study_date_;             /*< The date of the study */
  vil_dicom_header_type_of<vil_dicom_header_DA>::type series_date_;            /*< The date this series was collected */
  vil_dicom_header_type_of<vil_dicom_header_DA>::type acquisition_date_;       /*< The date of acquisition */
  vil_dicom_header_type_of<vil_dicom_header_DA>::type image_date_;             /*< The date of this image */
  vil_dicom_header_type_of<vil_dicom_header_TM>::type study_time_;            /*< The time of the study */
  vil_dicom_header_type_of<vil_dicom_header_TM>::type series_time_;           /*< The time of the series */
  vil_dicom_header_type_of<vil_dicom_header_TM>::type acquisition_time_;      /*< The time of acquisition */
  vil_dicom_header_type_of<vil_dicom_header_TM>::type image_time_;            /*< The time of the image */
  vil_dicom_header_type_of<vil_dicom_header_SH>::type accession_number_; /*< The accession number for this image */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type modality_;         /*< The imaging modality */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type manufacturer_;     /*< The name of the scanner manufacturer */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type institution_name_; /*< The name of the institution */
  vil_dicom_header_type_of<vil_dicom_header_ST>::type institution_addr_; /*< The address of the institution */
  vil_dicom_header_type_of<vil_dicom_header_PN>::type ref_phys_name_;    /*< The name of the referring physician */
  vil_dicom_header_type_of<vil_dicom_header_SH>::type station_name_;     /*< The name of the station used */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type study_desc_;       /*< A description of the study */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type series_desc_;      /*< A description of the series */
  vil_dicom_header_type_of<vil_dicom_header_PN>::type att_phys_name_;    /*< The name of the attending physician */
  vil_dicom_header_type_of<vil_dicom_header_PN>::type operator_name_;    /*< The name of the MR operator */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type model_name_;       /*< The name of the MR scanner model */

  // Patient info
  vil_dicom_header_type_of<vil_dicom_header_PN>::type patient_name_;  /*< Patient's name */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type patient_id_;    /*< Patient's ID */
  vil_dicom_header_type_of<vil_dicom_header_DA>::type patient_dob_;         /*< The patient's date of birth */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type patient_sex_;   /*< The sex of the patient */
  vil_dicom_header_type_of<vil_dicom_header_AS>::type patient_age_;   /*< The age of the patient */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type patient_weight_;     /*< The weight of the patient */
  vil_dicom_header_type_of<vil_dicom_header_LT>::type patient_hist_;  /*< Any additional patient history */

  // Acquisition Info
  vil_dicom_header_type_of<vil_dicom_header_CS>::type scanning_seq_;  /*< A description of the scanning sequence */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type sequence_var_;  /*< A description of the sequence variant */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type scan_options_;  /*< A description of various scan options */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type mr_acq_type_;   /*< The acquisition type for this scan */
  vil_dicom_header_type_of<vil_dicom_header_SH>::type sequence_name_; /*< The name of the sequence */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type angio_flag_;    /*< The angio flag for this sequence */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type slice_thickness_;    /*< Slice thickness (for voxel size) */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type repetition_time_;    /*< Scan repetition time */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type echo_time_;          /*< Scan echo time */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type inversion_time_;     /*< Scan inversion time */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type number_of_averages_; /*< The number of averages for this scan */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type echo_numbers_;         /*< The echo numbers for this scan */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type mag_field_strength_; /*< The strength of the magnetic field */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type echo_train_length_;    /*< The length of the echo train */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type pixel_bandwidth_;    /*< The bandwidth of the pixels */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type software_vers_; /*< Versions of the scanner software used */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type protocol_name_; /*< The name of the protocol used */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type heart_rate_;           /*< The patient's heart rate */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type card_num_images_;      /*< The cardiac number of images */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type trigger_window_;       /*< The trigger window for this image */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type reconst_diameter_;   /*< The reconstruction diameter */
  vil_dicom_header_type_of<vil_dicom_header_SH>::type receiving_coil_;/*< Details of the receiving coil */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type phase_enc_dir_; /*< The phase encoding direction */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type flip_angle_;         /*< The flip angle */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type sar_;                /*< The specific absorption rate */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type patient_pos_;   /*< The position of the patient in the scanner */

  // Relationship info
  vil_dicom_header_type_of<vil_dicom_header_UI>::type stud_ins_uid_;  /*< The study instance unique id */
  vil_dicom_header_type_of<vil_dicom_header_UI>::type ser_ins_uid_;   /*< The series instance unique id */
  vil_dicom_header_type_of<vil_dicom_header_SH>::type study_id_;      /*< The id of this study */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type series_number_;        /*< The number of this series */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type acquisition_number_;   /*< The number of the acquisition */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type image_number_;         /*< The number of this image instance */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type pat_orient_;    /*< The orientation of the patient */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type image_pos_;     /*< The image position relative to the patient */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type image_orient_;  /*< The image orientation relative to the patient */
  vil_dicom_header_type_of<vil_dicom_header_UI>::type frame_of_ref_;  /*< The frame of reference */
  vil_dicom_header_type_of<vil_dicom_header_IS>::type images_in_acq_;        /*< Then number ot images in the acquisition */
  vil_dicom_header_type_of<vil_dicom_header_LO>::type pos_ref_ind_;   /*< The position reference indicator */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type slice_location_;     /*< The location of the slice */

  // Image info
  vil_dicom_header_type_of<vil_dicom_header_US>::type pix_samps_; /*< The number of samples per pixel */
  vil_dicom_header_type_of<vil_dicom_header_CS>::type photo_interp_;  /*< The photometric interpretation */
  vil_dicom_header_type_of<vil_dicom_header_US>::type size_x_;      /*< The number of columns */
  vil_dicom_header_type_of<vil_dicom_header_US>::type size_y_;      /*< The number of rows */
  vil_dicom_header_type_of<vil_dicom_header_US>::type size_z_;      /*< The number of planes */
  vil_dicom_header_type_of<vil_dicom_header_US>::type high_bit_;  /*< The bit used as the high bit */
  vil_dicom_header_type_of<vil_dicom_header_US>::type small_im_pix_val_;   /*< The smallest image pixel value */
  vil_dicom_header_type_of<vil_dicom_header_US>::type large_im_pix_val_;   /*< The largest image pixel value */
  vil_dicom_header_type_of<vil_dicom_header_SS>::type pixel_padding_val_;  /*< The value used for padding pixels */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type window_centre_;      /*< The value of the image window's centre */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type window_width_;       /*< The actual width of the image window */

  // Info from the tags specifically for reading the image data
  vil_dicom_header_type_of<vil_dicom_header_DS>::type spacing_x_;                   /*< The pixel spacing in x */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type spacing_y_;                   /*< The pixel spacing in y */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type spacing_slice_;           /*< The pixel spacing in z */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type res_intercept_;           /*< The image rescale intercept */
  vil_dicom_header_type_of<vil_dicom_header_DS>::type res_slope_;               /*< The image rescale slope */
  vil_dicom_header_type_of<vil_dicom_header_US>::type pix_rep_;        /*< The pixel representation (+/-) */
  vil_dicom_header_type_of<vil_dicom_header_US>::type stored_bits_;    /*< The bits stored */
  vil_dicom_header_type_of<vil_dicom_header_US>::type allocated_bits_; /*< The bits allocated */
};


//: Clears a header info struct
// \relates vil_dicom_header_info
void vil_dicom_header_info_clear( vil_dicom_header_info& info );


const short VIL_DICOM_HEADER_UNSPECIFIED = -1;
const unsigned short VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED = (unsigned short) -1;
const double VIL_DICOM_HEADER_DEFAULTSIZE = 1.0;
const float VIL_DICOM_HEADER_DEFAULTSIZE_FLOAT = 1.0f;
const float VIL_DICOM_HEADER_DEFAULTINTERCEPT = 0.0f;
const float VIL_DICOM_HEADER_DEFAULTSLOPE = 1.0f;

//: A class to read and write the header part of a dicom file
//
//  This class reads and writes the header section of a dicom file,
//  determining the type and storing the information in a dicom
//  structure.
class vil_dicom_header_format
{
 public:

  vil_dicom_header_format();
 ~vil_dicom_header_format();

  //: True if it is known DICOM format
  bool isDicomFormat(vil_stream &);

  //: Read the dicom header into a header struct
  //  Reads the header section of the dicom file and stores the
  //  result in a header struct, which it returns. When finished,
  // the file stream should be pointing at the start of the image
  // data
  // \param fs The file stream to read - on exit points to the start of the image data
  // \return A dicom header struct containing the info gained from the header
  // \sa lastHeader(), headerValid(), last_read_() and info_valid_()
  vil_dicom_header_info readHeader(vil_stream &fs);

  //: Method to return the last struct of header info set
  //
  //  The class holds a dicom header info struct containing the
  //  info from the last dicom file read.
  //  \return A dicom header struct containing the info gained from the last header read
  vil_dicom_header_info lastHeader(void);

  //: Method to indicate if the header info held is valid or not
  //
  //  \return TRUE if the header info is valid, otherwise false
  bool headerValid(void);

  //: Return the current system endian
  //
  //  \return The system endian for the architecture running the application
  vil_dicom_header_endian systemEndian(void);

  //: Return the endian of the header being read
  //
  //  \return The endian of the file currently being read.
  //  \sa determineMetaInfo() and file_endian_()

  vil_dicom_header_endian fileEndian(void);

  //: Return any encapsulated image type
  //
  //  \return The encapsulated image type of the file currently being read.
  vil_dicom_header_image_type imageType(void);

 private:

  //: Method to determine the file type
  //
  //  \param fs The file stream for the image to test
  //  \return The dicom file type or unknown
  vil_dicom_header_type determineFileType(vil_stream &fs);

  //: Method to read the elements from the header
  //
  //  This method fills the last_read_ header structure with
  //  the necessary fields
  //  \param fs The file stream to read from
  //  \sa readIdentifyingElements(), readPatientElements(),
  //      readAcquisitionElements(), readRelationshipElements(),
  //      readImageElements, readDelimiterElements(),
  //      readHeader() and last_read_()
  void readHeaderElements(vil_stream &fs);

  //: Method to read the identifying group's details
  //
  //  Fills the identifying details in the last_read_ structure
  //  from the file header
  //  \param element The element being read in the identifying
  //                 group
  //  \param dblock_size The size of the data block for this
  //                     element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readPatientElements(),
  //      readAcquisitionElements(), readRelationshipElements(),
  //      readImageElements(), readDelimiterElements() and last_read_()
  void readIdentifyingElements(short element, int dblock_size,
                               vil_stream &fs);

  //: Method to read the patient details
  //
  //  Fills the patient details in the last_read_ structure from
  //  the file header
  //  \param element The element being read in the patient group
  //  \param dblock_size The size of the data block for this
  //                     element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readIdentifyingElements(),
  //      readAcquisitionElements(), readRelationshipElements(),
  //      readImageElements(), readDelimiterElements() and last_read_()
  void readPatientElements(short element, int dblock_size,
                           vil_stream &fs);

  //: Method to read the acquisition group's details
  //
  //  Fills the acquisition details in the last_read_ structure
  //  from the file header
  //  \param element The element being read in the acquisition
  //                 group
  //  \param dblock_size The size of the data block for this
  //                     element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readIdentifyingElements(),
  //      readPatientElements(), readRelationshipElements(),
  //      readImageElements(), readDelimiterElements() and last_read_()
  void readAcquisitionElements(short element, int dblock_size,
                               vil_stream &fs);

  //: Method to read the relationship group's details
  //
  //  Fills the relationship details in the last_read_ structure
  //  from the file header
  //  \param element The element being read in the relationship
  //                 group
  //  \param dblock_size The size of the data block for this
  //                     element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readIdentifyingElements(),
  //      readPatientElements(), readAcquisitionElements(),
  //    readImageElements(), readDelimiterElements()
  //    and last_read_()
  void readRelationshipElements(short element, int dblock_size,
                                vil_stream &fs);

  //: Method to read the image group's details
  //
  //  Fills the image details in the last_read_ structure from
  //  the file header
  //  \param element The element being read in the image group
  //  \param dblock_size The size of the data block for this
  //                     element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readIdentifyingElements(),
  //      readPatientElements(), readAcquisitionElements(),
  //    readRelationshipElements(), readDelimiterElements()
  //    and last_read_()
  void readImageElements(short element, int dblock_size,
                         vil_stream &fs);

  //: Method to read the delimiter group's details
  //
  //  Certain field in the delimiter group have no data block, and
  //  so should not be skipped. This method takes the appropriate
  //  action - to skip or not to skip
  //  \param element The element being read in the delimiter group
  //  \param dblock_size The size of the data block for this element
  //  \param fs The file stream to read from
  //  \sa readHeaderElements(), readIdentifyingElements(),
  //      readPatientElements(), readAcquisitionElements(),
  //    readRelationshipElements(), readImageElements()
  //    and last_read_()
  void readDelimiterElements(short element, int dblock_size,
                             vil_stream &fs);

  //: Method to convert the Value Representation (VR) (if it exists)
  //
  //  \param data_block Contains the VR or data block size (always
  //                    holds the data block size on exit
  //  \param fs The file stream to read from
  //  \sa readHeaderElements()
  bool convertValueRepresentation(unsigned int &dblock_size,
                                  vil_stream &ifs);

  //: Method to determine whether or not the actual pixel data has been reached
  //
  //  \param group The current group to test
  //  \param element The element within that group
  //  \return TRUE if it is the pixel data, otherwise false
  //  \sa readHeaderElements()
  bool pixelDataFound(short group, short element);

  // Method to clear a header info struct
  //
  //  \sa last_read_() and info_valid_()
  void clearInfo(void);

  //: Work out whether the current architecture is big or little endian
  //
  //  \return The system endian value calculated
  //  \sa systemEndian() and endian_()
  vil_dicom_header_endian calculateEndian(void);

  //: Initialise all the necessary meta-file stuff
  //
  //  \return The endian of the file
  //  \sa fileEndian(), file_endian_(), imageType(), and image_type_();
  vil_dicom_header_endian determineMetaInfo(vil_stream &fs);

  //: Method to byte swap an unsigned short int if necessary
  //
  //  \param short_in The unsigned short to swap
  //  \return The swapped unsigned short
  //  \sa intSwap() and charSwap()
  vxl_uint_16 shortSwap(vxl_uint_16 short_in);

  //: Method to byte swap an unsigned int if necessary
  //
  //  \param int_in The unsigned int to swap
  //  \return The swapped unsigned int
  //  \sa shortSwap() and charSwap()
  vxl_uint_32 intSwap(vxl_uint_32 int_in);

  //: Method to swap a number inside a char array
  //
  //  \param char_in The char array to swap - on exit contains the result
  //  \param val_size The size of the var to swap
  void charSwap(char *char_in, int val_size);

 private:

  //: Holds the info from the last header read
  //
  //  \sa readHeader(), lastHeader(), headerValid(), clearInfo(),
  //      readHeaderElements() and info_valid_()
  vil_dicom_header_info last_read_;

  //: TRUE if the header info is valid (i.e. has been read) otherwise FALSE
  //
  // \sa readHeader(), lastHeader(), headerValid(), clearInfo() and last_read_()
  bool info_valid_;

  //: A variable to hold the discovered architecture endian
  //
  //  \sa calculateEndian() and systemEndian()
  vil_dicom_header_endian endian_;

  //: A variable to hold the current header file's endian
  //
  //  \sa determineMetaInfo() and fileEndian()
  vil_dicom_header_endian file_endian_;

  //: A variable to hole the current encapsulate file type (if any)
  //
  //    \sa determineMetaInfo() and imageType()
  vil_dicom_header_image_type image_type_;
};

void vil_dicom_header_print(vcl_ostream &os, const vil_dicom_header_info &s);

#endif // vil_dicom_header_h_
