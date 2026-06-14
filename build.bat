@echo off

nasm -f win64 rutinas.asm -o rutinas.obj
gcc main.c rutinas.obj -o BitQuest.exe

BitQuest.exe