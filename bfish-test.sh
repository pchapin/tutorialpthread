#!/bin/bash

# Check for executable files.
if [ ! -f bfish -o ! -x bfish ]; then
  echo "Error: can't locate executable file 'bfish'"
  exit 1
fi

if [ ! -f bfishmt -o ! -x bfishmt ]; then
  echo "Error: can't locate executable file 'bfishmt'"
  exit 1
fi

# Check for test files.
if [ -f afile.txt -o -f afile.enc -o -f afile.out ]; then
  echo "Warning: Test file: afile.txt, afile.enc, or afile.out exists"
  echo "Overwrite? "
  read RESPONSE junk
  if [ $RESPONSE = 'n' ]; then
    exit 1
  fi
  rm -f afile.txt afile.enc afile.out
fi

# Should search the path for makebig. Maybe later

# Perform tests.
ERROR=0
for SIZE in 2048 4096 6144 8192 10250; do
  echo "Size = $SIZE"
  makebig -b$SIZE > afile.txt 2> /dev/null

  echo "bfish..."
  bfish -e afile.txt afile.enc "Hello, World"
  bfish -d afile.enc afile.out "Hello, World"
  cmp afile.txt afile.out
  if [ $? -ne 0 ]; then
    ERROR=1
  fi

  echo "bfishmt..."
  bfishmt -e afile.txt afile.enc "Hello, World"
  bfishmt -d afile.enc afile.out "Hello, World"
  cmp afile.txt afile.out
  if [ $? -ne 0 ]; then
    ERROR=1
  fi

  echo "bfish to bfishmt..."
  bfish -e afile.txt afile.enc "Hello, World"
  bfishmt -d afile.enc afile.out "Hello, World"
  cmp afile.txt afile.out
  if [ $? -ne 0 ]; then
    ERROR=1
  fi

  echo "bfishmt to bfish..."
  bfishmt -e afile.txt afile.enc "Hello, World"
  bfish -d afile.enc afile.out "Hello, World"
  cmp afile.txt afile.out
  if [ $? -ne 0 ]; then
    ERROR=1
  fi

done

# Clean up.
rm -f afile.txt afile.enc afile.out

if [ $ERROR -eq 1 ]; then
  echo 'FAIL!'
else
  echo 'PASS!'
fi

