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
  else
  {
    // It's not a dicom file, so can't read
    vcl_cerr << "Unknown file type - not a DICOM file...\n"
             << "File header not read\n";
  } // End of else

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
    // The file is Part10 with 128 vxl_byte pre-amble
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
          unsigned data_block_size, num_elements;

          fs.read((char *)&group, sizeof(vxl_uint_16));
          group = shortSwap(group);
          fs.read((char *)&element, sizeof(vxl_uint_16));
          element = shortSwap(element);
          fs.read((char *)&data_block_size, sizeof(int));
          data_block_size = intSwap(data_block_size);

          num_elements = 0;

          while (group < VIL_DICOM_HEADER_IDENTIFYINGGROUP &&
                 num_elements < VIL_DICOM_HEADER_MAXHEADERSIZE &&
                 fs.ok())
          {
            // It's not what we want, so skip it and
            // get the next one
            fs.seek(data_block_size + fs.tell());


            fs.read((char *)&group, sizeof(vxl_uint_16));
            group = shortSwap(group);
            fs.read((char *)&element, sizeof(vxl_uint_16));
            element = shortSwap(element);
            fs.read((char *)&data_block_size, sizeof(int));
            data_block_size = intSwap(data_block_size);

            num_elements++;
          } // End of while (group...

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
            } // End of if (element...
            // Now a non-standard non-Part10
            else if (element == VIL_DICOM_HEADER_IDIMAGETYPE ||
                     element == VIL_DICOM_HEADER_IDLENGTHTOEND ||
                     element == VIL_DICOM_HEADER_IDSPECIFICCHARACTER)
            {
              // Put the file back at the beginning
              fs.seek(0);
              result = VIL_DICOM_HEADER_DTNON_PART10;
              known = true;
            } // End of else if (element...
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
  vxl_uint_16 group, element;   // The groups and elements read from the header part of the dicom file
  unsigned int data_block_size; // The size of the information held for this group/element pair

  // Read the first group/element pair
  fs.read((char *)&group, sizeof(vxl_uint_16));
  fs.read((char *)&element, sizeof(vxl_uint_16));

  // Swap them if necessary
  group = shortSwap(group);
  element = shortSwap(element);

  // Loop until the file ends (unexpectedly!) or the
  // pixel data is found
  while (fs.ok() && !pixelDataFound(group, element))
  {
    fs.read((char *)&data_block_size, sizeof(unsigned int));
    data_block_size = intSwap(data_block_size);
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
    fs.read((char *)&group, sizeof(vxl_uint_16));
    fs.read((char *)&element, sizeof(vxl_uint_16));

    // Swap them if necessary
    group = shortSwap(group);
    element = shortSwap(element);
  } // End of while

  // Read the final block size info - throw away!
  fs.read((char *)&data_block_size, sizeof(int));
  data_block_size = intSwap(data_block_size);
  convertValueRepresentation(data_block_size, fs);
}

//================================================================

void vil_dicom_header_format::readIdentifyingElements(short element,
                                                       int dblock_size,
                                                       vil_stream &fs)
{
  // Pointer to any data read
  char *data_p = 0;

  // Check the elements
  switch ((vxl_uint_16)element)
  {
   case VIL_DICOM_HEADER_IDIMAGETYPE :
    // It's the image type
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_id_type_ = data_p;
    }
    break;

   case VIL_DICOM_HEADER_IDSOPCLASSID :
    // It's the SOP class ID
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.sop_cl_uid_ = data_p;
    }
    break;

   case VIL_DICOM_HEADER_IDSOPINSTANCEID :
    // It's the SOP instance ID
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.sop_in_uid_ = data_p;
    }
    break;

   case VIL_DICOM_HEADER_IDSTUDYDATE :
    // It's the study date
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.study_date_ = atol(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDSERIESDATE :
    // It's the series date
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.series_date_ = atol(data_p);
    }
    break;

   case VIL_DICOM_HEADER_IDACQUISITIONDATE :
    // It's the acquisition date
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.acquisition_date_ = atol(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDIMAGEDATE :
    // It's the image date
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_date_ = atol(data_p);
    }
    break;

   case VIL_DICOM_HEADER_IDSTUDYTIME :
    // It's the study time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.study_time_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDSERIESTIME :
    // It's the series time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.series_time_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDACQUISITIONTIME :
    // It's the acquisition time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.acquisition_time_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDIMAGETIME :
    // It's the image time
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_time_ = (float) atof(data_p);
    }
    break;

   case VIL_DICOM_HEADER_IDACCESSIONNUMBER :
    // It's the accession number
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.accession_number_ = data_p;
    }
    break;

   case VIL_DICOM_HEADER_IDMODALITY :
    // It's the imaging modality
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.modality_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDMANUFACTURER :
    // It's the manufacturer name
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.manufacturer_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDINSTITUTIONNAME :
    // It's the institution name
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.institution_name_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDINSTITUTIONADDRESS :
    // It's the institution address
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.institution_addr_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDREFERRINGPHYSICIAN :
    // It's the referring physician's name
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.ref_phys_name_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDSTATIONNAME :
    // It's the imaging station name
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.station_name_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDSTUDYDESCRIPTION :
    // It's the study description
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.study_desc_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDSERIESDESCRIPTION :
    // It's the series description
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.series_desc_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDATTENDINGPHYSICIAN :
    // It's the name of the attending physician
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.att_phys_name_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDOPERATORNAME :
    // It's the name of the scanner operator
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.operator_name_=data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IDMANUFACTURERMODEL :
    // It's the scanner model
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.model_name_=data_p;
    } // End of if (data_p)
    break;

    // It's nothing we want, so skip it!
   default:
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
   case VIL_DICOM_HEADER_PIPATIENTNAME :
    // It's the patient's name
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_name_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_PIPATIENTID :
    // It's the patient's id
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_id_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_PIPATIENTBIRTHDATE :
    // It's the patient's date of birth
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_dob_ = atol(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_PIPATIENTSEX :
    // It's the patient's sex
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_sex_ = data_p;
    } // End of if (data_p)
    break;


   case VIL_DICOM_HEADER_PIPATIENTAGE :
    // It's the patient's age
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_age_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_PIPATIENTWEIGHT :
    // It's the patient's weight
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_weight_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_PIPATIENTHISTORY :
    // It's the patient's history
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_hist_ = data_p;
    } // End of if (data_p)
    break;

    // It's nothing we want, so skip it!
   default:
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
   case VIL_DICOM_HEADER_AQSCANNINGSEQUENCE :
    // It's the scanning sequence
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.scanning_seq_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSEQUENCEVARIANT :
    // It's the sequence variant
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.sequence_var_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSCANOPTIONS :
    // It's the scan options
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.scan_options_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQMRACQUISITIONTYPE :
    // It's the MR acquisition type
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.mr_acq_type_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSEQUENCENAME :
    // It's the sequence name
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.sequence_name_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQANGIOFLAG :
    // It's the angio flag
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.angio_flag_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSLICETHICKNESS :
    // It's the slice thickness
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.slice_thickness_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQREPETITIONTIME :
    // It's the repetition time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.repetition_time_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQECHOTIME :
    // It's the echo time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.echo_time_= (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQINVERSIONTIME :
    // It's the inversion time
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.inversion_time_= (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQNUMBEROFAVERAGES :
    // It's the number of averages
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.number_of_averages_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQECHONUMBERS :
    // It's the echo numbers
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.echo_numbers_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQMAGNETICFIELDSTRENGTH :
    // It's the magnetic field strength
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.mag_field_strength_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSLICESPACING:
    // It's the slice spacing
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.slice_spacing_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQECHOTRAINLENGTH :
    // It's the echo train length
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.echo_train_length_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQPIXELBANDWIDTH :
    // It's the pixel bandwidth
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.pixel_bandwidth_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSOFTWAREVERSION :
    // It's the scanner software version
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.software_vers_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQPROTOCOLNAME :
    // It's the protocol name
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.protocol_name_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQHEARTRATE :
    // It's the heart rate
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.heart_rate_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQCARDIACNUMBEROFIMAGES :
    // It's the cardiac number of images
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.card_num_images_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQTRIGGERWINDOW :
    // It's the trigger window
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.trigger_window_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQRECONTRUCTIONDIAMETER :
    // It's the reconstruction diameter
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.reconst_diameter_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQRECEIVINGCOIL :
    // It's the receiving coil
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.receiving_coil_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQPHASEENCODINGDIRECTION :
    // It's the phase encoding direction
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.phase_enc_dir_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQFLIPANGLE :
    // It's the flip angle
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.flip_angle_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQSAR :
    // It's the sar
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.sar_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_AQPATIENTPOSITION:
    // It's the patient position
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.patient_pos_ = data_p;
    } // End of if (data_p)
    break;

    // It's nothing we want, so skip it!
   default:
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
   case VIL_DICOM_HEADER_RSSTUDYINSTANCEUID :
    // It's the study instance id
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.stud_ins_uid_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSSERIESINSTANCEUID :
    // It's the series instance id
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.ser_ins_uid_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSSTUDYID :
    // It's the study id
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.study_id_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSSERIESNUMBER :
    // It's the series number
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.series_number_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSAQUISITIONNUMBER :
    // It's the acqusition number
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.acquisition_number_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSIMAGENUMBER :
    // It's the image number
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_number_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSPATIENTORIENTATION :
    // It's the patient orientation
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.pat_orient_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSIMAGEPOSITION :
    // It's the image position
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_pos_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSIMAGEORIENTATION :
    // It's the image orientation
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.image_orient_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSFRAMEOFREFERENCEUID :
    // It's the frame of reference uid
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.frame_of_ref_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSIMAGESINACQUISITION:
    // It's the number of images in the acquisition
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.images_in_acq_ = atoi(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSPOSITIONREFERENCE :
    // It's the position reference
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.pos_ref_ind_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_RSSLICELOCATION :
    // It's the slice location
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      last_read_.slice_location_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

    // It's nothing we want, so skip it!
   default:
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
   case VIL_DICOM_HEADER_IMSAMPLESPERPIXEL :
    // It's the samples per pixel
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.pix_samps_ = *((vxl_uint_16*)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMPHOTOMETRICINTERP :
    // It's the photometric interpretation
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.photo_interp_ = data_p;
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMROWS :
    // It's the rows
    data_p = new char[dblock_size+1]; // Ensure room for 0
    if (data_p)
    {
      fs.read(data_p, dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.dimx_ = *((vxl_uint_16*)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMCOLUMNS :
    // It's the columns
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.dimy_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMPLANES :
    // It's the planes
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.dimz_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMPIXELSPACING :
    // It's the pixel spacing
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.xsize_ = (float) atof(data_p);

      // The y size should come after a '\'
      // If only a 0 is found, ysize = xsize
      char gone = 'x';
      while (gone != 0 && gone != '\\')
      {
        gone = data_p[0];

        for (int i=0; i<dblock_size; i++)
        {
          data_p[i] = data_p[i+1];
        }
      } // End of while

      if (gone == '\\')
      {
        last_read_.ysize_ = (float) atof(data_p);
      }
      else
      {
        last_read_.ysize_ = (float) last_read_.xsize_;
      }
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMBITSALLOCATED :
    // It's the allocated bits
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.allocated_bits_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMBITSSTORED :
    // It's the stored bits info
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p,sizeof(vxl_uint_16));
      last_read_.stored_bits_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMHIGHBIT :
    // It's the high bit
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(vxl_uint_16));
      last_read_.high_bit_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMPIXELREPRESENTATION :
    // It's the pixel representation
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p,sizeof(vxl_uint_16));
      last_read_.pix_rep_ = *((vxl_uint_16 *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMSMALLIMPIXELVALUE :
    // It's the smallest image pixel value
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(short));
      last_read_.small_im_pix_val_ = *((short *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMLARGEIMPIXELVALUE :
    // It's the largest image pixel value
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(short));
      last_read_.large_im_pix_val_ = *((short *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMPIXELPADDINGVALUE :
    // It's the pixel padding value
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      charSwap(data_p, sizeof(short));
      last_read_.pixel_padding_val_ = *((short *)data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMWINDOWCENTER :
    // It's the window centre
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.window_centre_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMWINDOWWIDTH :
    // It's the window width
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.window_width_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMRESCALEINTERCEPT :
    // It's the rescale intercept
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.res_intercept_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

   case VIL_DICOM_HEADER_IMRESCALESLOPE :
    // It's the rescale slope
    data_p = new char[dblock_size+1];
    if (data_p)
    {
      fs.read(data_p,dblock_size);
      data_p[dblock_size]=0;
      last_read_.res_slope_ = (float) atof(data_p);
    } // End of if (data_p)
    break;

    // It's nothing we want, so skip it!
   default:
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
    unsigned int int_val;
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
      fs.read((char *)&dblock_size, sizeof(int));
      dblock_size = 0;
      result = true;
    } // End of if (first...
    else if (first == VIL_DICOM_HEADER_OTHERBYTE ||
             first == VIL_DICOM_HEADER_OTHERWORD ||
             last == VIL_DICOM_HEADER_OTHERBYTE  ||
             last == VIL_DICOM_HEADER_OTHERWORD)
    {
      fs.read((char *)&dblock_size, sizeof(int));
      dblock_size = shortSwap(dblock_size);
      result = true;
    } // End of else if (first...
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
    } // End of else if (first...
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
    } // End of else if (last...
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
  // Clear all the elements of the info structure

  // Identity info
  last_read_.file_type_ = VIL_DICOM_HEADER_DTUNKNOWN;
  last_read_.file_endian_ = VIL_DICOM_HEADER_DEUNKNOWN;
  last_read_.sys_endian_ = VIL_DICOM_HEADER_DEUNKNOWN;
  last_read_.image_type_ = VIL_DICOM_HEADER_DITUNKNOWN;
  last_read_.image_id_type_ = "";
  last_read_.sop_cl_uid_ = "";
  last_read_.sop_in_uid_ = "";
  last_read_.study_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.series_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.acquisition_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.image_date_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.study_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.series_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.acquisition_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.image_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.accession_number_ = "";
  last_read_.modality_ = "";
  last_read_.manufacturer_ = "";
  last_read_.institution_name_ = "";
  last_read_.institution_addr_ = "";
  last_read_.ref_phys_name_ = "";
  last_read_.station_name_ = "";
  last_read_.study_desc_ = "";
  last_read_.att_phys_name_ = "";
  last_read_.operator_name_ = "";
  last_read_.model_name_ = "";

  // Patient info
  last_read_.patient_name_ = "";
  last_read_.patient_id_ = "";
  last_read_.patient_dob_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.patient_sex_ = "";
  last_read_.patient_age_ = "";
  last_read_.patient_weight_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.patient_hist_ = "";

  // Acquisition info
  last_read_.scanning_seq_ = "";
  last_read_.sequence_var_ = "";
  last_read_.scan_options_ = "";
  last_read_.mr_acq_type_ = "";
  last_read_.sequence_name_ = "";
  last_read_.angio_flag_ = "";
  last_read_.slice_thickness_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.repetition_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.echo_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.inversion_time_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.number_of_averages_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.echo_numbers_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.mag_field_strength_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.echo_train_length_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.pixel_bandwidth_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.software_vers_ = "";
  last_read_.protocol_name_ = "";
  last_read_.heart_rate_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.card_num_images_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.trigger_window_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.reconst_diameter_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.receiving_coil_ = "";
  last_read_.phase_enc_dir_ = "";
  last_read_.flip_angle_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.sar_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.patient_pos_ = "";

  // Relationship info
  last_read_.stud_ins_uid_= "";
  last_read_.ser_ins_uid_ = "";
  last_read_.study_id_ = "";
  last_read_.series_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.acquisition_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.image_number_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.pat_orient_ = "";
  last_read_.image_pos_ = "";
  last_read_.image_orient_ = "";
  last_read_.frame_of_ref_ = "";
  last_read_.images_in_acq_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.pos_ref_ind_ = "";
  last_read_.slice_location_ = VIL_DICOM_HEADER_UNSPECIFIED;

  // Image info
  last_read_.pix_samps_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.photo_interp_ = "";
  last_read_.dimx_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.dimy_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.dimz_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.high_bit_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.small_im_pix_val_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.large_im_pix_val_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.pixel_padding_val_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.window_centre_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.window_width_ = VIL_DICOM_HEADER_UNSPECIFIED;

  // Info for loading image
  last_read_.xsize_ = VIL_DICOM_HEADER_DEFAULTSIZE;
  last_read_.ysize_ = VIL_DICOM_HEADER_DEFAULTSIZE;
  last_read_.slice_spacing_ = VIL_DICOM_HEADER_DEFAULTSIZE;
  last_read_.res_intercept_ = VIL_DICOM_HEADER_DEFAULTINTERCEPT;
  last_read_.res_slope_ = VIL_DICOM_HEADER_DEFAULTSLOPE;
  last_read_.pix_rep_ = VIL_DICOM_HEADER_UNSPECIFIED_UNSIGNED;
  last_read_.stored_bits_ = VIL_DICOM_HEADER_UNSPECIFIED;
  last_read_.allocated_bits_ = VIL_DICOM_HEADER_UNSPECIFIED;

  // And make it invalid
  info_valid_ = false;
}

//================================================================

vil_dicom_header_endian vil_dicom_header_format::calculateEndian(void)
{
  // Create a union to test endian
  union int_byte
  {
    int int_val;
    unsigned char by_val[4];
  } calc_endian;

  // Put 1 into the union
  calc_endian.int_val = 1;

  // Test which vxl_byte has the value 1 in it
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
  unsigned int data_block_size;
  char *tfx_type=0;
  vil_streampos ret_pos = fs.tell(); // Maintain the file position

  // The first section of the file header is always little endian,
  // so set the file endian
  file_endian_ = VIL_DICOM_HEADER_DELITTLEENDIAN;
  image_type_ = VIL_DICOM_HEADER_DITUNKNOWN;

  // Read the next group
  fs.read((char *)&group,sizeof(vxl_uint_16));
  group = shortSwap(group);

  while (fs.ok() && group <= VIL_DICOM_HEADER_METAFILEGROUP)
  {
    // Read the element
    fs.read((char *)&element,sizeof(vxl_uint_16));
    element = shortSwap(element);

    // Read the data block size
    fs.read((char *)&data_block_size,sizeof(unsigned int));
    data_block_size = intSwap(data_block_size);
    convertValueRepresentation(data_block_size,fs);

    if (group == VIL_DICOM_HEADER_METAFILEGROUP &&
        element == VIL_DICOM_HEADER_MFTRANSFERSYNTAX)
    {
      // This tells us the transfer syntax for the file
      tfx_type = new char[data_block_size+1]; // Ensure room for 0
      if (tfx_type)
      {
        fs.read(tfx_type, data_block_size);
        tfx_type[data_block_size]=0;

        // Now see what it is

        vcl_string temp = tfx_type;

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
    } // End of if (group...
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
    fs.read((char *)&group,sizeof(vxl_uint_16));
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
      char byte_val[2];
    } short_swap;

    // Set the swapper
    short_swap.short_val = short_in;

    // Swap them over
    char temp = short_swap.byte_val[0];
    short_swap.byte_val[0]=short_swap.byte_val[1];
    short_swap.byte_val[1]=temp;

    result = short_swap.short_val;
  } // End of if (file_endian_ != endian_)

  return result;
}

//===============================================================

int vil_dicom_header_format::intSwap(int int_in)
{
  int result = int_in;

  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (file_endian_ != endian_)
  {
    // Create an int unioned with four chars
    union int_char
    {
      int int_val;
      char byte_val[4];
    } int_swap;

    // Set the swapper
    int_swap.int_val = int_in;

    // Swap them over (end ones first)
    char temp = int_swap.byte_val[0];
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
    // Create a char the same size to swap
    char *temp = new char [val_size];

    if (temp)
    {
      // Copy from the first vcl_string into the temp
      for (int i=0; i<val_size; i++)
      {
        temp[i]=char_in[i];
      }

      // Now put back in reverse
      for (int i=0; i<val_size; i++)
      {
        char_in[(val_size-i)-1] = temp[i];
      } // End of for

      delete[] temp;
    } // End of if (temp)
    else
    {
      vcl_cerr << "Couldn't create temp in charSwap!\n"
               << "Value remains unswapped!\n";
    }
  } // End of if (file_endian_ != endian)
}
