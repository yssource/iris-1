# FIND LIBZMQ AND SETUP VARIABLES

## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

add_subdirectory(nginx_log_publisher)
add_subdirectory(music_tag_server)
add_subdirectory(number_stats_server)
add_subdirectory(periodic_timers)
add_subdirectory(oneshot_timers)
