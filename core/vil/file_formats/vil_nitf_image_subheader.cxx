// This is core/vil/file_formats/vil_nitf_image_subheader.cxx
#include "vil_nitf_image_subheader.h"
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief This file implements the NITF image subheader base class.
// NOTE: Ported from TargetJr ImageSubHeader.  (Code in file NITFHeader.C)
//
// \date: 2003/12/26
// \author: mlaymon

#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vcl_cstddef.h>
#include <vcl_cassert.h>

#include "vil_nitf_util.h"
#include "vil_nitf_version.h"

static int debug_level = 0 ;


vil_nitf_image_subheader::vil_nitf_image_subheader()
    : IID (ID), IDATIM_ (DT), ITITLE (TITLE),
      ISCLAS (CLAS), ISCODE (CODE), ISCTLH (CTLH),
      ISREL (REL),ISCAUT (CAUT), ISCTLN (CTLN),
      ISDWNG (DWNG), ISDEVT (DEVT)
{
    RPC_present = false;
    ICHIPB_present = false;
    I2MAPD_present = false;
    PIAIMC_present = false;
    STDID_present = false;
    IM = TGTID = ISORCE = 0;
    STRCPY(IM,     "");
    STRCPY (TGTID,  "");
    STRCPY (ISORCE, "");

    ICORDS = NONE;

    IGEOLO = 0;
    STRCPY (IGEOLO, "");

    IC = COMRAT = 0;
    STRCPY (IC,     "");
    STRCPY (COMRAT, "");

    NBANDS = 0;
    bands = 0;

    ISYNC = 0;
    IMODE_ = NOT_INTERLEAVED;

    NROWS = NCOLS = NBPR = NBPC = NPPBH = NPPBV = 0;

    ABPP   = NBPP = 0;
    PJUST  = PJUST_RIGHT;
    PVTYPE_ = VIL_PIXEL_FORMAT_INT_32;

    IREP=0; ICAT=0;
    STRCPY (IREP,"MONO    ");
    STRCPY (ICAT,"VIS     ");

    IMAG = 0;
    STRCPY (IMAG,   "");

    UDIDL_ = 0;
    UDID_  = 0;
    XSHD  = new vil_nitf_extended_subheader (0);

    ANAMRPH[0] = 1; // Default aggregation mode is 1x1
    ANAMRPH[1] = 1;

    // MPP 4/16/2001
    // Added support for PIAIMC extension
    // Initialize string pointers so later STRCPY's work OK.
    SENSMODE = 0; STRCPY(SENSMODE, "");
    SENSNAME = 0; STRCPY(SENSNAME, "");
    SOURCE = 0; STRCPY(SOURCE, "");
    PIAMSNNUM = 0; STRCPY(PIAMSNNUM, "");
    CAMSPECS = 0; STRCPY(CAMSPECS, "");
    PROJID = 0; STRCPY(PROJID, "");
    OTHERCOND = 0; STRCPY(OTHERCOND, "");
    IDATUM = 0; STRCPY(IDATUM, "");
    IELLIP = 0; STRCPY(IELLIP, "");
    PREPROC = 0; STRCPY(PREPROC, "");
    IPROJ = 0; STRCPY(IPROJ, "");

    // GWB 2/10/2003
    // Added for STDIDx support
    ACQUISITION_DATE = 0; STRCPY(ACQUISITION_DATE, "");
    MISSION = 0; STRCPY(MISSION, "");
    PASS = 0; STRCPY(PASS, "");
    START_SEGMENT = 0; STRCPY(START_SEGMENT, "");
    REPLAY_REGEN = 0; STRCPY(REPLAY_REGEN, "");
    BLANK_FILL = 0; STRCPY(BLANK_FILL, "");
    END_SEGMENT = 0; STRCPY(END_SEGMENT, "");
    COUNTRY = 0; STRCPY(COUNTRY, "");
    LOCATION = 0; STRCPY(LOCATION, "");
    RESERVED_1 = 0; STRCPY(RESERVED_1, "");
    RESERVED_2 = 0; STRCPY(RESERVED_2, "");
}

//====================================================================
//: Copy constructor for vil_nitf_image_subheader.
//====================================================================
vil_nitf_image_subheader::vil_nitf_image_subheader (const vil_nitf_image_subheader& header)
    : vil_nitf_header (header), IID (ID), IDATIM_ (DT), ITITLE (TITLE), ISCLAS (CLAS),
      ISCODE (CODE), ISCTLH (CTLH), ISREL (REL), ISCAUT (CAUT), ISCTLN (CTLN),
      ISDWNG (DWNG), ISDEVT (DEVT)
{
        // MPP 4/15/2001
        // Removed - duplicate in RPC block below
    // RPC_present = header.RPC_present;

    IM     = new_strdup (header.IM);
    TGTID  = new_strdup (header.TGTID);
    ISORCE = new_strdup (header.ISORCE);

    ICORDS = header.ICORDS;
    IGEOLO = new_strdup (header.IGEOLO);

    ICOM_.clear();

    if (header.ICOM_.size() > 0) {
      //  COPY CONTENTS OF header.ICOM_.  SHOULD WE USE COPY CONSTRUCTOR ??  MAL 9oct2003
        for (vcl_size_t i = 0; i < header.ICOM_.size(); ++i) {
            vcl_string temp_str = header.ICOM_[i];
            this->ICOM_.push_back (temp_str);
        }
    }

    IC     = new_strdup (header.IC);
    COMRAT = new_strdup (header.COMRAT);

    NBANDS = header.NBANDS;
    bands  = (vil_nitf_image_subheader_band**)NULL;
    if (NBANDS > 0) {
        bands = new vil_nitf_image_subheader_band*[NBANDS];

        unsigned int i;
        for (i = 0; i < NBANDS; i++) {
            bands[i] = new vil_nitf_image_subheader_band;

            vil_nitf_image_subheader_band * band = bands[i];
            assert(header.bands[i] != NULL);

            band->ITYPE = new_strdup(header.bands[i]->ITYPE);
            band->IFC   = new_strdup(header.bands[i]->IFC);
            band->IMFLT = new_strdup(header.bands[i]->IMFLT);

            band->LUTD = 0;
            if (header.bands[i]->NLUTS>0 && header.bands[i]->NELUT>0) {
                band->NLUTS = header.bands[i]->NLUTS;
                band->NELUT = header.bands[i]->NELUT;
                band->LUTD  = new unsigned char*[band->NLUTS];

                unsigned int j;
                for (j = 0; j < band->NLUTS; j++) {
                    band->LUTD[j] = new unsigned char[band->NELUT];
                    vcl_memcpy(band->LUTD[j], header.bands[i]->LUTD[j], band->NELUT);
                }
            }
            else {
                band->NLUTS = band->NELUT = 0;
            }
        }
    }

    ISYNC  = header.ISYNC;
    IMODE_  = header.IMODE_;

    NROWS  = header.NROWS;
    NCOLS  = header.NCOLS;
    NBPR   = header.NBPR;
    NBPC   = header.NBPC;
    NPPBH  = header.NPPBH;
    NPPBV  = header.NPPBV;

    ABPP   = header.ABPP;
    NBPP   = header.NBPP;
    PJUST  = header.PJUST;
    PVTYPE_ = header.PVTYPE_;
    // delete IREP ;  // Do not delete. Not initialized. mal 4may2004
    IREP = new_strdup(header.IREP);
    // delete ICAT ;  // Do not delete. Not initialized. mal 4may2004
    ICAT = new_strdup(header.ICAT);

    IMAG   = new_strdup(header.IMAG);

    UDIDL_  = header.UDIDL_;
    UDID_   = new char[UDIDL_ + 1];
    UDID_[UDIDL_] = (char)NULL;
    vcl_memcpy (UDID_, header.UDID_, UDIDL_);

    assert(header.XSHD != NULL);
    XSHD   = new vil_nitf_extended_subheader (*header.XSHD);

    // PMP added this stuff for RPC capability
    RRDSset = header.RRDSset;
    ULl = header.ULl;  ULs = header.ULs;
    URl = header.URl;  URs = header.URs;
    LLl = header.LLl;  LLs = header.LLs;
    LRl = header.LRl;  LRs = header.LRs;
    RPC_present = header.RPC_present;
    RPC_TYPE = header.RPC_TYPE;
    ERR_BIAS = header.ERR_BIAS;
    ERR_RAND = header.ERR_RAND;
    LINE_OFF = header.LINE_OFF;
    SAMP_OFF = header.SAMP_OFF;
    LAT_OFF = header.LAT_OFF;
    LONG_OFF = header.LONG_OFF;
    HEIGHT_OFF = header.HEIGHT_OFF;
    LINE_SCALE = header.LINE_SCALE;
    SAMP_SCALE = header.SAMP_SCALE;
    LAT_SCALE = header.LAT_SCALE;
    LONG_SCALE = header.LONG_SCALE;
    HEIGHT_SCALE = header.HEIGHT_SCALE;

    int k;
    for (k=0; k<20; k++) {
       LINE_NUM[k] = header.LINE_NUM[k];
       LINE_DEN[k] = header.LINE_DEN[k];
       SAMP_NUM[k] = header.SAMP_NUM[k];
       SAMP_DEN[k] = header.SAMP_DEN[k];
    }// next set of RPCs

    // Added by GWB to support Anamorphic Images
    ANAMRPH[0] = header.ANAMRPH[0];
    ANAMRPH[1] = header.ANAMRPH[1];

    // Added by GWB to support ICHIPx
    ICHIPB_present = header.ICHIPB_present;
    XFRM_FLAG = header.XFRM_FLAG;
    SCALE_FACTOR = header.SCALE_FACTOR;
    ANAMRPH_CORR = header.ANAMRPH_CORR;
    SCANBLK_NUM = header.SCANBLK_NUM;
    OP_ROW_11 = header.OP_ROW_11;
    OP_COL_11 = header.OP_COL_11;
    OP_ROW_12 = header.OP_ROW_12;
    OP_COL_12 = header.OP_COL_12;
    OP_ROW_21 = header.OP_ROW_21;
    OP_COL_21 = header.OP_COL_21;
    OP_ROW_22 = header.OP_ROW_22;
    OP_COL_22 = header.OP_COL_22;
    FI_ROW_11 = header.FI_ROW_11;
    FI_COL_11 = header.FI_COL_11;
    FI_ROW_12 = header.FI_ROW_12;
    FI_COL_12 = header.FI_COL_12;
    FI_ROW_21 = header.FI_ROW_21;
    FI_COL_21 = header.FI_COL_21;
    FI_ROW_22 = header.FI_ROW_22;
    FI_COL_22 = header.FI_COL_22;
    FI_ROW = header.FI_ROW;
    FI_COL = header.FI_COL;

        // MPP 4/15/2001
        // Added to support PIAIMC extension
        // Initialize string pointers so later STRNCPY's work OK.
        SENSMODE = 0;
        SENSNAME = 0;
        SOURCE = 0;
        PIAMSNNUM = 0;
        CAMSPECS = 0;
        PROJID = 0;
        OTHERCOND = 0;
        IDATUM = 0;
        IELLIP = 0;
        PREPROC = 0;
        IPROJ = 0;

        // Copy existing header's data
        PIAIMC_present = header.PIAIMC_present;
        CLOUDCVR = header.CLOUDCVR;
        SRP = header.SRP;
        STRCPY(SENSMODE, header.SENSMODE);
        STRCPY(SENSNAME, header.SENSNAME);
        STRCPY(SOURCE, header.SOURCE);
        COMGEN = header.COMGEN;
        SUBQUAL = header.SUBQUAL;
        STRCPY(PIAMSNNUM, header.PIAMSNNUM);
        STRCPY(CAMSPECS, header.CAMSPECS);
        STRCPY(PROJID, header.PROJID);
        GENERATION = header.GENERATION;
        ESD = header.ESD;
        STRCPY(OTHERCOND, header.OTHERCOND);
        MEANGSD = header.MEANGSD;
        STRCPY(IDATUM, header.IDATUM);
        STRCPY(IELLIP, header.IELLIP);
        STRCPY(PREPROC, header.PREPROC);
        STRCPY(IPROJ, header.IPROJ);
        SATTRACK_PATH = header.SATTRACK_PATH;
        SATTRACK_ROW = header.SATTRACK_ROW;
        // end PIAIMC support

        // Begin STDIDx support
        ACQUISITION_DATE = 0;
        MISSION = 0;
        PASS = 0;
        START_SEGMENT = 0;
        REPLAY_REGEN = 0;
        BLANK_FILL = 0;
        END_SEGMENT = 0;
        COUNTRY = 0;
        LOCATION = 0;
        RESERVED_1 = 0;
        RESERVED_2 = 0;

        // Copy existing header data for STDIDx
        STRCPY(ACQUISITION_DATE, header.ACQUISITION_DATE);
        STRCPY(MISSION, header.MISSION);
        STRCPY(PASS, header.PASS);
        OP_NUM = header.OP_NUM;
        STRCPY(START_SEGMENT, header.START_SEGMENT);
        REPRO_NUM = header.REPRO_NUM;
        STRCPY(REPLAY_REGEN, header.REPLAY_REGEN);
        STRCPY(BLANK_FILL, header.BLANK_FILL);
        START_COLUMN = header.START_COLUMN;
        START_ROW = header.START_ROW;
        STRCPY(END_SEGMENT, header.END_SEGMENT);
        END_COLUMN = header.END_COLUMN;
        END_ROW = header.END_ROW;
        STRCPY(COUNTRY, header.COUNTRY);
        WAC = header.WAC;
        STRCPY(LOCATION, header.LOCATION);
        STRCPY(RESERVED_1, header.RESERVED_1);
        STRCPY(RESERVED_2, header.RESERVED_2);
        // End STDIDx support
}

vil_nitf_image_subheader::~vil_nitf_image_subheader()
{
    delete [] IM;
    delete [] TGTID;
    delete [] ISORCE;
    delete [] IGEOLO;
    delete [] IC;
    delete [] COMRAT;
//    delete XSHD ;

    ICOM_.clear();

    // Delete structures holding information for bands.
    //
    unsigned int i;
    for (i = 0; i < NBANDS; i++) {
        if (bands[i]) {
            delete [] bands[i]->ITYPE;
            delete [] bands[i]->IFC;
            delete [] bands[i]->IMFLT;
            unsigned int j;
            for (j = 0; j < bands[i]->NLUTS; j++)
                delete [] bands[i]->LUTD[j];
            delete [] bands[i]->LUTD;
            delete bands[i];
            }
    }
    delete [] bands;
    delete [] IREP;
    delete [] ICAT;
    delete [] IMAG;

        // MPP 4/15/2001
        // Added to support PIAIMC extension
        if (SENSMODE) delete [] SENSMODE;
        if (SENSNAME) delete [] SENSNAME;
        if (SOURCE) delete [] SOURCE;
        if (PIAMSNNUM) delete [] PIAMSNNUM;
        if (CAMSPECS) delete [] CAMSPECS;
        if (PROJID) delete [] PROJID;
        if (OTHERCOND) delete [] OTHERCOND;
        if (IDATUM) delete [] IDATUM;
        if (IELLIP) delete [] IELLIP;
        if (PREPROC) delete [] PREPROC;
        if (IPROJ) delete [] IPROJ;

        // GWB 2/10/2003
        // Added for STDIDx support
        if (ACQUISITION_DATE) delete [] ACQUISITION_DATE;
        if (MISSION) delete [] MISSION;
        if (PASS) delete [] PASS;
        if (START_SEGMENT) delete [] START_SEGMENT;
        if (REPLAY_REGEN) delete [] REPLAY_REGEN;
        if (BLANK_FILL) delete [] BLANK_FILL;
        if (END_SEGMENT) delete [] END_SEGMENT;
        if (COUNTRY) delete [] COUNTRY;
        if (LOCATION) delete [] LOCATION;
        if (RESERVED_1) delete [] RESERVED_1;
        if (RESERVED_2) delete [] RESERVED_2;

    delete [] UDID_;
    delete XSHD;
}

//====================================================================
//: Method on used to determine if the Image is compressed.
//  If the subclass specific to the NITF format does not use the virtual
//  method IsCompressed(), the NITF version will not use compression.
//====================================================================
bool vil_nitf_image_subheader::IsCompressed() const
{
    bool rval = false;
    return rval;
}

//====================================================================
//: Method used to retrieve the Image's cloud cover percentage.
//  If the subclass specific to the NITF format does not use the virtual
//  method IsCompressed(), the NITF version will not use compression.
//====================================================================
int vil_nitf_image_subheader::GetCloudCoverPercentage() const
{
  // By default, we don't know the cloud cover %
    int pct = 999;
    return pct;
}

//====================================================================
//: Method to return a copy of the vil_nitf_header.
//  The copy returned *must* be deleted by the caller.
//====================================================================
vil_nitf_header*
vil_nitf_image_subheader::Copy()
{
    vil_nitf_header* rval = new vil_nitf_image_subheader(*this);
    return rval;
}

//====================================================================
//: Method to copy over the contents of an vil_nitf_image_subheader.
//  If the supplied header is invalid, the results are unspecified.
//====================================================================
void vil_nitf_image_subheader::Copy (const vil_nitf_image_subheader* h)
{
    vil_nitf_header::Copy (h);

    FilledCopy (IM, h->IM);
    FilledCopy (TGTID, h->TGTID);
    FilledCopy( ISORCE, h->ISORCE);

    ICORDS = h->ICORDS;
    FilledCopy (IGEOLO, h->IGEOLO);

    int NICOM = h->ICOM_.size();
    ICOM_.clear();
    for (int n = 0;  n < NICOM; n++) {
        this->ICOM_.push_back (h->ICOM_[n]);
    }

    FilledCopy (IC, h->IC);
    FilledCopy (COMRAT, h->COMRAT);

    if (version_.length() == 0 || h->NBANDS < 1)
    {
        unsigned int i;
        for (i = 0; i < NBANDS; i++) {
            if (bands[i]) {
                delete [] bands[i]->ITYPE;
                delete [] bands[i]->IFC;
                delete [] bands[i]->IMFLT;
                unsigned int j;
                for (j=0; j<bands[i]->NLUTS; j++)
                    delete [] bands[i]->LUTD[j];
                delete [] bands[i]->LUTD;
                delete [] bands[i];
            }
        }

        delete [] bands;
        bands = 0;
    }
    else
    {
        vil_nitf_image_subheader_band** newbands = new vil_nitf_image_subheader_band*[h->NBANDS];

        // Copy over h->NBANDS of the existing vil_nitf_image_subheader_band structs.
        //
        unsigned int i,j;
        for (i=0; i<h->NBANDS && i<NBANDS; i++)
            newbands[i] = bands[i];

        // Allocate any structures we were able to reuse...
        //
        j = i;
        for (; i < h->NBANDS; i++) {
            newbands[i] = new_image_header_band();
        }
        // Free up any structures we are not reusing...
        //
        for (; j < NBANDS; j++)
        {
            if (!bands[j]) continue;
            delete []bands[j]->ITYPE;
            delete []bands[j]->IFC;
            delete []bands[j]->IMFLT;

            if (!bands[j]->LUTD) continue;
            for (i=0; i<bands[j]->NLUTS; j++)
                delete []bands[j]->LUTD[i];
            delete []bands[j]->LUTD;
            delete []bands[j];
            }
        delete []bands;

        NBANDS = h->NBANDS;
        bands  = newbands;
        for (i=0; i<NBANDS; i++)
        {
            vil_nitf_image_subheader_band* band = newbands[i];
            if (h->bands[i]==0 || band==0) continue;

            FilledCopy(band->ITYPE, h->bands[i]->ITYPE);
            FilledCopy(band->IFC,   h->bands[i]->IFC);
            FilledCopy(band->IMFLT, h->bands[i]->IMFLT);

            for (j=0; j<band->NLUTS; j++)
                delete []band->LUTD[j];
            delete []band->LUTD;
            band->LUTD = 0;
            if (h->bands[i]->NLUTS>0 && h->bands[i]->NELUT>0)
            {
                band->NLUTS = h->bands[i]->NLUTS;
                band->NELUT = h->bands[i]->NELUT;
                band->LUTD  = new unsigned char*[band->NLUTS];
                for (j=0; j<band->NLUTS; j++) {
                    band->LUTD[j] = new unsigned char[band->NELUT];
                    vcl_memcpy(band->LUTD[j], h->bands[i]->LUTD[j], band->NELUT);
                }
            }
            else
            {
                band->NLUTS = band->NELUT = 0;
            }
        }
    }

    NBPR   = h->NBPR;
    NBPC   = h->NBPC;
    NCOLS  = h->NCOLS;
    NPPBH  = h->NPPBH;
    NPPBV  = h->NPPBV;
    NROWS  = h->NROWS;

    ABPP   = h->ABPP;
    NBPP   = h->NBPP;
    PJUST  = h->PJUST;
    PVTYPE_ = h->PVTYPE_;

    FilledCopy (IMAG, h->IMAG);

    // PMP added this stuff for RPC capability
    RRDSset = h->RRDSset;
    ULl = h->ULl;  ULs = h->ULs;
    URl = h->URl;  URs = h->URs;
    LLl = h->LLl;  LLs = h->LLs;
    LRl = h->LRl;  LRs = h->LRs;
    RPC_present = h->RPC_present;
    RPC_TYPE = h->RPC_TYPE;
    ERR_BIAS = h->ERR_BIAS;
    ERR_RAND = h->ERR_RAND;
    LINE_OFF = h->LINE_OFF;
    SAMP_OFF = h->SAMP_OFF;
    LAT_OFF = h->LAT_OFF;
    LONG_OFF = h->LONG_OFF;
    HEIGHT_OFF = h->HEIGHT_OFF;
    LINE_SCALE = h->LINE_SCALE;
    SAMP_SCALE = h->SAMP_SCALE;
    LAT_SCALE = h->LAT_SCALE;
    LONG_SCALE = h->LONG_SCALE;
    HEIGHT_SCALE = h->HEIGHT_SCALE;

    int k;
    for (k=0; k<20; k++) {
       LINE_NUM[k] = h->LINE_NUM[k];
       LINE_DEN[k] = h->LINE_DEN[k];
       SAMP_NUM[k] = h->SAMP_NUM[k];
       SAMP_DEN[k] = h->SAMP_DEN[k];
    }// next set of RPCs

    // Added by GWB to support Anamorphic Images
    ANAMRPH[0] = h->ANAMRPH[0];
    ANAMRPH[1] = h->ANAMRPH[1];

    // Added by GWB to support ICHIPx
    ICHIPB_present = h->ICHIPB_present;
    XFRM_FLAG = h->XFRM_FLAG;
    SCALE_FACTOR = h->SCALE_FACTOR;
    ANAMRPH_CORR = h->ANAMRPH_CORR;
    SCANBLK_NUM = h->SCANBLK_NUM;
    OP_ROW_11 = h->OP_ROW_11;
    OP_COL_11 = h->OP_COL_11;
    OP_ROW_12 = h->OP_ROW_12;
    OP_COL_12 = h->OP_COL_12;
    OP_ROW_21 = h->OP_ROW_21;
    OP_COL_21 = h->OP_COL_21;
    OP_ROW_22 = h->OP_ROW_22;
    OP_COL_22 = h->OP_COL_22;
    FI_ROW_11 = h->FI_ROW_11;
    FI_COL_11 = h->FI_COL_11;
    FI_ROW_12 = h->FI_ROW_12;
    FI_COL_12 = h->FI_COL_12;
    FI_ROW_21 = h->FI_ROW_21;
    FI_COL_21 = h->FI_COL_21;
    FI_ROW_22 = h->FI_ROW_22;
    FI_COL_22 = h->FI_COL_22;
    FI_ROW = h->FI_ROW;
    FI_COL = h->FI_COL;

        // MPP 4/15/2001
        // Added to support PIAIMC extension
        PIAIMC_present = h->PIAIMC_present;
        CLOUDCVR = h->CLOUDCVR;
        SRP = h->SRP;
        STRCPY(SENSMODE, h->SENSMODE);
        STRCPY(SENSNAME, h->SENSNAME);
        STRCPY(SOURCE, h->SOURCE);
        COMGEN = h->COMGEN;
        SUBQUAL = h->SUBQUAL;
        STRCPY(PIAMSNNUM, h->PIAMSNNUM);
        STRCPY(CAMSPECS, h->CAMSPECS);
        STRCPY(PROJID, h->PROJID);
        GENERATION = h->GENERATION;
        ESD = h->ESD;
        STRCPY(OTHERCOND, h->OTHERCOND);
        MEANGSD = h->MEANGSD;
        STRCPY(IDATUM, h->IDATUM);
        STRCPY(IELLIP, h->IELLIP);
        STRCPY(PREPROC, h->PREPROC);
        STRCPY(IPROJ, h->IPROJ);
        SATTRACK_PATH = h->SATTRACK_PATH;
        SATTRACK_ROW = h->SATTRACK_ROW;

        // GWB 2/10/2003
        // Added for STDIDx support
        STDID_present = h->STDID_present;
        STRCPY(ACQUISITION_DATE, h->ACQUISITION_DATE);
        STRCPY(MISSION, h->MISSION);
        STRCPY(PASS, h->PASS);
        OP_NUM = h->OP_NUM;
        STRCPY(START_SEGMENT, h->START_SEGMENT);
        REPRO_NUM = h->REPRO_NUM;
        STRCPY(REPLAY_REGEN, h->REPLAY_REGEN);
        STRCPY(BLANK_FILL, h->BLANK_FILL);
        START_COLUMN = h->START_COLUMN;
        START_ROW = h->START_ROW;
        STRCPY(END_SEGMENT, h->END_SEGMENT);
        END_COLUMN = h->END_COLUMN;
        END_ROW = h->END_ROW;
        STRCPY(COUNTRY, h->COUNTRY);
        WAC = h->WAC;
        STRCPY(LOCATION, h->LOCATION );
        STRCPY(RESERVED_1, h->RESERVED_1);
        STRCPY(RESERVED_2, h->RESERVED_2);

    //Needed because of NITF2.1 generalizations
    FilledCopy(IREP, h->IREP);
    FilledCopy(ICAT, h->ICAT);
}

bool vil_nitf_image_subheader::GetImageBounds(
    double& lat_min, double& lat_max,
    double& lon_min, double& lon_max,
    double* lats,    double* lons) const
{
  //------------------------------------------------------------
  // Get the corners of the image form the NITF image subheader,
  // and return them with their bounding box.
  // Abort if there is no coordinate system

  if (this->ICORDS == NONE) {
    vcl_cout << "No coordinate system present for this image.\n";
    return false;
  }

  // Abort if lat-longs not found
  if (!this->IGEOLO) {
    vcl_cout << "Corner lat-longs not present for this image.\n";
    return false;
  }

  // Get the field containing lat-longs from the header
  char *igeolo = this->IGEOLO;

  // Visit all four corners and get the min and max lat-longs
  char cdeg[4], cmin[3], csec[3];
  double cur_val;
  lat_min = +10000.0;
  lat_max = -10000.0;
  lon_min = +10000.0;
  lon_max = -10000.0;

  for (int i = 1; i <= 4; i++) {
    // Get the latitude
    vcl_strncpy (cdeg, igeolo, 2); igeolo = igeolo + 2;
    cdeg[2] = 0;
    vcl_strncpy (cmin, igeolo, 2); igeolo = igeolo + 2;
    cmin[2] = 0;
    vcl_strncpy (csec, igeolo, 2); igeolo = igeolo + 2;
    csec[2] = 0;
    cur_val = (double) atoi (cdeg) + ((double) atoi(cmin)/60.0) +
              ((double) atoi(csec)/3600.0);

    // Determine if it is North or South
    if (igeolo[0] == 'S') cur_val = -cur_val;
    igeolo++;

    // Fix the latitudes
    if (lat_min > cur_val) lat_min = cur_val;
    if (lat_max < cur_val) lat_max = cur_val;
    if (lats) lats[i-1] = cur_val;

    // Get the longitude
    vcl_strncpy (cdeg, igeolo, 3); igeolo = igeolo + 3;
    cdeg[3] = 0;
    vcl_strncpy (cmin, igeolo, 2); igeolo = igeolo + 2;
    cmin[2] = 0;
    vcl_strncpy (csec, igeolo, 2); igeolo = igeolo + 2;
    csec[2] = 0;
    cur_val = (double)atoi(cdeg) + ((double)atoi(cmin)/60.0) +
              ((double)atoi(csec)/3600.0);

    // Determine if it is East or West
    if (igeolo[0] == 'W') cur_val = -cur_val;
    igeolo++;

    // Fix the longitudes
    if (lon_min > cur_val) lon_min = cur_val;
    if (lon_max < cur_val) lon_max = cur_val;

    if (lons) lons[i-1] = cur_val;
  }

  return true;
}

//-------------------------------------------------------------------
//: The NITF standard uses (0.5, 0.5) as the location of the (0, 0) pixel.
//  For now we will use (0, 0) as the TargetJr way.
//  NOTE: vil_nitf_rect_roi is just a typedef for vgl_box_2d<int>.
//
//  \param roi vcl_vector<int> (4) rectangular region of interest.
//       roi[0] = min_x
//       roi[1] = min_y
//       roi[2] = max_x
//       roi[3] = max_y
//  \param f1_row Number of rows in full image.  Set FI_ROW to this value.
//  \param f1_col Number of columns in full image.  Set FI_COL to this value.
//
void vil_nitf_image_subheader::EncodeICHIPB (
    vcl_vector<int>& roi,
    int f1_row, int f1_col)
{
  int min_x = roi[0];
  int min_y = roi[1];
  int max_x = roi[2];
  int max_y = roi[3];

  ICHIPB_present = true;
  XFRM_FLAG = 0;
  SCALE_FACTOR = 1.0; //For now we don't deal with Rsets
  if (ANAMRPH[0] == 1 && ANAMRPH[1] == 1) {
    ANAMRPH_CORR = 0;
  }
  else {
    ANAMRPH_CORR = 1;
  }
  SCANBLK_NUM = 0;
  OP_ROW_11 = 0.5;
  OP_COL_11 = 0.5;
  float ncols = 0.5f + (max_x - min_x) + 1;
  float nrows = 0.5f + (max_y - min_y) + 1;
  OP_ROW_12 = 0.5;
  OP_COL_12 = ncols;

  OP_ROW_21 = nrows;
  OP_COL_21 = 0.5;

  OP_ROW_22 = nrows;
  OP_COL_22 = ncols;

  float first_col = 0.5f + min_x;
  float first_row = 0.5f + min_y;
  float last_col = 0.5f + max_x;
  float last_row = 0.5f + max_y;

  FI_ROW_11 = first_row;
  FI_COL_11 = first_col;

  FI_ROW_12 = first_row;
  FI_COL_12 = last_col;

  FI_ROW_21 = last_row;
  FI_COL_21 = first_col;

  FI_ROW_22 = last_row;
  FI_COL_22 = first_col;

  FI_ROW = f1_row;
  FI_COL = f1_col;
}

// START RATIONAL CAMERA STUFF.

// WITH_SAR_RPC_EXTRACT is not defined right now,
// SO I DID NOT PORT sarrpcextractor CLASS FROM TARGET JR.
// LEAVE IN SO SOMEONE CAN IMPLEMENT IF NEEDED.   TBD: MAL 10dec2003
#ifdef WITH_SAR_RPC_EXTRACT
#include <IUCameras/RPC/sarrpcextractor.h>
#endif

/////////////////////////////////////////////////////////////////
//
//: Fill in passed vectors with RPC camera data from data in this image header
// Only getting data because cannot have dependency on rational camera class.
//
// \param samp_num (20) coefficients for sample numerator cubic
// \param samp_denom (20) coefficients for sample denominator cubic
// \param line_num (20) coefficients for line numerator cubic
// \param line_denom (20) coefficients for line denominator cubic
// \param scalex (2) longitude scale/offset
// \param scaley (2) latitude scale/offset
// \param scalez (2) height scale/offset
// \param scales (2) sample scale/offset
// \param scalel (2) line scale/offset
// \param init_pt (3) centroid of the four world corner points
// \param rescales (2) sample scale/offset for rescale
// \param rescale1 (2) line scale/offset for rescale
// \param scale_index index into scale vectors for scale factor
// \param offset_index index into scale vectors for offset factor
//
// \return true if vectors filled in successfully
//
/////////////////////////////////////////////////////////////////
bool vil_nitf_image_subheader::get_rational_camera_data(
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
    int offset_index) const
{
  static vcl_string method_name =
      "vil_nitf_image_subheader::get_rational_camera_data: ";

  int O_SCALE  = scale_index;
  int O_OFFSET = offset_index;

  // Before diving in whole-hog, check if the RPC data is valid.

  if (this->RPC_present == false)
  {
      vcl_string err_msg = "RPC data not present for this image.";

#ifdef WITH_SAR_RPC_EXTRACT
      if (vcl_strncmp (this->ICAT, "SAR", 3) != 0) {
          err_msg = "RPC data not valid for this image.  Image category != 'SAR'.";
#endif
          // PutErrorLine("RPC data not present or invalid for this image.");
          vcl_cout << method_name << err_msg << vcl_endl;
          return 0;

#ifdef WITH_SAR_RPC_EXTRACT
        }
#endif
    }

  // The sequence of rational polynomial coefficients in the NITF file
  //  is vastly different than the order CARMEN expects them to be in.
  //
  // So, we will need to resequence them in the 'right' order!
  //
  // Their order in the RPC00A tag is (these are obtained by looking at
  //   an NITF file, the IES GPA algorithm, and laborious test):
  //
  //  UNITY(0), L(1), P(2), H(3), LP(4),
  //  LH(5), HP(6), HPL(7), LL(8), PP(9),
  //  HH(10), LLL(11), LLP(12), LLH(13), LPP(14),
  //  PPP(15), HPP(16), LHH(17), PHH(18), HHH(19)
  // [where L is longitude, P is latitude, H is height]
  //
  // The order in the RPC00B tag (as described in STDI-0002, v2.1
  // dated 16 November 2000) is:
  //
  //  UNITY(0), L(1), P(2), H(3), LP(4),
  //  LH(5), HP(6), LL(7), PP(8), HH(9),
  //  PLH(10), LLL(11), LPP(12), LHH(13), LLP(14),
  //  PPP(15), PHH(16), LLH(17), PPH(18), HHH(19)
  // [where L is longitude, P is latitude, H is height]
  //
  // Their order in CARMEN is (obtained by looking at PowerVector):
  //
  //  XXX(0), XXY(1), XXZ(2), XX(3), XYY(4),
  //  XYZ(5), XY(6), XZZ(7), XZ(8), X(9),
  //  YYY(10), YYZ(11), YY(12), YZZ(13), YZ(14),
  //  Y(15), ZZZ(16), ZZ(17), Z(18), UNITY(19)
  // [where X is longitude, Y is latitude, Z is height]
  //

  if (this->RPC_present == true)
  {
      // this maps from NITF RPC00A order to CARMEN order
      int NITF_index_RPC00A[20] = { 11 , 12 , 13 ,  8 , 14 , 7 ,  4 , 17 , 5 , 1 ,
                                    15 , 16 ,  9 , 18 ,  6 , 2 , 19 , 10 , 3 , 0 };

      // this maps from NITF RPC00B order to CARMEN order
      int NITF_index_RPC00B[20] = { 11 , 14 , 17 ,  7 , 12 , 10,  4 , 13 , 5 , 1 ,
                                    15 , 18 ,  8 , 16 ,  6 , 2 , 19 ,  9 , 3 , 0 };

      int* NITF_index; // Point this to the right indexing array for the tag (A/B)
      switch (RPC_TYPE)
      {
       case RPC00A:
        NITF_index = NITF_index_RPC00A;
        if (debug_level > 1) {
          vcl_cout << "Switching to RPC00A mode\n";
        }
        break;
       case RPC00B:
        NITF_index = NITF_index_RPC00B;
        if (debug_level > 1) {
          vcl_cout << "Switching to RPC00B mode\n";
        }
        break;
       case UNDEFINED:
       default:
        // Should NEVER get here!
        vcl_cout << "UNKNOWN RPC tag present in this image!\n"
                 << "RPC_TYPE = " << RPC_TYPE << vcl_endl
                 << "Know Types are: " << RPC00A << ", " << RPC00B << vcl_endl;
        return 0;
      }

      // Copy (and scramble) all four polynomials.
      for (int index = 0; index < 20; index++)
      {
          samp_num[index] = this->SAMP_NUM[NITF_index[index]];
          samp_denom[index] = this->SAMP_DEN[NITF_index[index]];
          line_num[index] = this->LINE_NUM[NITF_index[index]];
          line_denom[index] = this->LINE_DEN[NITF_index[index]];
      }

      // Now, the easy part. Copy scale and offset parameters for all of
      //  of the variables (latitude/longitude/height/line/sample).

      // Okay, copy from the subheader into local variables.
      scalex[O_SCALE] = this->LONG_SCALE;
      scalex[O_OFFSET] = this->LONG_OFF;
      scaley[O_SCALE] = this->LAT_SCALE;
      scaley[O_OFFSET] = this->LAT_OFF;
      scalez[O_SCALE] = this->HEIGHT_SCALE;
      scalez[O_OFFSET] = this->HEIGHT_OFF;
      scalel[O_SCALE] = this->LINE_SCALE;
      scalel[O_OFFSET] = this->LINE_OFF;
      scales[O_SCALE] = this->SAMP_SCALE;
      scales[O_OFFSET] = this->SAMP_OFF;
    }
#ifdef WITH_SAR_RPC_EXTRACT
  else if ((vcl_strncmp (this->ICAT, "SAR", 3) == 0)) {
      sarrpcextractor extractor (this->XSHD->XHD,(long) this->XSHD->GetHeaderLength());
      if (!extractor.extractRPC()) {
        vcl_cout << method_name
                 << "RPC data not present or invalid for this image.\n";
          return 0;
      }
      // FIGURE OUT WHAT TO DO ABOUT THIS.  matx WAS DEFINED AS
      // vnl_matrix<double> matx (4, 20)   TBD: MAL 11dec2003
      extractor.populateCameraData (matx, scalex, scaley, scalez, scales, scalel);
      this->HEIGHT_OFF = extractor.getHeightOffset();
  }
#endif

  // Set the init_pt to be the centroid of the four world corner points
  // useful for computation.LEG.2/1/01
  double lat_min, lat_max, lon_min, lon_max;
  GetImageBounds (lat_min, lat_max, lon_min, lon_max);

  init_pt[0] = (lat_min + lat_max) / 2;
  init_pt[1] = (lon_min + lon_max) / 2;
  init_pt[2] = scalez[O_OFFSET];

  //
  // Get image chip information (obtained from I2MAPD).
  //

  // Grab data from NITF data structure and store locally.
  // reduced resolution data set (RRDS) of chip
  double scale_factor = 1;
  int ul_line = 0;
  int ul_sample = 0;

  if (I2MAPD_present) { //this is archaic and should not dominate
      scale_factor = 1 << this->RRDSset;
      ul_line  = this->ULl;
      ul_sample = this->ULs;
  }

  if (ICHIPB_present) { // the modern standard

      scale_factor = this->SCALE_FACTOR;
      ul_line  = int ((FI_ROW_11 + 0.5) - 1.0);
      ul_sample = int ((FI_COL_11 + 0.5) - 1.0);

      if (debug_level > 1) {
        vcl_cout << "ICHIPB Present - offsetting Rational Camera by ("
                 << ul_sample << ' ' << ul_line << ")\n";
      }
  }
  // Use this to rescale the image
  double scale = 1 / scale_factor;

  rescales[O_SCALE] = scale;
  rescales[O_OFFSET] = -scale * ul_sample;
  rescalel[O_SCALE] = scale;
  rescalel[O_OFFSET] = -scale * ul_line;

  // Display camera data - debugging output
  if (debug_level > 1) {
    vcl_cout << vcl_endl;
    display_camera_attributes (method_name);
    vcl_cout << vcl_endl;
  }

  // IF WE GOT HERE, EVERYTHING SET OK.
  return true;

}  // end method get_rational_camera_data

////////////////////////////////////////////////////////
//:
// Get the upper left, upper right, lower right, lower left coordinates of an image
// as four vectors containing longitude, latitude and elevation for each corner.
// Used to verify rational camera class, but may be useful for other stuff.
//
//  \param UL vector to be filled in with longitude, latitude and elevation for upper left
//  \param UR vector to be filled in with longitude, latitude and elevation for upper right
//  \param LR vector to be filled in with longitude, latitude and elevation for lower right
//  \param LL vector to be filled in with longitude, latitude and elevation for lower left
//
//  \return true if vectors filled in successfully
//
////////////////////////////////////////////////////////
bool vil_nitf_image_subheader::get_image_corners (
    vcl_vector<double>& UL,
    vcl_vector<double>& UR,
    vcl_vector<double>& LR,
    vcl_vector<double>& LL) const
{
  static vcl_string method_name = "vil_nitf_image_subheader::get_image_corners: ";

  double ULlat, ULlon;
  double URlat, URlon;
  double LLlat, LLlon;
  double LRlat, LRlon;

  // Extract values from the IGEOLO field
  geostr_to_latlon (IGEOLO,    &ULlat, &ULlon);
  geostr_to_latlon (IGEOLO+15, &URlat, &URlon);
  geostr_to_latlon (IGEOLO+30, &LRlat, &LRlon);
  geostr_to_latlon (IGEOLO+45, &LLlat, &LLlon);

  // Guess at the elevation
  double mean_elev = HEIGHT_OFF;

  // Fill in vectors
  UL[0] = ULlon;
  UL[1] = ULlat;
  UL[2] = mean_elev;

  UR[0] = URlon;
  UR[1] = URlat;
  UR[2] = mean_elev;

  LR[0] = LRlon;
  LR[1] = LRlat;
  LR[2] = mean_elev;

  LL[0] = LLlon;
  LL[1] = LLlat;
  LL[2] = mean_elev;

  // IF WE GOT HERE, EVERYTHING SET OK.
  return true;
}  // end method get_image_corners

// END RATIONAL CAMERA STUFF.

////////////////////////////////////////////////////////////////////////
//: Display attributes related to defining a rational polynomial camera.
////////////////////////////////////////////////////////////////////////
void vil_nitf_image_subheader::display_camera_attributes (vcl_string caller) const
{
    static vcl_string method_name = "vil_nitf_image_subheader::display_camera_attributes: ";

    // Only display method name if caller length is not zero.
    // Don't want method name if called from display_attributes.
    // display_attributes already does this.

    if (caller.length() > 0) {
        vcl_cout << method_name << " from " << caller << vcl_endl;
    }

    if (this->RPC_present == false) {
        vcl_cout << method_name
                 << "RPC data not present or invalid for this image.\n";
        return;
    }

    vcl_cout << "##### start RPC camera data #####\n"
             << "ICAT (image category) = " << this->ICAT << vcl_endl;

#ifdef WITH_SAR_RPC_EXTRACT
    // SAR = Synthetic Aperature Radar
    if (vcl_strncmp (this->ICAT, "SAR", 3) != 0)
      vcl_cout << "WARNING: ICAT != \"SAR\".  Necessary for RPC ?\n";
#endif

    vcl_cout << "LONG_SCALE = " << this->LONG_SCALE << vcl_endl
             << "LONG_OFF = " << this->LONG_OFF << vcl_endl
             << "LAT_SCALE = " << this->LAT_SCALE << vcl_endl
             << "LAT_OFF = " << this->LAT_OFF << vcl_endl
             << "HEIGHT_SCALE = " << this->HEIGHT_SCALE << vcl_endl
             << "HEIGHT_OFF = " << this->HEIGHT_OFF << vcl_endl
             << "LINE_SCALE = " << this->LINE_SCALE << vcl_endl
             << "LINE_OFF = " << this->LINE_OFF << vcl_endl
             << "SAMP_SCALE = " << this->SAMP_SCALE << vcl_endl
             << "SAMP_OFF = " << this->SAMP_OFF << vcl_endl

             << "##### end RPC camera data #####\n";

}  // end method display_camera_attributes

////////////////////////////////////////////////////////////////////////
//: Display attributes related to size of the image.
// Also check size of image using three checks:
//     image size = # columns X # rows X bytes per pixel
//     number of columns = # blocks per row X block size horizontal
//     number of rows = # blocks per column X block size vertical
////////////////////////////////////////////////////////////////////////
void vil_nitf_image_subheader::display_size_attributes (vcl_string caller) const
{
    static vcl_string method_name = "vil_nitf_image_subheader::display_size_attributes: ";

    // Only display method name if caller length is not zero.
    // Don't want method name if called from display_attributes.
    // display_attributes already does this.

    if (caller.length() > 0) {
        vcl_cout << method_name
                 << " from " << caller
                 << vcl_endl;
    }

    vcl_cout << "NCOLS (image cols) = " << this->NCOLS << vcl_endl
             << "NROWS (image rows) = " << this->NROWS << vcl_endl
             << "NBANDS (number bands) = " << this->NBANDS << vcl_endl
             << "NBPR (number of blocks per row) = " << this->NBPR << vcl_endl;
    if (this->NBPR) {
      vcl_cout << "NPPBH (number of pixels per block horizontal) = "
               << this->NPPBH << vcl_endl;
    }
    vcl_cout << "NBPC (number of blocks per column) = " << this->NBPC << vcl_endl;
    if (this->NBPC) {
      vcl_cout << "NPPBV (number of pixels per block vertical) = "
               << this->NPPBV << vcl_endl;
    }

    // CHECK VERTICAL IMAGE SIZE -> NCOLS = NBPR X NPPBH
    unsigned long calculated_pixels = this->NBPR * this->NPPBH;
    vcl_cout << "calculated columns = NBPR X NPPBH = " << calculated_pixels
             << vcl_endl;
    if (calculated_pixels != this->NCOLS) {
        long diff = (long)calculated_pixels - (long)this->NCOLS;
        vcl_cout << "WARNING: (NBPR X NPPBH) != NCOLS."
                 << "  Difference = " << diff << vcl_endl;
    }

    // CHECK VERTICAL IMAGE SIZE -> NROWS = NBPC X NPPBV
    calculated_pixels = this->NBPC * this->NPPBV;
    vcl_cout << "calculated rows = NBPC X NPPBV = " << calculated_pixels
             << vcl_endl;
    if (calculated_pixels != this->NROWS) {
        long diff = (long)calculated_pixels - (long)this->NROWS;
        vcl_cout << "WARNING: (NBPC X NPPBV) != NROWS."
                 << "  Difference = " << diff << vcl_endl;
    }

    vcl_cout << "NBPP (stored bits per pixel) = " << this->NBPP << vcl_endl
             << "ABPP (actual bits per pixel per band) = "
             << this->ABPP << vcl_endl

             << "PJUST (justification for ABPP) = " << this->PJUST << vcl_endl
             << "PVTYPE = " << this->PVTYPE_ << vcl_endl;

    unsigned int bytes_per_pixel = NBPP / 8;
    if ((NBPP % 8) != 0) {
        vcl_cerr << "WARNING: NBBP is not a multiple of 8.\n";
    }
    vcl_cout << "IMODE (image mode = interleave type for pixels) = "
             << this->IMODE_ << vcl_endl;

    // CHECK IMAGE SIZE = NCOLS X NROWS X (NBPP / 8) X NBANDS
    vcl_cout << "image data size = " << this->get_data_length() << vcl_endl;
    unsigned long image_size = this->NCOLS * this->NROWS * bytes_per_pixel * this->NBANDS;
    vcl_cout << "calculated image data size using NCOLS X NROWS X bytes_per_pixel X NBANDS = "
             << image_size << vcl_endl;
    if (image_size != this->get_data_length())
    {
        long diff = (long)image_size - (long)this->get_data_length();
        vcl_cout << "WARNING: calculated image size using pixels != image size in header.\n"
                 << "Difference = " << diff << vcl_endl;
    }
    else {
        vcl_cout << "Calculated image size using pixels OK.\n";
    }

    // CHECK IMAGE SIZE = bytes per block X blocks per row X blocks per column X NBANDS
    if ((this->NBPR > 0) || (this->NBPC > 0))
    {
      unsigned long bytes_per_block = NPPBH * NPPBV * bytes_per_pixel;
      image_size = bytes_per_block * NBPR * NBPC;

      vcl_cout << "bytes_per_block = " << bytes_per_block << vcl_endl
               << "calculated image data size using # bytes per block X # blocks per row X # blocks per column = "
               << image_size << vcl_endl;

        if (this->NBANDS > 1) {
            image_size *= this->NBANDS;
            vcl_cout << "calculated image data size X NBANDS = "
                     << image_size << vcl_endl;
        }
        if (image_size != this->get_data_length()) {
            long diff = (long)image_size - (long)this->get_data_length();
            vcl_cout << "WARNING: calculated image size != image size in header.\n"
                     << "Difference = " << diff << vcl_endl;
        }
        else {
            vcl_cout << "Calculated image size using blocks OK.\n";
        }
    }

}  // end method display_size_attributes
//void vil_nitf_image_subheader::display_attributes (vcl_ostream out) const
void vil_nitf_image_subheader::display_attributes (vcl_string caller) const
{
    static vcl_string method_name = "vil_nitf_image_subheader::display_attributes: ";

    vcl_cout << "\n##### " << method_name;
    if (caller.length() > 0) {
      vcl_cout << " from " << caller;
    }
    vcl_cout << "\nIDATIM (image date and time) = " << this->IDATIM_ << vcl_endl;

    display_size_attributes ("");

    vcl_cout << "NBANDS (number of bands) = " << this->NBANDS << vcl_endl
             << "IMODE (image mode) = " << this->IMODE_ << vcl_endl

             << "IREP (image representation) = <" << this->IREP << ">\n"
             << "ICAT (image category) = " << this->ICAT << vcl_endl

             << "isCompressed = " << this->IsCompressed() << vcl_endl
             << "IC (compression code) = " << this->IC << vcl_endl

             << "ICORDS = " << this->ICORDS << vcl_endl

             << "##### exit " << method_name;

}  // end method display_attributes
