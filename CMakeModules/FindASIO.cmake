

# asio sdk
set(ASIO_SDK asiosdk_2.3.3_2019-06-14)

# ASIO
set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/asiosdk/${ASIO_SDK})

add_library(asio STATIC
        ${ASIO_DIR}/common/asio.cpp
        ${ASIO_DIR}/common/asio.h
#        ${ASIO_DIR}/common/asiodrvr.cpp
#        ${ASIO_DIR}/common/asiodrvr.h
        ${ASIO_DIR}/common/asiosys.h
        ${ASIO_DIR}/common/iasiodrv.h
        ${ASIO_DIR}/host/asiodrivers.cpp
        ${ASIO_DIR}/host/asiodrivers.h
        ${ASIO_DIR}/host/asiodrivers.cpp
        ${ASIO_DIR}/host/ginclude.h
        ${ASIO_DIR}/host/ASIOConvertSamples.cpp
        ${ASIO_DIR}/host/ASIOConvertSamples.h
        ${ASIO_DIR}/host/pc/asiolist.cpp
        ${ASIO_DIR}/host/pc/asiolist.h)

target_include_directories(asio
        PUBLIC
            ${ASIO_DIR}/common
            ${ASIO_DIR}/host
            ${ASIO_DIR}/host/pc)

