ADD_DEFINITIONS(-pthread -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE "$ENV{CONDA_PREFIX}/include")

SET (QUADMATH_ARCHIVE "-lquadmath")

SET (MKL_PARDISO_INCLUDE $ENV{CONDA_PREFIX}/include)
SET (MKL_LIB_DIR         $ENV{CONDA_PREFIX}/lib)
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#requires duplicate library for 1-pass linker
SET (BLAS_ARCHIVE
 ${EXTERNAL_LIB}/getrf/build/libgetrf.a
 -L${MKL_LIB_DIR} -Wl,--no-as-needed -lmkl_rt -lpthread -lm -ldl
)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)

SET (SQLITE3_INCLUDE $ENV{CONDA_PREFIX}/include)
SET (SQLITE3_ARCHIVE $ENV{CONDA_PREFIX}/lib/libsqlite3.a)

SET (ZLIB_INCLUDE $ENV{CONDA_PREFIX}/include)
SET (ZLIB_ARCHIVE $ENV{CONDA_PREFIX}/lib/libz.a)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)

SET (PYTHON37_INCLUDE   $ENV{CONDA_PREFIX}/include/python3.7m)
SET (PYTHON37_ARCHIVE -L$ENV{CONDA_PREFIX}/lib   -lpython3.7m)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

