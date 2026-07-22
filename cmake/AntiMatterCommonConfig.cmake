# add dependencies
include(CMakeFindDependencyMacro)
# add all libs imported by find_package() in project
find_dependency(HDF5)

# config AntiMatterCommon
include("${CMAKE_CURRENT_LIST_DIR}/AntiMatterCommonTargets.cmake")
