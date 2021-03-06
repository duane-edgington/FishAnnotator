set( TEST_VIDEO_ANNOTATOR_SOURCES test_video_annotator.cc )
set( VIDEO_ANNOTATOR_SOURCES
  "../../src/common/metadata_dialog.cc"
  "../../src/video_annotator/mainwindow.cc"
  "../../src/video_annotator/video_annotation.cc"
  "../../src/video_annotator/player.cc"
  "../../src/video_annotator/reassign_dialog.cc"
  )
include_directories(
  "../../src/common"
  "../../src/video_annotator"
  "${CMAKE_CURRENT_BINARY_DIR}/../../src/common/common_autogen/include"
  "${CMAKE_CURRENT_BINARY_DIR}/../../src/common"
  )
if( WIN32 )
  add_executable( test_video_annotator WIN32 
    ${TEST_VIDEO_ANNOTATOR_SOURCES} 
    ${VIDEO_ANNOTATOR_SOURCES}
    )
  target_link_libraries( 
    test_video_annotator 
    common
    ${WINDOWS_LIBRARIES}
    Qt5::Widgets
    Qt5::Test
    ${QT_THIRD_PARTY_LIBS}
    ${Boost_LIBRARIES}
    ${FFMPEG_LIBRARY_DIR}/avformat.lib
    ${FFMPEG_LIBRARY_DIR}/avcodec.lib
    ${FFMPEG_LIBRARY_DIR}/avutil.lib
    ${FFMPEG_LIBRARY_DIR}/swscale.lib
    )
  target_include_directories(
    test_video_annotator
    PUBLIC
    ${FFMPEG_INCLUDE_DIR}
    )
  set_target_properties(
    test_video_annotator
    PROPERTIES
    LINK_FLAGS "${LINK_FLAGS} /SUBSYSTEM:CONSOLE"
    )
  add_test( 
    NAME test_video_annotator 
    COMMAND test_video_annotator 
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
  set_tests_properties( 
    test_video_annotator PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAIL"
    )
  file( COPY slow_motion_drop.mp4 DESTINATION Release )
elseif( APPLE )
  add_executable( test_video_annotator 
    ${TEST_VIDEO_ANNOTATOR_SOURCES} 
    ${VIDEO_ANNOTATOR_SOURCES}
    )
  target_link_libraries( 
    test_video_annotator 
    common
    ${APPLE_LIBRARIES}
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::Test
    ${QT_THIRD_PARTY_LIBS}
    ${Boost_LIBRARIES}
    avformat
    avcodec
    avutil
    swscale
    )
  add_test( 
    NAME test_video_annotator 
    COMMAND test_video_annotator 
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
  set_tests_properties( 
    test_video_annotator PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAIL"
    )
  file( COPY slow_motion_drop.mp4 DESTINATION . )
elseif( UNIX )
  add_executable( test_video_annotator 
    ${TEST_VIDEO_ANNOTATOR_SOURCES} 
    ${VIDEO_ANNOTATOR_SOURCES}
    )
  target_link_libraries( 
    test_video_annotator 
    dl
    common
    ${UNIX_LIBRARIES}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::PrintSupport
    Qt5::Test
    ${QT_THIRD_PARTY_LIBS}
    Qt5::DBus
    ${Boost_LIBRARIES}
    avformat
    avcodec
    avutil
    swscale
    )
  add_test( 
    NAME test_video_annotator 
    COMMAND test_video_annotator 
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
  set_tests_properties( 
    test_video_annotator PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAIL"
    )
  file( COPY slow_motion_drop.mp4 DESTINATION . )
endif()
target_include_directories( test_video_annotator PUBLIC
  "../../include/fish_annotator/video_annotator"
  "../../include/fish_annotator/common"
  )
