# Ubuntu 12.04 32/64 bit
ADD_DEFINITIONS(-pthread -fvisibility=hidden)
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE /usr/include/python2.7)
SET (PYTHON_ARCHIVE /usr/lib/x86_64-linux-gnu/libpython2.7.so)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (ZLIB_INCLUDE /usr/include)
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE /usr/lib/liblapack.so.3 /usr/lib/libblas.so.3 ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
SET (ZLIB_ARCHIVE /usr/lib/i386-linux-gnu/libz.a)
SET (SQLITE3_ARCHIVE /usr/lib/i386-linux-gnu/libsqlite3.a)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE /usr/lib/liblapack.so.3 /usr/lib/libblas.so.3 ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
SET (ZLIB_ARCHIVE /usr/lib/x86_64-linux-gnu/libz.a)
SET (SQLITE3_ARCHIVE /usr/lib/x86_64-linux-gnu/libsqlite3.a)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (QUADMATH_ARCHIVE "-lquadmath")
SET (PTHREAD_LIB -lpthread)
SET (DLOPEN_LIB -ldl)
SET (SQLITE3_INCLUDE /usr/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
