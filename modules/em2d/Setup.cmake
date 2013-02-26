if("${OPENCV22_LIBRARIES}" STREQUAL "" AND "${OPENCV21_LIBRARIES}" STREQUAL "")
# disable em2d
  file(STRINGS ${PROJECT_BINARY_DIR}/data/build_info/disabled disabled)
  list(APPEND disabled "em2d")
  list(REMOVE_DUPLICATES disabled)
  foreach(l ${disabled})
    if(DEFINED output)
      set(output "${output}
${l}")
    else()
      set(output ${l})
    endif()
  endforeach(l)
  file(WRITE ${PROJECT_BINARY_DIR}/data/build_info/disabled ${output})
endif()