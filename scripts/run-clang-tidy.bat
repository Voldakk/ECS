clang-tidy %1 ../src/ecs/*.hpp
clang-tidy %1 ../src/ecs/*.cpp

clang-tidy %1 ../include/ecs/*.hpp
clang-tidy %1 ../include/ecs/*.cpp

::clang-tidy %1 ../test/src/*.hpp
::clang-tidy %1 ../test/src/*.cpp

pause
