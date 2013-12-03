/*
--------------------
  Работа с файлами и логами

  © AreserA
--------------------
*/

#include <stdio.h>
#include "file.h"
#include <string.h>
#include "tools.h"
#include "types.h"

const char *FileMESSAGE[] = {
  "",           //0
  "[Critical error] ", //1
  "[Error] ",          //2
  "[Warning] ",        //3
  "[Debug] ",          //4
};

const char Filetime_align[13+1] = "             ";

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
----------
  Constructor
----------
*/
File::File()
:_logLevel(Filelevel_ALL)
{
  memset(this, 0, sizeof(*this));
}//constructor


/*
----------
  Constructor
----------
*/
File::File(const char *file_name, BYTE file_mode, BYTE warnLevel/* = Filelevel_all*/)
{
  memset(this, 0, sizeof(*this));
  this->Open(file_name, file_mode, warnLevel);
}//constructor


/*
----------
  Constructor
----------
*/
File::File(const std::string &file_name, BYTE file_mode, BYTE warnLevel/* = Filelevel_ALL*/)
{
  memset(this, 0, sizeof(*this));
  this->Open(file_name.data(), file_mode, warnLevel);
}//constructor



/*
----------
  EnableAutoFlush
  Включение/выключение режима автоматической записи кэша в файл
----------
*/
void File::EnableAutoFlush(bool enable)
{
  this->_auto_flush = enable;
}//EnableAutoFlush


/*
----------
  Destructor
----------
*/
File::~File()
{
  this->Close();
  if(this->_name)
    DEF_DELETE(this->_name);
}//destructor


/*
----------
  open
  Открытие файла
----------
*/
HRESULT File::Open(const char *file_name, BYTE file_mode, BYTE warnLevel/* = Filelevel_all*/)
{
  if(this->_file)
    this->Close();
  int len = int(strlen(file_name));
  DEF_DELETE(this->_name);
  std::string path, oldDir = tools::GetCurDirectory(), fileName = tools::GetFileNameFromPath(file_name);
  if((path = tools::GetPathFromFilePath(file_name)).length())  //need to change directory
  {
    tools::SetCurDirectory(tools::GetCorePath());
    if(path.length() && this->_mode != File_mode_READ)
      CreateDirectory(path.data(), 0);
    tools::SetCurDirectory(path);
  }
  //__log.PrintInfo(Filelevel_DEBUG, "=> [%s][%s]", path.data(), getCurrentDirectory().data());
  this->_name = new char[len + 1];
  memcpy(this->_name, file_name, len + 1);
  this->_mode = file_mode;
  char m[4];
  switch(this->_mode)
  {
  case File_mode_WRITE:
    m[0] = 'w';
    m[1] = 'b';
    m[2] = '+';
    m[3] = 0;
  break;
  case File_mode_READ:
    m[0] = 'r';
    m[1] = 'b';
    m[2] = 0;
  break;
  case File_mode_APPEND:
    m[0] = 'a';
    m[1] = 'b';
    m[2] = 0;
  break;
  case File_mode_EDIT:
    m[ 0 ] = 'r';
    m[ 1 ] = 'b';
    m[ 2 ] = '+';
    m[ 3 ] = 0;
  break;
  }
  fopen_s(&this->_file, fileName.data(), m);
  __log.PrintInfo(Filelevel_DEBUG, "File::Open('%s') open in directory %s", fileName.data(), tools::GetCurDirectory().data());
  if(path.length())
    tools::SetCurDirectory(oldDir);
  if(this->_file == 0)
  {
    __log.PrintInfo(Filelevel_WARNING, "File::Open('%s', '%s')  -  can't open file", this->_name, m);
    return aERR;
  }
  this->_length = tools::FileSize(this->_file);
  this->_logLevel = warnLevel;
  this->_cur_pos = 0;

  return aOK;
}//open



/*
----------
  open
  Открытие файла
----------
*/
HRESULT File::Open(const std::string &file_name, BYTE file_mode, BYTE warnLevel/* = Filelevel_all*/)
{
  return this->Open(file_name.data(), file_mode, warnLevel);
}//open



/*
----------
  close
  Закрытие файла
----------
*/
HRESULT File::Close()
{
  if(this->_file == 0)
    return aERR;
  fclose(this->_file);
  this->_file = 0;
  return aOK;
}//this->Close

/*
----------
  print
  Форматированная печать в файл
----------
*/
HRESULT File::Print(const char *str ...)
{
  if(this->_file == 0)
    return aERR;
  if(this->_mode == File_mode_READ)
    return aERR;
  if(str == 0)
    return aERR;

  try{

  va_list list;
  va_start(list, str);
  vfprintf(this->_file, str, list);
  va_end(list);

  if(this->_auto_flush)
    fflush(this->_file);

  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::Print");
  }

  this->_cur_pos = ftell( this->_file );

  return aOK;
}//print


/*
----------
  printUseTime
  Форматированная печать в файл с выводом текущего времени
----------
*/
HRESULT File::PrintUseTime(const char *str ...)
{
  try
  {
    if(this->_file == 0)
      return aERR;
    if(this->_mode == File_mode_READ)
      return aERR;
    if(str == 0)
      return aERR;
  
    SYSTEMTIME st;
    GetLocalTime(&st);
    char t[15];
    memset(t, 32, 14);
    sprintf_s(t, "%c%c:%c%c:%c%c:%c%c%c ", char(st.wHour*0.1f)+0x30, char(st.wHour%10)+0x30, char(st.wMinute*0.1f)+0x30, char(st.wMinute%10)+0x30, char(st.wSecond*0.1f)+0x30, char(st.wSecond%10)+0x30, char(st.wMilliseconds*0.01f)+0x30, (char(st.wMilliseconds*0.1f))%10+0x30, char(st.wMilliseconds%10)+0x30);
    fwrite(t, 13, 1, this->_file);

    va_list list;
    va_start(list, str);
    vfprintf(this->_file, str, list);
    va_end(list);

    if(this->_auto_flush)
      fflush(this->_file);
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::PrintUseTime");
  }
  this->_cur_pos = ftell( this->_file );
  this->_length = max( this->_length, this->_cur_pos );

  return aOK;
}//printUseTime


/*
----------
  printAligned
  Форматированная печать в файл со сдвигом длиной таймштампа printUseTime
----------
*/
HRESULT File::PrintAligned(const char *str ...)//с выравниванием под столбец со временем
{
  if(this->_file == 0)
    return aERR;
  if(this->_mode == File_mode_READ)
    return aERR;
  if(str == 0)
    return aERR;
  
  try{
  SYSTEMTIME st;
  GetSystemTime(&st);
  fwrite(Filetime_align, 13, 1, this->_file);

  va_list list;
  va_start(list, str);
  vfprintf(this->_file, str, list);
  va_end(list);

  if(this->_auto_flush)
    fflush(this->_file);

  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::PrintAligned");
  }
  this->_cur_pos = ftell( this->_file );
  this->_length = max( this->_length, this->_cur_pos );

  return aOK;
}//printAligned


/*
----------
  printInfo
  Форматированная печать в файл с выводом текущего времени и уровня уведомления(warning), ошибки (error) и прочего
----------
*/
HRESULT File::PrintInfo(BYTE level, const char *str ...)
{
  try
  {
    if(this->_file == 0)
      return aERR;
    if(this->_mode == File_mode_READ)
      return aERR;
    if(str == 0)
      return aERR;
    //this->PrintAligned("%d < %d ?", this->_logLevel, level);
    if(this->_logLevel < level)
      return aOK;
  
    SYSTEMTIME st;
    GetLocalTime(&st);
    char t[15];
    memset(t, 32, 14);
    sprintf_s(t, "%c%c:%c%c:%c%c:%c%c%c ", char(st.wHour*0.1f)+0x30, char(st.wHour%10)+0x30, char(st.wMinute*0.1f)+0x30, char(st.wMinute%10)+0x30, char(st.wSecond*0.1f)+0x30, char(st.wSecond%10)+0x30, char(st.wMilliseconds*0.01f)+0x30, (char(st.wMilliseconds*0.1f))%10+0x30, char(st.wMilliseconds%10)+0x30);
    fwrite(t, 13, 1, this->_file);
    fprintf(this->_file, "%s", FileMESSAGE[level]);

    va_list list;
    va_start(list, str);
    vfprintf(this->_file, str, list);
    va_end(list);

    fwrite("\n", 1, 1, this->_file);
    if(this->_auto_flush)
      fflush(this->_file);
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::PrintUseTime");
  }
  this->_cur_pos = ftell( this->_file );
  this->_length = max( this->_length, this->_cur_pos );

  return aOK;
}//printInfo


/*
----------
  write
  Запись в файл блока памяти
----------
*/
HRESULT File::Write(const void *data, DWORD length)
{
  if(this->_file == 0)
    return aERR;
  if(this->_mode == File_mode_READ)
    return aERR;
  if(data == 0)
    return aERR;
  
  try{
  fwrite(data, length, 1, this->_file);

  if(this->_auto_flush)
    fflush(this->_file);

  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::Write");
  }
  this->_cur_pos = ftell( this->_file );
  this->_length = max( this->_length, this->_cur_pos );

  return aOK;
}//write

/*
----------
  read
  Чтение блока из файла
----------
*/
size_t File::Read(void *dest, DWORD length)
{
  if(this->_file == 0)
    return 0;
  if(this->_mode != File_mode_READ)
    return 0;
  if(dest == 0)
    return 0;

  try{
    if(fread(dest, length, 1, this->_file) == 1)
      return length;
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::Read");
  }
  this->_cur_pos = ftell( this->_file );
  return 0;
}//read


/*
----------
  read
  Чтение нескольких блоков из файла
----------
*/
size_t File::Read(void *dest, DWORD length, DWORD num_blocks)
{
  if(this->_file == 0)
    return 0;
  if(this->_mode != File_mode_READ)
    return 0;
  if(dest == 0)
    return 0;

  try{
    size_t res = fread(dest, length, num_blocks, this->_file);
    this->_cur_pos = ftell( this->_file );
    return res;
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::Read");
  }
}//read


/*
----------
  seekFromStart
  Установка текущей позиции от начала файла
----------
*/
HRESULT File::SeekFromStart(long position)
{
  if(this->_file == 0)
    return aERR;
  
  try{
    fseek(this->_file, position, SEEK_SET);
    this->_cur_pos = ftell( this->_file );
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::SeekFromStart");
  }
  return aOK;
}//seekFromStart

/*
----------
  seekFromCurrentPos
  Смещение относительно текущей позиции
----------
*/
HRESULT File::SeekFromCurrentPos(long position)
{
  if(this->_file == 0)
    return aERR;
  
  try{
    fseek(this->_file, position, SEEK_CUR);
    this->_cur_pos = ftell( this->_file );
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::SeekFromCurrentPos");
  }
  return aOK;
}//seekFromCurrentPos

/*
----------
  seekFromEnd
  Установка текущей позиции от конца файла
----------
*/
HRESULT File::SeekFromEnd(long position)
{
  if(this->_file == 0)
    return aERR;
  
  try{
    fseek(this->_file, position, SEEK_END);
    this->_cur_pos = ftell( this->_file );
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::SeekFromEnd");
  }
  return aOK;
}//seekFromEnd

/*
----------
  getLength
  Возвращает длину файла
----------
*/
DWORD File::GetLength() const
{
  if(this->_file == 0)
    return 0;
  return this->_length;
}//getLength

/*
----------
  flush
  Сброс текущего буффера в файл
----------
*/
void File::Flush()
{
  if(this->_file == 0)
    return;
  if(this->_mode == File_mode_READ)
    return;
  try{
  fflush(this->_file);
  }catch(...)
  {
    __log.PrintInfo(Filelevel_CRITICALERROR, "File::flush");
  }
}//flush

/*
----------
  getFileName
  Возвращает название файла
----------
*/
const char *File::GetFileName() const
{
  return this->_name;
}//getFileName




/*
----------
  operator<<
----------
*/
File& File::operator<<(char *str)
{
  if(!str)
    return *this;

  if(this->_file)
    this->Write(str, strlen(str));

  return *this;
}//operator<<




/*
----------
  operator<<
----------
*/
File& File::operator<<(const std::string &str)
{
  if(this->_file)
    this->Write(str.data(), str.length());

  return *this;
}//operator<<




/*
----------
  operator<<
----------
*/
File& File::operator<<(const int &value)
{
  if(this->_file)
    this->Print("%d", value);

  return *this;
}//operator<<




/*
----------
  operator<<
----------
*/
File& File::operator<<(memory &data)
{
  if(!data.getLength())
    return *this;

  if(this->_file)
    this->Write(data.getData(), data.getLength());

  return *this;
}//operator<<




/*
----------
  operator<<
----------
*/
File& File::operator<<(FileStream value)
{
  switch(value)
  {
  case newLine:
  case nullSymb:
    this->Write(&value, 1);
  break;
  }

  return *this;
}//operator<<


/*
----------
  Rename
----------
*/
void File::Rename( const std::string source, const std::string dest )
{
  rename( source.c_str(), dest.c_str() );
}//Rename
