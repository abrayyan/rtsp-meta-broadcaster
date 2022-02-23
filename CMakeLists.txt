cmake_minimum_required(VERSION 3.10)
project(rtsp_encoder_decoder)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


#find_package(GLib REQUIRED)
find_package(GStreamer 1.0 REQUIRED)

#include_directories("${GLIB_INCLUDE_DIRS}")
#include_directories("${GSTREAMER_INCLUDE_DIRS}")
#include_directories("${GSTREAMER_BASE_INCLUDE_DIRS}")
#include_directories("${GSTREAMER_RTSP_INCLUDE_DIRS}")

set(SRC src/rtsp_meta_factory.cpp
        src/rtsp_meta_factory.h
        src/rtsp_streamer.cpp
        src/rtsp_streamer.h
        src/rtsp_factory.cpp
        src/rtsp_factory.h)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME}  PUBLIC ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GSTREAMER_LIBS} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)

set(GSTREAMER_LIBS
        gstreamer-1.0
        gstbase-1.0
        gstrtp-1.0
        gstsdp-1.0
        gstrtsp-1.0
        gstapp-1.0
        gstnet-1.0
        gobject-2.0
        gmodule-2.0
        gthread-2.0
        glib-2.0
        gio-2.0
        )

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${GSTREAMER_INCLUDE_DIRS} src)
target_link_libraries(main PUBLIC ${GSTREAMER_LIBS} ${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)

add_executable(demo demo.cpp)
target_include_directories(demo PUBLIC ${GSTREAMER_INCLUDE_DIRS} src)
target_link_libraries(demo PUBLIC ${GSTREAMER_LIBS} ${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)

add_executable(demo1 demo1.cpp)
target_include_directories(demo1 PUBLIC ${GSTREAMER_INCLUDE_DIRS} src)
target_link_libraries(demo1 PUBLIC ${GSTREAMER_LIBS} ${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)

add_executable(demo2 demo2.cpp)
target_include_directories(demo2 PUBLIC ${GSTREAMER_INCLUDE_DIRS} src)
target_link_libraries(demo2 PUBLIC ${GSTREAMER_LIBS} ${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)

add_executable(test_streamer test_streamer.cpp)
target_include_directories(test_streamer PUBLIC ${GSTREAMER_INCLUDE_DIRS} src)
target_link_libraries(test_streamer PUBLIC ${GSTREAMER_LIBS} ${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so)