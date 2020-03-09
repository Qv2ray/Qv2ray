# Directories to look for dependencies
set(DIRS "${CMAKE_BINARY_DIR}")

# Path used for searching by FIND_XXX(), with appropriate suffixes added
if(CMAKE_PREFIX_PATH)
    foreach(dir ${CMAKE_PREFIX_PATH})
        list(APPEND DIRS "${dir}/bin" "${dir}/lib")
    endforeach()
endif()

# Append Qt's lib folder which is two levels above Qt5Widgets_DIR
list(APPEND DIRS "${Qt5Widgets_DIR}/../..")
list(APPEND DIRS "/usr/local/lib")
list(APPEND DIRS "/usr/lib")

include(InstallRequiredSystemLibraries)

message(STATUS "APPS: ${APPS}")
message(STATUS "QT_PLUGINS: ${QT_PLUGINS}")
message(STATUS "DIRS: ${DIRS}")

install(CODE "include(BundleUtilities)
      fixup_bundle(\"${APPS}\" \"${QT_PLUGINS}\" \"${DIRS}\")")
