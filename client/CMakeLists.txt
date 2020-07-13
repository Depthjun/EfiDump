cmake_minimum_required(VERSION 3.16)

# Setup compiler variables
project(efidump)

set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -mconsole -static-libgcc -static-libstdc++ -Wall -Werror -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -mconsole -static-libgcc -static-libstdc++ -Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sourcefiles
    "src/*.h"
    "src/*.cc"
    "src/*.c"
)

add_executable(efidump.exe ${sourcefiles})
target_link_libraries(efidump.exe ntdll)