#!/usr/bin/env bash

#downloading the python test libraries
echo "setup the virtualenv..."
virtualenv .virt_gtodshim
source .virt_gtodshim/bin/activate

echo ""
echo "virtualenv created, now install test dependencies"
pip install sarge
pip install nose


#check if files are there. if not, call make
echo ""
echo "check if we compiled our things before..."
if [ ! -f test ]; then
    echo "turns out we did not."
    make
    make test
else
    echo "we did."
fi

#start tests
echo ""
echo "we're ready now. starting tests..."
cd tests;
nosetests -v
