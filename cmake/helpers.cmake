# This function should be passed a list of all files in a target. It will automatically generate
# file groups following the directory hierarchy, so that the layout of the files in IDEs matches the
# one in the filesystem.
function(create_target_directory_groups target_name)
    # Place any files that aren't in the source list in a separate group so that they don't get in
    # the way.
    source_group("Other Files" REGULAR_EXPRESSION ".")

    get_target_property(target_sources "${target_name}" SOURCES)

    foreach(file_name IN LISTS target_sources)
        get_filename_component(dir_name "${file_name}" PATH)
        # Group names use '\' as a separator even though the entire rest of CMake uses '/'...
        string(REPLACE "/" "\\" group_name "${dir_name}")
        source_group("${group_name}" FILES "${file_name}")
    endforeach()
endfunction()

function(strip target_name)
    add_custom_command(
        TARGET ${target_name} POST_BUILD
        DEPENDS ${target_name}
        COMMAND $<$<OR:$<CONFIG:MINSIZEREL>,$<CONFIG:RELEASE>>:${CMAKE_STRIP}>
        ARGS --strip-all "$<TARGET_FILE:${target_name}>"
    )
endfunction()

function(static_link target_name)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set_property(TARGET ${target_name} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_link_libraries(${target_name} PRIVATE -static)
    endif()
endfunction()
