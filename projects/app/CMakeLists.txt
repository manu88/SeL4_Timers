cmake_minimum_required(VERSION 3.7.2)

project(app C) 


add_executable(app src/main.c src/Bootstrap.c)


target_link_libraries(app sel4muslcsys  muslc sel4allocman)

DeclareRootserver(app)
