include(CMakeFindDependencyMacro)
set(_supported_components amsi::util amsi::multiscale amsi::analysis)
find_dependency(ModelTraits)
include("${CMAKE_CURRENT_LIST_DIR}/amsiutilTargets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/amsimultiscaleTargets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/amsianalysisTargets.cmake")
set(amsi_FOUND TRUE)
foreach(_comp ${amsi_FIND_COMPONENTS})
  if(NOT "${_supported_components}" MATCHES "${_comp}")
    set(amsi_FOUND FALSE)
    set(amsi_NOTFOUND_MESSAGE "Unsupported component: ${_comp}")
  endif()
endforeach()
