cmake_minimum_required(VERSION 3.22)
project(OCR C)

set(CMAKE_C_STANDARD 99)

add_executable(OCR main.c XorNN.h XorNN.c NNsaveV2.c NNsaveV2.h)
