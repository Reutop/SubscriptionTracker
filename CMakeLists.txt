cmake_minimum_required(VERSION 3.10)
project(SubscriptionTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

set(SOURCES
    src/main.cpp
    src/Subscription.cpp
    src/SubscriptionManager.cpp
    src/FileIO.cpp
    src/Menu.cpp
)

add_executable(SubscriptionTracker ${SOURCES})

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/data/subscriptions.txt)
    file(WRITE ${CMAKE_SOURCE_DIR}/data/subscriptions.txt 
        "1,Netflix,Развлечения,799,monthly,2026-01-15,active\n"
        "2,Spotify,Развлечения,399,monthly,2026-01-10,active\n"
        "3,Microsoft 365,Софт,4999,yearly,2026-02-01,active\n"
        "4,Coursera,Обучение,3500,monthly,2026-01-20,active\n"
        "5,Электроснабжение,Коммунальные,2500,monthly,2026-01-25,active\n"
    )
endif()