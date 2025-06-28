#! /bin/bash

cd wxWidgets-3.3.0
cmake . -DwxBUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=Release
sudo cmake --build . --config Release --target install

echo finished
