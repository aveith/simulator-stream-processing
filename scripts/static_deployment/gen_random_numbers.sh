#!/bin/bash

for r in `seq 0 20`; do
  r=$(( $RANDOM % 99999999 ))
  echo $r >> random_numbers
done;

