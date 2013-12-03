#include "tools.h"
#include "memory.h"
#include "file.h"
#include "types.h"
#include "time.h"
#include <deque>
#include <algorithm>


namespace tools
{
const float oneBy256 = 1.0f / 256.0f;

/*
----------
  Explode
  Разбивает строку source разделителем separator
----------
*/
std::deque< std::string > Explode( const std::string& source, const std::string& separator )
{
  if( !source.length() )
    return std::deque< std::string >();
  std::deque< std::string > res;
  std::deque< std::string >::iterator iter;
  for( unsigned long w, q = 0; q < source.length(); )
  {
    w = source.find( separator, q );
    if(w)
      res.push_back( source.substr( q, w - q ) );
    else  //last
      res.push_back( source.substr( q ) );
    iter = --res.end();
    q += iter->length() + separator.length();
  }
  return res;
}//Explode



/*
----------
  filesize
  Возвращает длину файла
----------
*/
long FileSize( FILE *stream )
{
  if( !stream )
    return 0;
  long curpos, length;
  curpos = ftell( stream );
  fseek( stream, 0L, SEEK_END );
  length = ftell( stream );
  fseek( stream, curpos, SEEK_SET );
  return length;
}//filesize


/*
----------
  getCorePath
  Возвращает путь к exe-файлу проекта
----------
*/
std::string GetCorePath()
{
  memory tmp( MAX_PATH_LENGTH );
  if( !GetModuleFileName( GetModuleHandle( NULL ), tmp.getData(), tmp.getLength() ) )
    __log.PrintInfo( Filelevel_ERROR, "tools::GetCorePath => GetModuleFileName => 0" );
  return tools::GetPathFromFilePath( ( char* ) tmp.getData() );
}//getCorePath


/*
----------
  getFileNameFromPath
  Возвращает имя файла + расширение из полного пути к файлу
----------
*/
std::string GetFileNameFromPath( const std::string& path )
{
  if( !path.length() )
    return std::string();
  for( int cur = path.length() - 1; cur >= 0; --cur )
    if( path[ cur ] == '/' || path[ cur ] == '\\' )
      return path.data() + cur + 1;
  return path;
}//getFileNameFromPath


/*
----------
  getPathFromFilePath
  Возвращает путь к файлу из _полного_ пути к файлу
----------
*/
std::string GetPathFromFilePath( const std::string& fullPath )
{
  if( !fullPath.length() )
    return std::string();
  std::string tmp = fullPath;
  for( int cur = tmp.length() - 1; cur >= 0; --cur )
    if( tmp[ cur ] == '/' || tmp[ cur ] == '\\' )
    {
      tmp[ cur + 1 ] = 0;
      return std::string( tmp.data() );
    }
  return "";
}//getPathFromFilePath


/*
----------
  getFileExtension
  Возвращает расширение файла
----------
*/
std::string GetFileExtension( const std::string& file )
{
  if( !file.length() )
    return std::string();
  for( int cur = file.length() - 1; cur >= 0; --cur )
    if( file[cur] == '.' )
      return file.data() + cur + 1;
    else  if( file[ cur ] == '/' )
      return "";
    else  if( file[ cur ] == '\\' )
      return "";
  return "";
}//getFileExtension


/*
----------
  getCurrentDirectory
  Возвращает текущую директорию (полный путь)
----------
*/
std::string GetCurDirectory()
{
  memory tmp( MAX_PATH_LENGTH );
  tmp[ 0L ] = 0;
  if( !GetCurrentDirectory( tmp.getLength(), tmp.getData() ) )
    __log.PrintInfo( Filelevel_ERROR, "GetCurrentDirectory => 0" );
  return std::string( tmp.getData() );
}//getCurrentDirectory



/*
----------
  setCurrentDirectory
  Устанавливает текущую директорию
----------
*/
void SetCurDirectory( const std::string& file )
{
  SetCurrentDirectory( file.data() );
}//setCurrentDirectory




/*
----------
  GetSubDirectory
  Возвращает текущую подпапку
----------
*/
std::string GetSubDirectory()
{
  std::string
      curDir    = GetCurDirectory(),
      corePath  = GetCorePath();
  return 
    ( curDir.length() + 1 > corePath.length() ?
        std::string( curDir.data() + corePath.length() ) + ( char* ) DIR_DELIMITER :
        ""
    );
}//getSubDirectory




/*
----------
  ModifyPathToMine
  Замена "\\" на "/"
----------
*/
std::string ModifyPathToMine( const std::string& path )
{
  std::string res = path;
  size_t length = res.length();
  if( !length )
    return res;

  for( size_t cur = 0; cur < length; ++cur )
    if( res[ cur ] == '\\' )
      res[ cur ] = '/';
  return res;
}//ModifyPathToMine


/*
----------
  ModifyPathToTheir
  Замена "/" на "\\" если платформа того требует
----------
*/
std::string ModifyPathToTheir( const std::string& path )
{
  if( std::string( DIR_DELIMITER ) == std::string( "/" ) )
    return path;
  size_t length = path.length();
  if( !length )
    return path;
  std::string res = path;

  for( size_t cur = 0; cur < length; ++cur )
    if( res[ cur ] == '/' )
      res[ cur ] = DIR_DELIMITER[ 0 ];
  return res;
}//ModifyPathToTheir




/*
----------
  getFileWriteTime
  Возвращает дату модификации файла в формате UNIX timestamp
----------
*/
Int64 GetFileWriteTime( const std::string& file )
{
  HANDLE hFile = ::CreateFile( file.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
  if( hFile == INVALID_HANDLE_VALUE )
  {
    __log.PrintInfo( Filelevel_WARNING, "getFileWriteTime('%s') => CreateFile() => INVALID_HANDLE_VALUE", file.data() );
    return 0;
  }
  FILETIME    creationTime, accessTime, writeTime;
  SYSTEMTIME  systemTime;
  if( !::GetFileTime( hFile, &creationTime, &accessTime, &writeTime ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "getFileWriteTime('%s') => GetFileTime() => FALSE", file.data() );
    ::CloseHandle( hFile );
    return 0;
  }
  if( !FileTimeToSystemTime( &writeTime, &systemTime ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "getFileWriteTime('%s') => FileTimeToSystemTime() => FALSE", file.data() );
    ::CloseHandle( hFile );
    return 0;
  }
  ::CloseHandle( hFile );
  tm timestamp;
  memset( &timestamp, 0, sizeof( timestamp ) );

  timestamp.tm_hour = systemTime.wHour;
  timestamp.tm_min  = systemTime.wMinute;
  timestamp.tm_sec  = systemTime.wSecond;

  timestamp.tm_mday = systemTime.wDay;
  timestamp.tm_mon  = systemTime.wMonth - 1;
  timestamp.tm_year = systemTime.wYear - 1900;
  return Int64( mktime( &timestamp ) );
}//getFileWriteTime



/*
----------
  Dump
  Дамп куска памяти в лог
----------
*/
void Dump( void *mem, Dword size, const std::string& comment )
{
  static char dec2hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  __log.PrintUseTime("\n============\nMemory dump at x%X: %s (%d byte[s])\n============\n", mem, comment.c_str(), size);
  Byte *data = ( Byte* ) mem;
  Dword sub;
  for( Dword num = 0; num < size; num += 16 )
  {
    __log.Print( "%11d| ", num >> 4 );
    for( sub = num; sub < num + 16 && sub < size; ++sub )
      __log.Print( "%c%c ", dec2hex[ data[ sub ] >> 4 ], dec2hex[ data[ sub ] % 16 ] );
    while( sub++ < num + 16 )
      __log.Print( "   " );
    for( sub = num; sub < num + 16 && sub < size; ++sub )
      __log.Print( "%c", ( data[ sub ] < 32 ? '.' : data[ sub ] ) );
    __log.Print( "\n" );
  }
  __log.Print( "============\nEnd of dump\n============\n" );
}//Dump



/*
----------
  StrToLowerCase
  Перевод строки в нижний регистр
----------
*/
std::string StrToLowerCase( std::string str )
{
  transform( str.begin(), str.end(), str.begin(), ::tolower );
  return str;
}//StrToLowerCase


/*
----------
  StrToUpperCase
  Перевод строки в нижний регистр
----------
*/
std::string StrToUpperCase( std::string str )
{
  transform( str.begin(), str.end(), str.begin(), ::toupper );
  return str;
}//StrToUpperCase



Vec4 StringToColor( const std::string& str ) {
  Vec4 color( Vec4One );
  if( str.length() != 8 ) {
    __log.PrintInfo( Filelevel_WARNING, "tools::StringToColor => bad string '%s'", str.c_str() );
  } else {
    std::string newStr = StrToLowerCase( str );
    Byte *data = ( Byte* ) newStr.c_str();
    color.Set(
      float( ( ( data[ 0 ] - ( data[ 0 ] <= '9' ? '0' : 'a' - 10 ) ) << 4 ) + data[ 1 ] - ( data[ 1 ] <= '9' ? '0' : 'a' - 10 ) ) * oneBy256,
      float( ( ( data[ 2 ] - ( data[ 2 ] <= '9' ? '0' : 'a' - 10 ) ) << 4 ) + data[ 3 ] - ( data[ 3 ] <= '9' ? '0' : 'a' - 10 ) ) * oneBy256,
      float( ( ( data[ 4 ] - ( data[ 4 ] <= '9' ? '0' : 'a' - 10 ) ) << 4 ) + data[ 5 ] - ( data[ 5 ] <= '9' ? '0' : 'a' - 10 ) ) * oneBy256,
      float( ( ( data[ 6 ] - ( data[ 6 ] <= '9' ? '0' : 'a' - 10 ) ) << 4 ) + data[ 7 ] - ( data[ 7 ] <= '9' ? '0' : 'a' - 10 ) ) * oneBy256
      );
  }
  return color;
}//StringToColor



}//namespace tools