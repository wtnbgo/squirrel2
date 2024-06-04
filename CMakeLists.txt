cmake_minimum_required(VERSION 3.22)

# UTF-8 source
add_compile_options("$<$<AND:$<C_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/Zc:__cplusplus>")

project(squirrel VERSION 2.2.4)

# ----------------------------------------------------
# squirrel library
# ----------------------------------------------------

add_library(libsquirrel STATIC
	squirrel/sqapi.cpp
	squirrel/sqbaselib.cpp
	squirrel/sqclass.cpp
	squirrel/sqcompiler.cpp
	squirrel/sqdebug.cpp
	squirrel/sqfuncstate.cpp
	squirrel/sqlexer.cpp
	squirrel/sqobject.cpp
	squirrel/sqstate.cpp
	squirrel/sqtable.cpp
	squirrel/sqvm.cpp
	sqstdlib/sqstdblob.cpp
	sqstdlib/sqstdio.cpp
	sqstdlib/sqstdstream.cpp
	sqstdlib/sqstdmath.cpp
	sqstdlib/sqstdstring.cpp
	sqstdlib/sqstdaux.cpp
	sqstdlib/sqstdrex.cpp
)

target_sources(libsquirrel PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
	BASE_DIRS include
	FILES
		include/sqstdaux.h
		include/sqstdblob.h
		include/sqstdio.h
		include/sqstdmath.h
		include/sqstdstring.h
		include/sqstdsystem.h
		include/squirrel.h
)

target_include_directories(libsquirrel PUBLIC
	include
)

target_include_directories(libsquirrel PRIVATE 
	sqstdlib
	squirrel
)

# ----------------------------------------------------
# sq object
# ----------------------------------------------------

add_library(libsqobject STATIC
	sqobject/sqobjectinfo.cpp
	sqobject/sqthread.cpp
	sqobject/sqobjectclass.cpp
	sqobject/sqmemfile.cpp
	sqobject/sqratfunc.cpp
)

target_sources(libsqobject PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
	BASE_DIRS sqobject
	FILES
	sqobject/sqcont.h
	sqobject/sqobjectclass.h
	sqobject/sqobjectinfo.h  
	sqobject/sqthread.h
)

target_sources(libsqobject PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
	BASE_DIRS sqrat
	FILES
	sqrat/sqrat.h
	sqrat/sqratimport.h
	sqrat/sqrat/sqratAllocator.h
	sqrat/sqrat/sqratBridgeMethods.h
	sqrat/sqrat/sqratCastMethods.h
	sqrat/sqrat/sqratClass.h
	sqrat/sqrat/sqratClassType.h
	sqrat/sqrat/sqratConst.h
	sqrat/sqrat/sqratFunction.h
	sqrat/sqrat/sqratGlobalMethods.h
	sqrat/sqrat/sqratMemberMethods.h
	sqrat/sqrat/sqratObject.h
	sqrat/sqrat/sqratOverloadMethods.h
	sqrat/sqrat/sqratPrintf.h
	sqrat/sqrat/sqratProxyMethods.h
	sqrat/sqrat/sqratScript.h
	sqrat/sqrat/sqratTable.h
	sqrat/sqrat/sqratTypes.h
	sqrat/sqrat/sqratUtil.h
)

target_include_directories(libsqobject PRIVATE
	sqobject
	sqrat
)

target_link_libraries(libsqobject PUBLIC
	libsquirrel
)

# ----------------------------------------------------
# sq command
# ----------------------------------------------------

add_executable( sq
	sq/sq.cpp
	sqstdlib/sqstdsystem.cpp
	sqobject/sqfile.cpp
	squirrel/sqmem.cpp
)

target_link_libraries( sq PRIVATE
	libsquirrel
	libsqobject
)

# ----------------------------------------------------
# install
# ----------------------------------------------------

install(TARGETS libsquirrel libsqobject sq FILE_SET public_headers)
