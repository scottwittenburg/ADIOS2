#!/usr/bin/env bash

if [ -z "${DO_CL_ANAlYZE}" ]
then
  g++ "$@"
else
  { RESULT=$(${CXX_ANALYZER} "$@" 2>&1 1>&3-) ;} 3>&1
  shopt -s nocasematch
  if [[ "$RESULT" =~ warning|error ]];
  then
    echo "clang-analyer ERROR: ${RESULT}"
    exit 1
  fi
  echo "${RESULT}"
  exit 0
fi
