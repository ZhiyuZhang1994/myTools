aux_source_directory(. source_list)
include_directories(${PROJECT_SOURCE_DIR}/include)
# include_directories(${PROJECT_SOURCE_DIR}/third_party/easylogging)

# add_executable(main ${source_list})
# add_executable(Tutorial_Step1 MACOSX_BUNDLE ${source_list} )
add_executable(Tutorial_Step1 MACOSX_BUNDLE ./box_selection_points/highligth_points_use_vertex_filter.cpp )

target_link_libraries(Tutorial_Step1 PRIVATE ${VTK_LIBRARIES})

# target_link_libraries(main ${PROJECT_BINARY_DIR}/lib/libeasy_log.a)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS Tutorial_Step1
  MODULES ${VTK_LIBRARIES}
)