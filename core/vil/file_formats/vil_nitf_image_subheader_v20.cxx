// This is core/vil/file_formats/vil_nitf_image_subheader_v20.cxx
#include "vil_nitf_image_subheader_v20.h"
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief This file implements the NITF image subheader for NITF v2.0.
// NOTE: Ported from TargetJr ImageSubHeader_v20. (Code in file NITFV20Header.C)
//
// \date: 2003/12/26
// \author: mlaymon

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_ctime.h>
#include <vcl_iostream.h>
#include <vcl_ios.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>

#include "vil_nitf_util.h"
#include "vil_nitf_version_v20.h"
#include "vil_nitf_header_v20.h"

static
inline float nitf_atoff( const char* s )
{
  return static_cast<float>( vcl_atof(s) );
}

static const vcl_string VALID_ICORDS_CODES = "CGNUcgnu";

vil_nitf_image_subheader_v20::vil_nitf_image_subheader_v20()
{
    init();
}

//====================================================================
//: Builds a Version 2.0 vil_nitf_image_subheader from an arbitrary header.
//====================================================================
vil_nitf_image_subheader_v20::vil_nitf_image_subheader_v20(const vil_nitf_image_subheader& h)
{
    init();
    Copy(&h);
}

//====================================================================
//: Copy constructor for vil_nitf_image_subheader_v20.
//====================================================================
vil_nitf_image_subheader_v20::vil_nitf_image_subheader_v20(const vil_nitf_image_subheader_v20& h)
    : vil_nitf_image_subheader(h)
{
    init();
    FilledCopy(IDATIM_, GetVersion20Date());
}

vil_nitf_image_subheader_v20::~vil_nitf_image_subheader_v20()
{
    // These must not be cleared here --
    // now they belong to the vil_nitf_image_subheader class
    // delete IREP;
    // delete ICAT;
}

bool vil_nitf_image_subheader_v20::IsCompressed() const
{
    bool rval = false;
    if (!vcl_strcmp(IC, "C0") || !vcl_strcmp(IC, "C1") ||
        !vcl_strcmp(IC, "C2") || !vcl_strcmp(IC, "C3")) {
        rval = true;
    }
    return rval;
}

int vil_nitf_image_subheader_v20::GetCloudCoverPercentage() const
{
        // By default, we don't know what the cloud cover % is
    int pct = 999;

        // If a PIAIMC TRE is defined for this image...
        if (PIAIMC_present) {
                // Fetch the PIAIMC CLOUDCVR field value
          pct = CLOUDCVR;
        }

    return pct;
}

//: Read image subheader from vil_stream.
//
// \param file input stream
// \return StatusCode == STATUS_GOOD if success

StatusCode vil_nitf_image_subheader_v20::Read(vil_stream* file)
{
    static vcl_string method_name = "vil_nitf_image_subheader_v20::Read: ";

    if (!file || !file->ok()) {
      return STATUS_BAD;
    }

    int curr_pos = file->tell();

    vcl_cout << "\n#### enter " << method_name
             << "file position = " << curr_pos << vcl_endl;

    char buffer[6];  // Max size of any numeric field is 5.

    bool error = false;
    bool done  = false;

    while (!done)
    {
        buffer[1] = 0;  // For when ISCLAS is read in.

        if ((file->read(IM,      2) <  2) ||
            (vcl_strcmp(IM, "IM"))                 ||
            (file->read(IID,    10) < 10) ||
            (file->read(IDATIM_, 14) < 14) ||
            (file->read(TGTID,  17) < 17) ||
            (file->read(ITITLE, 80) < 80) ||
            (file->read(buffer,  1) <  1) ||
            (!ValidClassification(buffer))) {
            error = true;
            break;
        }
        ISCLAS = ConvertClassification(buffer);

        buffer[1] = 0;                // For when ENCRYP is read in.

        if ((file->read(ISCODE, 40) < 40) ||
            (file->read(ISCTLH, 40) < 40) ||
            (file->read(ISREL,  40) < 40) ||
            (file->read(ISCAUT, 20) < 20) ||
            (file->read(ISCTLN, 20) < 20) ||
            (file->read(ISDWNG,  6) <  6) ||
            (!vcl_strcmp(ISDWNG, "999998") &&
             file->read(ISDEVT, 40) < 40) ||
            (file->read(buffer,  1) <  1) ||
            (!vcl_strpbrk(buffer, "01")))
        {
            error = true;
            break;
        }
        ENCRYP = buffer[0] == '0' ? NOTENCRYPTED : ENCRYPTED;

        if ((file->read(ISORCE, 42) < 42) ||
            !get_unsigned(buffer, &NROWS, 8, file)   ||
            !get_unsigned(buffer, &NCOLS, 8, file)) {
          error = true;
          break;
        }

        if (file->read(buffer,  3) < 3) {
          error = true;
          break;
        }
        buffer[3] = '\0';
        vcl_cout << method_name << "PVTYPE string = <" << buffer << ">\n";

  // MAKE SURE FLOAT AND COMPLEX BELOW SHOULD NOT BE DOUBLE.  MAL 8oct2003
        if (!nitf_strcasecmp(buffer, "B  "))  // B = bit-mapped. TargetJr used PVTYPE_INTEGER
          PVTYPE_ = VIL_PIXEL_FORMAT_BOOL;
        else if (!nitf_strcasecmp(buffer, "C  "))  // C = complex (2 32 bit values)
          PVTYPE_ = VIL_PIXEL_FORMAT_COMPLEX_FLOAT;  // not COMPLEX_DOUBLE !
        else if (!nitf_strcasecmp(buffer, "INT"))
          PVTYPE_ = VIL_PIXEL_FORMAT_UINT_16;  // INT and SI are 16 bit values
        else if (!nitf_strcasecmp(buffer, "R  "))
          PVTYPE_ = VIL_PIXEL_FORMAT_FLOAT;  // 32 bit IEEE floating point number.
        else if (!nitf_strcasecmp(buffer, "SI "))
            PVTYPE_ = VIL_PIXEL_FORMAT_INT_16;  // INT and SI are 16 bit values
        else if (!nitf_strcasecmp(buffer, "U  "))
            PVTYPE_ = VIL_PIXEL_FORMAT_UNKNOWN;
        else
        {
            error = true;
            break;
        }

        if (file->read(IREP, 8) < 8) {
            error = true;
            break;
        }

        vcl_cout << method_name
                 << "IREP string = <" << IREP << ">  file position = "
                 << file->tell() << vcl_endl;

        if (nitf_strcasecmp(IREP, "MONO    ") &&
            nitf_strcasecmp(IREP, "RGB     ") &&
            nitf_strcasecmp(IREP, "NODISPLY") &&
            nitf_strcasecmp(IREP, "RGB/LUT ") &&
            nitf_strcasecmp(IREP, "MULTI   ") &&
            nitf_strcasecmp(IREP, "YCbCr601")) {
          error = true;
          break;
        }

        if (file->read(ICAT, 8) < 8) {
            error = true;
            break;
        }
        vcl_cout << method_name
                 << "ICAT string = <" << ICAT << ">  file position = "
                 << file->tell() << vcl_endl;

        if (nitf_strcasecmp(ICAT, "VIS     ") &&
            nitf_strcasecmp(ICAT, "MAP     ") &&
            nitf_strcasecmp(ICAT, "SAR     ") &&
            nitf_strcasecmp(ICAT, "IR      ") &&
            nitf_strcasecmp(ICAT, "FP      ") &&
            nitf_strcasecmp(ICAT, "MRI     ") &&
            nitf_strcasecmp(ICAT, "XRAY    ") &&
            nitf_strcasecmp(ICAT, "CAT     ") &&
            nitf_strcasecmp(ICAT, "MS      ") &&
            vcl_strcmp(ICAT, "        ")) {
          error = true;
          break;
        }

        if (!get_unsigned(buffer, &ABPP, 2, file) || (ABPP < 1 || ABPP > 32)) {
          error = true;
          break;
        }
        buffer[2] = '\0';
        vcl_cout << method_name
                 << "ABPP string = <" << buffer << ">  file position = "
                 << file->tell() << vcl_endl;

        if (file->read(buffer, 1) < 1) {
          error = true;
          break;
        }
        if (buffer[0] == 'L' || buffer[0] == 'l')
            PJUST = PJUST_LEFT;
        else
            PJUST = PJUST_RIGHT;

        buffer[1] = '\0';  // For ICORDS value.

        // Read single character for ICORDS code.  Check against valid values.
        // If value is C, G, or U, read in 60 characters for IGEOLO field.

        if (file->read(buffer, 1) <  1) {
            error = true;
            break;
        }
        vcl_cout << method_name << "ICORDS string = <" << buffer << ">\n";

        switch (buffer[0])
        {
            case 'c':
            case 'C': ICORDS = GEOCENTRIC; break;
            case 'g':
            case 'G': ICORDS = GEOGRAPHIC; break;
            case 'n':
            case 'N': ICORDS = NONE;       break;
            case 'u':
            case 'U': ICORDS = UTM;        break;

            default:
              error = true;
              break;
        }
        if (ICORDS != NONE) {
          if (file->read(IGEOLO, 60) < 60) {
              error = true;
              break;
          }
        }

        unsigned int NICOM;
        if (!get_unsigned(buffer, &NICOM, 1, file))
        {
            error = true;
            break;
        }
        vcl_cout << method_name
                 << "NICOM string = <" << buffer << ">  file position = "
                 << file->tell() << vcl_endl;

        ICOM_.clear();

        char tmpc[81];
        tmpc[80] = 0;
        vcl_string temp_str;

        vcl_cout << method_name << "clear ICOM_ OK.\n";

        for (unsigned int n = 0; n < NICOM; n++)
        {
            if (file->read(tmpc, 80) < 80)
            {
                error = true;
                break;
            }
            vcl_cout << method_name
                     << "ICOM[" << n << "] = <" << tmpc << ">  file position = "
                     << file->tell() << vcl_endl;
            temp_str = tmpc;
            ICOM_.push_back(temp_str);
        }

        if ((file->read(IC, 2) < 2)    ||
            (vcl_strcmp(IC, "NC") && vcl_strcmp(IC, "C0")&&
             vcl_strcmp(IC, "C3") &&
             vcl_strcmp(IC, "C1") && vcl_strcmp(IC, "C2"))  ||
            (vcl_strcmp(IC, "NC") &&
             file->read(COMRAT, 4) < 4))
        {
            error = true;
            break;
        }

        for (unsigned int n = 0; n < NBANDS; n++) delete bands[n];
        delete bands;
        bands = 0;
        NBANDS = 0;
        if (!get_unsigned(buffer, &NBANDS, 1, file) || NBANDS == 0)
        {
            error = true;
            break;
        }
        if (NBANDS > 0) {
          bands = new vil_nitf_image_subheader_band*[NBANDS];
          for (unsigned int n = 0; n < NBANDS; n++) bands[n] = 0;
        }
        vcl_cout << method_name << "NBANDS = " << NBANDS
                 << vcl_endl;
        for (unsigned int n = 0; n < NBANDS; n++)
        {
            vcl_cout << method_name << "read data for band[" << n << "]\n";

            bands[n] = version_->newImageHeaderBand();
            if ((file->read(bands[n]->ITYPE, 8) < 8) ||
                (file->read(bands[n]->IFC,   1) < 1) ||
                ((bands[n]->IFC[0] != 'N') && (bands[n]->IFC[0] != ' ')) ||
                (file->read(bands[n]->IMFLT, 3) < 3) ||
                (!get_unsigned(buffer, &bands[n]->NLUTS, 1, file)) ||
                (bands[n]->NLUTS > 4)) {
              bands[n]->NLUTS = 0;
              error = true;
              break;
            }
            vcl_cout << method_name << "band[" << n << ']'
                     << " ITYPE = <" << bands[n]->ITYPE << '>'
                     << " IFC = <" << bands[n]->IFC << '>'
                     << " NLUTS = " << bands[n]->NLUTS << vcl_endl;
            if (bands[n]->NLUTS > 0)
            {
                bands[n]->LUTD = new unsigned char*[bands[n]->NLUTS];
                unsigned int nn;
                for (nn = 0; nn < bands[n]->NLUTS; nn++) bands[n]->LUTD[nn] = 0;
                if (!GetInt(buffer, &bands[n]->NELUT, 5, file) ||
                    bands[n]->NELUT > MAX_UINT_16 ||
                    bands[n]->NELUT < 1)
                {
                    error = true;
                    break;
                }
                for (nn = 0; nn < bands[n]->NLUTS; nn++)
                {
                    bands[n]->LUTD[nn] = new unsigned char[bands[n]->NELUT];
                    if (file->read(bands[n]->LUTD[nn], bands[n]->NELUT) < bands[n]->NELUT)
                    {
                        error = true;
                        break;
                    }
                }
                if (error) break;
            }
        }  // end for NBANDS

        if (error) break;

        if (!GetInt(buffer, &ISYNC, 1, file) ||
            (file->read(buffer,     1) < 1)  ||
            (!vcl_strpbrk(buffer, "BSPbsp")))
        {
            error = true;
            break;
        }
        vcl_cout << method_name << "IMODE string = <" << buffer[0] << ">\n";

        if (*buffer == 'B' || *buffer == 'b')
            IMODE_ = BLOCK_INTERLEAVED;
        else if (*buffer == 'P' || *buffer == 'p')
            IMODE_ = PIXEL_INTERLEAVED;
        else if (*buffer == 'S' || *buffer == 's')
            IMODE_ = NOT_INTERLEAVED;
        else
        {
            bool unrecognizedInterleaveType = true;
            assert(unrecognizedInterleaveType == false);
        }

        if (!get_unsigned(buffer, &NBPR, 4, file) ||
            (NBPR < 1)                            ||
            !get_unsigned(buffer, &NBPC, 4, file) ||
            (NBPC < 1)                            ||
            !get_unsigned(buffer,&NPPBH, 4, file) ||
            (NPPBH > 9999)         ||
            !get_unsigned(buffer,&NPPBV, 4, file) ||
            (NPPBV > 9999)         ||
            !get_unsigned(buffer, &NBPP, 2, file) ||
            (NBPP < 1) || (NBPP > 16)             ||
            !GetInt(buffer, &DLVL,       3, file) ||
            !GetInt(buffer, &ALVL,       3, file) ||
            (ALVL > 998)                          ||
            !GetInt(buffer, &LOCrow,     5, file) ||
            !GetInt(buffer, &LOCcolumn,  5, file) ||
            (file->read(IMAG,       4) < 4))
        {
            error = true;
            break;
        }

        // And finally, read any extended header information.
        //
        if (!GetInt(buffer, &UDIDL_, 5, file))
        {
            error = true;
            break;
        }
        vcl_cout << method_name << "UDIDL_ = " << UDIDL_
                 << " (user defined image data length).\n";

        delete UDID_;
        UDID_ = 0;
        if (UDIDL_ > 0)
        {
            UDID_ = new char[UDIDL_ + 1];
            UDID_[UDIDL_] = 0;
            if (file->read(UDID_, UDIDL_) < UDIDL_)
            {
                error = true;
                break;
            }
        }

        int tmp;
        if (!GetInt(buffer, &tmp, 5, file))
        {
            error = true;
            break;
        }

        XSHD->XHDL = tmp;

        if (XSHD->Read(file) != STATUS_GOOD)
        {
            error = true;
            break;
        }

        vcl_cout << method_name << "XSHD->XHDL = " << XSHD->XHDL << vcl_endl;
        vcl_cout << method_name << "file position after reading extended header = "
                 << file->tell() << vcl_endl;
#if 0
        if (XSHD->XHDL > 0) {
          vcl_cout << method_name << "XSHD->XHD = <"
                   << XSHD->XHD << ">\n";
        }
#endif
        done = true;
    }  // end while (!done)

    if (error) file->seek(curr_pos);

    // PMP (8/14/97)
    //
    // If present, the Rational Polynomial Coefficients (RPC00A) which
    //  are written out by the IDEX Program's Output Data Server
    //  (the ODS) are found in the Extended Subheader.
    // For NITF chips, the offset and scale information is found in the
    //  I2MAPD (for IDEX II). This is a non-standard user-defined data
    //  record extension documented in two internal LMMS engineering memoranda
    //  entitled:
    //  (1) "Users Guide to Mensurating for Output Products Using I2MAPD"
    //  (2) "Output Data Server NITF 2.0 Registered Tag Requirements"
    // WARNING: Other non-LockMart systems (IHS) do not do this!
    // The ODS writes into UDID/UDIDL the I{2,3,6}MAPD tag, which
    //  contains the geographic corner points of the image as it can best be
    //  computed given the support data.
    // For further details on RPC, you'll need to consult a classified document
    //  containing Support Data Extensions for the NITF.
    // The following code loops through the XSHD looking for that elusive
    //  RPC00A tag. It sets the RPC data into the Image SubHeader fields
    //  once it is found.
    // NOTE: Someone will later need to read/write these to file.
    //
    // (PMP - 4/16/99)
    // Added code up front to check for anamorphic images and return error
    //  if one is detected (not 1x1 aggregation mode).

    char i2mapd[4096]; // working space to store the I2MAPD data for parsing

    // PMP added the following line for aggregation mode processing
    char mpd[512];     // working space to store the MPD26A data for parsing
    char work[1500];   // working space to store the RPC data for parsing
    char temp[32];     // some working space for individual fields
    int itemp;         // some working space for unused fields
    double ftemp;      // some working space for unused fields

    RPC_present = false; // to begin with, assume no RPC data is present
    LLl = LRl = ULl = URl = 0;
    LLs = LRs = ULs = URs = 0;
    RRDSset = 0;

    // Check if there are user-defined record extensions
    if (UDIDL_ != 0) {
// PMP added the following 48 lines for aggregation mode processing
      // Look for the MPD26A tag. (Look for the union label...)

      char *mpd_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD, "MPD26A") : 0;

      // Check if there is MPD26A tag in the subheader.
      if (mpd_tag == NULL) {
        vcl_cout << "Cannot determine aggregation mode\n";
  }// no MPD26A tag (cross your fingers!)
      else {
        // Compute byte offset in UDID to where the 'MPD26A' tag begins.
        int map_off = mpd_tag - XSHD->XHD;
        vcl_cout << "MPD26A tag found in image's subheader.\n";

        map_off += vcl_strlen("MPD26A"); // advance over extension type identifier

        // First off, check if the length of the CEDATA field is
        // favorable to our cause.
        vcl_strncpy(work, &XSHD->XHD[map_off], 5); // grab length of CEDATA field
        work[5] = 0;                         // ensure NULL terminator
        map_off += 5;                        // advance over length of CEDATA field
        int map_size = vcl_atoi(work);           // get size in bytes

        // If the length of the data is incorrect, the MPD26A data is suspect
        //  and cannot be used.
        if ((map_size != 346)) {
          vcl_cout << "MPD26A data determined to be invalid -- TJ will ignore it.\n"
                   << "Therefore, TJ cannot determine if this is a non-1x1 image.\n";
          // vcl_cout << "WARNING: If this is an anamorphic image, TJ-weirdness will ensue.\n";
        }  // bad MPD26A data (cross your fingers tighter!)
        else
        {
          // Okay, the MPD26A data is good. We'll use it.
          vcl_cout << "MPD26A data determined to be valid -- using it.\n";

          // Let's copy the whole MPD26A tag contents into our working space.
          vcl_strncpy(mpd,&XSHD->XHD[map_off],map_size);  // copy all data
          mpd[map_size] = 0;                          // ensure NULL terminator

          // check for isotropic image aggregation mode
          if ((mpd[45] != '1') || (mpd[47] != '1'))
          {
            vcl_cout << "This image is anamorphic!\n";
            char tmp_ana[2]; tmp_ana[1]=0; //Null terminated string
            tmp_ana[0] = mpd[45];
            ANAMRPH[0] = vcl_atoi(&tmp_ana[0]);
            tmp_ana[0] = mpd[47];
            ANAMRPH[1] = vcl_atoi(&tmp_ana[0]);
            vcl_cout << "Anamorphic ratio: " << ANAMRPH[0] << 'x' << ANAMRPH[1] << ".\n";
            if ((ANAMRPH[0] == 5) || (ANAMRPH[0] > 6) || (ANAMRPH[0] == 0)) {
              vcl_cout << "Anamorphic coefficients determined to be invalid!\n"
                       << "This NITF file should not be processed any further.\n";
              return error ? STATUS_BAD : STATUS_GOOD;
            }
            if ((ANAMRPH[1] == 5) || (ANAMRPH[1] > 6) || (ANAMRPH[1] == 0)) {
              vcl_cout << "Anamorphic coefficients determined to be invalid!\n"
                       << "This NITF file should not be processed any further.\n";
              return error ? STATUS_BAD : STATUS_GOOD;
            }
          }  // anamorphic image
          else {
            ANAMRPH[0] = 1;
            ANAMRPH[1] = 1;
            vcl_cout << "TJ can process this image - it is isotropic (1x1)!\n";
          }
        }  // valid MPD26A (TJ likes it!)
      }  // there was an MPD26A tag
      // PMP added the above 48 lines for aggregation mode processing

      // Look for the I2MAPD tag. (Look for the union label...)
      char *map_tag = UDID_ ?  vcl_strstr(UDID_, "I2MAPD") : 0;

      // If there is no I2MAPD tag in the subheader, our job is done and we
      //  return the 'error' variable to the caller.
      if (map_tag == NULL)
      {
        vcl_cout << "There is no I2MAPD tag in this image's subheader.\n";

        // Removed return, since we can continue if there is an ICHIPB and RPC
        vcl_cout << "Hopefully there is an ICHIPB tag.  Continuing...\n";

        //return error ? STATUS_BAD : STATUS_GOOD;
      }  // no I2MAPD tag (there is great sadness)
      else
      {
        // Compute byte offset in UDID to where the 'I2MAPD' tag begins.
        int map_off = map_tag - UDID_;
        vcl_cout << "I2MAPD tag found in image's subheader.\n";

        map_off += vcl_strlen("I2MAPD"); // advance over extension type identifier

        // First off, check if the length of the REDATA field is
        // favorable to our cause.
        vcl_strncpy(work, &UDID_[map_off], 5);  // grab length of REDATA field
        work[5] = 0;                            // ensure NULL terminator
        map_off += 5;                           // advance over length of REDATA field
        int map_size = vcl_atoi(work);          // get size in bytes

        // If the length of the data is incorrect, the I2MAPD data is suspect
        //  and should not be used.
        if ((map_size < 256) || (map_size > 3616)) {
          vcl_cout << "I2MAPD data determined to be invalid -- ignoring it.\n";
          return error ? STATUS_BAD : STATUS_GOOD;
        }  // bad I2MAPD data

        // Okay, the I2MAPD data is good. We'll use it.
        vcl_cout << "I2MAPD data determined to be valid -- using it.\n";
        I2MAPD_present = true;
        // Let's copy the whole I2MAPD tag contents into our working space.
        vcl_strncpy(i2mapd, &UDID_[map_off], map_size);  // copy all data
        i2mapd[map_size] = 0;                     // ensure NULL terminator

        map_off = 0;  // Let's start clean at the beginning.

        // Grab output product size, line direction
        vcl_strncpy(temp,&i2mapd[map_off],6); // grab string from buffer
        temp[6] = 0;                          // ensure NULL terminator
        map_off += 6;                         // advance to next field
        itemp = vcl_atoi(temp);               // ASCII -> number
        vcl_cout << "Lines in output product: " << itemp << vcl_endl;

        // Grab output product size, sample direction
        vcl_strncpy(temp,&i2mapd[map_off],6); // grab string from buffer
        temp[6] = 0;                          // ensure NULL terminator
        map_off += 6;                         // advance to next field
        itemp = vcl_atoi(temp);               // ASCII -> number
        vcl_cout << "Samples in output product: " << itemp << vcl_endl;

        // Grab reduced resolution data set (RRDS) and STORE IT!
        vcl_strncpy(temp,&i2mapd[map_off],2); // grab string from buffer
        temp[2] = 0;                          // ensure NULL terminator
        map_off += 2;                         // advance to next field
        itemp = vcl_atoi(temp);               // ASCII -> number
        RRDSset = itemp;                      // store in global data
        vcl_cout << "RRDS of output product: " << itemp << vcl_endl;

        // Grab downsampling factor relative to the full image
        vcl_strncpy(temp,&i2mapd[map_off],8); // grab string from buffer
        temp[8] = 0;                          // ensure NULL terminator
        map_off += 8;                         // advance to next field
        ftemp = vcl_atof(temp);               // ASCII -> number
        vcl_cout << "Reciprocal of RRDS magnification: " << ftemp << vcl_endl;

        // Grab dewarp flag (it damn well better not be set!)
        vcl_strncpy(temp,&i2mapd[map_off],2); // grab string from buffer
        temp[2] = 0;                          // ensure NULL terminator
        map_off += 2;                         // advance to next field
        itemp = vcl_atoi(temp);               // ASCII -> number
        vcl_cout << "Dewarp flag of output product: " << itemp << vcl_endl;

        // Leave if the image has been dewarped (we can't use [u,v] values!).
        if (itemp != 0) {
          vcl_cout << "This image is dewarped! (ODS shouldn't do this).\n";
          return error ? STATUS_BAD : STATUS_GOOD;
        }  // image is dewarped -- pixel values are not useful

        // Skip over image geographic location (it's same as IGEOLO)
        map_off += 60; // length of geodetic corner coordinates

        // Skip over other size fields we don't need to look at.
        map_off += 6;  // start of image data within output product, line index
        map_off += 6;  // start of image data within output product, sample index
        map_off += 6;  // number of significant pixels in line direction
        map_off += 6;  // number of significant pixels in sample direction
        map_off += 2;  // number of grid points, line direction
        map_off += 2;  // number of grid points, sample direction
        map_off += 12; // first grid point output product line index
        map_off += 12; // first grid point output product sample index
        map_off += 12; // spacing between grid points, line direction
        map_off += 12; // spacing between grid points, sample direction

        // Grab line index of upper left corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        ULl = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Upper left corner (L): " << ULl << vcl_endl;

        // Grab sample index of upper left corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        ULs = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Upper left corner (S): " << ULs << vcl_endl;

        // Grab line index of upper right corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        URl = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Upper right corner (L): " << URl << vcl_endl;

        // Grab sample index of upper right corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        URs = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Upper right corner (S): " << URs << vcl_endl;

        // Grab line index of lower left corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        LLl = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Lower left corner (L): " << LLl << vcl_endl;

        // Grab sample index of lower left corner and STORE IT!
        vcl_strncpy(temp,&i2mapd[map_off],12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        LLs = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Lower left corner (S): " << LLs << vcl_endl;

        // Grab line index of lower right corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        LRl = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Lower right corner (L): " << LRl << vcl_endl;

        // Grab sample index of lower right corner and STORE IT!
        vcl_strncpy(temp, &i2mapd[map_off], 12); // grab string from buffer
        temp[12] = 0;                          // ensure NULL terminator
        map_off += 12;                         // advance to next field
        ftemp = vcl_atof(temp);                // ASCII -> number
        LRs = ((int)(ftemp+0.5)) - 1;          // store in global data
        vcl_cout << "Lower right corner (S): " << LRs << vcl_endl;

        // Whew!

        vcl_cout << "Parsed and stored I2MAPD data.\n";
      } // there is an I2MAPD tag
    }  // there are user-defined record extensions

    // Check if there is extended subheader information and, if so, look
    //  through it for RPC00A and RPC00B tags.
    if (XSHD->XHDL != 0)
    {
        StatusCode s = extract_rpc00x_extension();
        if (s == STATUS_BAD) return s;

        // For the time being, we MUST extract the ICHIPB tag prior
        // to the STDIDx tag.  Don't change the order of the parsing.
        s = extract_ichipb_extension();
        if (s == STATUS_BAD) return s;
        s = extract_piaimc_extension();
        if (s == STATUS_BAD) return s;
        s = extract_stdid_extension();
        if (s == STATUS_BAD) return s;
    }  // end if (XSHD->XHDL != 0) -> non-empty Extended Subheader

    vcl_cout << "#### exit " << method_name
             << "file position = " << file->tell() << vcl_endl;

    return error ? STATUS_BAD : STATUS_GOOD;
}  // end method Read

StatusCode vil_nitf_image_subheader_v20::extract_user_defined_record_extensions()
{
    bool error = false;
    return error ? STATUS_BAD : STATUS_GOOD;
}

StatusCode vil_nitf_image_subheader_v20::Write(vil_stream* file)
{
        if (!file || !file->ok()) return STATUS_BAD;

        int curpos = file->tell();

        char buffer[6];                     // Max size of any numeric field is 5.

        // MPP 4/15/2001  Moved from RPC_present code -- need to support
        // independent RPC/ICHIPB/I2MAPD and PIAIMC extensions.

        char* cur_XHD_pos = NULL;

        if (RPC_present || PIAIMC_present)
        {
                // Make a new extended subheader with plenty of space.
                XSHD = new vil_nitf_extended_subheader(2048);
                XSHD->XHD = new char[2048];
                cur_XHD_pos = XSHD->XHD;
        }
        // end MPP 4/15/2001

        // RWMC (11/6/97)

        if (RPC_present)
        {
                // Convert the RPC stuff, as decribed by PMP.

                // First the IMAP in user-defined record extensions
                vcl_ostringstream imap;

                // Insert the I2MAPD tag.
                imap << "I2MAPD";

                // Store the length of the REDATA field.
                imap << " 256 ";

                // Write output product size, line direction
                imap << vcl_setw(6) << (LLl - ULl);
                // Write output product size, sample direction
                imap << vcl_setw(6) << (URs - ULs);
                // Write reduced resolution data set (RRDS) and STORE IT!
                imap << vcl_setw(2) << RRDSset;
                // Write downsampling factor relative to the full image
                imap << vcl_setw(8) << (1.0/vcl_pow(2.0,RRDSset));
                // Write dewarp flag (it damn well better not be set!)
                imap << vcl_setw(2) << (int)0;
                // Write in image geographic location (it's same as IGEOLO)
                imap << vcl_setw(60) << IGEOLO;
                // Write other size fields we don't need to look at.
                imap << vcl_setw(6) << (int)0
                     << vcl_setw(6) << (int)0
                     << vcl_setw(6) << (LLl - ULl)
                     << vcl_setw(6) << (URs - ULs)
                     << vcl_setw(2) << (int)0
                     << vcl_setw(2) << (int)0
                     << vcl_setw(12) << (int)0
                     << vcl_setw(12) << (int)0
                     << vcl_setw(12) << (int)0
                     << vcl_setw(12) << (int)0;

                // Write corners.
                imap << vcl_setw(12) << ULl+0.5
                     << vcl_setw(12) << ULs+0.5
                     << vcl_setw(12) << URl+0.5
                     << vcl_setw(12) << URs+0.5
                     << vcl_setw(12) << LLl+0.5
                     << vcl_setw(12) << LLs+0.5
                     << vcl_setw(12) << LRl+0.5
                     << vcl_setw(12) << LRs+0.5;

                // Now set the user defined record extensions.
                vcl_string temp_str = imap.str();
                UDIDL_ = temp_str.size();
                UDID_ = new char[UDIDL_ + 1];
                UDID_ = STRCPY(UDID_, temp_str.c_str());
                UDID_[UDIDL_] = '\0';

                // Encode the RPC extension
                vcl_ostringstream rpc_buf;
                encode_rpc00a_extension(rpc_buf);

                // Copy the generated string into the extended subheader.
                temp_str = rpc_buf.str();
                int len_rpc = temp_str.size();
                vcl_memcpy(cur_XHD_pos, temp_str.c_str(), len_rpc);
                cur_XHD_pos += len_rpc;

                // The more standard way than I2MAPD is ICHIPB
                if (ICHIPB_present)
                {
                  vcl_ostringstream  ichipb_buf;
                  encode_ichipb_extension(ichipb_buf);

                  // MPP 4/15/2001
                  // Changed len_ichipb computation to vcl_ostringstream query
                  // int len_ichipb = vcl_strlen("ICHIPB") + 5;//The 5 is for CDATA
                  // len_ichipb += 224;
                  // int len_ichipb = ichipb_buf.pcount();

                  temp_str = ichipb_buf.str();
                  int len_ichipb = temp_str.size();
                  vcl_memcpy(cur_XHD_pos, temp_str.c_str(), len_ichipb);
                  cur_XHD_pos += len_ichipb;
                }
        }

        // MPP 4/15/2001
        // Added support for PIAIMC extension
        if (PIAIMC_present)
        {
                vcl_ostringstream  piaimc_buf;
                encode_piaimc_extension(piaimc_buf);

                vcl_string temp_str = piaimc_buf.str();
                int len_piaimc = temp_str.size();

                vcl_memcpy(cur_XHD_pos, temp_str.c_str(), len_piaimc);
                cur_XHD_pos += len_piaimc;
        }

        bool error = false;
        bool done  = false;
        while (!done)
{
        buffer[0] = *ConvertClassification(ISCLAS);
        buffer[1] = 0;                // For when ISCLAS is read in.
        if ((file->write(IM,      2) <  2) ||
            (vcl_strcmp(IM, "IM"))                  ||
            (file->write(IID,    10) < 10) ||
            (file->write(IDATIM_, 14) < 14) ||
            (file->write(TGTID,  17) < 17) ||
            (file->write(ITITLE, 80) < 80) ||
            (file->write(buffer,  1) <  1) ||
            (!ValidClassification(buffer)))
        {
            error = true;
            break;
        }
        if ((file->write(ISCODE, 40) < 40) ||
            (file->write(ISCTLH, 40) < 40) ||
            (file->write(ISREL,  40) < 40) ||
            (file->write(ISCAUT, 20) < 20) ||
            (file->write(ISCTLN, 20) < 20) ||
            (file->write(ISDWNG,  6) <  6) ||
            (!vcl_strcmp(ISDWNG, "999998") &&
             file->write(ISDEVT, 40) < 40) ||
            !((ENCRYP == NOTENCRYPTED &&
               file->write((const char*)"0", 1)==1)     ||
              (ENCRYP == ENCRYPTED &&
               file->write((const char*)"1", 1)==1))    ||
            (file->write(ISORCE, 42) < 42) ||
            (NROWS > NBPC * NPPBV)      ||
            (!put_unsigned(buffer, NROWS, 8, file))     ||
            (NCOLS > NBPR * NPPBH)      ||
            (!put_unsigned(buffer, NCOLS, 8, file)))
        {
            error = true;
            break;
        }

        const char* pvtype = 0;
        if (PVTYPE_ == VIL_PIXEL_FORMAT_COMPLEX_FLOAT) pvtype = "C  ";
        else if (PVTYPE_== VIL_PIXEL_FORMAT_UINT_32 && ABPP==1) pvtype = "B  ";
        else if (PVTYPE_ == VIL_PIXEL_FORMAT_UINT_32) pvtype = "INT";
        else if (PVTYPE_ == VIL_PIXEL_FORMAT_FLOAT) pvtype = "R  ";
        else if (PVTYPE_ == VIL_PIXEL_FORMAT_INT_32) pvtype = "SI ";
        else if (PVTYPE_ == VIL_PIXEL_FORMAT_UNKNOWN) pvtype = "U  ";

        const char* pjust = PJUST == PJUST_LEFT ? "L" : "R";

        const char* icords = 0;
        switch (ICORDS)
        {
            case GEOCENTRIC:    icords = "C"; break;
            case GEOGRAPHIC:    icords = "G"; break;
            default:
            case NONE:          icords = "N"; break;
            case UTM:           icords = "U"; break;
        }

        if ((pvtype == 0)                       ||
            (file->write(pvtype,  3) <  3)      ||
            (nitf_strcasecmp(IREP, "MONO    ") &&
             nitf_strcasecmp(IREP, "RGB     ") &&
             nitf_strcasecmp(IREP, "RGB/LUT ") &&
             nitf_strcasecmp(IREP, "MULTI   ") &&
             nitf_strcasecmp(IREP, "YCbCr601"))      ||
            (file->write(IREP,    8) <  8)      ||
            (nitf_strcasecmp(ICAT, "VIS     ") &&
             nitf_strcasecmp(ICAT, "MAP     ") &&
             nitf_strcasecmp(ICAT, "SAR     ") &&
             nitf_strcasecmp(ICAT, "IR      ") &&
             nitf_strcasecmp(ICAT, "FP      ") &&
             nitf_strcasecmp(ICAT, "MRI     ") &&
             nitf_strcasecmp(ICAT, "XRAY    ") &&
             nitf_strcasecmp(ICAT, "CAT     ") &&
             nitf_strcasecmp(ICAT, "MS      "))      ||
            (file->write(ICAT,    8) <  8)      ||
            (ABPP<1 || ABPP>16)                 ||
            (!PutInt(buffer, ABPP, 2, file))    ||
            (file->write(pjust,   1) <  1)      ||
            (file->write(icords,  1) <  1) ||
            (vcl_strpbrk(icords, "CGU") &&
             file->write(IGEOLO, 60) < 60))
        {
            error = true;
            break;
        }

        unsigned int NICOM = ICOM_.size();
        if (!PutInt(buffer, NICOM, 1, file))
        {
            error = true;
            break;
        }
        for (unsigned int n = 0; n < NICOM; n++)
        {
          const char *str_ptr = ICOM_[n].c_str();
          if (file->write(str_ptr, 80) < 80)
          {
            error = true;
            break;
          }
        }

        if ((file->write(IC, 2) < 2)    ||
            (vcl_strcmp(IC, "NC") && vcl_strcmp(IC, "C0") &&
             vcl_strcmp(IC, "C3")&&
             vcl_strcmp(IC, "C1")&& vcl_strcmp(IC,"C2"))  ||
            (vcl_strcmp(IC, "NC") &&
             file->write(COMRAT,4) < 4))
        {
            error = true;
            break;
        }

        if (!PutInt(buffer, NBANDS, 1, file) || NBANDS == 0)
        {
            error = true;
            break;
        }
        for (unsigned int n = 0; n < NBANDS; n++)
        {
            if ((file->write(bands[n]->ITYPE, 8) < 8) ||
                (file->write(bands[n]->IFC,   1) < 1) ||
                (bands[n]->IFC[0] != 'N')                 ||
                (file->write(bands[n]->IMFLT, 3) < 3) ||
                (!PutInt(buffer,bands[n]->NLUTS, 1, file)) ||
                (bands[n]->NLUTS > 4))
            {
                bands[n]->NLUTS = 0;
                error = true;
                break;
            }
            if (bands[n]->NLUTS > 0)
            {
                if (!PutInt(buffer, bands[n]->NELUT, 5, file) ||
                    bands[n]->NELUT > MAX_UINT_16 ||
                    bands[n]->NELUT < 1)
                {
                    error = true;
                    break;
                }
                unsigned int nn;
                for (nn = 0; nn < bands[n]->NLUTS; nn++)
                {
                    if (file->write(bands[n]->LUTD[nn],
                                    bands[n]->NELUT) < bands[n]->NELUT)
                    {
                        error = true;
                        break;
                    }
                }
                if (error) break;
            }
        }
        if (error) break;

        const char* imode = IMODE_== BLOCK_INTERLEAVED ? "B" : "S";
        if (!PutInt(buffer, ISYNC,      1, file) ||
            (file->write(imode, 1) < 1)          ||
            !PutInt(buffer, NBPR, 4, file)       ||
            (NBPR < 1)                           ||
            !PutInt(buffer, NBPC, 4, file)       ||
            (NBPC < 1)                           ||
            !PutInt(buffer, NPPBH, 4, file)      ||
            (NPPBH > 9999)                       ||
            !PutInt(buffer, NPPBV,      4, file) ||
            (NPPBV > 9999)                       ||
            !PutInt(buffer, NBPP,       2, file) ||
            (NBPP < 1) || (NBPP > 16)            ||
            !PutInt(buffer, DLVL,       3, file) ||
            !PutInt(buffer, ALVL,       3, file) ||
            (ALVL > 998)                         ||
            !PutInt(buffer, LOCrow,     5, file) ||
            !PutInt(buffer, LOCcolumn,  5, file) ||
            (file->write(IMAG, 4) < 4))
        {
            error = true;
            break;
        }

        // And finally, write any extended header information.
        //
        if (!PutInt(buffer, UDIDL_, 5, file))
        {
            error = true;
            break;
        }
        if (UDIDL_ > 0)
        {
            if (file->write(UDID_, UDIDL_) < UDIDL_)
            {
                error = true;
                break;
            }
        }

        if (!PutInt(buffer, XSHD->XHDL, 5, file))
        {
            error = true;
            break;
        }
        if (XSHD->Write(file) != STATUS_GOOD)
        {
            error = true;
            break;
        }

        done = true;
        }

    if (error) file->seek(curpos);

    return error ? STATUS_BAD : STATUS_GOOD;
}

unsigned long
vil_nitf_image_subheader_v20::GetHeaderLength() const
{
    int length = 426 + ICOM_.size() * 80 + NBANDS*13 +
                 UDIDL_ + XSHD->GetHeaderLength();

    if (!vcl_strcmp(ISDWNG, "999998")) length += 40;
    if (ICORDS == GEOCENTRIC || ICORDS == GEOGRAPHIC || ICORDS == UTM) length += 60;
    if (vcl_strcmp(COMRAT, "NC")) length += 4;
    for (unsigned int n = 0; n < NBANDS; n++)
      if (bands[n]->NLUTS > 0) {
        length += 5;
        if (bands[n]->NELUT > 0)
          length += bands[n]->NLUTS * bands[n]->NELUT;
      }
    return length;
}

//  Initial header values to default values.
//  Combined from ImageSubHeader_v20 methods init and Init.
//  Seemed stupid to have two methods which were always called together.

void vil_nitf_image_subheader_v20::init()
{
    IREP = 0;
    ICAT = 0;

    setVersion(vil_nitf_version_v20::GetVersion());

    char initstr[80];                   // Will be filled with spaces.

    vcl_memset(initstr,' ',80);

    STRNCPY(IM,  "IM",     2);
    STRNCPY(IID, initstr, 10);

    // We must fool SCCS, otherwise it will try to
    // be smart and interpret the percents in the string.
    // We don't use SCCS, so don't try to write to a read
    // only string!
    vcl_time_t clock = vcl_time(NULL);
    vcl_tm *t_m = vcl_localtime(&clock);
    // const char* format = "%d%H%M%SZ%h%y";

    STRNCPY(IDATIM_, initstr, 14);
    vcl_strftime(IDATIM_, 15, date_format, t_m);

    STRNCPY(TGTID,  initstr, 17);
    STRNCPY(ITITLE, initstr, 80);
    ISCLAS = DefaultClassification;
    STRNCPY(ISCODE, initstr, 40);
    STRNCPY(ISCTLH, initstr, 40);
    STRNCPY(ISREL,  initstr, 40);
    STRNCPY(ISCAUT, initstr, 20);
    STRNCPY(ISCTLN, initstr, 20);
    STRNCPY(ISDWNG, initstr,  6);
    STRNCPY(ISDEVT, initstr, 40);
    ENCRYP = NOTENCRYPTED;
    STRNCPY(ISORCE, initstr, 42);
    STRNCPY(IREP,   initstr,  8);       // Unique to v2.0.
    STRNCPY(ICAT,   initstr,  8);       // Unique to v2.0.
    ICORDS = NONE;
    STRNCPY(IGEOLO, initstr, 60);

    ICOM_.clear();

    STRNCPY(IC,     "NC",     2);
    STRNCPY(COMRAT, initstr,  4);

    for (unsigned int n = 0; n < NBANDS; n++)
    {
        if (bands[n] != NULL)
        {
            delete bands[n]->ITYPE;
            delete bands[n]->IFC;
            delete bands[n]->IMFLT;
            if (bands[n]->NLUTS>0 && bands[n]->LUTD!=NULL)
            {
                unsigned int m;
                for (m=0; m<bands[n]->NLUTS; m++)
                    delete bands[n]->LUTD[m];
                delete bands[n]->LUTD;
            }
            delete bands[n];
        }
    }
    NBANDS = 0;
    delete bands;
    bands = 0;

    ISYNC  = 0;
    IMODE_  = NOT_INTERLEAVED;
    NBPR   = NBPC = NCOLS = NROWS = NPPBH = NPPBV = ABPP = NBPP = 1;
    PJUST  = PJUST_RIGHT;
    PVTYPE_ = VIL_PIXEL_FORMAT_UINT_32;
    DLVL   = 1;
    ALVL   = LOCrow = LOCcolumn = 0;
    STRNCPY(IMAG,  initstr, 4);

    UDIDL_ = 0;
    delete UDID_;
    UDID_ = 0;
    delete XSHD;
    XSHD = new vil_nitf_extended_subheader(0);

    //set up some defaults for the 2.0 specifics
    STRCPY(IREP, "MONO    ");
    STRCPY(ICAT, "VIS     ");
}  // end method init

////////////////////////////////////////////////////////////////
/// Read RPC data from header.
////////////////////////////////////////////////////////////////
StatusCode vil_nitf_image_subheader_v20::extract_rpc00x_extension()
{
  bool error = false;
  char work[1500];
  char temp[32];

  // Look for the RPC tag. (Look for the union label...)
  // It can be an A or a B tag so we just look for RPC00
//char *rpc_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD,"RPC00A") : 0;
  char *rpc_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD, "RPC00") : 0;

      // If there is no RPC tag in the subheader, our job is done and we
      //  return the 'error' variable to the caller.
      if (rpc_tag == 0) {
        vcl_cout << "There is no RPC tag in this image's subheader.\n";
        return error ? STATUS_BAD : STATUS_GOOD;
      }// no RPC tag (there is great sadness)

      // Compute byte offset in XHD to where the 'RPC00X' tag begins.
      int offset = rpc_tag - XSHD->XHD;
      vcl_cout << "RPC tag found in image's subheader.\n";

      // offset += vcl_strlen("RPC00A"); // advance over extension type identifier
      // DON'T advance over the RPC00A tag; we need to know if it's A or B type

      // Parse the A or B from the RPC00x tag. GWB 13Nov2003
      vcl_strncpy(work, &XSHD->XHD[offset], 6); // Grab the RPC00x tag
      work[6] = 0;  // ensure NULL terminator
      offset += 6;    // increment the offset past the tag.
      if (vcl_strncmp(work, "RPC00A", 6) == 0) {
        RPC_TYPE = RPC00A;
        vcl_cout << "RPC Tag is RPC00A\n";
      }
      else if (vcl_strncmp(work, "RPC00B", 6) == 0) {
        RPC_TYPE = RPC00B;
        vcl_cout << "RPC Tag is RPC00B\n";
      }
      else {
        RPC_TYPE = UNDEFINED;
        vcl_cout << "Encountered UNKNOWN RPC00x tag!.\n";
        return error ? STATUS_BAD : STATUS_GOOD;
      }  // Unknown RPC tag!

      // First off, check if the length of the CEDATA field and
      //  reject/success flag are all favorable to our cause.
      vcl_strncpy(work, &XSHD->XHD[offset], 5); // grab length of CEDATA field
      work[5] = 0;                        // ensure NULL terminator
      offset += 5;                        // advance over length of CEDATA field
      // If the length of the data is incorrect or the reject/success flag
      //  is set to 'reject', the RPC data is suspect and should not be used.
      if ((vcl_atoi(work) != 1041) || (XSHD->XHD[offset] != '1')) {
        vcl_cout << "RPC data determined to be invalid -- ignoring it.\n";
        error = true;
        return error ? STATUS_BAD : STATUS_GOOD;
      }  // bad RPC data

      // Okay, the RPC data is good. We'll use it.
      vcl_cout << "RPC data determined to be valid -- using it.\n";
      offset++;           // advance over reject/success flag

      // Let's copy the whole RPC tag contents into our working space.
      vcl_strncpy(work, &XSHD->XHD[offset], 1041);  // copy all RPC data
      work[1041] = 0;                         // ensure NULL terminator

      offset = 0;  // Let's start clean at the beginning.

      // Grab ERR_BIAS
      vcl_strncpy(temp, &work[offset], 7); // grab string from buffer
      temp[7] = 0;                   // ensure NULL terminator
      offset += 7;                   // advance to next field
      ERR_BIAS = vcl_atof(temp);         // store value into data structure
      vcl_cout << "Error bias (meters): " << ERR_BIAS << vcl_endl;

      // Grab ERR_RAND
      vcl_strncpy(temp, &work[offset], 7); // grab string from buffer
      temp[7] = 0;                   // ensure NULL terminator
      offset += 7;                   // advance to next field
      ERR_RAND = vcl_atof(temp);         // store value into data structure
      vcl_cout << "Error random (meters): " << ERR_RAND << vcl_endl;

      // Grab LINE_OFF
      vcl_strncpy(temp,&work[offset],6); // grab string from buffer
      temp[6] = 0;                   // ensure NULL terminator
      offset += 6;                   // advance to next field
      LINE_OFF = vcl_atof(temp);         // store value into data structure
      vcl_cout << "Line offset (pixels): " << LINE_OFF << vcl_endl;

      // Grab SAMP_OFF
      vcl_strncpy(temp,&work[offset],5); // grab string from buffer
      temp[5] = 0;                   // ensure NULL terminator
      offset += 5;                   // advance to next field
      SAMP_OFF = vcl_atof(temp);         // store value into data structure
      vcl_cout << "Sample offset (pixels): " << SAMP_OFF << vcl_endl;

      // Grab LAT_OFF
      vcl_strncpy(temp,&work[offset],8); // grab string from buffer
      temp[8] = 0;                   // ensure NULL terminator
      offset += 8;                   // advance to next field
      LAT_OFF = vcl_atof(temp);          // store value into data structure
      vcl_cout << "Latitude offset (degrees): " << LAT_OFF << vcl_endl;

      // Grab LONG_OFF
      vcl_strncpy(temp,&work[offset],9); // grab string from buffer
      temp[9] = 0;                   // ensure NULL terminator
      offset += 9;                   // advance to next field
      LONG_OFF = vcl_atof(temp);         // store value into data structure
      vcl_cout << "Longitude offset (degrees): " << LONG_OFF << vcl_endl;

      // Grab HEIGHT_OFF
      vcl_strncpy(temp,&work[offset],5); // grab string from buffer
      temp[5] = 0;                   // ensure NULL terminator
      offset += 5;                   // advance to next field
      HEIGHT_OFF = vcl_atof(temp);       // store value into data structure
      vcl_cout << "Height offset (meters): " << HEIGHT_OFF << vcl_endl;

      // Grab LINE_SCALE
      vcl_strncpy(temp,&work[offset],6); // grab string from buffer
      temp[6] = 0;                   // ensure NULL terminator
      offset += 6;                   // advance to next field
      LINE_SCALE = vcl_atof(temp);       // store value into data structure
      vcl_cout << "Line scale (pixels): " << LINE_SCALE << vcl_endl;

      // Grab SAMP_SCALE
      vcl_strncpy(temp,&work[offset],5); // grab string from buffer
      temp[5] = 0;                   // ensure NULL terminator
      offset += 5;                   // advance to next field
      SAMP_SCALE = vcl_atof(temp);       // store value into data structure
      vcl_cout << "Sample scale (pixels): " << SAMP_SCALE << vcl_endl;

      // Grab LAT_SCALE
      vcl_strncpy(temp,&work[offset],8); // grab string from buffer
      temp[8] = 0;                   // ensure NULL terminator
      offset += 8;                   // advance to next field
      LAT_SCALE = vcl_atof(temp);        // store value into data structure
      vcl_cout << "Latitude scale (degrees): " << LAT_SCALE << vcl_endl;

      // Grab LONG_SCALE
      vcl_strncpy(temp,&work[offset],9); // grab string from buffer
      temp[9] = 0;                   // ensure NULL terminator
      offset += 9;                   // advance to next field
      LONG_SCALE = vcl_atof(temp);       // store value into data structure
      vcl_cout << "Longitude scale (degrees): " << LONG_SCALE << vcl_endl;

      // Grab HEIGHT_SCALE
      vcl_strncpy(temp,&work[offset],5); // grab string from buffer
      temp[5] = 0;                   // ensure NULL terminator
      offset += 5;                   // advance to next field
      HEIGHT_SCALE = vcl_atof(temp);     // store value into data structure
      vcl_cout << "Height scale (meters): " << HEIGHT_SCALE << vcl_endl;

      int c; // Will need this to loop through the RPC coefficients...

      // Grab LINE_NUMs
      for (c=0; c<20; c++) {
         vcl_strncpy(temp,&work[offset],12); // grab string from buffer
         temp[12] = 0;                   // ensure NULL terminator
         offset += 12;                   // advance to next field
         LINE_NUM[c] = vcl_atof(temp);       // store value into data structure
         //vcl_cout << "Line numerator #" << c << ": " << LINE_NUM[c] << vcl_endl;
      }// line numerator coefficients

      // Grab LINE_DENs
      for (c=0; c<20; c++) {
         vcl_strncpy(temp,&work[offset],12); // grab string from buffer
         temp[12] = 0;                   // ensure NULL terminator
         offset += 12;                   // advance to next field
         LINE_DEN[c] = vcl_atof(temp);       // store value into data structure
         //vcl_cout << "Line denominator #" << c << ": " << LINE_DEN[c] << vcl_endl;
      }// line denominator coefficients

      // Grab SAMP_NUMs
      for (c=0; c<20; c++) {
         vcl_strncpy(temp,&work[offset],12); // grab string from buffer
         temp[12] = 0;                   // ensure NULL terminator
         offset += 12;                   // advance to next field
         SAMP_NUM[c] = vcl_atof(temp);       // store value into data structure
         //vcl_cout << "Sample numerator #" << c << ": " << SAMP_NUM[c] << vcl_endl;
      }// sample numerator coefficients

      // Grab SAMP_DENs
      for (c=0; c<20; c++) {
         vcl_strncpy(temp,&work[offset],12); // grab string from buffer
         temp[12] = 0;                   // ensure NULL terminator
         offset += 12;                   // advance to next field
         SAMP_DEN[c] = vcl_atof(temp);       // store value into data structure
         //vcl_cout << "Sample denominator #" << c << ": " << SAMP_DEN[c] << vcl_endl;
      }// sample denominator coefficients

      // Whew!
      vcl_cout << "Parsed and stored RPC data.\n";
      RPC_present = true; // Right on, baby!
      return error ? STATUS_BAD : STATUS_GOOD;
}
// end method extract_rpc00x_extension()

StatusCode vil_nitf_image_subheader_v20::extract_ichipb_extension()
{
  bool error = false;
  char work[250];
  char temp[32];
  // Look for the ICHIPB tag.
  char *ICHIPB_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD,"ICHIPB") : 0;

  // If there is no ICHIPB tag in the subheader, our job is done and we
  //  return the 'error' variable to the caller.
  if (ICHIPB_tag == NULL)
{
      vcl_cout << "There is no ICHIPB tag in this image's subheader.\n";
      return error ? STATUS_BAD : STATUS_GOOD;
}// no ICHIPB tag (there is great sadness)

  // Compute byte offset in XHD to where the 'ICHIPB' tag begins.
  int offset = ICHIPB_tag - XSHD->XHD;
  vcl_cout << "ICHIPB tag found in image's subheader.\n";

  offset += vcl_strlen("ICHIPB"); // advance over extension type identifier

  // First off, check if the length of the CEDATA field and
      //  reject/success flag are all favorable to our cause.
  vcl_strncpy(work,&XSHD->XHD[offset],5); // grab length of CEDATA field
  work[5] = 0;                        // ensure NULL terminator
  offset += 5;                        // advance over length of CEDATA field

  // If the length of the data is incorrect or the reject/success flag
  //  is set to 'reject',the ICHIPB data is suspect and should not be used.
  if (vcl_atoi(work) != 224)
{
      vcl_cout << "ICHIPB data determined to be invalid -- ignoring it.\n";
      error = true;
        return error ? STATUS_BAD : STATUS_GOOD;
}// bad ICHIPB data

  // Okay, the ICHIPB data is good. We'll use it.
  vcl_cout << "ICHIPB data determined to be valid -- using it.\n";

  // Let's copy the whole ICHIPB tag contents into our working space.
  vcl_strncpy(work,&XSHD->XHD[offset],224);  // copy all ICHIPB data
  work[225] = 0;                         // ensure NULL terminator

  offset = 0;  // Let's start clean at the beginning.

      // Grab XFRM_ FLAG
      vcl_strncpy(temp,&work[offset],2); // grab string from buffer
      temp[2] = 0;                   // ensure NULL terminator
      offset += 2;                   // advance to next field
      XFRM_FLAG = vcl_atoi(temp);         // store value into data structure
      vcl_cout << "xfrm flag: " << XFRM_FLAG << vcl_endl;

      // Grab SCALE_FACTOR
      vcl_strncpy(temp,&work[offset],10); // grab string from buffer
      temp[10] = 0;                   // ensure NULL terminator
      offset += 10;                   // advance to next field
      SCALE_FACTOR = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Scale Factor: " << SCALE_FACTOR << vcl_endl;

      // Grab ANAMRPH_CORR
      vcl_strncpy(temp,&work[offset],2); // grab string from buffer
      temp[2] = 0;                   // ensure NULL terminator
      offset += 2;                   // advance to next field
      ANAMRPH_CORR = vcl_atoi(temp);         // store value into data structure
      vcl_cout << "Anamorphic Correction: " << ANAMRPH_CORR << vcl_endl;

      // Grab SCANBLK_NUM
      vcl_strncpy(temp,&work[offset],2); // grab string from buffer
      temp[2] = 0;                   // ensure NULL terminator
      offset += 2;                   // advance to next field
      SCANBLK_NUM = vcl_atoi(temp);         // store value into data structure
      vcl_cout << "Scan Block Number: " << SCANBLK_NUM << vcl_endl;

      // Grab OP_ROW_11
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_ROW_11 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Row Number (1, 1): " << OP_ROW_11 << vcl_endl;

      // Grab OP_COL_11
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_COL_11 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Col Number(1, 1): " << OP_COL_11 << vcl_endl;

      // Grab OP_ROW_12
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_ROW_12 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Row Number(1, 2): " << OP_ROW_12 << vcl_endl;

      // Grab OP_COL_12
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_COL_12 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Col Number(1, 2): " << OP_COL_12 << vcl_endl;

      // Grab OP_ROW_21
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_ROW_21 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Row Number(2, 1): " << OP_ROW_21 << vcl_endl;

      // Grab OP_COL_21
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_COL_21 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Col Number(2, 1): " << OP_COL_21 << vcl_endl;

      // Grab OP_ROW_22
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_ROW_22 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Row Number(2, 2): " << OP_ROW_22 << vcl_endl;

      // Grab OP_COL_22
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      OP_COL_22 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Output Product Col Number(2, 2): " << OP_COL_22 << vcl_endl;

      // Grab FI_ROW_11
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_ROW_11 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Row(1, 1): " << FI_ROW_11 << vcl_endl;

      // Grab FI_COL_11
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_COL_11 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Col(1, 1): " << FI_COL_11 << vcl_endl;

      // Grab FI_ROW_12
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_ROW_12 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Row(1, 2): " << FI_ROW_12 << vcl_endl;

      // Grab FI_COL_12
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_COL_12 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Col(1, 2): " << FI_COL_12 << vcl_endl;

      // Grab FI_ROW_21
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_ROW_21 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Row(2, 1): " << FI_ROW_21 << vcl_endl;

      // Grab FI_COL_21
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_COL_21 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Col(2, 1): " << FI_COL_21 << vcl_endl;

      // Grab FI_ROW_22
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_ROW_22 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Row(2, 2): " << FI_ROW_22 << vcl_endl;

      // Grab FI_COL_22
      vcl_strncpy(temp,&work[offset],12); // grab string from buffer
      temp[12] = 0;                   // ensure NULL terminator
      offset += 12;                   // advance to next field
      FI_COL_22 = nitf_atoff(temp);         // store value into data structure
      vcl_cout << "Full Image Col(2, 2): " << FI_COL_22 << vcl_endl;

      // Grab FI_ROW
      vcl_strncpy(temp,&work[offset],8); // grab string from buffer
      temp[8] = 0;                   // ensure NULL terminator
      offset += 8;                   // advance to next field
      FI_ROW = vcl_atoi(temp);         // store value into data structure
      vcl_cout << ":Full Image Row " << FI_ROW << vcl_endl;

      // Grab FI_COL
      vcl_strncpy(temp,&work[offset],8); // grab string from buffer
      temp[8] = 0;                   // ensure NULL terminator
      offset += 8;                   // advance to next field
      FI_COL = vcl_atoi(temp);         // store value into data structure
      vcl_cout << ":Full Image Col " << FI_COL << vcl_endl;

      // Whew!
      vcl_cout << "Parsed and stored ICHIPB data.\n";
      ICHIPB_present = true; // Right on, baby!
      return error ? STATUS_BAD : STATUS_GOOD;
}

// MPP 4/15/2001
// Added support for PIAIMC extension
StatusCode vil_nitf_image_subheader_v20::extract_piaimc_extension()
{
        bool error = false;
        char work[PIAIMC_CEL + 1];
        char temp[256];

        // Look for the PIAIMC tag.
        char *PIAIMC_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD,"PIAIMC") : 0;

        // If there is no PIAIMC tag in the subheader, our job is done and we
        //  return the 'error' variable to the caller.
        if (PIAIMC_tag == NULL)
        {
                vcl_cout << "There is no PIAIMC tag in this image's subheader.\n";
                return error ? STATUS_BAD : STATUS_GOOD;
        }// no PIAIMC tag (there is great sadness)

        // Compute byte offset in XHD to where the 'PIAIMC' tag begins.
        int offset = PIAIMC_tag - XSHD->XHD;
        vcl_cout << "PIAIMC tag found in image's subheader.\n";

        offset += vcl_strlen("PIAIMC"); // advance over extension type identifier

        // Check if the length of the CEDATA field is favorable to our cause.
        vcl_strncpy(work,&XSHD->XHD[offset],5); // grab length of CEDATA field
        work[5] = 0;                        // ensure NULL terminator
        offset += 5;                        // advance over length of CEDATA field

        // If the length of the data is incorrect, the PIAIMC
        // data is suspect and should not be used.
        if (vcl_atoi(work) != PIAIMC_CEL)
        {
          vcl_cout << "PIAIMC data determined to be invalid -- ignoring it.\n";
          error = true;
          return error ? STATUS_BAD : STATUS_GOOD;
        }// bad PIAIMC data

        // Okay, the PIAIMC data is good. We'll use it.
        vcl_cout << "PIAIMC data determined to be valid -- using it.\n";

        // Let's copy the whole PIAIMC tag contents into our working space.
        vcl_strncpy(work, &XSHD->XHD[offset], PIAIMC_CEL); // copy all PIAIMC data
        work[PIAIMC_CEL] = 0;                          // ensure NULL terminator
        vcl_cout << "PIAIMC Header = \"" << work << "\"\n";

        offset = 0;  // Let's start clean at the beginning.

        // Grab CLOUDCVR
        vcl_strncpy(temp, &work[offset], 3);           // grab string from buffer
        temp[3] = 0;                                   // ensure NULL terminator
        offset += 3;                                   // advance to next field
        CLOUDCVR = vcl_atoi(temp);                     // store value
        vcl_cout << ":Cloud Cover " << CLOUDCVR << vcl_endl;

        // Grab SRP
        vcl_strncpy(temp, &work[offset], 1);           // grab string from buffer
        temp[1] = 0;                                   // ensure NULL terminator
        offset += 1;                                   // advance to next field
        SRP = (nitf_strcasecmp(temp, "N") ? false : true);  // store value
        vcl_cout << ":Standard Radiometric Product " << SRP << vcl_endl;

        // Grab SENSMODE
        STRNCPY(SENSMODE, &work[offset], 12);          // grab string from buffer
        offset += 12;                                  // advance to next field
        vcl_cout << ":Sensor Mode " << SENSMODE << vcl_endl;

        // Grab SENSNAME
        STRNCPY(SENSNAME, &work[offset], 18);          // grab string from buffer
        offset += 18;                                  // advance to next field
        vcl_cout << ":Sensor Name " << SENSNAME << vcl_endl;

        // Grab SOURCE
        STRNCPY(SOURCE, &work[offset], 255);           // grab string from buffer
        offset += 255;                                 // advance to next field
        vcl_cout << ":Source " << SOURCE << vcl_endl;

        // Grab COMGEN
        vcl_strncpy(temp, &work[offset], 2);           // grab string from buffer
        temp[2] = 0;                                   // ensure NULL terminator
        offset += 2;                                   // advance to next field
        COMGEN = vcl_atoi(temp);                           // store value
        vcl_cout << ":Compression Generation " << COMGEN << vcl_endl;

        // Grab SUBQUAL
        vcl_strncpy(temp, &work[offset], 1);               // grab string from buffer
        temp[1] = 0;                                   // ensure NULL terminator
        offset += 1;                                   // advance to next field
        SUBQUAL = temp[0];                             // store value
        vcl_cout << ":Subjective Quality " << SUBQUAL << vcl_endl;

        // Grab PIAMSNNUM
        STRNCPY(PIAMSNNUM, &work[offset], 7);          // grab string from buffer
        offset += 7;                                   // advance to next field
        vcl_cout << ":PIA Mission Number " << PIAMSNNUM << vcl_endl;

        // Grab CAMSPECS
        STRNCPY(CAMSPECS, &work[offset], 32);          // grab string from buffer
        offset += 32;                                  // advance to next field
        vcl_cout << ":Camera Specs " << CAMSPECS << vcl_endl;

        // Grab PROJID
        STRNCPY(PROJID, &work[offset], 2);             // grab string from buffer
        offset += 2;                                   // advance to next field
        vcl_cout << ":Project ID Code " << PROJID << vcl_endl;

        // Grab GENERATION
        vcl_strncpy(temp, &work[offset], 1);           // grab string from buffer
        temp[1] = 0;                                   // ensure NULL terminator
        offset += 1;                                   // advance to next field
        GENERATION = vcl_atoi(temp);                   // store value
        vcl_cout << ":Generation " << GENERATION << vcl_endl;

        // Grab ESD
        vcl_strncpy(temp, &work[offset], 1);           // grab string from buffer
        temp[1] = 0;                                   // ensure NULL terminator
        offset += 1;                                   // advance to next field
        ESD = (nitf_strcasecmp(temp, "N") ? false : true);  // store value
        vcl_cout << ":Exploitation Support Data " << ESD << vcl_endl;

        // Grab OTHERCOND
        STRNCPY(OTHERCOND, &work[offset], 2);          // grab string from buffer
        offset += 2;                                   // advance to next field
        vcl_cout << ":Other Conditions " << OTHERCOND << vcl_endl;

        // Grab MEANGSD
        vcl_strncpy(temp, &work[offset], 7);           // grab string from buffer
        temp[7] = 0;                                   // ensure NULL terminator
        offset += 7;                                   // advance to next field
        MEANGSD = nitf_atoff(temp);                          // store value
        vcl_cout << ":Mean GSD " << MEANGSD << vcl_endl;

        // Grab IDATUM
        STRNCPY(IDATUM, &work[offset], 3);             // grab string from buffer
        offset += 3;                                   // advance to next field
        vcl_cout << ":Image Datum " << IDATUM << vcl_endl;

        // Grab IELLIP
        STRNCPY(IELLIP, &work[offset], 3);             // grab string from buffer
        offset += 3;                                   // advance to next field
        vcl_cout << ":Image Ellipsoid " << IELLIP << vcl_endl;

        // Grab PREPROC
        STRNCPY(PREPROC, &work[offset], 2);            // grab string from buffer
        offset += 2;                                   // advance to next field
        vcl_cout << ":Image Proccessing Level Code " << PREPROC << vcl_endl;

        // Grab IPROJ
        STRNCPY(IPROJ, &work[offset], 2);              // grab string from buffer
        offset += 2;                                   // advance to next field
        vcl_cout << ":Image Projection System " << IPROJ << vcl_endl;

        // Grab SATTRACK_PATH
        vcl_strncpy(temp, &work[offset], 4);               // grab string from buffer
        temp[4] = 0;                                   // ensure NULL terminator
        offset += 4;                                   // advance to next field
        SATTRACK_PATH = vcl_atoi(temp);                    // store value
        vcl_cout << ":Satellite Track PATH " << SATTRACK_PATH << vcl_endl;

        // Grab SATTRACK_ROW
        vcl_strncpy(temp, &work[offset], 4);               // grab string from buffer
        temp[4] = 0;                                   // ensure NULL terminator
        offset += 4;                                   // advance to next field
        SATTRACK_ROW = vcl_atoi(temp);                     // store value
        vcl_cout << ":Satellite Track ROW " << SATTRACK_ROW << vcl_endl;

        // Whew!
        vcl_cout << "Parsed and stored PIAIMC data.\n";
        PIAIMC_present = true; // Right on, baby!
        return error ? STATUS_BAD : STATUS_GOOD;
}

// G. W. Brooksby 2/10/2003
// Added support for STDIDx extension
StatusCode vil_nitf_image_subheader_v20::extract_stdid_extension()
{
    bool status = false;
    char work[101];
    char temp[101];

    // Look for the STDIDx tag.  We only look for the STDID string, in case
    // there are variants of this tag (STDIDx).

    char *STDID_tag = XSHD->XHD ? vcl_strstr(XSHD->XHD, "STDID") : 0;

    // If there is not STDID tag in the subheader, exit and return "False".
    if (STDID_tag == NULL) {
        vcl_cout << "There is no STDIDx tag in this image's subheader.\n";
        status = false;
        return status;
    }

    // If we're here, there must be an STDIDx tag.  Find where it starts.
    int offset = STDID_tag - XSHD->XHD;
    offset += vcl_strlen("STDID"); // advance over the extension type ID

    // Find out what kind of STDID tag we have...
    char tag_type;
    vcl_strncpy(&tag_type,&XSHD->XHD[offset],1);  // extract the tag type (A,B,C?)
    vcl_cout << "STDID" << tag_type << " tag found in image's subheader.\n";
    offset++;

    vcl_strncpy(work,&XSHD->XHD[offset],5); // parse out the CEL
    work[5]=0; // ensure NULL terminator
    offset += 5;

    // define some variables relevant for the tag.
    // this will allow us to do dynamic tag parsing.
    int acq_date=0, mission=1, pass=2, op_num=3, start_seg=2, repro_num=2;
    int replay_regen=3, blank_fill=1, start_col=0, start_row=5, end_seg=2;
    int end_col=0, end_row=5, country=2, wac=4, loc=11, res1=5, res2=7;

  // now populate the variables based on the tag_type.
    switch (tag_type) {
        case 'a':
        case 'A':
        case 'b':
        case 'B':
          acq_date=7; mission=4; pass=2; op_num=3; start_seg=2;
          repro_num=2; replay_regen=3; blank_fill=1; start_col=2;
          start_row=5; end_seg=2; end_col=2; end_row=5; country=2;
          wac=4; loc=11; res1=5; res2=7;
          break;
        case 'c':
        case 'C':
          acq_date=14; mission=14; pass=2; op_num=3; start_seg=2;
          repro_num=2; replay_regen=3; blank_fill=1; start_col=3;
          start_row=5; end_seg=2; end_col=3; end_row=5; country=2;
          wac=4; loc=11; res1=5; res2=8;
          break;
    }

    // Grab the ACQUISITION_DATE
    STRNCPY(ACQUISITION_DATE, &XSHD->XHD[offset], acq_date);
    offset += acq_date;
    vcl_cout << "ACQUISITION_DATE = " << ACQUISITION_DATE << vcl_endl;

    // Grab the MISSION
    STRNCPY(MISSION, &XSHD->XHD[offset],mission);
    offset += mission;
    vcl_cout << "MISSION = " << MISSION << vcl_endl;

    // Grab the PASS
    STRNCPY(PASS, &XSHD->XHD[offset], pass);
    offset += pass;
    vcl_cout << "PASS = " << PASS << vcl_endl;

    // Grab the OP_NUM
    vcl_strncpy(temp, &XSHD->XHD[offset],op_num);
    temp[op_num]=0;
    offset += op_num;
    OP_NUM = vcl_atoi(temp);
    vcl_cout << "OP_NUM = " << OP_NUM << vcl_endl;

    // Grab the START_SEGMENT
    STRNCPY(START_SEGMENT, &XSHD->XHD[offset],start_seg);
    offset += start_seg;
    vcl_cout << "START_SEGMENT = " << START_SEGMENT << vcl_endl;

    // Grab the REPRO_NUM
    vcl_strncpy(temp, &XSHD->XHD[offset],repro_num);
    temp[repro_num]=0;
    offset += repro_num;
    REPRO_NUM = vcl_atoi(temp);
    vcl_cout << "REPRO_NUM = " << REPRO_NUM << vcl_endl;

    // Grab the REPLAY_REGEN
    STRNCPY(REPLAY_REGEN, &XSHD->XHD[offset],replay_regen);
    offset += replay_regen;
    vcl_cout << "REPLAY_REGEN = " << REPLAY_REGEN << vcl_endl;

    // Grab the BLANK_FILL
    STRNCPY(BLANK_FILL, &XSHD->XHD[offset],blank_fill);
    offset += blank_fill;
    vcl_cout << "BLANK_FILL = " << BLANK_FILL << vcl_endl;

    // Grab the START_COLUMN
    vcl_strncpy(temp, &XSHD->XHD[offset],start_col);
    temp[start_col]=0;
    offset += start_col;
    START_COLUMN= vcl_atoi(temp);
    vcl_cout << "START_COLUMN = " << START_COLUMN << vcl_endl;

    // Grab the START_ROW
    vcl_strncpy(temp, &XSHD->XHD[offset],start_row);
    temp[start_row]=0;
    offset += start_row;
    START_ROW = vcl_atoi(temp);
    vcl_cout << "START_ROW = " << START_ROW << vcl_endl;

    // Grab the END_SEGMENT
    STRNCPY(END_SEGMENT, &XSHD->XHD[offset],end_seg);
    offset += end_seg;
    vcl_cout << "END_SEGMENT = " << END_SEGMENT << vcl_endl;

    // Grab the END_COLUMN
    vcl_strncpy(temp, &XSHD->XHD[offset],end_col);
    temp[end_col]=0;
    offset += end_col;
    END_COLUMN= vcl_atoi(temp);
    vcl_cout << "END_COLUMN = " << END_COLUMN << vcl_endl;

    // Grab the END_ROW
    vcl_strncpy(temp, &XSHD->XHD[offset],end_row);
    temp[end_row]=0;
    offset += end_row;
    END_ROW= vcl_atoi(temp);
    vcl_cout << "END_ROW = " << END_ROW << vcl_endl;

    // Grab the COUNTRY
    STRNCPY(COUNTRY, &XSHD->XHD[offset],country);
    offset += country;
    vcl_cout << "COUNTRY = " << COUNTRY << vcl_endl;

    // Grab the WAC
    vcl_strncpy(temp, &XSHD->XHD[offset],wac);
    temp[wac]=0;
    offset += wac;
    WAC = vcl_atoi(temp);
    vcl_cout << "WAC = " << WAC << vcl_endl;

    // Grab the LOCATION
    STRNCPY(LOCATION, &XSHD->XHD[offset],loc);
    offset += loc;
    vcl_cout << "LOCATION = " << LOCATION << vcl_endl;

    // Grab the RESERVED_1 field
    STRNCPY(RESERVED_1, &XSHD->XHD[offset],res1);
    offset += res1;
    vcl_cout << "RESERVED_1 = " << RESERVED_1 << vcl_endl;

    // Grab the RESERVED_2 field
    STRNCPY(RESERVED_2, &XSHD->XHD[offset],res2);
    offset += res2;
    vcl_cout << "RESERVED_2 = " << RESERVED_2 << vcl_endl;

    // Now we fix the RPC line & sample offset if this image
    // does not have START_ROW = 0 and START_COLUMN = 0.
    // ...But only do this if the image is not a chip!

    if (!ICHIPB_present) {
      if (START_ROW != 1) LINE_OFF -= ((START_ROW-1) * NPPBV);
      if (START_COLUMN != 1) SAMP_OFF -= ((START_COLUMN-1) * NPPBH);
    }

    status = true;
    return status;
}  // end method extract_stdid_extension

//----------------------------------------------------------------
//:
//   Is this ugly or what!! It seems that the NITF standard requires
//   +x.xxxxxxE+x .  I haven't found any way to get C++ streams to do this
//   using the standard formating commands. Use our own strstream so that
//   the state of s is unaffected.
//
//  NOTE: this function must appear before the method encode_rpc00a_extension for
//      now.  The definition acts as the declaration.
//
static void es(vcl_ostringstream& s, double v)
{
    vcl_stringstream ss;
    if (v == 0)
    {
        s << "+0.000000E+0";
        return;
    }

    double l = vcl_log10(vcl_fabs(v));
    if (vcl_fabs(l) >= 10)
    {
        vcl_cout << "In vil_nitf_v20_header.C - invalid exponent\n";
        s << "+0.000000E+0";
        return;
     }

    int exp = int(l);
    if (exp <= 0)   //fully use the 6 digit precision after the .
      exp--;

    double x = v/vcl_pow(10.0, exp);  //gives a single digit 0-9 before .
    //could be 10.xxx etc so back off one
    if (vcl_fabs(x) >= 10) {
        exp++;
        x /= 10.0;
    }
    ss.setf(vcl_ios_fixed, vcl_ios_floatfield);  //fixed floating point
    ss.setf(vcl_ios_showpos);  //leading +-
    ss.setf(vcl_ios_showpoint);  //fill in trailing zeros
    ss << vcl_setw(9) << vcl_setprecision(6) << x << 'E';  //use up 6 digits after the .
    ss.setf(vcl_ios_dec, vcl_ios_basefield);  //integer
    ss.setf(vcl_ios_showpos);  //with leading +-
    ss << vcl_setw(2) << exp << vcl_ends;  //force to E+x

    s << ss.str();
}

void vil_nitf_image_subheader_v20::encode_rpc00a_extension(vcl_ostringstream& rpc_buf)
{
        // Put the tag into the header.
        rpc_buf << "RPC00A";

        // length of the CEDATA field.
        rpc_buf << "1041 ";

        // Success/reject.
        rpc_buf << '1';

        // Write ERR_BIAS
        rpc_buf << vcl_setw(7) << ERR_BIAS;

        // Write ERR_RAND
        rpc_buf << vcl_setw(7) << ERR_RAND;

        // Write OFFSETS
        rpc_buf << vcl_setw(6) << LINE_OFF
                << vcl_setw(5) << SAMP_OFF
                << vcl_setw(8) << LAT_OFF
                << vcl_setw(9) << LONG_OFF
                << vcl_setw(5) << HEIGHT_OFF;

        // Write SCALES.
        rpc_buf << vcl_setw(6) << LINE_SCALE
                << vcl_setw(5) << SAMP_SCALE
                << vcl_setw(8) << vcl_setprecision(3) << LAT_SCALE
                << vcl_setw(9) << vcl_setprecision(4) << LONG_SCALE
                << vcl_setw(5) << HEIGHT_SCALE;

        int c; // Will need this to loop through the RPC coefficients...

        // Write LINE_NUMs
        for (c=0; c<20; c++) {
    //    rpc_buf << vcl_setw(12) << vcl_setprecision(6) << LINE_NUM[c];
          es(rpc_buf, LINE_NUM[c]);
        }
        // Write LINE_DENs
        for (c=0; c<20; c++) {
    //    rpc_buf << vcl_setw(12) << vcl_setprecision(6) << LINE_DEN[c];
          es(rpc_buf, LINE_DEN[c]);
        }
        // Write SAMP_NUMs
        for (c=0; c<20; c++) {
    //    rpc_buf << vcl_setw(12) << vcl_setprecision(6) << SAMP_NUM[c];
          es(rpc_buf, SAMP_NUM[c]);
        }
        // Write SAMP_DENs
        for (c=0; c<20; c++) {
    //    rpc_buf << vcl_setw(12) << vcl_setprecision(6) << SAMP_DEN[c];
          es(rpc_buf, SAMP_DEN[c]);
        }
        rpc_buf << vcl_ends;
}

void vil_nitf_image_subheader_v20::encode_ichipb_extension(vcl_ostringstream& ichipb_buf)
{
        // Put the tag into the header.
        ichipb_buf << "ICHIPB";

        // length of the CEDATA field.
        ichipb_buf << "224  ";

        // Write XFRM_FLAG
        ichipb_buf << vcl_setw(2) << XFRM_FLAG;

        // Write SCALE_FACTOR
        ichipb_buf << vcl_setw(10) << SCALE_FACTOR;

        // Write ANAMRPH_CORR
        ichipb_buf << vcl_setw(2) << ANAMRPH_CORR;

        // Write SCANBLK_NUM
        ichipb_buf << vcl_setw(2) << SCANBLK_NUM;

        // Write OP_ROW_11
        ichipb_buf << vcl_setw(12) << OP_ROW_11;

        // Write OP_COL_11
        ichipb_buf << vcl_setw(12) << OP_COL_11;

        // Write OP_ROW_12
        ichipb_buf << vcl_setw(12) << OP_ROW_12;

        // Write OP_COL_12
        ichipb_buf << vcl_setw(12) << OP_COL_12;

        // Write OP_ROW_21
        ichipb_buf << vcl_setw(12) << OP_ROW_21;

        // Write OP_COL_21
        ichipb_buf << vcl_setw(12) << OP_COL_21;

        // Write OP_ROW_22
        ichipb_buf << vcl_setw(12) << OP_ROW_22;

        // Write OP_COL_22
        ichipb_buf << vcl_setw(12) << OP_COL_22;

        // Write FI_ROW_11
        ichipb_buf << vcl_setw(12) << FI_ROW_11;

        // Write FI_COL_11
        ichipb_buf << vcl_setw(12) << FI_COL_11;

        // Write FI_ROW_12
        ichipb_buf << vcl_setw(12) << FI_ROW_12;

        // Write FI_COL_12
        ichipb_buf << vcl_setw(12) << FI_COL_12;

        // Write FI_ROW_21
        ichipb_buf << vcl_setw(12) << FI_ROW_21;

        // Write FI_COL_12
        ichipb_buf << vcl_setw(12) << FI_COL_21;

        // Write FI_ROW_22
        ichipb_buf << vcl_setw(12) << FI_ROW_22;

        // Write FI_COL_22
        ichipb_buf << vcl_setw(12) << FI_COL_22;

        // Write FI_ROW
        ichipb_buf << vcl_setw(8) << FI_ROW;

        // Write FI_COL
        ichipb_buf << vcl_setw(8) << FI_COL << vcl_ends;
}

// MPP 4/15/2001
// Added support for PIAIMC extension
void vil_nitf_image_subheader_v20::encode_piaimc_extension(vcl_ostringstream& piaimc_buf)
{
        // Write CETAG
        piaimc_buf << "PIAIMC";

        // Write CEL
        piaimc_buf << vcl_setw(5) << PIAIMC_CEL;

        // Write CLOUDCVR
        piaimc_buf << vcl_setw(3) << vcl_setfill('0') << CLOUDCVR;

        // Write SRP
        piaimc_buf << vcl_setw(1) << (SRP ? 'Y' : 'N');

        // Write SENSMODE
        piaimc_buf << vcl_setw(12) << SENSMODE;

        // Write SENSNAME
        piaimc_buf << vcl_setw(18) << SENSNAME;

        // Write SOURCE
        piaimc_buf << vcl_setw(255) << SOURCE;

        // Write COMGEN
        piaimc_buf << vcl_setw(2) << vcl_setfill('0') << COMGEN;

        // Write SUBQUAL
        piaimc_buf << vcl_setw(1) << SUBQUAL;

        // Write PIAMSNNUM
        piaimc_buf << vcl_setw(7) << PIAMSNNUM;

        // Write CAMSPECS
        piaimc_buf << vcl_setw(32) << CAMSPECS;

        // Write PROJID
        piaimc_buf << vcl_setw(2) << PROJID;

        // Write GENERATION
        piaimc_buf << vcl_setw(1) << GENERATION;

        // Write ESD
        piaimc_buf << vcl_setw(1) << (ESD ? 'Y' : 'N');

        // Write OTHERCOND
        piaimc_buf << vcl_setw(5) << OTHERCOND;

        // Write MEANGSD
        piaimc_buf << vcl_setw(7) << vcl_setfill('0') << MEANGSD;

        // Write IDATUM
        piaimc_buf << vcl_setw(3) << IDATUM;

        // Write IELLIP
        piaimc_buf << vcl_setw(3) << IELLIP;

        // Write PREPROC
        piaimc_buf << vcl_setw(2) << PREPROC;

        // Write IPROJ
        piaimc_buf << vcl_setw(2) << IPROJ;

        // Write SATTRACK_PATH
        piaimc_buf << vcl_setw(4) << vcl_setfill('0') << SATTRACK_PATH;

        // Write SATTRACK_ROW
        piaimc_buf << vcl_setw(4) << vcl_setfill('0') << SATTRACK_ROW
                   << vcl_ends;
}  // end method encode_piaimc_extension
