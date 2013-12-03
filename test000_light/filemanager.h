/*==============================
  ����� FileManager
  ��������� ������ � �������
  ��� ������������� ������ ������ ������ "������" �������� ���������
  ����� � ������� ���������� ����� ����� �� ���� (��� ������� ���������� ���������� ������ � ������ �������)
  ������� ����� ���������� (��������, .svn ��� ������ �� �����)
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

//��������� ������������, �.�. ��������� ����� ��������� ����� � ����
#pragma pack(push,1)

struct _FileManager_FileInfo
{
  Dword nameLength;   //����� ����� ��� ����� ��������� ����
  Dword size;         //������ ������ �����
  Dword sizePacked;   //������ �������� �����
  Int64 time;         //����� ��������� �����
  Dword offset;       //�������� ����� �� ������ ������
  Byte  type;         //��� ������ (0 - ��� ������, 1 - zlib)
  Char* name;         //��� ����� (� ����)
};
#pragma pack(pop)

class FileManager
{
private:
  struct FileManagerItem
  {
    Int64   timestamp;  //Unix timestamp �������
    size_t  size,       //������ �����
            sizePacked, //������ ����� � ������ ����
            start;      //�������� ����� �� ������ ������ (0 ���� �� � ������)
    Byte    type;       //��� ������: -1 - ��������� �� ����������������, 0 - ������ �����������, 1 - zlib
    std::string
            name,       //��� �����
            path,       //���� � ����� + ���������
            fullPath,   //������ ���� � ������ �����
            pack,       //��� ������, � ������� ��������� ����
            ext;        //���������� �����

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