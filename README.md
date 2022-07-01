# VFS_2
Virtual File System ( Test Task)

Виртуальная файловая система, принцип работы которой основан на хранении всех виртуальных файлов в одном (можно разбить на несколько физических файлов, ограничив вместимость каждого из них). Вместимость виртуальных файлов также задается константой **MAX_CHAR** в ``` TestTask.h ```.

При создании виртуального файла в физическом резервируется место в **MAX_CHAR** элементов.Содержимое виртуальных файлов распологается в физическом в порядке их создания.Т.е. есть порядковый номер в VFS файла == 5 => контент файла расположен в диапозоне (5* **MAX_CHAR** - 1; 6* **MAX_CHAR**). При чтении содержимое виртуального файла извлекается из соответствующего места в физическом файле и записывается в буфер, при записи - производится запись непосредственно в физический файл.
При окончании работы все данные о файлах записываются в файла "directory/info.txt" , содержимое файлов находится в "directory/Storage.txt".
Возможность восстановления данных всех файлов при перезапуске программы не реализована за неимением достаточного количества времени, однако интуитивно понятно, как это можно сделать.

Данные о файлах 

# using
для использования достаточно в рабочем файле прописать:
```C++
#include <TestTask.hpp>
```
# compile 
```
g++ ./main.cpp ./TestTask.cpp
```
С++17 

При завершении работы виртуальной системы данные о файлах записываются в info.txt в таком порядке : 


1. Имя файла 
2. порядковый номер файла
3. стартовая позиция в физическом файле (отступ)
4. количество символов
