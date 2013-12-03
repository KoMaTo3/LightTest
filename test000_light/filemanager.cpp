#include "filemanager.h"
#include "zlib/zutil.h"
#include "file.h"
#include "tools.h"


FileManager *__fileManager = NULL;


FileManager::FileManagerItem::FileManagerItem()
:type( FileManager_type_UNKNOWN )
{
}


FileManager::FileManager()
{
}//constructor


FileManager::~FileManager()
{
}//desctuctor


/*
----------
  ScanAll
  Сканирование всех папок и архивов, начиная с заданной
----------
*/
void FileManager::ScanAll( const std::string& subDir )
{
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::ScanAll" );

  WIN32_FIND_DATA find;
  HANDLE hFind = NULL;
  std::string oldDir = tools::GetCurDirectory();
  memset( &find, 0, sizeof( find ) );

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::ScanAll => move to dir '%s'", subDir.c_str() );
  ::SetCurrentDirectory( ( tools::GetCorePath() + DIR_DELIMITER + subDir ).c_str() );

  if( INVALID_HANDLE_VALUE == ( hFind = ::FindFirstFile( "*", &find ) ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "FileManager::ScanAll => FindFirstFile failed" );
    return;
  }

  //main cycle
  do
  {
    if( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  //dir
    {
      __log.PrintInfo( Filelevel_DEBUG, "finded: dir [%s]", find.cFileName );
      if( !( find.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) //not hidden
      if( !( std::string( find.cFileName ) == "." || std::string( find.cFileName ) == ".." ) )  //не "." и не ".."
      {
        __log.PrintInfo( Filelevel_DEBUG, "scan it sub-items" );
        tools::SetCurDirectory( oldDir );
        this->ScanAll( subDir + DIR_DELIMITER + find.cFileName );
        ::SetCurrentDirectory( ( tools::GetCorePath() + DIR_DELIMITER + subDir ).c_str() );
      }
    }
    else  //file
    {
      __log.PrintInfo( Filelevel_DEBUG, "finded: file [%s]", find.cFileName );
      if( tools::GetFileExtension( find.cFileName ) != FileManager_EXTENSION ) //обычный файл, не архив
        this->AddFile( subDir, find.cFileName );
      else
        this->AddPack( subDir, find.cFileName ); //пак - добавляем его файлы
    }
  } while( FindNextFile( hFind, &find ) );

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::ScanAll => Done" );

  tools::SetCurDirectory( oldDir );
}//ScanAll


/*
----------
  AddFile
  Добавление файла в менеджер
----------
*/
void FileManager::AddFile( const std::string& path, const std::string& fileName )
{
  std::string oldDir = tools::GetCurDirectory();
  tools::SetCurDirectory( path );

  std::string newPath = tools::ModifyPathToMine( path );

  FILE *f = NULL;
  size_t  fileSize = 0;
  fopen_s( &f, fileName.c_str(), "rb" );
  if( !f )
  {
    __log.PrintInfo( Filelevel_WARNING, "FileManager::AddFile => can't open file [%s/%s]", newPath.c_str(), fileName.c_str() );
    tools::SetCurDirectory( oldDir );
    return;
  }
  fileSize = tools::FileSize( f );
  fclose( f );

  Int64 fileTime = tools::GetFileWriteTime( fileName.c_str() );

  if( this->_items.find( newPath + "/" + fileName ) != this->_items.end() ) //файл уже есть в менеджере, проверяем дату
    if( fileTime < this->_items[ newPath + "/" + fileName ].timestamp )     //если даты равны, то всё-равно берём этот внешний файл,
    {                                                                       //для ускорения работы с ресурсами
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddFile => file too old" );
      return;
    }

  FileManager::FileManagerItem& item = this->_items[ newPath + "/" + fileName ]; //в архиве делимитер всегда "/"
  item.fullPath   = newPath + "/" + fileName;
  item.name       = fileName;
  item.path       = newPath;
  item.size       = fileSize;
  item.sizePacked = 0;
  item.start      = 0;  //для архивных файлов такого быть не может
  item.timestamp  = fileTime;
  item.type       = FileManager_type_FILE;
  item.ext        = tools::GetFileExtension( fileName );

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddFile => [%s / %s] time[%d]", newPath.c_str(), fileName.c_str(), item.timestamp );

  tools::SetCurDirectory( oldDir );
}//AddFile


/*
----------
  AddPack
  Добавление файлов из архива в менеджер
----------
*/
void FileManager::AddPack( const std::string& path, const std::string& fileName )
{
  std::string oldDir = tools::GetCurDirectory();
  tools::SetCurDirectory( path );

  std::string newPath = tools::ModifyPathToMine( path );

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack( '%s' )", ( path + "/" + fileName ).c_str() );

  FILE *fHandle = NULL;
  size_t  fileSize = 0;
  fopen_s( &fHandle, fileName.c_str(), "rb" );
  if( !fHandle )
  {
    __log.PrintInfo( Filelevel_WARNING, "FileManager::AddPack => can't open pack-file [%s/%s]", newPath.c_str(), fileName.c_str() );
    tools::SetCurDirectory( oldDir );
    return;
  }
  fileSize = tools::FileSize( fHandle );
  fclose( fHandle );

  Dword headerSize;
  File f( fileName.c_str(), File_mode_READ );
  f.SeekFromStart( f.GetLength() - sizeof( Dword ) );
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => file size = %d", f.GetLength() );
  f.Read( &headerSize, sizeof( Dword ) );
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => header size = %d", headerSize );
  f.SeekFromStart( f.GetLength() - headerSize );  //переходим к началу заголовка
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => header start = %d", f.GetCurrentPos() );

  Dword curPos = 4,
        constHeaderSize = sizeof( _FileManager_FileInfo ) - sizeof( Char* );
  _FileManager_FileInfo fileInfo;
  memory tmp;

  while( curPos < headerSize )
  {
    __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => read (next) file info..." );
    f.Read( &fileInfo, constHeaderSize );
    tmp.realloc( fileInfo.nameLength + 1 );
    f.Read( tmp.getData(), tmp.getLength() );
    __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => file in pack: '%s'", tmp.getData() );
    curPos += constHeaderSize + tmp.getLength();
    //header fully readed
    if( this->_items.find( tmp.getData() ) != this->_items.end() )  //файл уже есть в менеджере, проверяем дату
    {
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => file '%s' from archive '%s' (time = %d) already in manager (time = %d)", tmp.getData(), ( path + "/" + fileName ).c_str(), Dword( fileInfo.time ), Dword( this->_items[ tmp.getData() ].timestamp ) );
      if( this->_items[ tmp.getData() ].timestamp > fileInfo.time ) //в менеджере более новый файл
        __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => file too old, skip" );
      else
      if( ( this->_items[ tmp.getData() ].timestamp == fileInfo.time &&       //в менеджере точно такой же файл
            this->_items[ tmp.getData() ].type == FileManager_type_PACKED &&  //если в менеджере - запакованный, а новый файл - не запакованный
            fileInfo.type == FileManager_type_NOTPACKED ) ||                  //или
          this->_items[ tmp.getData() ].timestamp < fileInfo.time )           //в менеджере устаревший файл
      {                                                                       //то заменяем
        __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => replace this file" );
        this->_items[ tmp.getData() ].pack = path + "/" + fileName;
        this->_items[ tmp.getData() ].size = fileInfo.size;
        this->_items[ tmp.getData() ].sizePacked = fileInfo.sizePacked;
        this->_items[ tmp.getData() ].start = f.GetCurrentPos() - constHeaderSize + tmp.getLength();
        this->_items[ tmp.getData() ].timestamp = fileInfo.time;
        this->_items[ tmp.getData() ].type = fileInfo.type;
      }
      else
        __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => file too old or files equal, but file in manager not packed" );
    }
    else  //добавляем файл в менеджер
    {
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::AddPack => add this file" );
      this->_items[ tmp.getData() ].fullPath = tmp;
      this->_items[ tmp.getData() ].name = tools::GetFileNameFromPath( tmp.getData() );
      this->_items[ tmp.getData() ].pack = path + "/" + fileName;
      this->_items[ tmp.getData() ].path = tools::GetPathFromFilePath( tmp.getData() );
      this->_items[ tmp.getData() ].size = fileInfo.size;
      this->_items[ tmp.getData() ].sizePacked = fileInfo.sizePacked;
      this->_items[ tmp.getData() ].start = fileInfo.offset;
      this->_items[ tmp.getData() ].timestamp = fileInfo.time;
      this->_items[ tmp.getData() ].type = fileInfo.type;
      this->_items[ tmp.getData() ].ext = tools::GetFileExtension( tmp.getData() );
    }
  }

  tools::SetCurDirectory( oldDir );
}//AddPack


/*
----------
  Dump
----------
*/
void FileManager::Dump()
{
  __log.PrintInfo( Filelevel_DEBUG, "\n===== FileManager =====" );

  for( _FileManagerItems::iterator iter = this->_items.begin(); iter != this->_items.end(); ++iter )
    __log.PrintAligned( "[%s][%s] %s, size[%d] packed[%d] start[%d]\n", iter->second.path.c_str(), iter->second.name.c_str(), ( iter->second.type == FileManager_type_FILE ? "Real file" : ( iter->second.type == FileManager_type_NOTPACKED ? "File in archive not packed" : ( iter->second.type == FileManager_type_PACKED ? "File in archive packed" : "Unknown file" ) ) ), iter->second.size, iter->second.sizePacked, iter->second.start );

  __log.PrintInfo( Filelevel_DEBUG, "\n=======================" );
}//Dump


/*
----------
  SaveAll
----------
*/
void FileManager::SaveAll( const std::string& fileName, bool useCompression )
{
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll" );

  std::string oldDir = tools::GetCurDirectory();
  std::string tmpName = fileName + ".tmp";
  Dword filesWrited = 0;

  tools::SetCurDirectory( FileManager_DATADIRECTORY );

  File f;
  if( f.Open( tmpName, File_mode_WRITE ) != aOK )
  {
    __log.PrintInfo( Filelevel_ERROR, "FileManager::SaveAll => Failed to write into file '%s'", tmpName.c_str() );
    tools::SetCurDirectory( oldDir );
    return;
  }

  tools::SetCurDirectory( oldDir );

  f.Print( "%s", FileManager_HEADER );
  Dword version = FileManager_VERSION;
  f.Write( &version, 4 );  //4!

  _FileManager_FileInfo fileInfo;
  std::vector< _FileManager_FileInfo > header;
  header.resize( this->_items.size() );

  //write files
  for( _FileManagerItems::iterator iter = this->_items.begin(); iter != this->_items.end(); ++iter )
  {
    __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => file" );
    if( iter->second.type == FileManager_type_FILE )  //файл => память => ( => запаковать)? архив
    { //файл
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => file '%s'", iter->second.fullPath.c_str() );
      memory buffer( iter->second.size );
      File fSrc;

      if( fSrc.Open( iter->second.fullPath, File_mode_READ ) == aOK )
      {
        fileInfo.name = ( Char* ) iter->second.fullPath.c_str();
        fileInfo.nameLength = iter->second.fullPath.length();
        fileInfo.size = fSrc.GetLength();
        fileInfo.sizePacked = 0;
        fileInfo.time = iter->second.timestamp;
        fileInfo.offset = f.GetCurrentPos();
        __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => file offset = %d", fileInfo.offset );

        __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => file opened '%s'", iter->second.fullPath.c_str() );
        //read file
        fSrc.Read( buffer.getData(), fSrc.GetLength() );
        fSrc.Close();
        uLongf resLength  = buffer.getLength();
        Byte  *resData    = ( Byte* ) buffer.getData();
        __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => file size: %d, buffer[x%X]", buffer.getLength(), buffer.getData() );
        memory destBuffer;

        //compress?
        if( useCompression )
        {
          fileInfo.type = FileManager_type_PACKED;
          destBuffer.alloc( buffer.getLength() < 1024 * 1024 * 10 ? buffer.getLength() * 2 : buffer.getLength() + 1024 * 16 );  //16k на всякий случай
          resLength = destBuffer.getLength();
          int compressResult;
          __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => use compression" );
          if( Z_OK == ( compressResult = compress( ( Byte* ) destBuffer.getData(), &resLength, ( const Byte* ) buffer.getData(), buffer.getLength() ) ) )
          { //ok, compresser
            __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => compressed to %d byte[s]", resLength );
            //iter->second.sizePacked = resLength;
            fileInfo.sizePacked = resLength;
            resData = ( Byte* ) destBuffer.getData();
          }
          else
          {
            __log.PrintInfo( Filelevel_ERROR, "FileManager::SaveAll => failed compress() = %d", compressResult );
            fileInfo.type = FileManager_type_NOTPACKED;
            resLength = buffer.getLength();
          }
        }
        else
          fileInfo.type = FileManager_type_NOTPACKED;

        __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => write to pack %d bytes...", resLength );
        f.Write( resData, resLength );  //write
        header[ filesWrited++ ] = fileInfo;

        __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => writing done" );
        fSrc.Close();
      }
      else
        __log.PrintInfo( Filelevel_ERROR, "FileManager::SaveAll => Can't read file '%s'", iter->second.fullPath.c_str() );
    }
  }//files

  //headers
  Dword headerSize = sizeof( Dword ), //конечный Dword = размер заголовка
        constHeaderSize = sizeof( _FileManager_FileInfo ) - sizeof( Char* );
  for( std::vector< _FileManager_FileInfo >::iterator iter = header.begin(); iter != header.end(); ++iter )
  {
    f.Write( &(*iter), constHeaderSize );
    f.Write( iter->name, iter->nameLength + 1 );
    headerSize += constHeaderSize + iter->nameLength + 1;
    __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => offset '%s' in header = %d", iter->name, iter->offset );
  }

  f.Write( &headerSize, sizeof( headerSize ) );
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll => header size = %d, const header size = %d", headerSize, constHeaderSize );

  f.Close();

  File::Rename( std::string( FileManager_DATADIRECTORY ) + DIR_DELIMITER + tmpName, std::string( FileManager_DATADIRECTORY ) + DIR_DELIMITER + fileName );

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::SaveAll Done" );
}//SaveAll


/*
----------
  GetFile
  addLastNull - добавить ли завершающий 0 после файла (например, если файл текстовый и нам необходимо его представить как C-строку)
----------
*/
bool FileManager::GetFile( const std::string& fileName, memory& dest, bool addLastNull )
{
  __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile ( '%s' )", fileName.c_str() );
  if( this->_items.find( fileName ) == this->_items.end() )
  {
    __log.PrintInfo( Filelevel_ERROR, "FileManager::GetFile => file not found" );
    return false;
  }

  FileManager::FileManagerItem file = this->_items[ fileName ];

  switch( file.type )
  {
  case FileManager_type_FILE: //просто файл => читаем, отдаём
    {
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile => getting real file" );
      File f;
      if( f.Open( file.fullPath.c_str(), File_mode_READ ) != aOK )
        return false;
      dest.alloc( f.GetLength() + ( addLastNull ? 1 : 0 ) );
      f.Read( dest.getData(), f.GetLength() );
      f.Close();
      if( addLastNull )
        dest.getData()[ dest.getLength() - 1 ] = 0;
    }
  break;

  case FileManager_type_NOTPACKED:  //в архиве, но не запакован => достаём, отдаём
    {
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile => getting file from not compressed pack" );
      File f;
      if( f.Open( file.pack, File_mode_READ ) != aOK )
        return false;

      f.SeekFromStart( file.start );
      dest.alloc( file.size + ( addLastNull ? 1 : 0 ) );
      f.Read( dest.getData(), file.size );
      f.Close();
      if( addLastNull )
        dest.getData()[ dest.getLength() - 1 ] = 0;
    }
  break;

  case FileManager_type_PACKED: //в архиве, запакован => достаём, распаковываем, отдаём
    {
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile => getting file from compressed pack" );
      File f;
      if( f.Open( file.pack, File_mode_READ ) != aOK )
        return false;

      f.SeekFromStart( file.start );
      __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile => file start = %d", f.GetCurrentPos() );
      memory tmp( file.sizePacked );
      f.Read( tmp.getData(), tmp.getLength() );
      f.Close();

      dest.alloc( file.size + ( addLastNull ? 1 : 0 ) );
      uLongf destSize = file.size;
      int res;
      if( Z_OK != ( res = uncompress( ( Byte* ) dest.getData(), &destSize, ( Byte* ) tmp.getData(), tmp.getLength() ) ) )
      {
        __log.PrintInfo( Filelevel_ERROR, "FileManager::GetFile => uncompress failed (%d)", res );
        dest.free();
        return false;
      }
      if( addLastNull )
        dest.getData()[ dest.getLength() - 1 ] = 0;
    }
  break;
  }//switch type

  __log.PrintInfo( Filelevel_DEBUG, "FileManager::GetFile => Done" );

  return true;
}//GetFile


/*
----------
  FileExists
----------
*/
bool FileManager::FileExists( const std::string& fileName )
{
  return
    this->_items.find( fileName ) != this->_items.end();
}//FileExists


/*
----------
  FindFile
----------
*/
void FileManager::FindFiles( const std::string& extension, std::vector< std::string >& result )
{
  result.clear();
  for( _FileManagerItems::iterator iter = this->_items.begin(); iter != this->_items.end(); ++iter )
    if( iter->second.ext == extension )
    {
      result.resize( result.size() + 1 );
      result[ result.size() - 1 ] = iter->second.fullPath;
    }
}//FindFile
