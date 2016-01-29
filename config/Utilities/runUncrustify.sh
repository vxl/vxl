#!/bin/bash

## the intent of this file is to clean the files, make them KWSTYLE compliant,
## and then make sure that subsequent runs of this script do not modify `
## The goal of this script is to develop a consistent system of auto-formating
## and format testing such that if uncrustify is run, then the code will pass
## the KWStyle format checker

## http://www.itk.org/Wiki/ITKv4_StyleChangeProposal

## Hint for how to run on all code
if [[ 0 -eq 1 ]]; then
  for vxl_subdir in config Testing contrib core vcl scripts; do
    find ${vxl_subdir} -name "*.[thc]xx" |xargs config/Utilities/runUncrustify.sh ;
    find ${vxl_subdir} -name "*.h" |xargs config/Utilities/runUncrustify.sh ;
  done
fi

if [[ -z "${UNCRUSTIFYBIN}" ]]; then
  UNCRUSTIFYBIN=/opt/uncrustify/bin/uncrustify
fi
if [[ ! -f ${UNCRUSTIFYBIN} ]];then
  UNCRUSTIFYBIN=/usr/local/bin/uncrustify
fi

CONFIG=$(dirname $0)/uncrustify_vxl_aggressive.cfg

## uncrustify -c ${CONFIG} --update-config-with-doc  -o ${CONFIG}_NEW

if [[ ! -f "${CONFIG}" ]]; then
  echo "failed to find config file"
  exit -1
fi

for file in $@; do
    echo "======= ${file}"
    if [[ ! -f ${KWERROS}_skip_uncrustify ]]; then
        ### A possible bug in uncrustify breaks complicated macros by adding extra spaces surrounding "##" and "#"
        ### This is an interaction betweeen the macro directives and indenting of includes and defines.
        #echo ${UNCRUSTIFYBIN} -c ${CONFIG} -l CPP -f ${file} | sed 's/  *##  */##/g' | sed 's/#  */#/g'
        #${UNCRUSTIFYBIN} -c ${CONFIG} -l CPP -f ${file} | sed 's/  *##  */##/g' | sed 's/#  */#/g' |sed 's///g' > ${file}_uncrustify
        ${UNCRUSTIFYBIN} -c ${CONFIG} -l CPP -f ${file} | sed 's///g' > ${file}_uncrustify
        mv ${file}_uncrustify ${file}
    fi
done
