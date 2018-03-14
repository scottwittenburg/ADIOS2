#!/usr/bin/env bash

if [ -z "${DOCPPCHECK}" ]
then
  g++ "$@"
else
  while [ $# -gt 1 ]
  do
    if [ "$1" == "-c" ]
    then
      { RESULT=$(${CPPCHECK_EXE} --enable=style --template="CPPCHECK CATCH - {file}:{line}: ({severity}) => {id} {message}" "$2" 2>&1 1>&3-) ;} 3>&1
      if [[ "$RESULT" =~ "CPPCHECK CATCH" ]]; then
        echo "${RESULT}"
        exit 1
      fi
    fi
    shift
  done
fi
