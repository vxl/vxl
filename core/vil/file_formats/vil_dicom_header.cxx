// DicomHeaderFormat.cxx
// =====================
//
// The implementation of the vil_dicom_header_format class to read
// and write the header part of a dicom file.
//
// Author: Chris Wolstenholme
// E-mail: cwolstenholme@imorphics.com
// Copyright (c) 2001 iMorphics Ltd

#include "vil_dicom_header.h"
#include <vil/vil_stream.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

//================================================================

void
vil_dicom_header_info_clear( vil_dicom_header_info& info )
{
  // Clear all the elements of the info structure

  // Identity info
  info.file_type_ = VIL_DICOM_HEADER_DTUNKNOWN;
  info.file_endian_ = VIL_DICOM_HEADER_DEUNKNOWN;
  info.sys_endian_ = VIL_DICOM_HEADER_DEUNKNOWN;
  info.image_type_ = VIL_DICOM_HEADER_DITUNKNOWN;
  info.image_id_type_ = "";
  info.sop_cl_uid_ = "";
  info.sop_in_uid_ = "";
  info.study_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.series_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.acquisition_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.image_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.study_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.series_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.acquisition_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.image_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.accession_number_ = "";
  info.modality_ = "";
  info.manufacturer_ = "";
  info.institution_name_ = "";
  info.institution_addr_ = "";
  info.ref_phys_name_ = "";
  info.station_name_ = "";
  info.study_desc_ = "";
  info.att_phys_name_ = "";
  info.operator_name_ = "";
  info.model_name_ = "";

  // Patient info
  info.patient_name_ = "";
  info.patient_id_ = "";
  info.patient_dob_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.patient_sex_ = "";
  info.patient_age_ = "";
  info.patient_weight_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.patient_hist_ = "";

  // Acquisition info
  info.scanning_seq_ = "";
  info.sequence_var_ = "";
  info.scan_options_ = "";
  info.mr_acq_type_ = "";
  info.sequence_name_ = "";
  info.angio_flag_ = "";
  info.slice_thickness_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.repetition_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.echo_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.inversion_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.number_of_averages_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.echo_numbers_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.mag_field_strength_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.echo_train_length_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.pixel_bandwidth_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.software_vers_ = "";
  info.protocol_name_ = "";
  info.heart_rate_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.card_num_images_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.trigger_window_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.reconst_diameter_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.receiving_coil_ = "";
  info.phase_enc_dir_ = "";
  info.flip_angle_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.sar_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.patient_pos_ = "";

  // Relationship info
  info.stud_ins_uid_= "";
  info.ser_ins_uid_ = "";
  info.study_id_ = "";
  info.series_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.acquisition_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.image_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.pat_orient_ = "";
  info.image_pos_ = 0;
  info.image_orient_ = 0;
  info.frame_of_ref_ = "";
  info.images_in_acq_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.pos_ref_ind_ = "";
  info.slice_location_ = VIL_DICOM_HEADER_UNSPECIFIED;

  // Image info
  info.pix_samps_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.photo_interp_ = "";
  info.size_x_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.size_y_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.size_z_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.high_bit_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.small_im_pix_val_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.large_im_pix_val_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.pixel_padding_val_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.window_centre_ = VIL_DICOM_HEADER_UNSPECIFIED;
  info.window_width_ = VIL_DICOM_HEADER_UNSPECIFIED;

  // Info for loading image
  info.spacing_x_ = VIL_DICOM_HEADER_DEFAULTSIZE_FLOAT;
  info.spacing_y_ = VIL_DICOM_HEADER_DEFAULTSIZE_FLOAT;
  info.spacing_slice_ = VIL_DICOM_HEADER_DEFAULTSIZE_FLOAT;
  info.res_intercept_ = VIL_DICOM_HEADER_DEFAULTINTERCEPT;
  info.res_slope_ = VIL_DICOM_HEADER_DEFAULTSLOPE;
  info.pix_rep_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.stored_bits_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  info.allocated_bits_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
}



//================================================================
// Default constructor
//================================================================
vil_dicom_header_format::vil_dicom_header_format() :
info_valid_(false),
file_endian_(VIL_DICOM_HEADER_DEUNKNOWN),
image_type_(VIL_DICOM_HEADER_DITUNKNOWN)
{
  // Work out the endianism of this architecture
  endian_ = calculateEndian();
}

//================================================================

vil_dicom_header_format::~vil_dicom_header_format()
{
  // Do Nothing
}

//================================================================

bool vil_dicom_header_format::isDicomFormat(vil_stream &fs)
{
  vil_dicom_header_type dtype;

  dtype = determineFileType(fs);

  return dtype!=VIL_DICOM_HEADER_DTUNKNOWN;
}

//================================================================

vil_dicom_header_info vil_dicom_header_format::readHeader(vil_stream &fs)
{
  vil_dicom_header_type dtype;

  // Clear the current header
  clearInfo();

  dtype = determineFileType(fs);
  // Check if the file is dicom first
  if (dtype != VIL_DICOM_HEADER_DTUNKNOWN)
  {
    last_read_.file_type_ = dtype;
    last_read_.sys_endian_ = systemEndian();

    if (dtype == VIL_DICOM_HEADER_DTPART10)
    {
      file_endian_ = determineMetaInfo(fs);
    }

    last_read_.file_endian_ = fileEndian();
    last_read_.image_type_ = imageType();

    readHeaderElements(fs);

    info_valid_ = true;
  } // End of if (dtype != VIL_DICOM_HEADER_DTUNKNOWN)
  else // It's not a dicom file, so can't read
  {
    vcl_cerr<<"Unknown file type - not a DICOM file...\n"
            <<"File header not read\n";
  }
  return last_read_;
}

//================================================================

vil_dicom_header_info vil_dicom_header_format::lastHeader(void)
{
  return last_read_;
}

//================================================================

bool vil_dicom_header_format::headerValid(void)
{
return info_valid_;
}

//================================================================

vil_dicom_header_endian vil_dicom_header_format::systemEndian(void)
{
  return endian_;
}

//================================================================

vil_dicom_header_endian vil_dicom_header_format::fileEndian(void)
{
  return file_endian_;
}

//================================================================

vil_dicom_header_image_type vil_dicom_header_format::imageType(void)
{
  return image_type_;
}

//================================================================

vil_dicom_header_type vil_dicom_header_format::determineFileType(vil_stream &fs)
{
  vil_dicom_header_type result = VIL_DICOM_HEADER_DTUNKNOWN;

  // Check the file is open, otherwise fail - the vcl_fstream should
  // be controlled outside of this class
  if (fs.ok())
  {
    // There are four possibilities:
    // The file is Part10 with 128 byte pre-amble
    // The file is Part10 with no pre-amble
    // The file is a non-Part10 dicom file
    // The file is not a dicom file (or can't be determined as such)
    char dicm_read[5];
    vcl_string dicm_test;

    // Skip 128 byte pre-amble and test for DICM again
    fs.seek(128);
    fs.read(dicm_read,4);
    dicm_read[4]=0;
    dicm_test=dicm_read;

    if (dicm_test == "DICM")
    {
      result = VIL_DICOM_HEADER_DTPART10;
    }
    else
    {
      // Read the first four chars and see if they are the letters
      // DICM
      fs.seek(0);
      fs.read(dicm_read,4);
      dicm_read[4]=0;    // Null terminate
      dicm_test=dicm_read;

      if (dicm_test == "DICM")
      {
        result = VIL_DICOM_HEADER_DTPART10;
      }
      else
      {
        // Some other format - test it with both little and
        // big endian
        int num_tries = 0;
        bool known = false;
        vil_dicom_header_endian old_endian = file_endian_;
        file_endian_ = VIL_DICOM_HEADER_DEBIGENDIAN;

        while (num_tries < 2 && !known)
        {
          // Go back to the beginning and search for the
          // first element of the Identifying group
          fs.seek(0);
          vxl_uint_16 group, element;
          vxl_uint_32 data_block_size, num_elements;

          fs.read(&group, sizeof(vxl_uint_16));
          group = shortSwap(group);
          fs.read(&element, sizeof(vxl_uint_16));
          element = shortSwap(element);
          fs.read(&data_block_size, sizeof(vxl_uint_32));
          data_block_size = intSwap(data_block_size);
          if (data_block_size > 0x1000000) {
            vcl_cerr<< __FILE__ << ": " << __LINE__ << " : WARNING:\n"
                    <<"data_block_size=" << data_block_size << " is most probably too large\n";
            break;
          }

          num_elements = 0;

          while (group < VIL_DICOM_HEADER_IDENTIFYINGGROUP &&
                 num_elements < VIL_DICOM_HEADER_MAXHEADERSIZE &&
                 fs.ok())
          {
            // It's not what we want, so skip it and
            // get the next one
            fs.seek(data_block_size + fs.tell());


            fs.read(&group, sizeof(vxl_uint_16));
            group = shortSwap(group);
            fs.read(&element, sizeof(vxl_uint_16));
            element = shortSwap(element);
            fs.read(&data_block_size, sizeof(vxl_uint_32));
            data_block_size = intSwap(data_block_size);
            if (data_block_size > 0x1000000) {
              vcl_cerr<< __FILE__ << ": " << __LINE__ << " : WARNING:\n"
                      <<"data_block_size=" << data_block_size << " is most probably too large\n";
              break;
            }

            num_elements++;
          } // End of while (group...)

          // Check the elements read and see if it fits
          // with a known header
          if (group == VIL_DICOM_HEADER_IDENTIFYINGGROUP)
          {
            // First, standard non-Part10 header
            if (element == VIL_DICOM_HEADER_IDGROUPLENGTH &&
                data_block_size == 4)
            {
              // Put the file back at the beginning
              fs.seek(0);
              result = VIL_DICOM_HEADER_DTNON_PART10;
              known = true;
            } // End of if (element...)
            // Now a non-standard non-Part10
            else if (element == VIL_DICOM_HEADER_IDIMAGETYPE ||
                     element == VIL_DICOM_HEADER_IDLENGTHTOEND ||
                     element == VIL_DICOM_HEADER_IDSPECIFICCHARACTER)
            {
              // Put the file back at the beginning
              fs.seek(0);
              result = VIL_DICOM_HEADER_DTNON_PART10;
              known = true;
            } // End of else if (element...)
          } // End of if (group == VIL_DICOM_HEADER_IDENTIFYINGGROUP)

          if (!known)
          {
            file_endian_ = VIL_DICOM_HEADER_DELITTLEENDIAN;
          }

          num_tries++;
        } // End of while (num_tries < 2 && !known)

        if (!known)
        {
          file_endian_ = old_endian;
        }
      } // End of else
    } // End of else
  } // End of if (fs.ok())
  else
  {
    vcl_cerr << "File not open for reading:\n"
             << "vil_dicom_header_format::determineFileType()\n";
  } // End of else

  return result;
}

//================================================================

void vil_dicom_header_format::readHeaderElements(vil_stream &fs)
{
  vxl_uint_16 group, element;  // The groups and elements read from the header part of the dicom file
  vxl_uint_32 data_block_size; // The size of the information held for this group/element pair

  // Read the first group/element pair
  fs.read(&group, sizeof(vxl_uint_16));
  fs.read(&element, sizeof(vxl_uint_16));

  // Swap them if necessary
  group = shortSwap(group);
  element = shortSwap(element);

  // Loop until the file ends (unexpectedly!) or the
  // pixel data is found
  while (fs.ok() && !pixelDataFound(group, element))
  {
    if (sizeof(vxl_uint_32) != fs.read(&data_block_size, sizeof(vxl_uint_32)))
      break;
    data_block_size = intSwap(data_block_size);
    if (data_block_size > 0x1000000) {
      vcl_cerr<< __FILE__ << ": " << __LINE__ << " : WARNING:\n"
              <<"data_block_size=" << data_block_size << " is most probably too large\n";
      break;
    }
    convertValueRepresentation(data_block_size, fs);

    switch (group)
    {
     // Read identifying info
     case VIL_DICOM_HEADER_IDENTIFYINGGROUP:
      readIdentifyingElements(element, data_block_size, fs);
      break;

     // Read the patient info
     case VIL_DICOM_HEADER_PATIENTINFOGROUP:
      readPatientElements(element, data_block_size, fs);
      break;

     case VIL_DICOM_HEADER_ACQUISITIONGROUP:
      readAcquisitionElements(element, data_block_size, fs);
      break;

     case VIL_DICOM_HEADER_RELATIONSHIPGROUP:
      readRelationshipElements(element, data_block_size, fs);
      break;

     case VIL_DICOM_HEADER_IMAGEGROUP:
      readImageElements(element, data_block_size, fs);
      break;

     case VIL_DICOM_HEADER_DELIMITERGROUP:
      readDelimiterElements(element, data_block_size, fs);
      break;

     // Nothing found, so skip this data block
     default:
      fs.seek(data_block_size + fs.tell());
      break;
    } // End of switch

    // Read the next group and element
    fs.read(&group, sizeof(vxl_uint_16));
    fs.read(&element, sizeof(vxl_uint_16));

    // Swap them if necessary
    group = shortSwap(group);
    element = shortSwap(element);
  } // End of while


  // Read the final block size info - throw away!
  if (sizeof(vxl_uint_32) != fs.read(&data_block_size, sizeof(vxl_uint_32)))
    return;
  data_block_size = intSwap(data_block_size);
  if (data_block_size > 0x1000000)
    vcl_cerr << __FILE__ << ": " << __LINE__ << " : WARNING\n"
             <<"data_block_size=" << data_block_size << " is most probably too large\n";
  else
    convertValueRepresentation(data_block_size, fs);
}

//================================================================

// These macros will be used a lot of times in the subsequent read* methods
#define CASE(X,M,F) \
   case X : \
    data_p = new char[dblock_size+1]; /* Ensure room for 0 */ \
    if (data_p) \
    { \
      fs.read(data_p,dblock_size); \
      data_p[dblock_size]=0; \
      last_read_.M = F(data_p); \
    } \
    break

#define CASE_SWP(X,M) \
   case X : \
    data_p = new char[dblock_size+1]; /* Ensure room for 0 */ \
    if (data_p) \
    { \
      fs.read(data_p,dblock_size); \
      data_p[dblock_size]=0; \
      charSwap(data_p, sizeof(vxl_uint_16)); \
      last_read_.M = *((vxl_uint_16*)data_p); \
    } \
    break

void vil_dicom_header_format::readIdentifyingElements(short element,
                                                      int dblock_size,
                                                      vil_stream &fs)
{
  // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   CASE(VIL_DICOM_HEADER_IDIMAGETYPE,         image_id_type_, (char *)); // It's the image type
   CASE(VIL_DICOM_HEADER_IDSOPCLASSID,        sop_cl_uid_, (char *)); // It's the SOP class ID
   CASE(VIL_DICOM_HEADER_IDSOPINSTANCEID,     sop_in_uid_, (char *)); // It's the SOP instance ID
   CASE(VIL_DICOM_HEADER_IDSTUDYDATE,         study_date_,vcl_atol); // It's the study date
   CASE(VIL_DICOM_HEADER_IDSERIESDATE,        series_date_,vcl_atol); // It's the series date
   CASE(VIL_DICOM_HEADER_IDACQUISITIONDATE,   acquisition_date_,vcl_atol); // It's the acquisition date
   CASE(VIL_DICOM_HEADER_IDIMAGEDATE,         image_date_,vcl_atol); // It's the image date
   CASE(VIL_DICOM_HEADER_IDSTUDYTIME,         study_time_,(float)vcl_atof); // It's the study time
   CASE(VIL_DICOM_HEADER_IDSERIESTIME,        series_time_,(float)vcl_atof); // It's the series time
   CASE(VIL_DICOM_HEADER_IDACQUISITIONTIME,   acquisition_time_,(float)vcl_atof); // It's the acquisition time
   CASE(VIL_DICOM_HEADER_IDIMAGETIME,         image_time_,(float)vcl_atof); // It's the image time
   CASE(VIL_DICOM_HEADER_IDACCESSIONNUMBER,   accession_number_, (char *)); // It's the accession number
   CASE(VIL_DICOM_HEADER_IDMODALITY,          modality_, (char *)); // It's the imaging modality
   CASE(VIL_DICOM_HEADER_IDMANUFACTURER,      manufacturer_, (char *)); // It's the manufacturer name
   CASE(VIL_DICOM_HEADER_IDINSTITUTIONNAME,   institution_name_, (char *)); // It's the institution name
   CASE(VIL_DICOM_HEADER_IDINSTITUTIONADDRESS,institution_addr_, (char *)); // It's the institution address
   CASE(VIL_DICOM_HEADER_IDREFERRINGPHYSICIAN,ref_phys_name_, (char *)); // It's the referring physician's name
   CASE(VIL_DICOM_HEADER_IDSTATIONNAME,       station_name_, (char *)); // It's the imaging station name
   CASE(VIL_DICOM_HEADER_IDSTUDYDESCRIPTION,  study_desc_, (char *)); // It's the study description
   CASE(VIL_DICOM_HEADER_IDSERIESDESCRIPTION, series_desc_, (char *)); // It's the series description
   CASE(VIL_DICOM_HEADER_IDATTENDINGPHYSICIAN,att_phys_name_, (char *)); // It's the name of the attending physician
   CASE(VIL_DICOM_HEADER_IDOPERATORNAME,      operator_name_, (char *)); // It's the name of the scanner operator
   CASE(VIL_DICOM_HEADER_IDMANUFACTURERMODEL, model_name_, (char *)); // It's the scanner model
   default: // It's nothing we want, so skip it!
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch

  delete[] data_p;
}

//================================================================

void vil_dicom_header_format::readPatientElements(short element,
                                                  int dblock_size,
                                                  vil_stream &fs)
{
  // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   CASE(VIL_DICOM_HEADER_PIPATIENTNAME,     patient_name_, (char *)); // It's the patient's name
   CASE(VIL_DICOM_HEADER_PIPATIENTID,       patient_id_, (char *)); // It's the patient's id
   CASE(VIL_DICOM_HEADER_PIPATIENTBIRTHDATE,patient_dob_,vcl_atol); // It's the patient's date of birth
   CASE(VIL_DICOM_HEADER_PIPATIENTSEX,      patient_sex_, (char *)); // It's the patient's sex
   CASE(VIL_DICOM_HEADER_PIPATIENTAGE,      patient_age_, (char *)); // It's the patient's age
   CASE(VIL_DICOM_HEADER_PIPATIENTWEIGHT,   patient_weight_,(float)vcl_atof); // It's the patient's weight
   CASE(VIL_DICOM_HEADER_PIPATIENTHISTORY,  patient_hist_, (char *)); // It's the patient's history
   default: // It's nothing we want, so skip it!
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch

  delete[] data_p;
}

//================================================================

void vil_dicom_header_format::readAcquisitionElements(short element,
                                                      int dblock_size,
                                                      vil_stream &fs)
{
  // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   CASE(VIL_DICOM_HEADER_AQSCANNINGSEQUENCE,      scanning_seq_, (char *)); // It's the scanning sequence
   CASE(VIL_DICOM_HEADER_AQSEQUENCEVARIANT,       sequence_var_, (char *)); // It's the sequence variant
   CASE(VIL_DICOM_HEADER_AQSCANOPTIONS,           scan_options_, (char *)); // It's the scan options
   CASE(VIL_DICOM_HEADER_AQMRACQUISITIONTYPE,     mr_acq_type_, (char *)); // It's the MR acquisition type
   CASE(VIL_DICOM_HEADER_AQSEQUENCENAME,          sequence_name_, (char *)); // It's the sequence name
   CASE(VIL_DICOM_HEADER_AQANGIOFLAG,             angio_flag_, (char *)); // It's the angio flag
   CASE(VIL_DICOM_HEADER_AQSLICETHICKNESS,        slice_thickness_,(float)vcl_atof); // It's the slice thickness
   CASE(VIL_DICOM_HEADER_AQREPETITIONTIME,        repetition_time_,(float)vcl_atof); // It's the repetition time
   CASE(VIL_DICOM_HEADER_AQECHOTIME,              echo_time_,(float)vcl_atof); // It's the echo time
   CASE(VIL_DICOM_HEADER_AQINVERSIONTIME,         inversion_time_,(float)vcl_atof); // It's the inversion time
   CASE(VIL_DICOM_HEADER_AQNUMBEROFAVERAGES,      number_of_averages_,(float)vcl_atof); // It's the number of averages
   CASE(VIL_DICOM_HEADER_AQECHONUMBERS,           echo_numbers_,vcl_atoi); // It's the echo numbers
   CASE(VIL_DICOM_HEADER_AQMAGNETICFIELDSTRENGTH, mag_field_strength_,(float)vcl_atof); // It's the magnetic field strength
   CASE(VIL_DICOM_HEADER_AQSLICESPACING,          spacing_slice_,(float)vcl_atof); // It's the slice spacing
   CASE(VIL_DICOM_HEADER_AQECHOTRAINLENGTH,       echo_train_length_,(float)vcl_atof); // It's the echo train length
   CASE(VIL_DICOM_HEADER_AQPIXELBANDWIDTH,        pixel_bandwidth_,(float)vcl_atof); // It's the pixel bandwidth
   CASE(VIL_DICOM_HEADER_AQSOFTWAREVERSION,       software_vers_, (char *)); // It's the scanner software version
   CASE(VIL_DICOM_HEADER_AQPROTOCOLNAME,          protocol_name_, (char *)); // It's the protocol name
   CASE(VIL_DICOM_HEADER_AQHEARTRATE,             heart_rate_,vcl_atoi); // It's the heart rate
   CASE(VIL_DICOM_HEADER_AQCARDIACNUMBEROFIMAGES, card_num_images_,vcl_atoi); // It's the cardiac number of images
   CASE(VIL_DICOM_HEADER_AQTRIGGERWINDOW,         trigger_window_,vcl_atoi); // It's the trigger window
   CASE(VIL_DICOM_HEADER_AQRECONTRUCTIONDIAMETER, reconst_diameter_,(float)vcl_atof); // It's the reconstruction diameter
   CASE(VIL_DICOM_HEADER_AQRECEIVINGCOIL,         receiving_coil_, (char *)); // It's the receiving coil
   CASE(VIL_DICOM_HEADER_AQPHASEENCODINGDIRECTION,phase_enc_dir_, (char *)); // It's the phase encoding direction
   CASE(VIL_DICOM_HEADER_AQFLIPANGLE,             flip_angle_,(float)vcl_atof); // It's the flip angle
   CASE(VIL_DICOM_HEADER_AQSAR,                   sar_,(float)vcl_atof); // It's the sar
   CASE(VIL_DICOM_HEADER_AQPATIENTPOSITION,       patient_pos_, (char *)); // It's the patient position
   default: // It's nothing we want, so skip it!
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch

  delete[] data_p;
}

//================================================================

void vil_dicom_header_format::readRelationshipElements(short element,
                                                       int dblock_size,
                                                       vil_stream &fs)
{
  // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   CASE(VIL_DICOM_HEADER_RSSTUDYINSTANCEUID,   stud_ins_uid_, (char *)); // It's the study instance id
   CASE(VIL_DICOM_HEADER_RSSERIESINSTANCEUID,  ser_ins_uid_, (char *)); // It's the series instance id
   CASE(VIL_DICOM_HEADER_RSSTUDYID,            study_id_, (char *)); // It's the study id
   CASE(VIL_DICOM_HEADER_RSSERIESNUMBER,       series_number_,vcl_atoi); // It's the series number
   CASE(VIL_DICOM_HEADER_RSAQUISITIONNUMBER,   acquisition_number_,vcl_atoi); // It's the acqusition number
   CASE(VIL_DICOM_HEADER_RSIMAGENUMBER,        image_number_,vcl_atoi); // It's the image number
   CASE(VIL_DICOM_HEADER_RSPATIENTORIENTATION, pat_orient_, (char *)); // It's the patient orientation
   CASE(VIL_DICOM_HEADER_RSIMAGEPOSITION,      image_pos_, (float)vcl_atof); // It's the image position
   CASE(VIL_DICOM_HEADER_RSIMAGEORIENTATION,   image_orient_, (float)vcl_atof); // It's the image orientation
   CASE(VIL_DICOM_HEADER_RSFRAMEOFREFERENCEUID,frame_of_ref_, (char *)); // It's the frame of reference uid
   CASE(VIL_DICOM_HEADER_RSIMAGESINACQUISITION,images_in_acq_,vcl_atoi); // It's the number of images in the acquisition
   CASE(VIL_DICOM_HEADER_RSPOSITIONREFERENCE,  pos_ref_ind_, (char *)); // It's the position reference
   CASE(VIL_DICOM_HEADER_RSSLICELOCATION,      slice_location_,(float) vcl_atof); // It's the slice location
   default: // It's nothing we want, so skip it!
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch

  delete[] data_p;
}


//================================================================

void vil_dicom_header_format::readImageElements(short element,
                                                int dblock_size,
                                                vil_stream &fs)
{
    // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   CASE_SWP(VIL_DICOM_HEADER_IMSAMPLESPERPIXEL,    pix_samps_); // It's the samples per pixel
   CASE(VIL_DICOM_HEADER_IMPHOTOMETRICINTERP,      photo_interp_, (char *)); // It's the photometric interpretation
   CASE_SWP(VIL_DICOM_HEADER_IMROWS,               size_y_); // It's the rows
   CASE_SWP(VIL_DICOM_HEADER_IMCOLUMNS,            size_x_); // It's the columns
   CASE_SWP(VIL_DICOM_HEADER_IMPLANES,             size_z_); // It's the planes
   CASE_SWP(VIL_DICOM_HEADER_IMBITSALLOCATED,      allocated_bits_); // It's the allocated bits
   CASE_SWP(VIL_DICOM_HEADER_IMBITSSTORED,         stored_bits_); // It's the stored bits info
   CASE_SWP(VIL_DICOM_HEADER_IMHIGHBIT,            high_bit_); // It's the high bit
   CASE_SWP(VIL_DICOM_HEADER_IMPIXELREPRESENTATION,pix_rep_); // It's the pixel representation
   CASE_SWP(VIL_DICOM_HEADER_IMSMALLIMPIXELVALUE,  small_im_pix_val_); // It's the smallest image pixel value
   CASE_SWP(VIL_DICOM_HEADER_IMLARGEIMPIXELVALUE,  large_im_pix_val_); // It's the largest image pixel value
   CASE_SWP(VIL_DICOM_HEADER_IMPIXELPADDINGVALUE,  pixel_padding_val_); // It's the pixel padding value
   CASE(VIL_DICOM_HEADER_IMWINDOWCENTER,           window_centre_,(float) vcl_atof); // It's the window centre
   CASE(VIL_DICOM_HEADER_IMWINDOWWIDTH,            window_width_,(float) vcl_atof); // It's the window width
   CASE(VIL_DICOM_HEADER_IMRESCALEINTERCEPT,       res_intercept_,(float) vcl_atof); // It's the rescale intercept
   CASE(VIL_DICOM_HEADER_IMRESCALESLOPE,           res_slope_,(float) vcl_atof); // It's the rescale slope
   case VIL_DICOM_HEADER_IMPIXELSPACING : // It's the pixel spacing
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.spacing_x_ = (float) vcl_atof(data_p);

      // The y size should come after a '\'
      // If only a 0 is found, ysize = xsize
      char gone = 'x';
      while (gone != 0 && gone != '\\')
      {
        gone = data_p[0];
        for (int i=0; i<dblock_size; i++)
          data_p[i] = data_p[i+1];
      }
      if (gone == '\\')
        last_read_.spacing_y_ = (float) vcl_atof(data_p);
      else
        last_read_.spacing_y_ = (float) last_read_.spacing_x_;
    }
    break;

   default: // It's nothing we want, so skip it!
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch

  delete[] data_p;
}

//================================================================

void vil_dicom_header_format::readDelimiterElements(short element,
                                                    int dblock_size,
                                                    vil_stream &fs)
{
  // Check the elements
  switch ((vxl_uint_16)element)
  {
   case VIL_DICOM_HEADER_DLITEM:
   case VIL_DICOM_HEADER_DLITEMDELIMITATIONITEM:
   case VIL_DICOM_HEADER_DLSEQDELIMITATIONITEM:
    // There's no data block
    break;
    // It's nothing we want, so skip it!
   default:
    fs.seek(dblock_size + fs.tell());
    break;
  } // End of switch
}

//================================================================

bool vil_dicom_header_format::convertValueRepresentation(unsigned int &dblock_size,
                                                         vil_stream &fs)
{
  bool result = false;
  vcl_string first, last;
  char temp[3];

  // Union to convert the int to chars
  union int_char
  {
    vxl_uint_32 int_val;
    char char_val[4];
  } conv_dblock;

  if (last_read_.file_type_ != VIL_DICOM_HEADER_DTUNKNOWN)
  {
    conv_dblock.int_val = dblock_size;

    // Create the strings to check (the latter two positions in
    // the vcl_string are swapped in case of little endian-ness)
    temp[0] = conv_dblock.char_val[0];
    temp[1] = conv_dblock.char_val[1];
    temp[2] = 0;

    first = temp;

    temp[0] = conv_dblock.char_val[3];
    temp[1] = conv_dblock.char_val[2];

    last = temp;

    // Check if VR is a nested sequence (SQ)
    if (first == VIL_DICOM_HEADER_SEQUENCE || last == VIL_DICOM_HEADER_SEQUENCE)
    {
      fs.read(&dblock_size, sizeof(int));
      dblock_size = 0;
      result = true;
    } // End of if (first...)
    else if (first == VIL_DICOM_HEADER_OTHERBYTE ||
             first == VIL_DICOM_HEADER_OTHERWORD ||
             last == VIL_DICOM_HEADER_OTHERBYTE  ||
             last == VIL_DICOM_HEADER_OTHERWORD)
    {
      fs.read(&dblock_size, sizeof(int));
      dblock_size = intSwap(dblock_size);
      result = true;
    } // End of else if (first...)
    else if (dblock_size == VIL_DICOM_HEADER_ALLSET)
    {
      dblock_size = 0;
      result = true;
    } // End of else if (dblock_size == VIL_DICOM_HEADER_ALLSET)
    else if (first == VIL_DICOM_HEADER_APPLICATIONENTRY    ||
             first == VIL_DICOM_HEADER_AGESTRING           ||
             first == VIL_DICOM_HEADER_ATTRIBUTETAG        ||
             first == VIL_DICOM_HEADER_CODESTRING          ||
             first == VIL_DICOM_HEADER_DATE                ||
             first == VIL_DICOM_HEADER_DECIMALSTRING       ||
             first == VIL_DICOM_HEADER_DATETIME            ||
             first == VIL_DICOM_HEADER_FLOATINGPOINTDOUBLE ||
             first == VIL_DICOM_HEADER_FLOATINGPOINTSINGLE ||
             first == VIL_DICOM_HEADER_INTEGERSTRING       ||
             first == VIL_DICOM_HEADER_LONGSTRING          ||
             first == VIL_DICOM_HEADER_LONGTEXT            ||
             first == VIL_DICOM_HEADER_PERSONNAME          ||
             first == VIL_DICOM_HEADER_SHORTSTRING         ||
             first == VIL_DICOM_HEADER_SIGNEDLONG          ||
             first == VIL_DICOM_HEADER_SIGNEDSHORT         ||
             first == VIL_DICOM_HEADER_SHORTTEXT           ||
             first == VIL_DICOM_HEADER_TIME                ||
             first == VIL_DICOM_HEADER_UNIQUEIDENTIFIER    ||
             first == VIL_DICOM_HEADER_UNSIGNEDLONG        ||
             first == VIL_DICOM_HEADER_UNSIGNEDSHORT)
    {
      if (last_read_.sys_endian_ == VIL_DICOM_HEADER_DELITTLEENDIAN)
      {
        dblock_size = (unsigned int)((256*conv_dblock.char_val[3]) + conv_dblock.char_val[2]);
      }
      else
      {
        dblock_size = (unsigned int)((256*conv_dblock.char_val[2]) + conv_dblock.char_val[3]);
      }

      result = true;
    } // End of else if (first...)
    else if (last == VIL_DICOM_HEADER_APPLICATIONENTRY    ||
             last == VIL_DICOM_HEADER_AGESTRING           ||
             last == VIL_DICOM_HEADER_ATTRIBUTETAG        ||
             last == VIL_DICOM_HEADER_CODESTRING          ||
             last == VIL_DICOM_HEADER_DATE                ||
             last == VIL_DICOM_HEADER_DECIMALSTRING       ||
             last == VIL_DICOM_HEADER_DATETIME            ||
             last == VIL_DICOM_HEADER_FLOATINGPOINTDOUBLE ||
             last == VIL_DICOM_HEADER_FLOATINGPOINTSINGLE ||
             last == VIL_DICOM_HEADER_INTEGERSTRING       ||
             last == VIL_DICOM_HEADER_LONGSTRING          ||
             last == VIL_DICOM_HEADER_LONGTEXT            ||
             last == VIL_DICOM_HEADER_PERSONNAME          ||
             last == VIL_DICOM_HEADER_SHORTSTRING         ||
             last == VIL_DICOM_HEADER_SIGNEDLONG          ||
             last == VIL_DICOM_HEADER_SIGNEDSHORT         ||
             last == VIL_DICOM_HEADER_SHORTTEXT           ||
             last == VIL_DICOM_HEADER_TIME                ||
             last == VIL_DICOM_HEADER_UNIQUEIDENTIFIER    ||
             last == VIL_DICOM_HEADER_UNSIGNEDLONG        ||
             last == VIL_DICOM_HEADER_UNSIGNEDSHORT)
    {
      if (last_read_.sys_endian_ == VIL_DICOM_HEADER_DELITTLEENDIAN)
      {
        dblock_size = (unsigned int)((256*conv_dblock.char_val[1]) + conv_dblock.char_val[0]);
      }
      else
      {
        dblock_size = (unsigned int)((256*conv_dblock.char_val[0]) + conv_dblock.char_val[1]);
      }

      result = true;
    } // End of else if (last...)
  } // End of if (last_read_.file_type_ != VIL_DICOM_HEADER_DTUNKNOWN)

  return result;
}

//================================================================

bool vil_dicom_header_format::pixelDataFound(short group, short element)
{
  bool result = false;

  // Check if it's the pixel data
  if ((vxl_uint_16)group == VIL_DICOM_HEADER_PIXELGROUP &&
      (vxl_uint_16)element == VIL_DICOM_HEADER_PXPIXELDATA)
  {
    result = true;
  }

  return result;
}

//================================================================

void vil_dicom_header_format::clearInfo(void)
{
  vil_dicom_header_info_clear( last_read_ );

  // And make it invalid
  info_valid_ = false;
}

//================================================================

vil_dicom_header_endian vil_dicom_header_format::calculateEndian(void)
{
  // Create a union to test endian
  union int_byte
  {
    vxl_uint_32 int_val;
    vxl_byte by_val[4];
  } calc_endian;

  // Put 1 into the union
  calc_endian.int_val = 1;

  // Test which byte has the value 1 in it
  return calc_endian.by_val[0] == 1 ?
    VIL_DICOM_HEADER_DELITTLEENDIAN :
    VIL_DICOM_HEADER_DEBIGENDIAN;
}

//===============================================================

vil_dicom_header_endian vil_dicom_header_format::determineMetaInfo(vil_stream &fs)
{
  vil_dicom_header_endian ret_end = VIL_DICOM_HEADER_DELITTLEENDIAN; // Assume little if none found
  //vil_dicom_header_endian ret_end = VIL_DICOM_HEADER_DEBIGENDIAN;

  vxl_uint_16 group, element;
  vxl_uint_32 data_block_size;
  vil_streampos ret_pos = fs.tell(); // Maintain the file position

  // The first section of the file header is always little endian,
  // so set the file endian
  file_endian_ = VIL_DICOM_HEADER_DELITTLEENDIAN;
  image_type_ = VIL_DICOM_HEADER_DITUNKNOWN;

  // Read the next group
  fs.read(&group,sizeof(vxl_uint_16));
  group = shortSwap(group);

  while (fs.ok() && group <= VIL_DICOM_HEADER_METAFILEGROUP)
  {
    // Read the element

    fs.read(&element,sizeof(vxl_uint_16));
    element = shortSwap(element);

    // Read the data block size
    if (sizeof(vxl_uint_32) != fs.read(&data_block_size, sizeof(vxl_uint_32)))
      break;

    data_block_size = intSwap(data_block_size);

    if (data_block_size > 0x1000000) {
      vcl_cerr<< __FILE__ << ": " << __LINE__ << " : WARNING:\n"
              <<"data_block_size=" << data_block_size << " is most probably too large\n";
      break;
    }

    convertValueRepresentation(data_block_size,fs);

    if (group == VIL_DICOM_HEADER_METAFILEGROUP &&
        element == VIL_DICOM_HEADER_MFTRANSFERSYNTAX)
    {
      // This tells us the transfer syntax for the file
      char * tfx_type = new char[data_block_size+1]; // Ensure room for 0
      if (tfx_type)
      {
        fs.read(tfx_type, data_block_size);
        tfx_type[data_block_size]=0;

        // Now see what it is

        vcl_string temp = tfx_type;
        delete [] tfx_type;

        if (temp == VIL_DICOM_HEADER_IMPLICITLITTLE ||
            temp == VIL_DICOM_HEADER_EXPLICITLITTLE)
        {
          // Little endian
          ret_end = VIL_DICOM_HEADER_DELITTLEENDIAN;
        }
        else if (temp == VIL_DICOM_HEADER_EXPLICITBIG)
        {
          // Big endian
          ret_end = VIL_DICOM_HEADER_DEBIGENDIAN;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGBASELINE_P1)
        {
          // Jpeg Baseline
          image_type_ = VIL_DICOM_HEADER_DITJPEGBASE;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGDEFLOSSY_P2_4 ||
                 temp == VIL_DICOM_HEADER_JPEGEXTENDED_P3_5)
        {
          // Jpeg extended lossy
          image_type_ = VIL_DICOM_HEADER_DITJPEGEXTLOSSY;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGSPECTRAL_P6_8 ||
                 temp == VIL_DICOM_HEADER_JPEGSPECTRAL_P7_9)
        {
          // Jpeg spectral selection non-hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGSPECNH;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGFULLPROG_P10_12 ||
                 temp == VIL_DICOM_HEADER_JPEGFULLPROG_P11_13)
        {
          // Full progression non-hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGFULLNH;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGLOSSLESS_P14 ||
                 temp == VIL_DICOM_HEADER_JPEGLOSSLESS_P15)
        {
          // Lossless non-hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGLOSSLNH;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGEXTHIER_P16_18 ||
                 temp == VIL_DICOM_HEADER_JPEGEXTHIER_P17_19)
        {
          // Extended hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGEXTHIER;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGSPECHIER_P20_22 ||
                 temp == VIL_DICOM_HEADER_JPEGSPECHIER_P21_23)
        {
          // Spectral selection hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGSPECHIER;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGFULLHIER_P24_26 ||
                 temp == VIL_DICOM_HEADER_JPEGFULLHIER_P25_27)
        {
          // Full progression hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGFULLHIER;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGLLESSHIER_P28 ||
                 temp == VIL_DICOM_HEADER_JPEGLLESSHIER_P29)
        {
          // Lossless hierarchical
          image_type_ = VIL_DICOM_HEADER_DITJPEGLOSSLHIER;
        }
        else if (temp == VIL_DICOM_HEADER_JPEGLLESSDEF_P14_SV1)
        {
          // The default for lossless jpeg
          image_type_ = VIL_DICOM_HEADER_DITJPEGLOSSLDEF;
        }
        else if (temp == VIL_DICOM_HEADER_RLELOSSLESS)
        {
          // RLE encapsulated
          image_type_ = VIL_DICOM_HEADER_DITRLE;
        }

      } // End of if (tfx_type)
    } // End of if (group...)
    else if (group == VIL_DICOM_HEADER_DELIMITERGROUP &&
             (element == VIL_DICOM_HEADER_DLITEM ||
              element == VIL_DICOM_HEADER_DLITEMDELIMITATIONITEM ||
              element == VIL_DICOM_HEADER_DLSEQDELIMITATIONITEM))
    {
      // Do nothing
    }
    else
    {
        // Ignore the data that's there
        fs.seek(data_block_size + fs.tell());
    }

    ret_pos = fs.tell();

    // Read the next group

    fs.read(&group,sizeof(vxl_uint_16));
    group = shortSwap(group);

  } // End of while

  // Reset the pointer before the last read group
  fs.seek(ret_pos);

  return ret_end;
}

//===============================================================

vxl_uint_16 vil_dicom_header_format::shortSwap(vxl_uint_16 short_in)
{
  vxl_uint_16 result = short_in;

  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (file_endian_ != endian_)
  {
    // Create a short unioned with two chars
    union short_char
    {
      vxl_uint_16 short_val;
      vxl_byte byte_val[2];
    } short_swap;

    // Set the swapper
    short_swap.short_val = short_in;

    // Swap them over
    vxl_byte temp = short_swap.byte_val[0];
    short_swap.byte_val[0]=short_swap.byte_val[1];
    short_swap.byte_val[1]=temp;

    result = short_swap.short_val;
  } // End of if (file_endian_ != endian_)

  return result;
}

//===============================================================

vxl_uint_32 vil_dicom_header_format::intSwap(vxl_uint_32 int_in)
{
  vxl_uint_32 result = int_in;

  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (file_endian_ != endian_)
  {
    // Create a vxl_uint_32 unioned with four chars
    union int_char
    {
      vxl_uint_32 int_val;
      vxl_byte byte_val[4];
    } int_swap;

    // Set the swapper
    int_swap.int_val = int_in;

    // Swap them over (end ones first)
    vxl_byte temp = int_swap.byte_val[0];
    int_swap.byte_val[0]=int_swap.byte_val[3];
    int_swap.byte_val[3]=temp;

    // Now the middle ones
    temp = int_swap.byte_val[1];
    int_swap.byte_val[1] = int_swap.byte_val[2];
    int_swap.byte_val[2] = temp;

    result = int_swap.int_val;
  } // End of if (file_endian_ != endian_)

  return result;
}

//===============================================================

void vil_dicom_header_format::charSwap(char *char_in, int val_size)
{
  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (file_endian_ != endian_)
  {
    // Swap first with last, second with one-but-last, etc.
    for (int i=val_size/2-1; i>=0; --i)
    {
      char temp=char_in[i];
      char_in[i] = char_in[val_size-i-1];
      char_in[val_size-i-1] = temp;
    }
  }
}


void vil_dicom_header_print(vcl_ostream &os, const vil_dicom_header_info &s)
{
  os << "\n\nGeneral info fields\n"
     << " file_type        The type of dicom file: " << s.file_type_ << vcl_endl
     << " sys_endian       The endian of the architecture: " << s.sys_endian_ << vcl_endl
     << " image_type       The encapsulated (or not) image type: " <<s.image_type_ << vcl_endl

     << "\n\nIdentifying fields\n"
     << " image_id_type    The image type from the dicom header: " << s.image_id_type_ << vcl_endl
     << " sop_cl_uid       The class unique id for the Service/Object Pair: " << s.sop_cl_uid_ << vcl_endl
     << " sop_in_uid       The instance uid for the SOP: " << s.sop_in_uid_ << vcl_endl
     << " study_date       The date of the study: " << s.study_date_ << vcl_endl
     << " series_date      The date this series was collected: " << s.series_date_ << vcl_endl
     << " acquisition_date The date of acquisition: " << s.acquisition_date_ << vcl_endl
     << " image_date       The date of this image: " << s.image_date_ << vcl_endl
     << " study_time       The time of the study: " << s.study_time_ << vcl_endl
     << " series_time      The time of the series: " << s.series_time_ << vcl_endl
     << " acquisition_time The time acquisition: " << s.acquisition_time_ << vcl_endl
     << " image_time       The time of the image: " << s.image_time_ << vcl_endl
     << " accession_number The accession number for this image: " << s.accession_number_ << vcl_endl
     << " modality         The imaging modality: " << s.modality_ << vcl_endl
     << " manufacturer     The name of the scanner manufacturer: " << s.manufacturer_ << vcl_endl
     << " institution_name The name of the institution: " << s.institution_name_ << vcl_endl
     << " institution_addr The address of the institution: " << s.institution_addr_ << vcl_endl
     << " ref_phys_name    The name of the referring physician: " << s.ref_phys_name_ << vcl_endl
     << " station_name     The name of the station used: " << s.station_name_ << vcl_endl
     << " study_desc       A description of the study: " << s.study_desc_ << vcl_endl
     << " series_desc      A description of the series: " << s.series_desc_ << vcl_endl
     << " att_phys_name    The name of the attending physician: " << s.att_phys_name_ << vcl_endl
     << " operator_name    The name of the MR operator: " << s.operator_name_ << vcl_endl
     << " model_name       The name of the MR scanner model: " << s.model_name_ << vcl_endl

     << "\n\nPatient info\n"
     << " patient_name     Patient's name: " << s.patient_name_ << vcl_endl
     << " patient_id       Patient's ID: " << s.patient_id_ << vcl_endl
     << " patient_dob      The patient's date of birth: " << s.patient_dob_ << vcl_endl
     << " patient_sex      The sex of the patient: " << s.patient_sex_ << vcl_endl
     << " patient_age      The age of the patient: " << s.patient_age_ << vcl_endl
     << " patient_weight_  The weight of the patient: " << s.patient_weight_ << vcl_endl
     << " patient_hist     Any additional patient history: " << s.patient_hist_ << vcl_endl

     << "\n\nAcquisition Info\n"
     << " scanning_seq     A description of the scanning sequence: " << s.scanning_seq_ << vcl_endl
     << " sequence_var     A description of the sequence variant: " << s.sequence_var_ << vcl_endl
     << " scan_options     A description of various scan options: " << s.scan_options_ << vcl_endl
     << " mr_acq_type      The acquisition type for this scan: " << s.mr_acq_type_ << vcl_endl
     << " sequence_name    The name of the sequence: " << s.sequence_name_ << vcl_endl
     << " angio_flag       The angio flag for this sequence: " << s.angio_flag_ << vcl_endl
     << " slice_thickness_ Slice thickness (for voxel size): " << s.slice_thickness_ << vcl_endl
     << " repetition_time_ Scan repetition time: " << s.repetition_time_ << vcl_endl
     << " echo_time        Scan echo time: " << s.echo_time_ << vcl_endl
     << " inversion_time   Scan inversion time: " << s.inversion_time_ << vcl_endl
     << " number_of_averages The number of averages for this scan: " << s.number_of_averages_ << vcl_endl
     << " echo_numbers     The echo numbers for this scan: " << s.echo_numbers_ << vcl_endl
     << " mag_field_strength The strength of the magnetic field: " << s.mag_field_strength_ << vcl_endl
     << " echo_train_length The length of the echo train: " << s.echo_train_length_ << vcl_endl
     << " pixel_bandwidth  The bandwidth of the pixels: " << s.pixel_bandwidth_ << vcl_endl
     << " software_vers_   Versions of the scanner software used: " << s.software_vers_ << vcl_endl
     << " protocol_name    The name of the protocol used: " << s.protocol_name_ << vcl_endl
     << " heart_rate       The patient's heart rate: " << s.heart_rate_ << vcl_endl
     << " card_num_images  The cardiac number of images: " << s.card_num_images_ << vcl_endl
     << " trigger_window   The trigger window for this image: " << s.trigger_window_ << vcl_endl
     << " reconst_diameter The reconstruction diameter: " << s.reconst_diameter_ << vcl_endl
     << " receiving_coil_  Details of the receiving coil: " << s.receiving_coil_ << vcl_endl
     << " phase_enc_dir    The phase encoding direction: " << s.phase_enc_dir_ << vcl_endl
     << " flip_angle       The flip angle: " << s.flip_angle_ << vcl_endl
     << " sar              The specific absorption rate: " << s.sar_ << vcl_endl
     << " patient_pos      The position of the patient in the scanner: " << s.patient_pos_ << vcl_endl

     << "\n\nRelationship info\n"
     << " stud_ins_uid     The study instance unique id: " << s.stud_ins_uid_ << vcl_endl
     << " ser_ins_uid      The series instance unique id: " << s.ser_ins_uid_ << vcl_endl
     << " study_id         The id of this study: " << s.study_id_ << vcl_endl
     << " series_number    The number of this series: " << s.series_number_ << vcl_endl
     << " acquisition_number The number of the acquisition: " << s.acquisition_number_ << vcl_endl
     << " image_number     The number of this image instance: " << s.image_number_ << vcl_endl
     << " pat_orient       The orientation of the patient: " << s.pat_orient_ << vcl_endl
     << " image_pos        The image position relative to the patient: " << s.image_pos_ << vcl_endl
     << " image_orient     The image orientation relative to the patient: " << s.image_orient_ << vcl_endl
     << " frame_of_ref     The frame of reference" << s.frame_of_ref_ << vcl_endl
     << " images_in_acq    Then number ot images in the acquisition: " << s.images_in_acq_ << vcl_endl
     << " pos_ref_ind      The position reference indicator: " << s.pos_ref_ind_ << vcl_endl
     << " slice_location   The location of the slice: " << s.slice_location_ << vcl_endl

     << "\n\nImage info\n"
     << " pix_samps        The number of samples per pixel: " << s.pix_samps_ << vcl_endl
     << " photo_interp     The photometric interpretation: " << s.photo_interp_ << vcl_endl
     << " size_x           The number of columns: " << s.size_x_ << vcl_endl
     << " size_y           The number of rows: " << s.size_y_ << vcl_endl
     << " size_z           The number of planes: " << s.size_z_ << vcl_endl
     << " high_bit         The bit used as the high bit: " << s.high_bit_ << vcl_endl
     << " small_im_pix_val The smallest image pixel value: " << s.small_im_pix_val_ << vcl_endl
     << " large_im_pix_val The largest image pixel value: " << s.large_im_pix_val_ << vcl_endl
     << " pixel_padding_val The value used for padding pixels: " << s.pixel_padding_val_ << vcl_endl
     << " window_centre    The value of the image window's centre: " << s.window_centre_ << vcl_endl
     << " window_width     The actual width of the image window: " << s.window_width_ << vcl_endl

     << "\n\nInfo from the tags specifically for reading the image data\n"
     << " spaxing_x        The pixel spacing in x: " << s.spacing_x_ << vcl_endl
     << " spacing_y        The pixel spacing in y: " << s.spacing_y_ << vcl_endl
     << " spacing_slice    The pixel spacing in z: " << s.spacing_slice_ << vcl_endl
     << " res_intercept    The image rescale intercept: " << s.res_intercept_ << vcl_endl
     << " res_slope        The image rescale slope: " << s.res_slope_ << vcl_endl
     << " pix_rep          The pixel representation (+/-): " << s.pix_rep_ << vcl_endl
     << " stored_bits      The bits stored: " << s.stored_bits_ << vcl_endl
     << " allocated_bits   The bits allocated: " << s.allocated_bits_ << vcl_endl;
}

