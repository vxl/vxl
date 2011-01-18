/*
**  EPSG Standard PCS codes -> ERM proj/datum LUT 
** 
**	EDITS:
**  md 5NOV99 [01] - updated entries for some stateplane projections and noted
**  				 which ones did not have a match in the GDT database. 
**  md 29Feb 2000 [02] - updated entries for some NUTM and SUTM zones - they
**					were incorrectly specified as NUTM9 etc instead of NUTM09
**					which is the correct GDT projection. 
**  md 27Jun00 [03] - updated entries for some stateplane projections in the GDT database. 
**  dc 11Sep01 [04] - included PCS code for the Netherlands
**	maw 31Jul02 [05] - added PCS codes for French NTF coordinate systems 
**  tfl 22Dec04 [06] - hack to fix GDA95/GEODETIC issue
**  GAN 07Mar06 [07] - Added PULKOVO projection and Datum information
**  md  21Mar06 [08] - Added another proj/datum ref for PULKOVO (was NULL NULL)
*/

struct pcs_erm_mapping {
    int pcs_code;
    char *projection;
    char *datum;
}; 


/* Standard UTM->ERM Mappings */


static struct pcs_erm_mapping pcs_erm_utm[] = {

	{PCS_GGRS87_Greek_Grid,NULL,NULL},
	{PCS_KKJ_Finland_zone_1,NULL,NULL},
	{PCS_KKJ_Finland_zone_2,NULL,NULL},
	{PCS_KKJ_Finland_zone_3,NULL,NULL},
	{PCS_KKJ_Finland_zone_4,NULL,NULL},
	{PCS_RT90_2_5_gon_W,NULL,NULL},
	{PCS_Lietuvos_Koordinoei_Sistema_1994,NULL,NULL},
	{PCS_Estonian_Coordinate_System_of_1992,NULL,NULL},
	{PCS_HD72_EOV,NULL,NULL},
	{PCS_Dealul_Piscului_1970_Stereo_70,"ROS4270","PULKOVO"},	// [08]
    {PCS_Adindan_UTM_zone_37N, "NUTM37", "ADINDAN"},
    {PCS_Adindan_UTM_zone_38N, "NUTM38", "ADINDAN"},
    {PCS_Ain_el_Abd_UTM_zone_37N, "NUTM37", "AINABD70"},
    {PCS_Ain_el_Abd_UTM_zone_38N, "NUTM38", "AINABD70"},
    {PCS_Ain_el_Abd_UTM_zone_39N, "NUTM39", "AINABD70"},
    {PCS_Aratu_UTM_zone_22S, "SUTM22", "ARATU"},
    {PCS_Aratu_UTM_zone_23S, "SUTM23", "ARATU"},
    {PCS_Aratu_UTM_zone_24S, "SUTM24", "ARATU"},
    {PCS_Batavia_UTM_zone_48S, "SUTM48", "BATAVIA"},
    {PCS_Batavia_UTM_zone_49S, "SUTM49", "BATAVIA"},
    {PCS_Batavia_UTM_zone_50S, "SUTM50", "BATAVIA"},
    {PCS_Beijing_Gauss_zone_13, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_14, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_15, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_16, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_17, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_18, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_19, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_20, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_21, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_22, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_23, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_13N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_14N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_15N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_16N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_17N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_18N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_19N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_20N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_21N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_22N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_23N, NULL, "BEIJING"},
    {PCS_Bogota_UTM_zone_17N, "NUTM17", "BOGOTA"},
    {PCS_Bogota_UTM_zone_18N, "NUTM18", "BOGOTA"},
    {PCS_British_National_Grid, "TMOSGB", "OSGB36"}, /* Added or British Ord Survey */
    {PCS_Camacupa_UTM_32S, "SUTM32", "CAMACUPA"},
    {PCS_Camacupa_UTM_33S, "SUTM33", "CAMACUPA"},
    {PCS_Carthage_UTM_zone_32N, "NUTM32", "CARTHAGE"},
    {PCS_Corrego_Alegre_UTM_23S, NULL, "CORRALEG"},
    {PCS_Corrego_Alegre_UTM_24S, NULL, "CORRALEG"},
    {PCS_Douala_UTM_zone_32N, "NUTM32", NULL},
    {PCS_ED50_UTM_zone_28N, "NUTM28", "ED50"},
    {PCS_ED50_UTM_zone_29N, "NUTM29", "ED50"},
    {PCS_ED50_UTM_zone_30N, "NUTM30", "ED50"},
    {PCS_ED50_UTM_zone_31N, "NUTM31", "ED50"},
    {PCS_ED50_UTM_zone_32N, "NUTM32", "ED50"},
    {PCS_ED50_UTM_zone_33N, "NUTM33", "ED50"},
    {PCS_ED50_UTM_zone_34N, "NUTM34", "ED50"},
    {PCS_ED50_UTM_zone_35N, "NUTM35", "ED50"},
    {PCS_ED50_UTM_zone_36N, "NUTM36", "ED50"},
    {PCS_ED50_UTM_zone_37N, "NUTM37", "ED50"},
    {PCS_ED50_UTM_zone_38N, "NUTM38", "ED50"},
    {PCS_Fahud_UTM_zone_39N, "NUTM39", "FAHUD"},
    {PCS_Fahud_UTM_zone_40N, "NUTM40", "FAHUD"},
    {PCS_Garoua_UTM_zone_33N, NULL, NULL},
    {PCS_ID74_UTM_zone_46N, "NUTM46", "IND74"},
    {PCS_ID74_UTM_zone_47N, "NUTM47", "IND74"},
    {PCS_ID74_UTM_zone_48N, "NUTM48", "IND74"},
    {PCS_ID74_UTM_zone_49N, "NUTM49", "IND74"},
    {PCS_ID74_UTM_zone_50N, "NUTM50", "IND74"},
    {PCS_ID74_UTM_zone_51N, "NUTM51", "IND74"},
    {PCS_ID74_UTM_zone_52N, "NUTM52", "IND74"},
    {PCS_ID74_UTM_zone_53N, "NUTM53", "IND74"},
    {PCS_ID74_UTM_zone_46S, "SUTM46", "IND74"},
    {PCS_ID74_UTM_zone_47S, "SUTM47", "IND74"},
    {PCS_ID74_UTM_zone_48S, "SUTM48", "IND74"},
    {PCS_ID74_UTM_zone_49S, "SUTM49", "IND74"},
    {PCS_ID74_UTM_zone_50S, "SUTM50", "IND74"},
    {PCS_ID74_UTM_zone_51S, "SUTM51", "IND74"},
    {PCS_ID74_UTM_zone_52S, "SUTM52", "IND74"},
    {PCS_ID74_UTM_zone_53S, "SUTM53", "IND74"},
    {PCS_ID74_UTM_zone_54S, "SUTM54", "IND74"},
    {PCS_Indian_1954_UTM_47N, "NUTM47", "INDIAN54"},
    {PCS_Indian_1954_UTM_48N, "NUTM48", "INDIAN54"},
    {PCS_Indian_1975_UTM_47N, "NUTM47", "INDIAN75"},
    {PCS_Indian_1975_UTM_48N, "NUTM48", "INDIAN75"},
    {PCS_Kertau_UTM_zone_47N, "NUTM47", "KERTAU"},
    {PCS_Kertau_UTM_zone_48N, "NUTM48", "KERTAU"},
    {PCS_La_Canoa_UTM_zone_20N, "NUTM20", "LACANOA"},
    {PCS_La_Canoa_UTM_zone_21N, "NUTM21", "LACANOA"},
    {PCS_PSAD56_UTM_zone_18N, "NUTM18", "PSAD56"},
    {PCS_PSAD56_UTM_zone_19N, "NUTM19", "PSAD56"},
    {PCS_PSAD56_UTM_zone_20N, "NUTM20", "PSAD56"},
    {PCS_PSAD56_UTM_zone_21N, "NUTM21", "PSAD56"},
    {PCS_PSAD56_UTM_zone_17S, "SUTM17", "PSAD56"},
    {PCS_PSAD56_UTM_zone_18S, "SUTM18", "PSAD56"},
    {PCS_PSAD56_UTM_zone_19S, "SUTM19", "PSAD56"},
    {PCS_PSAD56_UTM_zone_20S, "SUTM20", "PSAD56"},
    {PCS_Malongo_1987_UTM_32S,"SUTM32", NULL},
    {PCS_Massawa_UTM_zone_37N,"NUTM32", NULL},
    {PCS_Minna_UTM_zone_31N, "NUTM31", "MINAA"},
    {PCS_Minna_UTM_zone_32N, "NUTM32", "MINAA"},
    {PCS_Mhast_UTM_zone_32S, "SUTM32", NULL},
    {PCS_M_poraloko_UTM_32N, "NUTM32", NULL},
    {PCS_M_poraloko_UTM_32S, "SUTM32", NULL},
    {PCS_NAD27_UTM_zone_3N, "NUTM03", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_4N, "NUTM04", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_5N, "NUTM05", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_6N, "NUTM06", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_7N, "NUTM07", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_8N, "NUTM08", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_9N, "NUTM09", "NAD27"},		/*[02]*/
    {PCS_NAD27_UTM_zone_10N, "NUTM10", "NAD27"},
    {PCS_NAD27_UTM_zone_11N, "NUTM11", "NAD27"},
    {PCS_NAD27_UTM_zone_12N, "NUTM12", "NAD27"},
    {PCS_NAD27_UTM_zone_13N, "NUTM13", "NAD27"},
    {PCS_NAD27_UTM_zone_14N, "NUTM14", "NAD27"},
    {PCS_NAD27_UTM_zone_15N, "NUTM15", "NAD27"},
    {PCS_NAD27_UTM_zone_16N, "NUTM16", "NAD27"},
    {PCS_NAD27_UTM_zone_17N, "NUTM17", "NAD27"},
    {PCS_NAD27_UTM_zone_18N, "NUTM18", "NAD27"},
    {PCS_NAD27_UTM_zone_19N, "NUTM19", "NAD27"},
    {PCS_NAD27_UTM_zone_20N, "NUTM20", "NAD27"},
    {PCS_NAD27_UTM_zone_21N, "NUTM21", "NAD27"},
    {PCS_NAD27_UTM_zone_22N, "NUTM22", "NAD27"},
    {PCS_NAD83_UTM_zone_3N,  "NUTM03", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_4N,  "NUTM04", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_5N,  "NUTM05", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_6N,  "NUTM06", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_7N,  "NUTM07", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_8N,  "NUTM08", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_9N,  "NUTM09", "NAD83"},		/*[02]*/
    {PCS_NAD83_UTM_zone_10N, "NUTM10", "NAD83"},
    {PCS_NAD83_UTM_zone_11N, "NUTM11", "NAD83"},
    {PCS_NAD83_UTM_zone_12N, "NUTM12", "NAD83"},
    {PCS_NAD83_UTM_zone_13N, "NUTM13", "NAD83"},
    {PCS_NAD83_UTM_zone_14N, "NUTM14", "NAD83"},
    {PCS_NAD83_UTM_zone_15N, "NUTM15", "NAD83"},
    {PCS_NAD83_UTM_zone_16N, "NUTM16", "NAD83"},
    {PCS_NAD83_UTM_zone_17N, "NUTM17", "NAD83"},
    {PCS_NAD83_UTM_zone_18N, "NUTM18", "NAD83"},
    {PCS_NAD83_UTM_zone_19N, "NUTM19", "NAD83"},
    {PCS_NAD83_UTM_zone_20N, "NUTM20", "NAD83"},
    {PCS_NAD83_UTM_zone_21N, "NUTM21", "NAD83"},
    {PCS_NAD83_UTM_zone_22N, "NUTM22", "NAD83"},
    {PCS_NAD83_UTM_zone_23N, "NUTM23", "NAD83"},
	{PCS_NTF_Centre_France, "LM1FRA2D", "NTFPARIS"},						/*05]*/
	{PCS_NTF_France_I, "LM1FRE1D", "NTFPARIS"},									/*05]*/
	{PCS_NTF_France_II, "LM1FRE2D", "NTFPARIS"},								/*05]*/
	{PCS_NTF_France_III, "LM1FRE3D", "NTFPARIS"},								/*05]*/
	{PCS_NTF_Nord_France, "LM1FRA1D", "NTFPARIS"},							/*05]*/
	{PCS_NTF_Sud_France, "LM1FRA3D", "NTFPARIS"},							/*05]*/
    {PCS_Nahrwan_1967_UTM_38N, "NUTM38", "NAHRWAN "},
    {PCS_Nahrwan_1967_UTM_39N, "NUTM39", "NAHRWAN "},
    {PCS_Nahrwan_1967_UTM_40N, "NUTM40", "NAHRWAN "},
    {PCS_Naparima_UTM_20N, "NUTM20", NULL},
    {PCS_Datum_73_UTM_zone_29N, "NUTM29", NULL},
    {PCS_Point_Noire_UTM_32S, NULL, NULL},
    {PCS_GDA94_MGA_zone_48, "MGA48", "GDA94"},
    {PCS_GDA94_MGA_zone_49, "MGA49", "GDA94"},
    {PCS_GDA94_MGA_zone_50, "MGA50", "GDA94"},
    {PCS_GDA94_MGA_zone_51, "MGA51", "GDA94"},
    {PCS_GDA94_MGA_zone_52, "MGA52", "GDA94"},
    {PCS_GDA94_MGA_zone_53, "MGA53", "GDA94"},
    {PCS_GDA94_MGA_zone_54, "MGA54", "GDA94"},
    {PCS_GDA94_MGA_zone_55, "MGA55", "GDA94"},
    {PCS_GDA94_MGA_zone_56, "MGA56", "GDA94"},
    {PCS_GDA94_MGA_zone_57, "MGA57", "GDA94"},
    {PCS_GDA94_MGA_zone_58, "MGA58", "GDA94"},
	{PCS_PULKOVO_ROS4270,"ROS4270","PULKOVO"},								/*07]*/
    {PCS_Pulkovo_Gauss_zone_4, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_5, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_6, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_7, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_8, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_9, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_10, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_11, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_12, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_13, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_14, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_15, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_16, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_17, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_18, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_19, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_20, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_21, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_22, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_23, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_24, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_25, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_26, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_27, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_28, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_29, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_30, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_31, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_32, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_4N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_5N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_6N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_7N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_8N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_9N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_10N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_11N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_12N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_13N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_14N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_15N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_16N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_17N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_18N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_19N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_20N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_21N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_22N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_23N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_24N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_25N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_26N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_27N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_28N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_29N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_30N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_31N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_32N, NULL, "PULKOVO"},
    {PCS_SAD69_UTM_zone_18N, "NUTM18", "SAD69"},
    {PCS_SAD69_UTM_zone_19N, "NUTM19","SAD69"},
    {PCS_SAD69_UTM_zone_20N, "NUTM20","SAD69"},
    {PCS_SAD69_UTM_zone_21N, "NUTM21","SAD69"},
    {PCS_SAD69_UTM_zone_22N, "NUTM22", "SAD69"},
    {PCS_SAD69_UTM_zone_17S, "SUTM17","SAD69"},
    {PCS_SAD69_UTM_zone_18S, "SUTM18","SAD69"},
    {PCS_SAD69_UTM_zone_19S, "SUTM19","SAD69"},
    {PCS_SAD69_UTM_zone_20S, "SUTM20","SAD69"},
    {PCS_SAD69_UTM_zone_21S, "SUTM21","SAD69"},
    {PCS_SAD69_UTM_zone_22S, "SUTM22","SAD69"},
    {PCS_SAD69_UTM_zone_23S, "SUTM23","SAD69"},
    {PCS_SAD69_UTM_zone_24S, "SUTM24","SAD69"},
    {PCS_SAD69_UTM_zone_25S, "SUTM25","SAD69"},
    {PCS_Sapper_Hill_UTM_20S, "SUTM20", NULL},
    {PCS_Sapper_Hill_UTM_21S, "SUTM21", NULL},
    {PCS_Schwarzeck_UTM_33S, "SUTM33", NULL},
    {PCS_Sudan_UTM_zone_35N, "NUTM35", "SUDAN"},
    {PCS_Sudan_UTM_zone_36N, "NUTM36", "SUDAN"},
    {PCS_Tananarive_UTM_38S, "SUTM38", "TANANAR"},
    {PCS_Tananarive_UTM_39S, "SUTM39", "TANANAR"},
    {PCS_Timbalai_1948_UTM_49N, "NUTM49", "TIMBALAI"},
    {PCS_Timbalai_1948_UTM_50N, "NUTM50", "TIMBALAI"},
    {PCS_TC_1948_UTM_zone_39N, "NUTM39", "TRUCIAL"},
    {PCS_TC_1948_UTM_zone_40N, "NUTM40", "TRUCIAL"},
    {PCS_Nord_Sahara_UTM_29N, "NUTM29", NULL},
    {PCS_Nord_Sahara_UTM_30N, "NUTM30", NULL},
    {PCS_Nord_Sahara_UTM_31N, "NUTM31", NULL},
    {PCS_Nord_Sahara_UTM_32N, "NUTM32", NULL},
    {PCS_Yoff_UTM_zone_28N, "NUTM28", "YOFF2000"},
    {PCS_Zanderij_UTM_zone_21N, "NUTM21", NULL},
    {PCS_MGI_Austria_West, NULL, NULL},
    {PCS_MGI_Austria_Central, NULL, NULL},
    {PCS_MGI_Austria_East, NULL, NULL},
    {PCS_WGS72_UTM_zone_1N,  "NUTM1", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_2N,  "NUTM2", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_3N,  "NUTM03", "WGS72DOD"}, 	/*[02]*/
    {PCS_WGS72_UTM_zone_4N,  "NUTM04", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_5N,  "NUTM05", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_6N,  "NUTM06", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_7N,  "NUTM07", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_8N,  "NUTM08", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_9N,  "NUTM09", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_10N, "NUTM10", "WGS72DOD"},
	{PCS_WGS72_UTM_zone_11N, "NUTM11", "WGS72DOD"},
	{PCS_WGS72_UTM_zone_12N, "NUTM12", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_13N, "NUTM13", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_14N, "NUTM14", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_15N, "NUTM15", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_16N, "NUTM16", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_17N, "NUTM17", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_18N, "NUTM18", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_19N, "NUTM19", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_20N, "NUTM20", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_21N, "NUTM21", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_22N, "NUTM22", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_23N, "NUTM23", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_24N, "NUTM24", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_25N, "NUTM25", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_26N, "NUTM26", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_27N, "NUTM27", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_28N, "NUTM28", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_29N, "NUTM29", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_30N, "NUTM30", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_31N, "NUTM31", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_32N, "NUTM32", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_33N, "NUTM33", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_34N, "NUTM34", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_35N, "NUTM35", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_36N, "NUTM36", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_37N, "NUTM37", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_38N, "NUTM38", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_39N, "NUTM39", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_40N, "NUTM40", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_41N, "NUTM41", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_42N, "NUTM42", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_43N, "NUTM43", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_44N, "NUTM44", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_45N, "NUTM45", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_46N, "NUTM46", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_47N, "NUTM47", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_48N, "NUTM48", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_49N, "NUTM49", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_50N, "NUTM50", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_51N, "NUTM51", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_52N, "NUTM52", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_53N, "NUTM53", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_54N, "NUTM54", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_55N, "NUTM55", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_56N, "NUTM56", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_57N, "NUTM57", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_58N, "NUTM58", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_59N, "NUTM59", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_60N, "NUTM60", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_1S, "SUTM01", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_2S, "SUTM02", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_3S, "SUTM03", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_4S, "SUTM04", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_5S, "SUTM05", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_6S, "SUTM06", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_7S, "SUTM07", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_8S, "SUTM08", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_9S, "SUTM09", "WGS72DOD"},	/*[02]*/
    {PCS_WGS72_UTM_zone_10S, "SUTM10", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_11S, "SUTM11", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_12S, "SUTM12", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_13S, "SUTM13", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_14S, "SUTM14", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_15S, "SUTM15", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_16S, "SUTM16", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_17S, "SUTM17", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_18S, "SUTM18", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_19S, "SUTM19", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_20S, "SUTM20", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_21S, "SUTM21", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_22S, "SUTM22", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_23S, "SUTM23", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_24S, "SUTM24", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_25S, "SUTM25", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_26S, "SUTM26", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_27S, "SUTM27", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_28S, "SUTM28", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_29S, "SUTM29", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_30S, "SUTM30", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_31S, "SUTM31", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_32S, "SUTM32", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_33S, "SUTM33", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_34S, "SUTM34", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_35S, "SUTM35", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_36S, "SUTM36", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_37S, "SUTM37", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_38S, "SUTM38", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_39S, "SUTM39", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_40S, "SUTM40", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_41S, "SUTM41", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_42S, "SUTM42", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_43S, "SUTM43", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_44S, "SUTM44", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_45S, "SUTM45", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_46S, "SUTM46", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_47S, "SUTM47", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_48S, "SUTM48", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_49S, "SUTM49", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_50S, "SUTM50", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_51S, "SUTM51", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_52S, "SUTM52", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_53S, "SUTM53", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_54S, "SUTM54", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_55S, "SUTM55", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_56S, "SUTM56", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_57S, "SUTM57", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_58S, "SUTM58", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_59S, "SUTM59", "WGS72DOD"},
    {PCS_WGS72_UTM_zone_60S, "SUTM60", "WGS72DOD"},
    {PCS_WGS72BE_UTM_zone_1N, "NUTM1", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_2N, "NUTM2", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_3N, "NUTM03", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_4N, "NUTM04", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_5N, "NUTM05", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_6N, "NUTM06", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_7N, "NUTM07", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_8N, "NUTM08", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_9N, "NUTM09", "WGS72BE"},	/*[02]*/
    {PCS_WGS72BE_UTM_zone_10N, "NUTM10", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_11N, "NUTM11", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_12N, "NUTM12", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_13N, "NUTM13", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_14N, "NUTM14", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_15N, "NUTM15", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_16N, "NUTM16", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_17N, "NUTM17", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_18N, "NUTM18", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_19N, "NUTM19", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_20N, "NUTM20", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_21N, "NUTM21", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_22N, "NUTM22", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_23N, "NUTM23", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_24N, "NUTM24", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_25N, "NUTM25", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_26N, "NUTM26", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_27N, "NUTM27", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_28N, "NUTM28", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_29N, "NUTM29", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_30N, "NUTM30", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_31N, "NUTM31", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_32N, "NUTM32", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_33N, "NUTM33", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_34N, "NUTM34", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_35N, "NUTM35", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_36N, "NUTM36", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_37N, "NUTM37", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_38N, "NUTM38", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_39N, "NUTM39", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_40N, "NUTM40", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_41N, "NUTM41", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_42N, "NUTM42", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_43N, "NUTM43", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_44N, "NUTM44", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_45N, "NUTM45", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_46N, "NUTM46", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_47N, "NUTM47", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_48N, "NUTM48", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_49N, "NUTM49", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_50N, "NUTM50", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_51N, "NUTM51", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_52N, "NUTM52", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_53N, "NUTM53", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_54N, "NUTM54", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_55N, "NUTM55", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_56N, "NUTM56", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_57N, "NUTM57", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_58N, "NUTM58", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_59N, "NUTM59", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_60N, "NUTM60", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_1S, "SUTM01", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_2S, "SUTM02", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_3S, "SUTM03", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_4S, "SUTM04", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_5S, "SUTM05", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_6S, "SUTM06", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_7S, "SUTM07", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_8S, "SUTM08", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_9S, "SUTM09", "WGS72BE"},		/*[02]*/
    {PCS_WGS72BE_UTM_zone_10S, "SUTM10", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_11S, "SUTM11", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_12S, "SUTM12", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_13S, "SUTM13", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_14S, "SUTM14", "WGS72BE"},
	{PCS_WGS72BE_UTM_zone_15S, "SUTM15", "WGS72BE"},
	{PCS_WGS72BE_UTM_zone_16S, "SUTM16", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_17S, "SUTM17", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_18S, "SUTM18", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_19S, "SUTM19", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_20S, "SUTM20", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_21S, "SUTM21", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_22S, "SUTM22", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_23S, "SUTM23", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_24S, "SUTM24", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_25S, "SUTM25", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_26S, "SUTM26", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_27S, "SUTM27", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_28S, "SUTM28", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_29S, "SUTM29", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_30S, "SUTM30", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_31S, "SUTM31", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_32S, "SUTM32", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_33S, "SUTM33", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_34S, "SUTM34", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_35S, "SUTM35", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_36S, "SUTM36", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_37S, "SUTM37", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_38S, "SUTM38", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_39S, "SUTM39", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_40S, "SUTM40", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_41S, "SUTM41", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_42S, "SUTM42", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_43S, "SUTM43", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_44S, "SUTM44", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_45S, "SUTM45", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_46S, "SUTM46", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_47S, "SUTM47", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_48S, "SUTM48", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_49S, "SUTM49", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_50S, "SUTM50", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_51S, "SUTM51", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_52S, "SUTM52", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_53S, "SUTM53", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_54S, "SUTM54", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_55S, "SUTM55", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_56S, "SUTM56", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_57S, "SUTM57", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_58S, "SUTM58", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_59S, "SUTM59", "WGS72BE"},
    {PCS_WGS72BE_UTM_zone_60S, "SUTM60", "WGS72BE"},
    {PCS_WGS84_UTM_zone_1N, "NUTM01", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_2N, "NUTM02", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_3N, "NUTM03", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_4N, "NUTM04", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_5N, "NUTM05", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_6N, "NUTM06", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_7N, "NUTM07", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_8N, "NUTM08", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_9N, "NUTM09", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_10N, "NUTM10", "WGS84"},
    {PCS_WGS84_UTM_zone_11N, "NUTM11", "WGS84"},
    {PCS_WGS84_UTM_zone_12N, "NUTM12", "WGS84"},
    {PCS_WGS84_UTM_zone_13N, "NUTM13", "WGS84"},
    {PCS_WGS84_UTM_zone_14N, "NUTM14", "WGS84"},
    {PCS_WGS84_UTM_zone_15N, "NUTM15", "WGS84"},
    {PCS_WGS84_UTM_zone_16N, "NUTM16", "WGS84"},
    {PCS_WGS84_UTM_zone_17N, "NUTM17", "WGS84"},
    {PCS_WGS84_UTM_zone_18N, "NUTM18", "WGS84"},
    {PCS_WGS84_UTM_zone_19N, "NUTM19", "WGS84"},
    {PCS_WGS84_UTM_zone_20N, "NUTM20", "WGS84"},
    {PCS_WGS84_UTM_zone_21N, "NUTM21", "WGS84"},
    {PCS_WGS84_UTM_zone_22N, "NUTM22", "WGS84"},
    {PCS_WGS84_UTM_zone_23N, "NUTM23", "WGS84"},
    {PCS_WGS84_UTM_zone_24N, "NUTM24", "WGS84"},
    {PCS_WGS84_UTM_zone_25N, "NUTM25", "WGS84"},
    {PCS_WGS84_UTM_zone_26N, "NUTM26", "WGS84"},
    {PCS_WGS84_UTM_zone_27N, "NUTM27", "WGS84"},
    {PCS_WGS84_UTM_zone_28N, "NUTM28", "WGS84"},
    {PCS_WGS84_UTM_zone_29N, "NUTM29", "WGS84"},
    {PCS_WGS84_UTM_zone_30N, "NUTM30", "WGS84"},
    {PCS_WGS84_UTM_zone_31N, "NUTM31", "WGS84"},
    {PCS_WGS84_UTM_zone_32N, "NUTM32", "WGS84"},
    {PCS_WGS84_UTM_zone_33N, "NUTM33", "WGS84"},
    {PCS_WGS84_UTM_zone_34N, "NUTM34", "WGS84"},
    {PCS_WGS84_UTM_zone_35N, "NUTM35", "WGS84"},
    {PCS_WGS84_UTM_zone_36N, "NUTM36", "WGS84"},
    {PCS_WGS84_UTM_zone_37N, "NUTM37", "WGS84"},
    {PCS_WGS84_UTM_zone_38N, "NUTM38", "WGS84"},
    {PCS_WGS84_UTM_zone_39N, "NUTM39", "WGS84"},
    {PCS_WGS84_UTM_zone_40N, "NUTM40", "WGS84"},
    {PCS_WGS84_UTM_zone_41N, "NUTM41", "WGS84"},
    {PCS_WGS84_UTM_zone_42N, "NUTM42", "WGS84"},
    {PCS_WGS84_UTM_zone_43N, "NUTM43", "WGS84"},
    {PCS_WGS84_UTM_zone_44N, "NUTM44", "WGS84"},
    {PCS_WGS84_UTM_zone_45N, "NUTM45", "WGS84"},
    {PCS_WGS84_UTM_zone_46N, "NUTM46", "WGS84"},
    {PCS_WGS84_UTM_zone_47N, "NUTM47", "WGS84"},
    {PCS_WGS84_UTM_zone_48N, "NUTM48", "WGS84"},
    {PCS_WGS84_UTM_zone_49N, "NUTM49", "WGS84"},
    {PCS_WGS84_UTM_zone_50N, "NUTM50", "WGS84"},
    {PCS_WGS84_UTM_zone_51N, "NUTM51", "WGS84"},
    {PCS_WGS84_UTM_zone_52N, "NUTM52", "WGS84"},
    {PCS_WGS84_UTM_zone_53N, "NUTM53", "WGS84"},
    {PCS_WGS84_UTM_zone_54N, "NUTM54", "WGS84"},
    {PCS_WGS84_UTM_zone_55N, "NUTM55", "WGS84"},
    {PCS_WGS84_UTM_zone_56N, "NUTM56", "WGS84"},
    {PCS_WGS84_UTM_zone_57N, "NUTM57", "WGS84"},
    {PCS_WGS84_UTM_zone_58N, "NUTM58", "WGS84"},
    {PCS_WGS84_UTM_zone_59N, "NUTM59", "WGS84"},
    {PCS_WGS84_UTM_zone_60N, "NUTM60", "WGS84"},
    {PCS_WGS84_UTM_zone_1S, "SUTM01", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_2S, "SUTM02", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_3S, "SUTM03", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_4S, "SUTM04", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_5S, "SUTM05", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_6S, "SUTM06", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_7S, "SUTM07", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_8S, "SUTM08", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_9S, "SUTM09", "WGS84"},		/*[02]*/
    {PCS_WGS84_UTM_zone_10S, "SUTM10", "WGS84"},
    {PCS_WGS84_UTM_zone_11S, "SUTM11", "WGS84"},
    {PCS_WGS84_UTM_zone_12S, "SUTM12", "WGS84"},
    {PCS_WGS84_UTM_zone_13S, "SUTM13", "WGS84"},
    {PCS_WGS84_UTM_zone_14S, "SUTM14", "WGS84"},
    {PCS_WGS84_UTM_zone_15S, "SUTM15", "WGS84"},
    {PCS_WGS84_UTM_zone_16S, "SUTM16", "WGS84"},
    {PCS_WGS84_UTM_zone_17S, "SUTM17", "WGS84"},
    {PCS_WGS84_UTM_zone_18S, "SUTM18", "WGS84"},
    {PCS_WGS84_UTM_zone_19S, "SUTM19", "WGS84"},
    {PCS_WGS84_UTM_zone_20S, "SUTM20", "WGS84"},
    {PCS_WGS84_UTM_zone_21S, "SUTM21", "WGS84"},
    {PCS_WGS84_UTM_zone_22S, "SUTM22", "WGS84"},
    {PCS_WGS84_UTM_zone_23S, "SUTM23", "WGS84"},
    {PCS_WGS84_UTM_zone_24S, "SUTM24", "WGS84"},
    {PCS_WGS84_UTM_zone_25S, "SUTM25", "WGS84"},
    {PCS_WGS84_UTM_zone_26S, "SUTM26", "WGS84"},
    {PCS_WGS84_UTM_zone_27S, "SUTM27", "WGS84"},
    {PCS_WGS84_UTM_zone_28S, "SUTM28", "WGS84"},
    {PCS_WGS84_UTM_zone_29S, "SUTM29", "WGS84"},
    {PCS_WGS84_UTM_zone_30S, "SUTM30", "WGS84"},
    {PCS_WGS84_UTM_zone_31S, "SUTM31", "WGS84"},
    {PCS_WGS84_UTM_zone_32S, "SUTM32", "WGS84"},
    {PCS_WGS84_UTM_zone_33S, "SUTM33", "WGS84"},
    {PCS_WGS84_UTM_zone_34S, "SUTM34", "WGS84"},
    {PCS_WGS84_UTM_zone_35S, "SUTM35", "WGS84"},
    {PCS_WGS84_UTM_zone_36S, "SUTM36", "WGS84"},
    {PCS_WGS84_UTM_zone_37S, "SUTM37", "WGS84"},
    {PCS_WGS84_UTM_zone_38S, "SUTM38", "WGS84"},
    {PCS_WGS84_UTM_zone_39S, "SUTM39", "WGS84"},
    {PCS_WGS84_UTM_zone_40S, "SUTM40", "WGS84"},
    {PCS_WGS84_UTM_zone_41S, "SUTM41", "WGS84"},
    {PCS_WGS84_UTM_zone_42S, "SUTM42", "WGS84"},
    {PCS_WGS84_UTM_zone_43S, "SUTM43", "WGS84"},
    {PCS_WGS84_UTM_zone_44S, "SUTM44", "WGS84"},
    {PCS_WGS84_UTM_zone_45S, "SUTM45", "WGS84"},
    {PCS_WGS84_UTM_zone_46S, "SUTM46", "WGS84"},
    {PCS_WGS84_UTM_zone_47S, "SUTM47", "WGS84"},
    {PCS_WGS84_UTM_zone_48S, "SUTM48", "WGS84"},
    {PCS_WGS84_UTM_zone_49S, "SUTM49", "WGS84"},
    {PCS_WGS84_UTM_zone_50S, "SUTM50", "WGS84"},
    {PCS_WGS84_UTM_zone_51S, "SUTM51", "WGS84"},
    {PCS_WGS84_UTM_zone_52S, "SUTM52", "WGS84"},
    {PCS_WGS84_UTM_zone_53S, "SUTM53", "WGS84"},
    {PCS_WGS84_UTM_zone_54S, "SUTM54", "WGS84"},
    {PCS_WGS84_UTM_zone_55S, "SUTM55", "WGS84"},
    {PCS_WGS84_UTM_zone_56S, "SUTM56", "WGS84"},
    {PCS_WGS84_UTM_zone_57S, "SUTM57", "WGS84"},
    {PCS_WGS84_UTM_zone_58S, "SUTM58", "WGS84"},
    {PCS_WGS84_UTM_zone_59S, "SUTM59", "WGS84"},
    {PCS_WGS84_UTM_zone_60S, "SUTM60", "WGS84"},
	{PCS_Ain_el_Abd_Bahrain_Grid,NULL,"AINABD70"}, //the following Added by GAN 20/1/04
	{PCS_Afgooye_UTM_zone_38N,"NUTM38",NULL},      
	{PCS_Afgooye_UTM_zone_39N,"NUTM39",NULL},      
	{PCS_Lisbon_Portugese_Grid,NULL,NULL},			
	{PCS_Batavia_NEIEZ,NULL,"BATAVIA"},				
	{PCS_Belge_Lambert_50,NULL,NULL},
	{PCS_Bern_1898_Swiss_Old,NULL,NULL},
	{PCS_Bogota_Colombia_3W,NULL,"BOGOTA"},
	{PCS_Bogota_Colombia_Bogota,NULL,"BOGOTA"},
	{PCS_Bogota_Colombia_3E,NULL,"BOGOTA"},
	{PCS_Bogota_Colombia_6E,NULL,"BOGOTA"},
	{PCS_Carthage_Nord_Tunisie,NULL,"CARTHAGE"},
	{PCS_Carthage_Sud_Tunisie,NULL,"CARTHAGE"},
	{PCS_Jamaica_1875_Old_Grid,NULL,"JA1875"},
	{PCS_JAD69_Jamaica_Grid,NULL,"JAD69"},
	{PCS_Kalianpur_India_0,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_I,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IIa,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IIIa,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IVa,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IIb,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IIIb,NULL,"KALIANPR"},
	{PCS_Kalianpur_India_IVb,NULL,"KALIANPR"},
	{PCS_Kertau_Singapore_Grid,NULL,"KERTAU"},
	{PCS_Leigon_Ghana_Grid,NULL,"KERTAU"},
	{PCS_Lome_UTM_zone_31N,"NUTM31",NULL},
	{PCS_Makassar_NEIEZ,NULL,"MAKASSAR"},
	{PCS_Merchich_Nord_Maroc,NULL,NULL},
	{PCS_Merchich_Sud_Maroc,NULL,NULL},
	{PCS_Merchich_Sahara,NULL,NULL},
	{PCS_ATF_Nord_de_Guerre,NULL,NULL},
	{PCS_Timbalai_1948_Borneo,NULL,NULL},
	{PCS_Belge_Lambert_72,NULL,NULL},			//End of addition by GAN
	{0, NULL, NULL}
};



/* Standard TM->ERM Mappings */

static struct pcs_erm_mapping pcs_erm_tm[] = {
    {PCS_AGD66_AMG_zone_48, "TMAMG48", "AGD66"},
    {PCS_AGD66_AMG_zone_49, "TMAMG49", "AGD66"},
    {PCS_AGD66_AMG_zone_50, "TMAMG50", "AGD66"},
    {PCS_AGD66_AMG_zone_51, "TMAMG51", "AGD66"},
    {PCS_AGD66_AMG_zone_52, "TMAMG52", "AGD66"},
    {PCS_AGD66_AMG_zone_53, "TMAMG53", "AGD66"},
    {PCS_AGD66_AMG_zone_54, "TMAMG54", "AGD66"},
    {PCS_AGD66_AMG_zone_55, "TMAMG55", "AGD66"},
    {PCS_AGD66_AMG_zone_56, "TMAMG56", "AGD66"},
    {PCS_AGD66_AMG_zone_57, "TMAMG57", "AGD66"},
    {PCS_AGD66_AMG_zone_58, "TMAMG58", "AGD66"},
    {PCS_AGD84_AMG_zone_48, "TMAMG48", "AGD84"},
    {PCS_AGD84_AMG_zone_49, "TMAMG49", "AGD84"},
    {PCS_AGD84_AMG_zone_50, "TMAMG50", "AGD84"},
    {PCS_AGD84_AMG_zone_51, "TMAMG51", "AGD84"},
    {PCS_AGD84_AMG_zone_52, "TMAMG52", "AGD84"},
    {PCS_AGD84_AMG_zone_53, "TMAMG53", "AGD84"},
    {PCS_AGD84_AMG_zone_54, "TMAMG54", "AGD84"},
    {PCS_AGD84_AMG_zone_55, "TMAMG55", "AGD84"},
    {PCS_AGD84_AMG_zone_56, "TMAMG56", "AGD84"},
    {PCS_AGD84_AMG_zone_57, "TMAMG57", "AGD84"},
    {PCS_AGD84_AMG_zone_58, "TMAMG58", "AGD84"},
    {PCS_Arc_1950_Lo13, "STMLO13", "ARC1950"},
    {PCS_Arc_1950_Lo15, "STMLO15", "ARC1950"},
    {PCS_Arc_1950_Lo17, "STMLO17", "ARC1950"},
    {PCS_Arc_1950_Lo19, "STMLO19", "ARC1950"},
    {PCS_Arc_1950_Lo21, "STMLO21", "ARC1950"},
    {PCS_Arc_1950_Lo23, "STMLO23", "ARC1950"},
    {PCS_Arc_1950_Lo25, "STMLO25", "ARC1950"},
    {PCS_Arc_1950_Lo27, "STMLO27", "ARC1950"},
    {PCS_Arc_1950_Lo29, "STMLO29", "ARC1950"},
    {PCS_Arc_1950_Lo31, "STMLO31", "ARC1950"},
    {PCS_Arc_1950_Lo33, "STMLO33", "ARC1950"},
    {PCS_Arc_1950_Lo35, "STMLO35", "ARC1950"},
    {PCS_Beijing_Gauss_zone_13, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_14, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_15, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_16, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_17, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_18, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_19, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_20, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_21, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_22, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_zone_23, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_13N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_14N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_15N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_16N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_17N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_18N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_19N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_20N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_21N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_22N, NULL, "BEIJING"},
    {PCS_Beijing_Gauss_23N, NULL, "BEIJING"},
    {PCS_C_Inchauspe_Argentina_1, "TMARG1", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_2, "TMARG2", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_3, "TMARG3", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_4, "TMARG4", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_5, "TMARG5", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_6, "TMARG6", "CMPOINCH"},
    {PCS_C_Inchauspe_Argentina_7, "TMARG7", "CMPOINCH"},
    {PCS_Egypt_1907_Red_Belt, "TMEGYPTR", "EGYPT07"},
    {PCS_Egypt_1907_Purple_Belt, "TMEGYPTP", "EGYPT07"},
    {PCS_Egypt_1907_Ext_Purple, "TMEGYPTS", "EGYPT07"},
    {PCS_PSAD56_Peru_west_zone, "TMPERUW", "PSAD56"},
    {PCS_PSAD56_Peru_central, "TMPERUC", "PSAD56"},
    {PCS_PSAD56_Peru_east_zone, "TMPERUE", "PSAD56"},
    {PCS_Luzon_Philippines_I, "TMPHIL1", "LUZON11"},
    {PCS_Luzon_Philippines_II, "TMPHIL2", "LUZON11"},
    {PCS_Luzon_Philippines_III, "TMPHIL3", "LUZON11"},
    {PCS_Luzon_Philippines_IV, "TMPHIL4", "LUZON11"},
    {PCS_Luzon_Philippines_V, "TMPHIL5", "LUZON11"},
    {PCS_Minna_Nigeria_West, NULL, "MINAA"},
    {PCS_Minna_Nigeria_Mid_Belt, NULL, "MINAA"},
    {PCS_Minna_Nigeria_East, NULL, "MINAA"},
    {PCS_Monte_Mario_Italy_1, NULL, "MONTEMAR"},
    {PCS_Monte_Mario_Italy_2, NULL, "MONTEMAR"},
	{PCS_NAD27_Alabama_East, "NAD27", "TMALABEF"},
    {PCS_NAD27_Alabama_West, "NAD27", "TMALABWF"},
    {PCS_GD49_NZ_Map_Grid, "NZMG", "NZGD49"},
    {PCS_GD49_North_Island_Grid, NULL, "NZGD49"},
    {PCS_GD49_South_Island_Grid, NULL, "NZGD49"},
    {PCS_Pulkovo_Gauss_zone_4, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_5, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_6, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_7, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_8, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_9, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_10, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_11, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_12, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_13, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_14, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_15, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_16, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_17, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_18, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_19, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_20, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_21, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_22, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_23, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_24, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_25, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_26, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_27, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_28, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_29, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_30, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_31, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_zone_32, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_4N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_5N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_6N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_7N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_8N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_9N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_10N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_11N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_12N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_13N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_14N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_15N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_16N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_17N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_18N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_19N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_20N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_21N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_22N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_23N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_24N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_25N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_26N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_27N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_28N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_29N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_30N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_31N, NULL, "PULKOVO"},
    {PCS_Pulkovo_Gauss_32N, NULL, "PULKOVO"},
    {PCS_Qatar_National_Grid, NULL,"QATAR"},
    {PCS_RD_Netherlands_Old, NULL, "RD"},
    {PCS_RD_Netherlands_New, "NUTM31", "RD"}, /** [04] INCLUDED **/
    {PCS_Tananarive_Laborde, NULL, "TANANAR"},
    {PCS_TM65_Irish_Nat_Grid, "TMOSIRL", "TM65"},
    {PCS_Trinidad_1903_Trinidad, NULL, NULL},
    {PCS_Voirol_N_Algerie_ancien, NULL, NULL},
    {PCS_Voirol_S_Algerie_ancien, NULL, NULL},
    {PCS_Voirol_Unifie_N_Algerie, NULL, NULL},
    {PCS_Voirol_Unifie_S_Algerie, NULL, NULL},
    {PCS_Bern_1938_Swiss_New, NULL, NULL},
    {PCS_MGI_Austria_West, NULL, NULL},
    {PCS_MGI_Austria_Central, NULL, NULL},
    {PCS_MGI_Austria_East, NULL, NULL},
    {PCS_DHDN_Germany_zone_1, NULL, NULL},
    {PCS_DHDN_Germany_zone_2, NULL, NULL},
    {PCS_DHDN_Germany_zone_3, NULL, NULL},
    {PCS_DHDN_Germany_zone_4, NULL, NULL},
    {PCS_DHDN_Germany_zone_5, NULL, NULL},
	{0, NULL, NULL}
};

/* Standard SPCS->ERM Mappings */

static struct pcs_erm_mapping pcs_erm_spcs[] = {
    {PCS_NAD27_Alabama_East,  "TMALABEF", "NAD27"},
    {PCS_NAD27_Alabama_West, "TMALABWF", "NAD27"},
    {PCS_NAD27_Alaska_zone_1, "OMALSK1F", "NAD27"},
    {PCS_NAD27_Alaska_zone_2, "TMALSK2F", "NAD27"},
    {PCS_NAD27_Alaska_zone_3, "TMALSK3F", "NAD27"}, 		
    {PCS_NAD27_Alaska_zone_4, "TMALSK4F", "NAD27"},			
    {PCS_NAD27_Alaska_zone_5, "TMALSK5F", "NAD27"},			
    {PCS_NAD27_Alaska_zone_6, "TMALSK6F", "NAD27"},		
    {PCS_NAD27_Alaska_zone_7, "TMALSK7F", "NAD27"},
    {PCS_NAD27_Alaska_zone_8, "TMALSK8F", "NAD27"},
    {PCS_NAD27_Alaska_zone_9, "TMALSK9F", "NAD27"},
    {PCS_NAD27_Alaska_zone_10, "L2ALS10F", "NAD27"},		/** [03] CHANGED **/
    {PCS_NAD27_California_I, "LM2CAL1F", "NAD27"},
    {PCS_NAD27_California_II, "LM2CAL2F", "NAD27"},
    {PCS_NAD27_California_III, "LM2CAL3F", "NAD27"},
    {PCS_NAD27_California_IV, "LM2CAL4F", "NAD27"},
    {PCS_NAD27_California_V, "LM2CAL5F", "NAD27"},
    {PCS_NAD27_California_VI, "LM2CAL6F", "NAD27"},
    {PCS_NAD27_California_VII, "LM2CAL7F", "NAD27"},
    {PCS_NAD27_Arizona_East, "TMARIZEF", "NAD27"},
    {PCS_NAD27_Arizona_Central, "TMARIZCF", "NAD27"},
    {PCS_NAD27_Arizona_West, "TMALABWF", "NAD27"},
    {PCS_NAD27_Arkansas_North, "LM2ARKNF", "NAD27"},
    {PCS_NAD27_Arkansas_South, "LM2ARKSF", "NAD27"},
    {PCS_NAD27_Colorado_North, "LM2COLNF", "NAD27"},
    {PCS_NAD27_Colorado_Central, "LM2COLCF", "NAD27"},          /** [01] CHANGED **/
    {PCS_NAD27_Colorado_South, "LM2COLSF", "NAD27"},
    {PCS_NAD27_Connecticut, "LM2CONNF", "NAD27"},
    {PCS_NAD27_Delaware, "TMDELWRF", "NAD27"},                  /** [01] CHANGED **/
    {PCS_NAD27_Florida_East, "TMFLRAEF", "NAD27"},
    {PCS_NAD27_Florida_West, "TMFLRAWF", "NAD27"},
    {PCS_NAD27_Florida_North, "LM2FLANF", "NAD27"},
    {PCS_NAD27_Hawaii_zone_1, "TMHAWI1F", "NAD27"},
    {PCS_NAD27_Hawaii_zone_2, "TMHAWI2F", "NAD27"},
    {PCS_NAD27_Hawaii_zone_3, "TMHAWI3F", "NAD27"},
    {PCS_NAD27_Hawaii_zone_4, "TMHAWI4F", "NAD27"},
    {PCS_NAD27_Hawaii_zone_5, "TMHAWI5F", "NAD27"},
    {PCS_NAD27_Georgia_East, "TMGEOREF", "NAD27"},
    {PCS_NAD27_Georgia_West, "TMGEORWF", "NAD27"},
    {PCS_NAD27_Idaho_East, "TMIDAEFT", "NAD27"},
    {PCS_NAD27_Idaho_Central, "TMIDACFT", "NAD27"},
    {PCS_NAD27_Idaho_West, "TMIDAWFT", "NAD27"},
    {PCS_NAD27_Illinois_East, "TMILLEFT", "NAD27"},
    {PCS_NAD27_Illinois_West, "TMILLWFT", "NAD27"},
    {PCS_NAD27_Indiana_East, "TMINDEFT", "NAD27"},
    {PCS_NAD27_BLM_14N_feet, NULL, "NAD27"},            	/** [01] NOTFOUND **/    
    {PCS_NAD27_Indiana_West, "TMINDWFT", "NAD27"},
    {PCS_NAD27_BLM_15N_feet, NULL, "NAD27"},            	/** [01] NOTFOUND **/    
    {PCS_NAD27_Iowa_North, "LM2IOWNF", "NAD27"},
    {PCS_NAD27_BLM_16N_feet, NULL, "NAD27"},            	/** [01] NOTFOUND **/    
    {PCS_NAD27_Iowa_South, "LM2IOWSF", "NAD27"},
    {PCS_NAD27_BLM_17N_feet, NULL, "NAD27"},            	/** [01] NOTFOUND **/    
    {PCS_NAD27_Kansas_North, "LM2KANNF", "NAD27"},
    {PCS_NAD27_Kansas_South, "L2KANSF", "NAD27"},             	/** [03] CHANGED **/    
    {PCS_NAD27_Kentucky_North, "LM2KYNFT", "NAD27"},
    {PCS_NAD27_Kentucky_South, "LM2KYSFT", "NAD27"},
    {PCS_NAD27_Louisiana_North, "LM2LANFT", "NAD27"},
    {PCS_NAD27_Louisiana_South, "LM2LASFT", "NAD27"},
    {PCS_NAD27_Maine_East, "TMMAINEF", "NAD27"},
    {PCS_NAD27_Maine_West, "TMMAINWF", "NAD27"},
    {PCS_NAD27_Maryland, "LM2MARYF", "NAD27"},
    {PCS_NAD27_Massachusetts, "LM2MASMF", "NAD27"},
    {PCS_NAD27_Massachusetts_Is, "L2MASIF27", "NAD27"},         /** [03] CHANGED **/
    {PCS_NAD27_Michigan_North, "LM2MICNF", "NAD27"},
    {PCS_NAD27_Michigan_Central, "LM2MICCF", "NAD27"},          /** [01] CHANGED **/
    {PCS_NAD27_Michigan_South, "LM2MICSF", "NAD27"},
    {PCS_NAD27_Minnesota_North, "LM2MINNF", "NAD27"},
    {PCS_NAD27_Minnesota_Cent, "LM2MINCF", "NAD27"},            /** [01] CHANGED **/
    {PCS_NAD27_Minnesota_South, "LM2MINSF", "NAD27"},
    {PCS_NAD27_Mississippi_East, "TMMISSEF", "NAD27"},          /** [01] CHANGED **/
    {PCS_NAD27_Mississippi_West, "TMMISSWF", "NAD27"},          /** [01] CHANGED **/
    {PCS_NAD27_Missouri_East, "TMMISOEF", "NAD27"},
    {PCS_NAD27_Missouri_Central, "TMISOCF", "NAD27"},           /** [01] CHANGED **/
    {PCS_NAD27_Missouri_West, "TMMISOWF", "NAD27"},
    {PCS_NAD_Michigan_Michigan_East, "TMMICHEF", "NAD27"},
    {PCS_NAD_Michigan_Michigan_Old_Central, "TMMICHCF", "NAD27"},/** [01] CHANGED **/
    {PCS_NAD_Michigan_Michigan_West, "TMMICHWF", "NAD27"},
    {PCS_NAD83_Alabama_East, "TMALABEM", "NAD83"},            	/** [03] CHANGED **/  
    {PCS_NAD83_Alabama_West, "TMALABWM", "NAD83"},
    {PCS_NAD83_Alaska_zone_1, "OMALSK1M", "NAD83"},
    {PCS_NAD83_Alaska_zone_2, "TMALSK2M", "NAD83"},
    {PCS_NAD83_Alaska_zone_3, "TMALSK3M", "NAD83"},
    {PCS_NAD83_Alaska_zone_4, "TMALSK4M", "NAD83"},
    {PCS_NAD83_Alaska_zone_5, "TMALSK5M", "NAD83"},             /** [01] CHANGED **/
    {PCS_NAD83_Alaska_zone_6, "TMALSK6M", "NAD83"},
    {PCS_NAD83_Alaska_zone_7, "TMALSK7M", "NAD83"},
    {PCS_NAD83_Alaska_zone_8, "TMALSK8M", "NAD83"},
    {PCS_NAD83_Alaska_zone_9, "TMALSK9M", "NAD83"},
    {PCS_NAD83_Alaska_zone_10, "L2ALS10M", "NAD83"},          	/** [03] CHANGED **/    
    {PCS_NAD83_California_1, "L2CAL1M", "NAD83"},            	/** [03] CHANGED **/    
    {PCS_NAD83_California_2, "L2CAL2M", "NAD83"},            	/** [03] CHANGED **/  
    {PCS_NAD83_California_3, "L2CAL3M", "NAD83"},            	/** [03] CHANGED **/
    {PCS_NAD83_California_4, "L2CAL4M", "NAD83"},            	/** [03] CHANGED **/    
    {PCS_NAD83_California_5, "LM2CAL5M", "NAD83"},              /** [01] CHANGED **/
    {PCS_NAD83_California_6, "L2CAL6M", "NAD83"},            	/** [03] CHANGED **/  
    {PCS_NAD83_Arizona_East, "TMARIZEF83", "NAD83"},            /** [03] CHANGED **/   
    {PCS_NAD83_Arizona_Central, "TMARIZCF83", "NAD83"},         /** [03] CHANGED **/    
    {PCS_NAD83_Arizona_West, "TMARIZWF83", "NAD83"},            /** [03] CHANGED **/   
    {PCS_NAD83_Arkansas_North, "LM2ARKNM", "NAD83"},            /** [01] CHANGED **/
    {PCS_NAD83_Arkansas_South, "LM2ARKSM", "NAD83"},            /** [01] CHANGED **/
    {PCS_NAD83_Colorado_North, "L2COLNM", "NAD83"},             /** [03] CHANGED **/
    {PCS_NAD83_Colorado_Central, "L2COLCM", "NAD83"},     	/** [03] CHANGED **/
    {PCS_NAD83_Colorado_South, "L2COLSM", "NAD83"},             /** [03] CHANGED **/
    {PCS_NAD83_Connecticut, "LM2CONNM", "NAD83"},               /** [01] CHANGED **/
    {PCS_NAD83_Delaware, "TMDELWRM", "NAD83"},			/** [03] CHANGED **/	  
    {PCS_NAD83_Florida_East, "TMFLRAEM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Florida_West, "TMFLRAWM", "NAD83"},		/** [03] CHANGED **/	
    {PCS_NAD83_Florida_North, "TMFLRANM", "NAD83"},		/** [03] CHANGED **/	  
    {PCS_NAD83_Hawaii_zone_1, "TMHAWI1M", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Hawaii_zone_2, "TMHAWI2M", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Hawaii_zone_3, "TMHAWI3M", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Hawaii_zone_4, "TMHAWI4M", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Hawaii_zone_5, "TMHAWI5M", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Georgia_East, "TMGEOREM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Georgia_West, "TMGEORWM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Idaho_East, "TMIDAEM", "NAD83"},			/** [03] CHANGED **/	    
    {PCS_NAD83_Idaho_Central, "TMIDACM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Idaho_West, "TMIDAWM", "NAD83"},			/** [03] CHANGED **/	 
    {PCS_NAD83_Illinois_East, "TMILLEM", "NAD83"},		/** [03] CHANGED **/	 
    {PCS_NAD83_Illinois_West, "TMILLWM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Indiana_East, "TMINDEM", "NAD83"},		/** [03] CHANGED **/	  
    {PCS_NAD83_Indiana_West, "TMINDWM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Iowa_North, "L2IOWNM", "NAD83"},			/** [03] CHANGED **/	 
    {PCS_NAD83_Iowa_South, "L2IOWSM", "NAD83"},              	/** [03] CHANGED **/
    {PCS_NAD83_Kansas_North, "LM2KANNM", "NAD83"},              /** [01] CHANGED **/    
    {PCS_NAD83_Kansas_South, "L2KANSM", "NAD83"},		/** [03] CHANGED **/ 	
    {PCS_NAD83_Kentucky_North, "LM2KYNM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_Kentucky_South, "L2KYSM", "NAD83"},              /** [03] CHANGED **/
    {PCS_NAD83_Louisiana_North, "LM2LANM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Louisiana_South, "LM2LASM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Maine_East, "TMMAINEM", "NAD83"},                /** [01] CHANGED **/  
    {PCS_NAD83_Maine_West, "TMMAINWM", "NAD83"},                /** [03] CHANGED **/  
    {PCS_NAD83_Maryland, "L2MARYM", "NAD83"},			/** [03] CHANGED **/ 	 
    {PCS_NAD83_Massachusetts, "L2MASMM", "NAD83"},		/** [03] CHANGED **/ 	    
    {PCS_NAD83_Massachusetts_Is, "LM2MASIM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Michigan_North, "L2MICNM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Michigan_Central, "L2MICCM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Michigan_South, "L2MICSM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Minnesota_North, "L2MINNM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Minnesota_Cent, "L2MINCM", "NAD83"},		/** [03] CHANGED **/	  
    {PCS_NAD83_Minnesota_South, "L2MINSM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Mississippi_East, "TMMISSEM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Mississippi_West, "TMMISSWM", "NAD83"},          /** [01] CHANGED **/  
    {PCS_NAD83_Missouri_East, "TMMISOEM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Missouri_Central, "TMMISOCM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Missouri_West, "TMMISOWM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD27_Montana_North,"LM2MTNFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Montana_Central,"LM2MTCFT", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Montana_South,"LM2MTSFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Nebraska_North,"LM2NEBNF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Nebraska_South,"LM2NEBSF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Nevada_East,"TMNEVAEF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Nevada_Central,"TMNEVACF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Nevada_West,"TMNEVAWF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_New_Hampshire,"TMNEWHFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_New_Jersey,"TMNEWJFT", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_New_Mexico_East,"TMNEWMEF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_New_Mexico_Cent,"TMNEWMCF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_New_Mexico_West,"TMNEWMWF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_New_York_East,"TMNEWYEF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_New_York_Central,"TMNEWYCF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_New_York_West,"TMNEWYWF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_New_York_Long_Is,"L2NEWYLIF", "NAD27"},	        /** [03] CHANGED **/  
    {PCS_NAD27_North_Carolina,"LM2NCAFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_North_Dakota_N,"LM2NDNFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_North_Dakota_S,"LM2NDSFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Ohio_North,"LM2OHINF", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_Ohio_South,"LM2OHISF", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_Oklahoma_North,"LM2OKLNF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Oklahoma_South,"LM2OKLSF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Oregon_North,"LM2ORENF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Oregon_South,"LM2ORESF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Pennsylvania_N,"LM2PANFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Pennsylvania_S,"LM2PASFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Rhode_Island,"TMRHODIF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_South_Carolina_N,"LM2SCNFT", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_South_Carolina_S,"LM2SCSFT", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_South_Dakota_N,"LM2SDNFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_South_Dakota_S,"LM2SDSFT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Tennessee,"L2TENNF27", "NAD27"},			/** [03] CHANGED **/  
    {PCS_NAD27_Texas_North,"L2TXNF27", "NAD27"},		/** [03] CHANGED **/  
    {PCS_NAD27_Texas_North_Cen,"LM2TXNCF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Texas_Central,"LM2TXCF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Texas_South_Cen,"LM2TXSCF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Texas_South,"LM2TXSF", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_Utah_North,"LM2UTHNF", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_Utah_Central,"LM2UTHCF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Utah_South,"LM2UTHSF", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD27_Vermont,"TMVERMTF", "NAD27"},
    {PCS_NAD27_Virginia_North,"LM2VIRNF","NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Virginia_South,"LM2VIRSF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Washington_North,"LM2WSHNF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Washington_South,"LM2WSHSF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_West_Virginia_N,"LM2WVANF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_West_Virginia_S,"LM2WVASF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Wisconsin_North,"LM2WISNF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Wisconsin_Cen,"LM2WISCF", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Wisconsin_South,"LM2WISSF", "NAD27"},	        /** [01] CHANGED **/  
    {PCS_NAD27_Wyoming_East,"TMWY01FT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Wyoming_E_Cen,"TMWY02FT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Wyoming_W_Cen,"TMWY03FT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Wyoming_West,"TMWY04FT", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_Puerto_Rico,"LM2PRV1F", "NAD27"},		/** [01] CHANGED **/  
    {PCS_NAD27_St_Croix,"LM2PRV2F", "NAD27"},			/** [01] CHANGED **/  
    {PCS_NAD83_Montana, "LM2MTM", "NAD83"},			/** [01] CHANGED **/  
    {PCS_NAD83_Nebraska, "LM2NEBM", "NAD83"},			/** [01] CHANGED **/  
    {PCS_NAD83_Nevada_East, "TMNEVAEM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_Nevada_Central, "TMNEVACM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Nevada_West, "TMNEVAWM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_New_Hampshire, "TMNEWHM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_New_Jersey, "TMNEWJM", "NAD83"},			/** [03] CHANGED **/    
    {PCS_NAD83_New_Mexico_East, "TMNEWMEM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_New_Mexico_Cent, "TMNEWMCM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_New_Mexico_West, "TMNEWMWM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_New_York_East, "TMNEWYEM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_New_York_Central, "TMNEWYCM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_New_York_West, "TMNEWYWM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_New_York_Long_Is, "L2NEWYM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_North_Carolina, "L2NCAM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_North_Dakota_N, "LM2NDNM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_North_Dakota_S, "L2NDSM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Ohio_North, "L2OHINM", "NAD83"},			/** [03] CHANGED **/	    
    {PCS_NAD83_Ohio_South, "L2OHISM", "NAD83"},			/** [03] CHANGED **/	   
    {PCS_NAD83_Oklahoma_North, "LM2OKLNM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Oklahoma_South, "LM2OKLSM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Oregon_North, "L2ORENM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Oregon_South, "L2ORESM", "NAD83"},		/** [03] CHANGED **/	  
    {PCS_NAD83_Pennsylvania_N, "L2PANM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Pennsylvania_S, "L2PASM", "NAD83"},		/** [03] CHANGED **/	   
    {PCS_NAD83_Rhode_Island, "TMRHODIM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_South_Carolina, "L2SCM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_South_Dakota_N, "L2SDNM", "NAD83"},		/** [03] CHANGED **/   
    {PCS_NAD83_South_Dakota_S, "L2SDSM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Tennessee, "L2TENNM", "NAD83"},			/** [03] CHANGED **/   
    {PCS_NAD83_Texas_North, "L2TXNM", "NAD83"},			/** [03] CHANGED **/    
    {PCS_NAD83_Texas_North_Cen, "LM2TXNCM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Texas_Central, "LM2TXCM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_Texas_South_Cen, "LM2TXSCM", "NAD83"},	        /** [01] CHANGED **/  
    {PCS_NAD83_Texas_South, "LM2TXSM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_Utah_North, "L2UTHNM", "NAD83"},			/** [03] CHANGED **/   
    {PCS_NAD83_Utah_Central, "L2UTHCM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Utah_South, "L2UTHSM", "NAD83"},			/** [03] CHANGED **/    
    {PCS_NAD83_Vermont, "TMVERMTM", "NAD83"},			/** [03] CHANGED **/ 	    
    {PCS_NAD83_Virginia_North, "L2VIRNM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Virginia_South, "L2VIRSM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Washington_North, "L2WSHNM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Washington_South, "L2WSHSM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_West_Virginia_N, "L2WVANM", "NAD83"},		/** [03] CHANGED **/	  
    {PCS_NAD83_West_Virginia_S, "L2WVASM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Wisconsin_North, "L2WISNM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Wisconsin_Cen, "L2WISCM", "NAD83"},		/** [03] CHANGED **/	    
    {PCS_NAD83_Wisconsin_South, "L2WISSM", "NAD83"},		/** [03] CHANGED **/    
    {PCS_NAD83_Wyoming_East, "TMWYOEM", "NAD83"},		/** [03] CHANGED **/          
    {PCS_NAD83_Wyoming_E_Cen, "TMWYOECM", "NAD83"},		/** [01] CHANGED **/  
    {PCS_NAD83_Wyoming_W_Cen, "TMWYOWCM", "NAD83"},	        /** [03] CHANGED **/  
    {PCS_NAD83_Wyoming_West, "TMWYOWM", "NAD83"},		/** [03] CHANGED **/   
    {PCS_NAD83_Puerto_Rico_Virgin_Is, "L2PRVIM", "NAD83"},	/** [03] CHANGED **/  
    {PCS_GDA94_GEODETIC_FIX, "GEODETIC", "GDA94"},		/** [06] CHANGED **/  
    {4326, "GEODETIC", "WGS84"}, 
	{0, NULL, NULL}
};

static struct pcs_erm_mapping* arrayOfArrays[] = {
	pcs_erm_utm,
	pcs_erm_tm,
	pcs_erm_spcs,
	NULL };
