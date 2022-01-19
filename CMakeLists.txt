# SPDX-License-Identifier: LGPLv3

cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(lambdachip-zephyr)

zephyr_library_compile_options(-Wall -Wno-unused -Wextra
  -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-pointer-arith
  -fdiagnostics-color=always -Wno-strict-aliasing -Wno-discarded-qualifiers)

zephyr_include_directories(app PRIVATE ${ZEPHYR_BASE}/include)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
