cmake_minimum_required(VERSION 3.22)

# UTF-8 source
add_compile_options("$<$<AND:$<C_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/Zc:__cplusplus>")

project(squirrel VERSION 2.2.4)

set(LIBNAME lib${PROJECT_NAME})
set(EXENAME sq)

# ----------------------------------------------------
# squirrel library
# ----------------------------------------------------

add_library( ${LIBNAME} STATIC
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
	sqobject/sqobjectinfo.cpp
	sqobject/sqthread.cpp
	sqobject/sqobjectclass.cpp
	sqobject/sqmemfile.cpp
	sqobject/sqratfunc.cpp
)

target_include_directories( ${LIBNAME} PUBLIC
	include
	sqrat
	sqobject
)

target_include_directories( ${LIBNAME} PRIVATE 
	sqstdlib
	squirrel
)

target_compile_definitions( ${LIBNAME} PUBLIC
	USESQRAT
)

# ----------------------------------------------------
# sq command
# ----------------------------------------------------

add_executable( ${EXENAME}
	sq/sq.cpp
	sqstdlib/sqstdsystem.cpp
	sqobject/sqfile.cpp
	squirrel/sqmem.cpp
)

target_link_libraries( ${EXENAME} PRIVATE
	libsquirrel
)
