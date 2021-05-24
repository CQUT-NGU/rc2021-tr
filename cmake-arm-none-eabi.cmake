set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR arm)
set (CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (NOT TOOLCHAIN_PATH)
set (TOOLCHAIN_PATH   D:/Tools/gcc-arm-none-eabi)
endif (NOT TOOLCHAIN_PATH)
set (TOOLCHAIN_PREFIX arm-none-eabi)
if (WIN32)
  set (TOOLCHAIN_SUFFIX ".exe")
endif (WIN32)

set (TOOLCHAIN_BIN_DIR "${TOOLCHAIN_PATH}/bin")
set (TOOLCHAIN_INC_DIR "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}/include")
set (TOOLCHAIN_LIB_DIR "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}/lib")

set (CMAKE_C_COMPILER   "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-gcc${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "C Compiler")
set (CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-g++${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "C++ Compiler")
set (CMAKE_ASM_COMPILER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-gcc${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "ASM Compiler")

set (CMAKE_OBJCOPY "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-objcopy${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "Objcopy")
set (CMAKE_OBJDUMP "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-objdump${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "Objdump")
set (CMAKE_SIZE    "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-size${TOOLCHAIN_SUFFIX}"    CACHE INTERNAL "Size")
set (CMAKE_DEBUGER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-gdb${TOOLCHAIN_SUFFIX}"     CACHE INTERNAL "Debugger")
set (CMAKE_CPPFILT "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}-c++filt${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "C++filt")

set (CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}" ${EXTRA_FIND_PATH})
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

enable_language(ASM)
set (CMAKE_ASM_FLAGS "-x assembler-with-cpp ${CMAKE_ASM_FLAGS}")

# set (CMAKE_C_COMPILER_FORCED TRUE)
# set (CMAKE_ASM_COMPILER_FORCED TRUE)
# set (CMAKE_CXX_COMPILER_FORCED TRUE)
