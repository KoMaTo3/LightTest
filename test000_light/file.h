#pragma once

//#include "tools.h"
#include <windows.h>
//#include "types.h"
#include <string>
#include "memory.h"

class File;
extern File __log;

//### defines ###//

#define File_mode_WRITE  (1)
#define File_mode_READ   (2)
#define File_mode_APPEND (3)
#define File_mode_EDIT   ( 4 )

enum {
  Filelevel_INFO          = 0,
  Filelevel_CRITICALERROR = 1,
  Filelevel_ERROR         = 2,
  Filelevel_WARNING       = 3,
  Filelevel_DEBUG         = 4,
  Filelevel_ALL           = 255
};

enum FileStream
{
  nullSymb                  = 0,
  newLine                   = 0xA,
};


extern const char *FileMESSAGE[];

class File
{
private:
  FILE   *_file;
  char   *_name;
  BYTE    _mode;
  DWORD   _length;
  DWORD   _cur_pos;
  bool    _auto_flush;
  BYTE    _logLevel;  //уровень ведения логов
public:

public:

  File();
  File(const char *file_name, BYTE file_mode, BYTE warnLevel = Filelevel_ALL);
  File(const std::string &file_name, BYTE file_mode, BYTE warnLevel = Filelevel_ALL);
  virtual ~File();

  HRESULT Open          (const char *file_name, BYTE file_mode, BYTE warnLevel = Filelevel_ALL);
  HRESULT Open          (const std::string &file_name, BYTE file_mode, BYTE warnLevel = Filelevel_ALL);
  HRESULT Print         (const char *str ...);
  HRESULT PrintUseTime  (const char *str ...);
  HRESULT PrintAligned  (const char *str ...);//с выравниванием под столбец со временем
  HRESULT PrintInfo     (BYTE level, const char *str ...);  //выводит warning, error и прочее
  HRESULT Write         (const void *data, DWORD length);
  size_t  Read          (void *dest, DWORD length);
  size_t  Read          (void *dest, DWORD length, DWORD num_blocks);
  HRESULT SeekFromStart (long position);
  HRESULT SeekFromEnd   (long position);
  HRESULT SeekFromCurrentPos(long position);
  void    EnableAutoFlush(bool enable);
  HRESULT Close         ();
  void    Flush         ();
  DWORD   GetLength     () const;
  const
  char*   GetFileName   () const;
  __forceinline
  char    GetAutoFlush  () const { return _auto_flush; }
  __forceinline
  DWORD   GetCurrentPos () const { return _cur_pos; }
  __forceinline
  BYTE    SetLogLevel   (BYTE level) { return this->_logLevel = level; }

  static
  void    Rename        ( const std::string source, const std::string dest );

  File& operator<<(char *str);
  File& operator<<(const std::string &str);
  File& operator<<(memory &data);
  File& operator<<(const int &value);
  File& operator<<(FileStream value);
};
