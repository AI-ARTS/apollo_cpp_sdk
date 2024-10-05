
mkdir -p ./build
cd ./build && make clean 
cmake ..
make
./../bin/test