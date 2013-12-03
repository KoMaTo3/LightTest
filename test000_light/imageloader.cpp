#define XMD_H

#include "imageloader.h"
#include "file.h"
#include "tools.h"
#include "jpeglib/jpeglib.h"
#include "memoryreader.h"
#include "filemanager.h"

ImageLoader::ImageLoader()
:_size ( 0, 0 )
{
}//constructor


ImageLoader::~ImageLoader()
{
}//destructor


/*
----------
  LoadFromFile
----------
*/
Bool ImageLoader::LoadFromFile( const string fileName )
{
  if( !fileName.length() )
  {
    __log.PrintInfo( Filelevel_ERROR, "ImageLoader::LoadFromFile => File not found ('%s')", fileName.c_str() );
    return false;
  }

  if( !__fileManager )
  {
    __log.PrintInfo( Filelevel_ERROR, "ImageLoader::LoadFromFile => file manager is NULL" );
    return false;
  }

  /*
  File f;
  if( f.Open( fileName, File_mode_READ ) != aOK )
  {
    __log.PrintInfo( Filelevel_ERROR, "ImageLoader::LoadFromFile => Can't open file for read ('%s')", fileName.c_str() );
    return false;
  }

  memory buffer( f.GetLength() );

  f.Read( buffer.getData(), buffer.getLength() );
  Bool result = this->LoadFromBuffer( (Byte*) buffer.getData(), buffer.getLength() );
  f.Close();
  */
  memory buffer;
  if( !__fileManager->GetFile( fileName, buffer ) )
    return false;
  Bool result = this->LoadFromBuffer( (Byte*) buffer.getData(), buffer.getLength() );

  return result;
}//LoadFromFile


/*
----------
  LoadFromBuffer
----------
*/
Bool ImageLoader::LoadFromBuffer( Byte *buffer, Dword length )
{
  if( !buffer || !length )
  {
    __log.PrintInfo( Filelevel_ERROR, "ImageLoader::LoadFromBuffer => buffer[x%X] length[x%X]", buffer, length );
    return false;
  }

  if(length >= 2) //BMP
  {
    if(
      buffer[0] == 'B' &&
      buffer[1] == 'M'
      )
      return this->_LoadFromBufferBMP( buffer, length );
  }
  if(length >= 10)  //JPG
  {
    if(
      buffer[6] == 'J' &&
      buffer[7] == 'F' &&
      buffer[8] == 'I' &&
      buffer[9] == 'F'
      )
      return this->_LoadFromBufferJPG( buffer, length );
  }
  if(length >= 4)  //PNG
  {
    if(
      buffer[0] == 0x89 &&
      buffer[1] == 'P' &&
      buffer[2] == 'N' &&
      buffer[3] == 'G'
      )
      return this->_LoadFromBufferPNG( buffer, length );
  }
  if(length >= 3)  //TGA
  {
    if(
      buffer[0] == 0 &&
      buffer[1] == 0 &&
      (buffer[2] == 0x02 || buffer[2] == 0x0A)
      )
      return this->_LoadFromBufferTGA( buffer, length );
  }

  return false;
}//LoadFromBuffer


/*
----------
  _LoadFromBufferBMP
----------
*/
Bool ImageLoader::_LoadFromBufferBMP( Byte *buffer, Dword length )
{
  BITMAPFILEHEADER *fileHeader;
  BITMAPINFOHEADER *infoHeader;
  memory data;
  Pos<Dword> pos;
  Dword srcPos, destPos;

  if( length < sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "ImageLoader::_LoadFromBufferBMP => buffer too short" );
    return false;
  }

  fileHeader = (BITMAPFILEHEADER*) buffer;
  infoHeader = (BITMAPINFOHEADER*) ( buffer + sizeof( BITMAPFILEHEADER ) );

  this->_size.width = infoHeader->biWidth;
  this->_size.height = infoHeader->biHeight;
  this->_data.alloc( this->_size.width * this->_size.height * 4 );

  switch( infoHeader->biBitCount )
  {
  case 24:
    {
      for( pos.y = 0; pos.y < this->_size.height; ++pos.y )
      for( pos.x = 0; pos.x < this->_size.width; ++pos.x )
      {
        destPos = ( this->_size.height - pos.y - 1 ) * this->_size.width + pos.x;
        srcPos  = fileHeader->bfOffBits + (pos.y * this->_size.width + pos.x) * 3;
        ((Dword*) this->_data.getData())[ destPos ] = ARGB(
            0xFF,
            buffer[ srcPos + 0 ],
            buffer[ srcPos + 1 ],
            buffer[ srcPos + 2 ]
        );
      }
    }
  break;
  case 32:
    {
      for( pos.y = 0; pos.y < this->_size.height; ++pos.y )
      for( pos.x = 0; pos.x < this->_size.width; ++pos.x )
      {
        destPos = ( this->_size.height - pos.y - 1 ) * this->_size.width + pos.x;
        srcPos  = fileHeader->bfOffBits + (pos.y * this->_size.width + pos.x) * 4;
        ((Dword*) this->_data.getData())[ destPos ] = ARGB(
            buffer[ srcPos + 4 ], //255
            buffer[ srcPos + 0 ],
            buffer[ srcPos + 1 ],
            buffer[ srcPos + 2 ]
        );
      }
    }
  break;
  }

  //__log.PrintInfo( Filelevel_DEBUG, "BMP: bpp[%d] size[%d x %d] dataStart[%d]", infoHeader->biBitCount, this->_size.width, this->_size.height, fileHeader->bfOffBits );

  return true;
}//_LoadFromBufferBMP


/*
----------
  _LoadFromBufferTGA
----------
*/
Bool ImageLoader::_LoadFromBufferTGA( Byte *buffer, Dword length )
{
  if( !buffer || !length )
    return false;

  Byte *tmp = buffer;

  Byte idLength = tmp[ 0 ];
  if( tmp[ 1 ] )
    return false;

  Byte compressed = tmp[ 2 ]; //RLE-compression
  if( !( compressed == 2 || compressed == 0x0A ) )
    return false;

  Word *w = ( Word* ) ( tmp + 12 ),
        *h = ( Word* ) ( tmp + 14 );

  Byte bpp = tmp[ 16 ];
  if( !( bpp == 24 || bpp == 32 ) )
    return false;
  //__log.PrintInfo( Filelevel_DEBUG, ". bpp: %d", bpp );

  Dword src_pos = 18 + idLength, dest_pos = 0;

  this->_size.width = *w;
  this->_size.height = *h;
  //__log.PrintInfo( Filelevel_DEBUG, ". size: %d x %d", this->_size.width, this->_size.height );
  this->_data.alloc( this->_size.width * this->_size.height * 4 );
  Byte *dest = ( Byte* ) this->_data.getData();

  if( compressed == 2 )
  {//not compressed
    //__log.PrintInfo( Filelevel_DEBUG, ". compression: none" );
    Word x, y;
    Byte mult = bpp >> 3;
    for( y = 0; y < this->_size.height; y++)
    for( x = 0; x < this->_size.width; x++)
    {
      dest_pos = ( x + y * this->_size.width ) << 2;
      //dest_pos = ( x + ( this->_size.height - y - 1 ) * this->_size.width ) << 2;
      src_pos  = ( x + y * this->_size.width ) * mult + 18;
      dest[ dest_pos     ] = tmp[ src_pos + 2 ];
      dest[ dest_pos + 1 ] = tmp[ src_pos + 1 ];
      dest[ dest_pos + 2 ] = tmp[ src_pos + 0 ];
      dest[ dest_pos + 3 ] = ( bpp == 32 ? tmp[ src_pos + 3 ] : 255);
    }
  }
  else
  {//RLE-compression 0x0A
    //__log.PrintInfo( Filelevel_DEBUG, ". compression: RLE" );
    Dword src_pos = 18 + idLength, dest_pos = 0;
    Byte block, num, q;
    Byte r, g, b, a = 255;
    Word x = 0, y = 0;
    while( y < this->_size.height )
    {
      block = tmp[ src_pos++ ];
      num = block & 127;

      if( block & 128 )
      {//compressed block
        b = tmp[ src_pos     ];
        g = tmp[ src_pos + 1 ];
        r = tmp[ src_pos + 2 ];
        if( bpp == 32 )
        {
          a = tmp[ src_pos + 3 ];
          src_pos += 4;
        }
        else
          src_pos += 3;
        for( q = 0; q < num + 1; ++q )
        {
          dest_pos = ( x + y * this->_size.width ) << 2;
          //dest_pos = ( x + ( this->_size.height - y - 1 ) * this->_size.width ) << 2;
          dest[ dest_pos     ] = r;
          dest[ dest_pos + 1 ] = g;
          dest[ dest_pos + 2 ] = b;
          dest[ dest_pos + 3 ] = a;

          x = ( x + 1 ) % this->_size.width;
          if( !x )
            ++y;
        }
      }
      else
      {//not compressed block
        for( q = 0; q < num + 1; ++q )
        {
          dest_pos = ( x + y * this->_size.width ) << 2;
          //dest_pos = ( x + ( this->_size.height - y - 1 ) * this->_size.width ) << 2;
          b = tmp[ src_pos     ];
          g = tmp[ src_pos + 1 ];
          r = tmp[ src_pos + 2 ];
          if( bpp == 32 )
          {
            a = tmp[ src_pos + 3 ];
            src_pos += 4;
          }
          else
            src_pos += 3;

          dest[ dest_pos     ] = r;
          dest[ dest_pos + 1 ] = g;
          dest[ dest_pos + 2 ] = b;
          dest[ dest_pos + 3 ] = a;

          x = ( x + 1 ) % this->_size.width;
          if( !x )
            ++y;
        }//for q < num
      }//not compressed block
    }//while y < height
  }//RLE-compression

  return true;
}//_LoadFromBufferTGA


/*
----------
  GetImageSize
----------
*/
const Size* ImageLoader::GetImageSize()
{
  return &this->_size;
}//GetImageSize


/*
----------
  _LoadFromBufferJPG
----------
*/
Bool ImageLoader::_LoadFromBufferJPG( Byte *buffer, Dword length )
{
  //__log.PrintInfo( Filelevel_DEBUG, "ImageLoader::_LoadFromBufferJPG" );
  jpeg_decompress_struct	    cinfo;
  ImgJpg::_gImporterJPEGError	jerr;

  MemoryReader file;
  file.SetSource( buffer, length );

  cinfo.err = jpeg_std_error( &jerr.pub );		  // устанавливаем дефолтный менеджер обработки ошибок
  jerr.pub.error_exit = ImgJpg::JPEGErrorExit;	// присваиваем дефолтную функцию для обработки ошибки

  if( setjmp(jerr.setjmp_buffer) )
  {
	  jpeg_destroy_decompress( &cinfo );
	  file.Free();
    return false;
  }
  jpeg_create_decompress( &cinfo );
  ImgJpg::JPEGStdioSrcgMemoryReader( &cinfo, &file );
  jpeg_read_header( &cinfo, TRUE );
  jpeg_start_decompress( &cinfo );
  int row_stride = cinfo.output_width * cinfo.output_components;
  memory temp_mem( cinfo.output_height * row_stride );
  JSAMPARRAY bufferNew = ( *cinfo.mem->alloc_sarray )( ( j_common_ptr ) &cinfo, JPOOL_IMAGE, row_stride, 1 );
  memset( *bufferNew, 0, sizeof(JSAMPLE) * row_stride );

  int y = 0;
  while( cinfo.output_scanline < cinfo.output_height )
  {
	  jpeg_read_scanlines( &cinfo, bufferNew, 1 );
    byte* temp = ( BYTE* ) temp_mem.getData() + ( ( cinfo.output_height - y - 1 ) * row_stride );
	  memcpy( temp, bufferNew[0], row_stride );
	  ++y;
  }

  this->_size.width = cinfo.image_width;
  this->_size.height = cinfo.image_height;

  {
    Dword x, y;
    Int pos3b, pos4b;
    this->_data.alloc( this->_size.height * this->_size.width * 4 );
    Byte *src_mem = (Byte*) temp_mem.getData(), *dest_mem = (Byte*) this->_data.getData();
    for( y = 0; y < this->_size.height; ++y )
    for( x = 0; x < this->_size.width; ++x )
    {
      pos3b = ( x + y * this->_size.width ) * 3;
      //pos4b = ( x + ( /* this->_size.height - y - 1 */ y ) * this->_size.width ) * 4;
      pos4b = ( x + ( this->_size.height - y - 1 ) * this->_size.width ) * 4;
      dest_mem[ pos4b    ] = src_mem[ pos3b + 0 ];
      dest_mem[ pos4b + 1] = src_mem[ pos3b + 1 ];
      dest_mem[ pos4b + 2] = src_mem[ pos3b + 2 ];
      dest_mem[ pos4b + 3] = 255;
    }
  }
  temp_mem.free();

  jpeg_finish_decompress( &cinfo );
  jpeg_destroy_decompress( &cinfo );
  file.Free();

  //__log.PrintInfo( Filelevel_DEBUG, "ImageLoader::_LoadFromBufferJPG => Done" );

  return true;
}//_LoadFromBufferJPG


/*
----------
  _LoadFromBufferPNG
----------
*/
Bool ImageLoader::_LoadFromBufferPNG( Byte *buffer, Dword length )
{
  //__log.PrintInfo( Filelevel_DEBUG, "ImageLoader::_LoadFromBufferPNG" );

  png_structp   png_ptr;
  png_infop     info_ptr;
  unsigned int  sig_read = 0;
  png_uint_32   img_width = 0, img_height = 0;
  int           bit_depth = 0, color_type = 0, interlace_type = 0;

  MemoryReader memReader;
  memReader.SetSource( buffer, length );
  memory data_mem( 256 );
  Char *pdata = data_mem.getData();

  memReader.Read( pdata, 4 );
  if( !png_check_sig( ( BYTE* ) pdata ,4 ) )
  {
    memReader.Free();
    return false;
  }
  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
  if( png_ptr == NULL )
  {
    memReader.Free();
    return false;
  }
  info_ptr = png_create_info_struct( png_ptr );
  if( info_ptr == NULL )
  {
    memReader.Free();
    return false;
  }
  memReader.SeekFromStart( 4 );
  png_set_read_fn( png_ptr, ( void* ) &memReader, ImgPng::PNGReadFunctiongMemoryReader );//!!!

  png_set_sig_bytes( png_ptr, 4 );

  // Тут можно настроить прогрессбар, таким макаром:
  // описываем функцию где-то выше
  // void read_row_callback(png_ptr, png_uint_32 row, int pass) { }
  // информируем библиотеку об этой функции
  //	png_set_read_status_fn(png_ptr, read_row_callback);

  png_read_info( png_ptr, info_ptr );
  png_get_IHDR( png_ptr, info_ptr, &img_width, &img_height, &bit_depth,
    &color_type, &interlace_type, 0, 0 );
  if( bit_depth == 16 )
    png_set_strip_16( png_ptr );
  if( color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8 )
    png_set_palette_to_rgb( png_ptr );
  if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
    png_set_gray_1_2_4_to_8( png_ptr );
  if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
    png_set_tRNS_to_alpha( png_ptr );
  if( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
    png_set_gray_to_rgb( png_ptr );
  double gamma = 0.0;
  if( png_get_gAMA( png_ptr, info_ptr, &gamma ) )
    png_set_gamma( png_ptr, 2.2, gamma );
  else
    png_set_gamma( png_ptr, 2.2, 0.45455 );
  png_read_update_info( png_ptr, info_ptr );
  png_get_IHDR( png_ptr, info_ptr, &img_width, &img_height, &bit_depth, &color_type, 0, 0, 0 );
  png_uint_32 row_bytes = png_get_rowbytes( png_ptr, info_ptr );
  data_mem.alloc( img_height * row_bytes );
  pdata = data_mem.getData();
  png_byte **row_pointers = new png_byte * [ img_height ];
  for( unsigned int i = 0; i < img_height; ++i )
    row_pointers[ i ] = ( BYTE* ) pdata + i * row_bytes;
  png_read_image( png_ptr, row_pointers );
  png_destroy_read_struct( &png_ptr, 0, 0 );
  DEF_DELETE( row_pointers );
  memReader.Free();

  this->_size.width = img_width;
  this->_size.height = img_height;
  this->_data.alloc( this->_size.width * this->_size.height * 4 );

  //__log.PrintInfo( Filelevel_DEBUG, "color_type = %d, interlace_type = %d", color_type, interlace_type );
  if( color_type == PNG_COLOR_TYPE_RGB )  //2
  {
    Dword x, y, pos1, pos2;
    Byte *dataDest = ( Byte* ) this->_data.getData();
    for( y = 0; y < this->_size.height; ++y )
    for( x = 0; x < this->_size.width; ++x )
    {
      pos1 = x * 3 + row_bytes * y;
      pos2 = ( x + y * this->_size.width ) << 2;
      dataDest[ pos2     ] = pdata[ pos1 + 0 ];
      dataDest[ pos2 + 1 ] = pdata[ pos1 + 1 ];
      dataDest[ pos2 + 2 ] = pdata[ pos1 + 2 ];
      dataDest[ pos2 + 3 ] = 0xFF;
    }
  }
  else
  if( color_type == PNG_COLOR_TYPE_RGB_ALPHA )  //6
  {
    Dword x, y, pos;
    Byte *dataDest = ( Byte* ) this->_data.getData();
    for( y = 0; y < this->_size.height; ++y)
    for( x = 0; x < this->_size.width; ++x)
    {
      pos = ( x + y * this->_size.width ) << 2;
      dataDest[ pos     ] = pdata[ pos + 0 ];
      dataDest[ pos + 1 ] = pdata[ pos + 1 ];
      dataDest[ pos + 2 ] = pdata[ pos + 2 ];
      dataDest[ pos + 3 ] = pdata[ pos + 3 ];
    }
  }


  return true;
}//_LoadFromBufferPNG














/*==============================
  ImgJpg
==============================*/

const int ImgJpg::c_JPEGInputBufferSize = 4096;

// наша функция обработки ошибок
void ImgJpg::JPEGErrorExit(j_common_ptr cinfo)
{
    // получаем ссылочку на нашу структуру
  ImgJpg::_gImporterJPEGError* myerr = (ImgJpg::_gImporterJPEGError*)cinfo->err;
    // выводим сообщение об ошибке (наверное можно убрать)
    (*cinfo->err->output_message)(cinfo);
    // делаем прыжок на очистку данных и ретурн ошибки
    longjmp(myerr->setjmp_buffer, 1);
}

struct _gImporterJPEGSourcegFile
{
    jpeg_source_mgr  pub;    // ссылочка на стандартный менеджер
    File      *file;  // открытый файл
    JOCTET    *buffer;  // буфер данных
    bool      sof;    // признак того, что файл только что открыли
};

struct _gImporterJPEGSourcegMemoryReader
{
    jpeg_source_mgr  pub;    // ссылочка на стандартный менеджер
    MemoryReader *file;  // открытый 'файл'
    JOCTET    *buffer;  // буфер данных
    bool      sof;    // признак того, что файл только что открыли
};

// вызывается, когда переменная bytes_in_buffer достигает 0 и возникает
// необходимость в новой порции информации. возвращает TRUE, если буфер
// перезаполнен успешно, и FALSE если произошла ошибка ввода/вывода.
boolean JPEGFillInputBuffergFile(j_decompress_ptr cinfo)
{
  _gImporterJPEGSourcegFile* src = (_gImporterJPEGSourcegFile*)cinfo->src;
  // читаем кусками по ImgJpg::c_JPEGInputBufferSize байт
  size_t nbytes = src->file->Read(src->buffer, sizeof(JOCTET), ImgJpg::c_JPEGInputBufferSize);
  // если мы ничего не считали :(
  if (nbytes <= 0) {
    if ( src->sof )  return(FALSE); // блин, нам дали пустой файл - заорем "нехорошо" :)
    // если уже читали до этого, то вставляем в буфер инфу о конце файла
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  // загоняем инфу в буфер, и размер скока прочли
  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->sof = false;  // файл не пустой, пронесло :)))
  // возвращаем успешное выполнение операции
  return(TRUE);
}

boolean JPEGFillInputBuffergMemoryReader(j_decompress_ptr cinfo)
{
  _gImporterJPEGSourcegMemoryReader* src = (_gImporterJPEGSourcegMemoryReader*)cinfo->src;
  // читаем кусками по ImgJpg::c_JPEGInputBufferSize байт
  size_t nbytes = src->file->Read(src->buffer, sizeof(JOCTET), ImgJpg::c_JPEGInputBufferSize);
  // если мы ничего не считали :(
  if (nbytes <= 0) {
    if ( src->sof )  return(FALSE); // блин, нам дали пустой файл - заорем "нехорошо" :)
    // если уже читали до этого, то вставляем в буфер инфу о конце файла
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  // загоняем инфу в буфер, и размер скока прочли
  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->sof = false;  // файл не пустой, пронесло :)))
  // возвращаем успешное выполнение операции
  return(TRUE);
}

// инициализация источника. вызывается до того, как какая-нибудь информация
// будет из него прочтена.
void JPEGInitSourcegFile(j_decompress_ptr cinfo)
{
	_gImporterJPEGSourcegFile* src = (_gImporterJPEGSourcegFile*)cinfo->src;
	// говорим, шо файл тока шо открыт, выдыруг он пустой? :)
	src->sof = true;
}

void JPEGInitSourcegMemoryReader(j_decompress_ptr cinfo)
{
	_gImporterJPEGSourcegMemoryReader* src = (_gImporterJPEGSourcegMemoryReader*)cinfo->src;
	// говорим, шо файл тока шо открыт, выдыруг он пустой? :)
	src->sof = true;
}

// происходит, когда необходимо пропустить num_bytes. в случае опустошения
// буфера, его необходимо перезагрузить.
void JPEGSkipInputDatagFile(j_decompress_ptr cinfo, long num_bytes)
{
	_gImporterJPEGSourcegFile* src = (_gImporterJPEGSourcegFile*)cinfo->src;
	// если нужно снести 0 байт :) обижено уходим
	if (num_bytes <= 0) return;
	// выкидываем инфу из буфера и перегружаем его, пока num_bytes не станет
	// меньше размера буфера
	while (num_bytes > (long) src->pub.bytes_in_buffer) {
		num_bytes -= (long) src->pub.bytes_in_buffer;
		JPEGFillInputBuffergFile(cinfo);
	}
	// а теперь просто правильно настраиваем указатели на оставшуюся часть
	src->pub.next_input_byte += (size_t) num_bytes;
	src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

void JPEGSkipInputDatagMemoryReader(j_decompress_ptr cinfo, long num_bytes)
{
	_gImporterJPEGSourcegMemoryReader* src = (_gImporterJPEGSourcegMemoryReader*)cinfo->src;
	// если нужно снести 0 байт :) обижено уходим
	if (num_bytes <= 0) return;
	// выкидываем инфу из буфера и перегружаем его, пока num_bytes не станет
	// меньше размера буфера
	while (num_bytes > (long) src->pub.bytes_in_buffer) {
		num_bytes -= (long) src->pub.bytes_in_buffer;
		JPEGFillInputBuffergFile(cinfo);
	}
	// а теперь просто правильно настраиваем указатели на оставшуюся часть
	src->pub.next_input_byte += (size_t) num_bytes;
	src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

// убить ресурс. вызывается функцией jpeg_finish_decompress когда все данные будут
// прочтены. у нас ничего сносить не надо.
void JPEGTermSource(j_decompress_ptr cinfo) { }

void JPEGStdioSrcgFile(j_decompress_ptr cinfo, File* file)
{
  _gImporterJPEGSourcegFile* src = 0;
  // смотрим, выделена ли память под JPEG-декомпрессор менеджер?
  // возможна ситуация, когда происходит одновременное обращение к источнику
  // от нескольких библиотек
  if (cinfo->src == 0) {
    // выделим память под наш менеджер, и установим на него указатель глобальной структуры
    // библиотеки. так как я использую менеджер памяти библиотеки JPEG то позаботится об
    // освобождении она сама. JPOOL_PERMANENT - означает что эта память выделяется на все
    // время работы с библиотекой
    cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small) 
      ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(_gImporterJPEGSourcegFile));
    src = (_gImporterJPEGSourcegFile*) cinfo->src;
    // выделяю память для буффера данных, прочитанных из файла
    src->buffer = (JOCTET*) (*cinfo->mem->alloc_small) 
      ((j_common_ptr) cinfo, JPOOL_PERMANENT, ImgJpg::c_JPEGInputBufferSize * sizeof(JOCTET));
    memset(src->buffer, 0, ImgJpg::c_JPEGInputBufferSize * sizeof(JOCTET));
  }
  // для краткости - сестры таланта
  src = (_gImporterJPEGSourcegFile*)cinfo->src;
  // настраиваем обработчики событий на наши функции
  src->pub.init_source = JPEGInitSourcegFile;
  src->pub.fill_input_buffer = JPEGFillInputBuffergFile;
  src->pub.skip_input_data = JPEGSkipInputDatagFile;
  src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
  src->pub.term_source = JPEGTermSource;
  // теперь заполняем поля нашей структуры
  src->file = file;
  // настраиваем указатели на буфера
  src->pub.bytes_in_buffer = 0;  // forces fill_input_buffer on first read
  src->pub.next_input_byte = 0;  // until buffer loaded
}


void ImgJpg::JPEGStdioSrcgMemoryReader(j_decompress_ptr cinfo, MemoryReader* file)
{
  _gImporterJPEGSourcegMemoryReader * src = 0;
  // смотрим, выделена ли память под JPEG-декомпрессор менеджер?
  // возможна ситуация, когда происходит одновременное обращение к источнику
  // от нескольких библиотек
  if (cinfo->src == 0) {
    // выделим память под наш менеджер, и установим на него указатель глобальной структуры
    // библиотеки. так как я использую менеджер памяти библиотеки JPEG то позаботится об
    // освобождении она сама. JPOOL_PERMANENT - означает что эта память выделяется на все
    // время работы с библиотекой
    cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small) 
      ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(_gImporterJPEGSourcegMemoryReader));
    src = (_gImporterJPEGSourcegMemoryReader*) cinfo->src;
    // выделяю память для буффера данных, прочитанных из файла
    src->buffer = (JOCTET*) (*cinfo->mem->alloc_small) 
      ((j_common_ptr) cinfo, JPOOL_PERMANENT, ImgJpg::c_JPEGInputBufferSize * sizeof(JOCTET));
    memset(src->buffer, 0, ImgJpg::c_JPEGInputBufferSize * sizeof(JOCTET));
  }
  // для краткости - сестры таланта
  src = (_gImporterJPEGSourcegMemoryReader*)cinfo->src;
  // настраиваем обработчики событий на наши функции
  src->pub.init_source = JPEGInitSourcegMemoryReader;
  src->pub.fill_input_buffer = JPEGFillInputBuffergMemoryReader;
  src->pub.skip_input_data = JPEGSkipInputDatagMemoryReader;
  src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
  src->pub.term_source = JPEGTermSource;
  // теперь заполняем поля нашей структуры
  src->file = file;
  // настраиваем указатели на буфера
  src->pub.bytes_in_buffer = 0;  // forces fill_input_buffer on first read
  src->pub.next_input_byte = 0;  // until buffer loaded
}













/*==============================
  ImgPng
==============================*/

Void ImgPng::PNGReadFunctiongFile( png_structp png_ptr, png_bytep data, png_size_t length )
{
  File *file = ( File* ) png_get_io_ptr( png_ptr );
  file->Read( data, Dword( length ) );
}

Void ImgPng::PNGReadFunctiongMemoryReader( png_structp png_ptr, png_bytep data, png_size_t length )
{
  MemoryReader *file = ( MemoryReader* ) png_get_io_ptr( png_ptr );
  file->Read( data, long( length ) );
}
