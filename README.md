В этом репозитории я работаю над новой лабораторной работой по дисциплине "Программирование" 1 курса ФИТ НГУ

[![Testing](https://github.com/ptrvsrg/My_Lab_C/actions/workflows/cmake.yml/badge.svg)](https://github.com/ptrvsrg/My_Lab_C/actions/workflows/cmake.yml)

## Задание

Вход:

+ Строка 1 -- целое N от 0 до 100000 - число точек на плоскости;
+ Строки 2 - N+1 -- 2 целых числа - координаты точки на плоскости;

Выход:

+ Если N не от 0 до 100000, то `"bad number of points"`
+ Если строк меньше N+1, то `"bad number of lines"`
+ Иначе координаты точек наименьшего выпуклого многоугольника, для которого каждая точка из заданного множества точек либо лежит на границе этого многоугольника, либо находится в его внутренней области

Исполняемый файл + данные не более 16 * N + 4Mb

Пример 1

    Вход:
    4
    0 0
    2 1
    4 0
    0 4
    Выход:
    0 0
    4 0
    0 4

Пример 2

    Вход:
    4
    2 9
    2 1
    2 -6
    2 4
    Выход:
    2 -6
    2 9 

Пример 3:

    Вход:
    100001
    Выход:
    bad number of points
