cmake_minimum_required(VERSION 3.4.1)  

add_library( 
            ffi_path_helper
            SHARED
            src/main/cpp/ffi_path_helper.cc
        )
target_link_libraries(ffi_path_helper -landroid -llog)
