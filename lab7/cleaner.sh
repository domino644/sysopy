#!/bin/bash

for n in `ipcs -q | egrep ^q | awk '{ print $2; }'`; do ipcrm -q $n; done