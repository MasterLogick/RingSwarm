find_path(fuse3_INCLUDE_DIR NAMES fuse3/fuse.h fuse3/fuse_common.h
        DOC "The fuse3 include directory"
        )

find_library(fuse3_LIBRARY NAMES fuse3
        DOC "The fuse3 library"
        )

if (fuse3_INCLUDE_DIR AND EXISTS "${fuse3_INCLUDE_DIR}/fuse3/fuse_common.h")
    file(READ "${fuse3_INCLUDE_DIR}/fuse3/fuse_common.h" fuse3_common)

    string(REGEX REPLACE "^.*USE_MAJOR_VERSION[\t ]+([0-9]+).*$" "\\1" fuse3_MAJOR_VERSION "${fuse3_common}")
    string(REGEX REPLACE "^.*USE_MINOR_VERSION[\t ]+([0-9]+).*$" "\\1" fuse3_MINOR_VERSION "${fuse3_common}")
    set(fuse3_VERSION "${fuse3_MAJOR_VERSION}.${fuse3_MINOR_VERSION}")
endif ()
if (fuse3_INCLUDE_DIR)
    set(fuse3_INCLUDE_DIR "${fuse3_INCLUDE_DIR}/fuse3")
endif ()
FIND_PACKAGE_HANDLE_STANDARD_ARGS(fuse3
        REQUIRED_VARS fuse3_LIBRARY fuse3_INCLUDE_DIR
        VERSION_VAR fuse3_VERSION)

if (fuse3_FOUND)
    set(fuse3_LIBRARIES ${fuse3_LIBRARY})
    set(fuse3_INCLUDE_DIRS ${fuse3_INCLUDE_DIR})
    if (NOT TARGET fuse3::fuse3)
        add_library(fuse3::fuse3 UNKNOWN IMPORTED)
        set_target_properties(fuse3::fuse3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${fuse3_INCLUDE_DIRS}")
        set_property(TARGET fuse3::fuse3 APPEND PROPERTY IMPORTED_LOCATION "${fuse3_LIBRARY}")
    endif ()
endif ()

mark_as_advanced(fuse3_INCLUDE_DIR fuse3_LIBRARY)