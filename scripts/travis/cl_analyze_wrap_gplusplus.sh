#!/usr/bin/env bash

if [ -z "${DO_CL_ANAlYZE}" ]
then
  g++ "$@"
else
  { RESULT=$(${CXX_ANALYZER} "$@" 2>&1 1>&3-) ;} 3>&1
  echo "${RESULT}"
  shopt -s nocasematch
  if [[ "$RESULT" =~ warning|error ]]; then
    exit 1
  fi
  exit 0
fi
