set(CURRENT_LIBRARY_NAME Boost)

find_package(Boost CONFIG REQUIRED COMPONENTS headers random)

if(Boost_FOUND)
    message(STATUS "Found Boost version: ${Boost_VERSION}")

    list(APPEND PROJECT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    list(APPEND PROJECT_LIBRARIES_LIST Boost::boost Boost::random)
else()
    message(FATAL_ERROR "Boost headers not found. Please install Boost or update your BOOST_ROOT environment variable.")
endif()
