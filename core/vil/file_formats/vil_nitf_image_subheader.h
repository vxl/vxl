// This is core/vil/file_formats/vil_nitf_image_subheader.h
#ifndef vil_nitf_image_subheader_h_
#define vil_nitf_image_subheader_h_
//:
// \file
// \brief Base class for all image subheaders.
//  Extracted from NITFHeader.h from TargetJr.
//
//  \date: 2003/12/26
//  \author: mlaymon

#include <vcl_string.h>
#include <vcl_vector.h>

#include <vil/vil_pixel_format.h>

#include "vil_nitf_typeinfo.h"
#include "vil_nitf_header.h"
#include "vil_nitf_extended_header.h"

//====================================================================
// The Image Subheader
//====================================================================

struct vil_nitf_image_subheader_band
{
    char* ITYPE;
    char* IFC;
    char* IMFLT;
    unsigned int   NLUTS;
    int   NELUT;
    unsigned char** LUTD;               // will be uchar[NLUTS][NELUT].
};

class vil_nitf_image_subheader : public vil_nitf_header
{
 public:

  // NOTE: GEOCENTRIC is valid only for version 2.0
  //       UTMN, UTMS and DECIMAL_DEG are valid only for version 2.1
  enum Geo {NONE=0, GEOCENTRIC, GEOGRAPHIC, UTM, UTMN, UTMS, DECIMAL_DEG};

  vil_nitf_image_subheader();
  vil_nitf_image_subheader (const vil_nitf_image_subheader& header);
  virtual ~vil_nitf_image_subheader();

  bool get_rational_camera_data (
      vcl_vector<double>& samp_num,
      vcl_vector<double>& samp_denom,
      vcl_vector<double>& line_num,
      vcl_vector<double>& line_denom,
      vcl_vector<double>& scalex,
      vcl_vector<double>& scaley,
      vcl_vector<double>& scalez,
      vcl_vector<double>& scales,
      vcl_vector<double>& scalel,
      vcl_vector<double>& init_pt,
      vcl_vector<double>& rescales,
      vcl_vector<double>& rescalel,
      int scale_index,
      int offset_index) const;

  bool get_image_corners(vcl_vector<double>& UL,
                         vcl_vector<double>& UR,
                         vcl_vector<double>& LR,
                         vcl_vector<double>& LL) const;

  bool GetImageBounds(
      double& lat_min, double& lat_max,
      double& lon_min, double& lon_max,
      double* lats = NULL,
      double* lons = NULL
      ) const;

  // NOTE: TargetJr usec RectRoi as first parameter.  Initially, I used
  // vgl_box_2d<int> typedef'd as vil_nitf_rect_roi.  However vil classes
  // cannot depend on vgl, so I changed type to vcl_vector<int>.
  void EncodeICHIPB (vcl_vector<int>& roi, int f1_row, int f1_col);

  char*  IM;         // File Part Type
  char*& IID;        // Image ID
  char*& IDATIM_;    // Image Date and Time stored in format DDhhmmssZMMMYY where Z = time zone.
  char*  TGTID;      // Target ID  (BE # of primary targets)
  char*& ITITLE;
  NITFClass& ISCLAS; // Image Security Classification
  char*& ISCODE;     // Image Codewords
  char*& ISCTLH;     // Image Control and Handling
  char*& ISREL;      // Image Releasing Instructions
  char*& ISCAUT;     // Image Classification Authority
  char*& ISCTLN;     //  Not used in NITFV2.1
  char*& ISDWNG;     //  Not used in NITFV2.1
  char*& ISDEVT;     //  Not used in NITFV2.1
     // ENCRYP in vil_nitf_header is used here.
  char*  ISORCE;     // Image Source

  Geo    ICORDS;  //!< Code for coordinate system.
                  // Valid values:. v2.0 - C = geocentric, G = geographic, U = UTM, N = none".
                  // v2.1 - D = decimal degree, G = geographic,
                  // U = UTM Miltiary grid reference system (MGRS),
                  // N = UTM/UPS northern hemisphere, S = UTM/UPS southern hemisphere,
                  // blank = none

  char*  IGEOLO;  //!< String of length 60 containing rough coordinates of four corners of image in order
                  // (0,0), (0, maxCol), (maxRow, maxCol), (maxRow, 0).
                  // Order should be upper left, upper right, lower right, lower left.

  // int    NICOM;               // This does not exist; use the
                                 // size of ICOM instead.
  vcl_vector<vcl_string> ICOM_;

  virtual bool IsCompressed() const;
  char*   IC;
  char*   COMRAT;

  unsigned     NBANDS;  /// Number of bands
  vil_nitf_image_subheader_band** bands;

  int     ISYNC;

  // Storage information...
  //
  unsigned int NCOLS;  // Columns of valid Image data.
  unsigned int NROWS;  // Rows of valid Image data.

  vil_pixel_format PVTYPE_;  // Pixel type.  NOTE: NOT the same as bits per pixel.

  char* IREP;  /// Image representation (4 characters (+ 4 blanks in file))
                //      p. 37 of NITF v2.0 specification
  char* ICAT;  /// Image category (3 characters). p. 37 of NITF v2.0 specification
                //  Valid values: VIS = visible imagery, MAP = maps,
                //      SAR = Synthetic Aperature Radar
                //      IR = infrared
                //      MS = multi-spectral
  unsigned ABPP;  // Actual bits per pixel per band. (p. 64 of 2005BChangeNotice2)
  unsigned int NBPP;  //<: Storage bits per compent for pixel vector (Provided image compression (IC) == NC, NM, C4, M4)
              // Range: 1-64
              // If IC = C3, NBPP == 8 or 12. If IC = C3, NBPP == 8 or 11.
              //
              // (value must be >= ABPP ) [original comment]
              // MAL 16oct2003 NBPP = ABPP * NBANDS ?
              //
              // Line 2057, NITFImage.C:
              // SetBitsPixel(getHeader()->ABPP * getHeader()->NBANDS);
              //
              // (p. 65 of 2005BChangeNotice2)
              // This seems redundant, since NBPP should be able to
              // be calculated from ABPP and NBANDS.  However, code
              // tests to see that NBPP <= 16 and ABPP <=32.  This
              // seems backward.  Section 5.4.3.3.1, p. 65 of document
              // 2005BChangeNotice2 for a discussion.


  InterleaveType IMODE_;          // Storage order of pixels.

  unsigned NBPR;           // Blocks per row (x blocks).
  unsigned NBPC;           // Blocks per column (y blocks).
  unsigned NPPBH;          // Block size, pixels horizontal.
  unsigned NPPBV;          // Block size, pixels vertical.

  PixelJustify   PJUST;          // Pixel justification, LEFT or RIGHT.

  // ALVL, DLVL, LOCrow and LOCcolumn are in vil_nitf_header.
  char* IMAG;  //Image magnification

  int   UDIDL_; // User defined image data length
  char * UDID_;  // User defined image data

  vil_nitf_extended_subheader* XSHD;

  // PMP added this good stuff (8/14/97)
  // I2MAPD (IDEX's own home-grown tag) information
  bool           I2MAPD_present; //true if I2MAPD present and valid
  int            RRDSset;        // reduced resolution data set (RRDS)
  int            ULl,ULs;        // upper left corner (line,sample)
  int            URl,URs;        // upper right corner (line,sample)
  int            LLl,LLs;        // lower left corner (line,sample)
  int            LRl,LRs;        // lower right corner (line,sample)
  // RPC (Rational Polynomial Coefficient) information
  bool        RPC_present;       // True if RPC data present and valid
  rpc_type       RPC_TYPE;       // What kind of RPC tag is this?
  double         ERR_BIAS;       // error bias (used in CE predicts)
  double         ERR_RAND;       // error random (used in CE predicts)
  double         LINE_OFF;       // line offset (for un-normalization)
  double         SAMP_OFF;       // sample offset (for un-normalization)
  double         LAT_OFF;        // latitude offset (for normalization)
  double         LONG_OFF;       // longitude offset (for normalization)
  double         HEIGHT_OFF;     /// height offset (for normalization)
  double         LINE_SCALE;     // line scale (for un-normalization)
  double         SAMP_SCALE;     // sample scale (for un-normalization)
  double         LAT_SCALE;      // latitude scale (for normalization)
  double         LONG_SCALE;     // longitude scale (for normalization)
  double         HEIGHT_SCALE;   // height scale (for normalization)
    // Arrays for numerator/denominator rational cubics...
  double         LINE_NUM[20];   // numerator of line ratpoly
  double         LINE_DEN[20];   // denominator of line ratpoly
  double         SAMP_NUM[20];   // numerator of sample ratpoly
  double         SAMP_DEN[20];   // denominator of sample ratpoly

  // Added for anamorphic images. GWB 11/8/99
  int            ANAMRPH[2];

  // GWB added this for the ICHIPx SDE (2/18/99)
  // See ICHIPB Definition for full explanation of each parameter.
  // ("ICHIPB Support Data Extension for the NITF Standard", v1.0, 11/16/98)
  bool        ICHIPB_present; // True if the image has an ICHIP TRE.

  int         XFRM_FLAG;     // Non-linear Transform flag
  float       SCALE_FACTOR;   // Scale Factor Relative to R0
  int         ANAMRPH_CORR;   // Anamorphic Correction Indicator
  int         SCANBLK_NUM;    // Scan block number
  float       OP_ROW_11;      // Output Product Row # for grid pt. (1,1)
  float       OP_COL_11;      // Output Product Col # for grid pt. (1,1)
  float       OP_ROW_12;      // Output Product Row # for grid pt. (1,2)
  float       OP_COL_12;      // Output Product Col # for grid pt. (1,2)
  float       OP_ROW_21;      // Output Product Row # for grid pt. (2,1)
  float       OP_COL_21;      // Output Product Col # for grid pt. (2,1)
  float       OP_ROW_22;      // Output Product Row # for grid pt. (2,2)
  float       OP_COL_22;      // Output Product Col # for grid pt. (2,2)
  float       FI_ROW_11;      // Full Image Row # for grid pt. (1,1)
  float       FI_COL_11;      // Full Image Col # for grid pt. (1,1)
  float       FI_ROW_12;      // Full Image Row # for grid pt. (1,2)
  float       FI_COL_12;      // Full Image Col # for grid pt. (1,2)
  float       FI_ROW_21;      // Full Image Row # for grid pt. (2,1)
  float       FI_COL_21;      // Full Image Col # for grid pt. (2,1)
  float       FI_ROW_22;      // Full Image Row # for grid pt. (2,2)
  float       FI_COL_22;      // Full Image Col # for grid pt. (2,2)
  int         FI_ROW;         // Full Image number of rows.
  int         FI_COL;         // Full Image number of columns.

  // MPP added this for the PIAIMC SDE (4/12/2001)
  // See PIAIMC Definition for full explanation of each parameter.
  // ("STDI-002", v2.0, 4 March 1999, p. 30)
  bool        PIAIMC_present; // True if the image has a PIAIMC TRE.

  int         CLOUDCVR;       // Cloud Cover
  bool        SRP;            // Standard Radiometric Product
  char*       SENSMODE;       // Sensor Mode
  char*       SENSNAME;       // Sensor Name
  char*       SOURCE;         // Image Source
  int         COMGEN;         // Compression Generation
  char        SUBQUAL;        // Subjective Quality
  char*       PIAMSNNUM;      // PIA Mission Number
  char*       CAMSPECS;       // Camera Specs
  char*       PROJID;         // Project ID Code
  int         GENERATION;     // Generation
  bool        ESD;            // Exploitation Support Data
  char*       OTHERCOND;      // Other Conditions
  float       MEANGSD;        // Mean GSD
  char*       IDATUM;         // Image Datum
  char*       IELLIP;         // Image Ellipsoid
  char*       PREPROC;        // Image Proccessing Level Code
  char*       IPROJ;          // Image Projection System
  int         SATTRACK_PATH;  // Satellite Track PATH(J)
  int         SATTRACK_ROW;   // Satellite Track ROW(K)

  // Accessor for PIAIMC cloud cover percentage
  virtual int GetCloudCoverPercentage() const;

  // GWB: Added this for handling the STDIDx fields
  bool        STDID_present;   // This is true is the tag is present
  char*       ACQUISITION_DATE;// Acquisition Date
  char*       MISSION;         // Mission ID
  char*       PASS;            // Pass Number
  int         OP_NUM;          // Image Operation Number
  char*       START_SEGMENT;   // Start Segment ID
  int         REPRO_NUM;       // Reprocess Number
  char*       REPLAY_REGEN;    // Replay/Regen
  char*       BLANK_FILL;      // Blank or _
  int         START_COLUMN;    // Starting Column Block
  int         START_ROW;       // Starting Row Block
  char*       END_SEGMENT;     // Ending Segment ID
  int         END_COLUMN;      // Ending Column Block
  int         END_ROW;         // Ending Row Block
  char*       COUNTRY;         // Country Code
  int         WAC;             // Worl Aeronautical Chart number
  char*       LOCATION;        // Location
  char*       RESERVED_1;      // Reserved (5 spaces)
  char*       RESERVED_2;      // Reserved (8 spaces)

  virtual vil_nitf_header* Copy();
  virtual void Copy (const vil_nitf_image_subheader*);
  const   vil_nitf_image_subheader& operator= (const vil_nitf_image_subheader&);

  // FIGURE OUT HOW TO PASS vcl_out.
  //void display_attributes (vcl_ostream out) const;
  void display_attributes (vcl_string caller) const;
  void display_size_attributes (vcl_string caller) const;
  void display_camera_attributes (vcl_string caller) const;
};

inline const vil_nitf_image_subheader& vil_nitf_image_subheader::operator= (
    const vil_nitf_image_subheader& header)
{
  Copy(&header);
  return *this;
}

typedef vil_smart_ptr<vil_nitf_image_subheader> vil_nitf_image_subheader_sptr;

#endif  // end vil_nitf_image_subheader_h_
