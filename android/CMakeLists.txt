cmake_minimum_required(VERSION 3.4.1)  

add_library( 
            flutter_ffi_asset_helper
            SHARED
            src/main/cpp/flutter_ffi_asset_helper.cc
        )
target_link_libraries(flutter_ffi_asset_helper -landroid -llog)
