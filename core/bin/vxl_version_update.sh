#!/bin/sh

# Update date version in vxl_version.h.  Run this script with no
# arguments.  It will automatically find vxl_version.h and replace its
# DATE_YEAR, DATE_MONTH, and DATE_DAY macro definitions with the
# current date.

vxl_version_dir=`cd "\`echo $0 | sed -n '/\//{s/\/[^\/]*$//;p;}'\`";cd ..;pwd`
vxl_version_date_year=`date +%Y`
vxl_version_date_month=`date +%m`
vxl_version_date_day=`date +%d`

cat "${vxl_version_dir}/vxl_version.h" |
sed "
/^#define VXL_VERSION_DATE_YEAR .*\$/ {s/^.*\$/#define VXL_VERSION_DATE_YEAR ${vxl_version_date_year}/;}
/^#define VXL_VERSION_DATE_MONTH .*\$/ {s/^.*\$/#define VXL_VERSION_DATE_MONTH ${vxl_version_date_month}/;}
/^#define VXL_VERSION_DATE_DAY .*\$/ {s/^.*\$/#define VXL_VERSION_DATE_DAY ${vxl_version_date_day}/;}
" > "${vxl_version_dir}/vxl_version.h.new" &&
mv "${vxl_version_dir}/vxl_version.h.new" "${vxl_version_dir}/vxl_version.h" ||
rm -f "${vxl_version_dir}/vxl_version.h.new"
