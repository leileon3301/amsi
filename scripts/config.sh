#!/bin/bash 
# CMake config for AMSI
#
# usage: ./configamsi.sh [build_type] [install_prefix]
#

if [ ! -d ../build ]; then
  mkdir ../build
fi

cd ../build
rm -rf ./* #stupid and dangerous

HOSTNAME=`hostname`

if [ "$HOSTNAME" == "q.ccni.rpi.edu" ]; then
  module load xl
  module load parmetis/xl/4.0.3
  module load zoltan/xl/3.8
  module unload parmetis/xl/4.0.3
  module load /gpfs/u/home/PASC/PASCtbnw/barn-shared/petsc-3.5.2/arch-linux2-c-opt/lib/modules/3.5.2-arch-linux2-c-opt
  module load proprietary/simmetrix/simModSuite/10.0-150716
  module load boost

  export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$DEVROOT/install

   cmake \
    --trace \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$DEVROOT/install \
    -DSIMMETRIX_BUILD=TRUE \
    -DSIM_MPI=bgmpi \
    -DCMAKE_TOOLCHAIN_FILE=$CMAKE_XL_TOOLCHAIN \
    -DBOOST_INCLUDE_DIR=$BOOST_INCLUDE_DIR \
    ..

else
  module load parmetis/OpenMPI-1.3.3
  module load zoltan/OpenMPI-1.3.3
  module unload parmetis/OpenMPI-1.3.3
  module load ~/develop/petsc-3.5.3/arch-linux2-c-debug/lib/modules/3.5.3-arch-linux2-c-debug
  module load simmetrix/simModSuite

  cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$DEVROOT/install \
    -DSIMMETRIX_BUILD=TRUE \
    -DSIM_MPI=openmpi14 \
    ..
fi

# Special parameters for running the test on the Q 
# are also passed to CMake in the prior if statement

# Locations of some mpi libraries
# (those that are found automatically by find_package(MPI) on monopoly) 
# on the q if they are needed in the future. Set by using:
# SET(MPI_LIBRARIES ${MPI_LIBRARIES} example.a)
# /bgsys/drivers/ppcfloor/comm/gcc/lib/
# /bgsys/drivers/ppcfloor/gnu-linux/powerpc64-bgq-linux/lib/

