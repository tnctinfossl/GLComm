# CMakeのバージョンを設定
cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
set(CMAKE_CXX_FLAGS "-O2 -std=c++17 -Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 
# プロジェクト名と使用する言語を設定
project(glcomm CXX)
# a.outという実行ファイルをmain.cppとhello.cppから作成
add_executable(glcom main.cpp types.cpp)