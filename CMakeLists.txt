cmake_minimum_required(VERSION 3.29)
project(pro C)

set(CMAKE_C_STANDARD 11)

add_executable(pro main.c
        jeu.c
        interface.c
        sauvergarde.c
)
