# statdata_analyzer

#Create build directory
mkdir build &&cd build

#Configure and build
cmake.
.make

#Run main utility
./statdata_analyzer file1.bin file2.bin output.bin

#Tests
cd test

#Run test utility
./system_test

#Run test utility
./unit_tests

#Run all tests
ctest -V


Некоторые заменания:

Для очень больших массивов (близких к 100000 элементов) можно рассмотреть:
 - Параллельную обработку с помощью OpenMP
 - Использование более эффективных структур данных (например, хеш-таблиц) для объединения
 - Memory-mapped файлы для работы с очень большими дампами

Сравнение производительности:
Версия с хэш-таблицей (можно посмотреть uthash):
 - Время выполнения: O(n)
 - Память: O(n)
 - Лучше для больших массивов с малым количеством дубликатов

Версия с сортировкой:
 - Время выполнения: O(n log n)
 - Память: O(n)
 - Лучше для массивов со многими дубликатами

Оригинальная версия:
 - Время выполнения: O(n²) в худшем случае
 - Память: O(n)
 - Проще, но менее эффективна
