#!/bin/bash

# get script working path
script_run_dir=$(pwd)

echo "---------------------------------------------------------"
echo "-------------------SETTING UP PROTOBUF-------------------"
echo "---------------------------------------------------------"
cd protobuf
git checkout -b schema-integration origin/schema-integration
#cmake . -DCMAKE_CXX_STANDARD=14
cmake . -DCMAKE_CXX_STANDARD=14 -Dprotobuf_BUILD_TESTS=OFF
cmake --build . -- -j16
cd ..
echo "---------------------------------------------------------"
echo "-------------------PROTOBUF BUILD OVER-------------------"
echo "---------------------------------------------------------"
echo "---------------------SETTING UP QPL----------------------"
echo "---------------------------------------------------------"
cd qpl
mkdir build
cd build
read -p "Enter the (full) path for the qpl installation directory [Default: script_run_path_dir/qpl_install_dir]: " qpl_install_dir
default_dir="${script_run_dir}/qpl_install_dir"
qpl_install_dir=${qpl_install_dir:-$default_dir}
echo "Your installation dir is $qpl_install_dir."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$qpl_install_dir ..
cmake --build . --target install -j16
echo "---------------------------------------------------------"
echo "---------------------QPL BUILD OVER----------------------"
echo "---------------------------------------------------------"
