# (1)
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(mask_sim)

# (2)
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# (3)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# (4)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# (5)
add_executable(mask_sim mask_sim.cc ${sources} ${headers})
target_link_libraries(mask_sim ${Geant4_LIBRARIES})

set(MASKSIM_SCRIPTS
		init_vis.mac
		vis.mac
	)


foreach(_script ${MASKSIM_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/scripts/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# (7)
install(TARGETS mask_sim DESTINATION bin)
