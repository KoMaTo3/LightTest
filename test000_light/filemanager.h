/*==============================
  Класс FileManager
  Интерфейс работы с файлами
  При инициализации движка делает полный "снимок" файловой структуры
  Файлы в архивах выбираются самые новые по дате (при наличии нескольких одинаковых файлов в разных архивах)
  Скрытые папки пропускает (например, .svn нам нахрен не нужен)
==============================*/

#pragma once

#include "klib.h"
#include "memory.h"
#include "file.h"
#include <string>
#include <map>
#include <vector>

#define FileManager_type_NOTPACKED   (0)
#define FileManager_type_PACKED      (1)
#define FileManager_type_FILE        (2)
#define FileManager_type_UNKNOWN     (15)

#define FileManager_DATADIRECTORY    "data"
#define FileManager_HEADER           "KPCK"
#define FileManager_VERSION          (0x00000001)
#define FileManager_EXTENSION        "pck"

class FileManager;
extern FileManager *__fileManager;

//отключаем выравнивание, т.к. структуру почти полностью пишем в файл
#pragma pack(push,1)

struct _FileManager_FileInfo
{
  Dword nameLength;   //Длина имени без учёта конечного нуля
  Dword size;         //Полный размер файла
  Dword sizePacked;   //Размер пожатого файла
  Int64 time;         //Время изменения файла
  Dword offset;       //Смещение файла от начала архива
  Byte  type;         //Тип сжатия (0 - без сжатия, 1 - zlib)
  Char* name;         //Имя файла (с путём)
};
#pragma pack(pop)

class FileManager
{
private:
  struct FileManagerItem
  {
    Int64   timestamp;  //Unix timestamp времени
    size_t  size,       //размер файла
            sizePacked, //размер файла в сжатом виде
            start;      //смещение файла от начала архива (0 если не в архиве)
    Byte    type;       //тип сжатия: -1 - структура не инициализирована, 0 - сжатие отсутствует, 1 - zlib
    std::string
            name,       //имя файла
            path,       //путь к файлу + делимитер
            fullPath,   //полный путь с именем файла
            pack,       //имя архива, в котором находится файл
            ext;        //расширение файла

    FileManagerItem();
  };

private:
  typedef std::map< std::string, FileManagerItem > _FileManagerItems;
  _FileManagerItems _items;

public:
  FileManager     ();
  ~FileManager    ();

  void  AddFile   ( const std::string& path, const std::string& fileName );
  void  AddPack   ( const std::string& path, const std::string& fileName );
  void  ScanAll   ( const std::string& subDir = FileManager_DATADIRECTORY );
  void  SaveAll   ( const std::string& fileName, bool useCompression = true );

  bool  GetFile   ( const std::string& fileName, memory& dest, bool addLastNull = false );
  bool  FileExists( const std::string& fileName );
  void  FindFiles ( const std::string& extension, std::vector< std::string >& result );

  void  Dump      (); //!!!DEBUG!!!
};