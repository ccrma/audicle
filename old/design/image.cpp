
#include "image.h"
//PNG functionality

#ifdef WIN32
#ifndef _WINDOWS
#define _WINDOWS
#endif
#define ZLIB_DLL
#endif

#include "png.h"

/***********************************************************************
 * Method : Image::open_png
 * Params : char *file_name
 * Returns: FILE* (null on failure)
 * Effects: opens given file and verifies it is a png file
 ***********************************************************************/
FILE*
Image::open_png(char *file_name)
{
   // open the file and verify it is a PNG file:
   // Windows needs the following to be "rb", the b turns off
   // translating from DOS text to UNIX text
   FILE* fp = fopen(file_name, "rb");
   if (!fp) {
      fprintf(stderr,"Image::open_png: can't open file %s", file_name);
      return 0;
   }

   // Read in the signature bytes:
   unsigned char buf[PNG_BYTES_TO_CHECK];
   if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
      fprintf(stderr,"Image::open_png: can't read file %s", file_name);
      return 0;
   }

   // Compare the first PNG_BYTES_TO_CHECK bytes of the signature:
   if (!png_sig_cmp(buf, 0, PNG_BYTES_TO_CHECK)) {
      // OK: it is a PNG file
      return fp;
   } else {
      // it is not a PNG file
      // close the file, return NULL:
      fclose(fp);
      return 0;
   }
}

/***********************************************************************
 * Method : Image::read_png
 * Params : char *file_name
 * Returns: int (success/failure)
 * Effects: opens file, reads data into memory (if it is a png file)
 ***********************************************************************/
int
Image::read_png(char *file)
{
   // reset and free up memory:
   clear();

   FILE *fp = open_png(file);
   if (!fp)
      return 0;

   png_structp  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
   if (!png_ptr) {
      fclose(fp);
      fprintf(stderr,"Image::read_png: png_create_read_struct() failed");
      return 0;
   }

   // Allocate/initialize the memory for image information
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      fprintf(stderr,"Image::read_png: png_create_info_struct() failed");
      return 0;
   }

   // Set error handling
   if (setjmp(png_ptr->jmpbuf)) {
      // jump here from error encountered inside PNG code...
      // free all memory associated with the png_ptr and info_ptr
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      fclose(fp);
      fprintf(stderr,"Image::read_png: error reading file %s", file);
      clear();
      return 0;
   }

   // Set up the input control (using standard C streams)
   //

   png_init_io(png_ptr, fp);

   // indicate how much of the file has been read:
   png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

   // read information from the file before the first image data chunk
   png_read_info(png_ptr, info_ptr);

   // extract _width, _height, and other info from header:
   int bit_depth, color_type, interlace_type;
   png_get_IHDR(png_ptr, info_ptr,
                (unsigned long*)&_width,
                (unsigned long*)&_height,
                &bit_depth,
                &color_type,
                &interlace_type,
                NULL, NULL);

   // tell libpng to strip 16 bit/color files down to 8 bits/channel
   png_set_strip_16(png_ptr);

   // Extract multiple pixels with bit depths of 1, 2, and 4 from a single
   // byte into separate bytes (useful for paletted and grayscale images).
   png_set_packing(png_ptr);

   // Expand paletted colors into true RGB triplets
   if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);

   // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
   if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand(png_ptr);

   // Expand paletted or RGB images with transparency to full alpha channels
   // so the data will be available as RGBA quartets.
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_expand(png_ptr);

   // update the header to reflect transformations applied:
   png_read_update_info(png_ptr, info_ptr);

   // now it's safe to read the size of a row:
   unsigned long row_bytes = png_get_rowbytes(png_ptr, info_ptr);
   _bpp = row_bytes / _width;

   // make sure bytes per pixel is a valid number:
   if (_bpp < 1 || _bpp > 4) {
      fprintf(stderr,"Image::read_png: %d bytes/pixel not supported", _bpp);
   } else if (interlace_type != PNG_INTERLACE_NONE) {
      fprintf(stderr,"Image::read_png: unsupported interlace type (%d)",
              interlace_type);
   } else if ((_data = new uchar [ size() ]) == 0) {
      fprintf(stderr,"Image::read_png: can't allocate data");
   } else {
      _no_delete = 0;

      // no more excuses: read the image (inverted vertically):
      for (int y=_height-1; y>=0; y--)
         png_read_row(png_ptr, _data + y*row_bytes, 0);

      // read rest of file, and get additional
      // chunks in info_ptr - REQUIRED
      png_read_end(png_ptr, info_ptr);
   }

   // clean up after the read, and free any memory allocated - REQUIRED
   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

   // close the file
   fclose(fp);

   // return pixel data:
   if (_data)
      return 1;
   else {
      clear();
      return 0;
   }
}
static
inline
void
eat_whitespace(istream& in)
{
   while (isspace(in.peek())) {
      in.get();
   }
}

/***********************************************************************
 * Method : Image::copy
 * Params :
 * Returns: uchar* (copy of image data)
 * Effects: allocates an array, copies image data into it
 ***********************************************************************/
uchar*
Image::copy()
{
   if (empty())
      return 0;

   uchar* ret = new uchar [ size() ];

   if (ret)     memcpy(ret, _data, size());
   else         fprintf(stderr,"Image::copy: can't allocate data");

   return ret;
}


int 
Image::copy_tile(const Image& tile, uint i, uint j)
{
   // scenario:
   //   this is a big image.
   //   tile is small enough that lots of copies of 
   //   it could be arranged to "tile" into this one.
   //   we'll copy data from tile into a slot of this image. 
   //   the slot is over i and up j copies of the tile image.
   //
   // how come why:
   //   used to perform hi-res screen grabs.
   //   the screen is rendered in separate tiles which
   //   are then arranged into a single hi-res image.

   // precondition: both images have the same bytes
   //   per pixel, and the designated slot fits in
   //   this image.

   if (tile._width*(i+1) > _width ||
       tile._height*(j+1) > _height ||
       tile._bpp != _bpp) {
      fprintf(stderr,"Image::copy_tile: bad size in tiled image");
      return 0;
   }

   // copy each row into correct slot in this image:
   int row_offset = tile.row_size()*i;
   for (uint r=0; r<tile._height; r++) {
      memcpy(row(tile._height*j + r) + row_offset,      // destination
             tile.row(r),                               // source
             tile.row_size());                          // number of bytes
   }

   return 1;
}

/***********************************************************************
 * Method : Image::write_png
 * Params : char *file_name
 * Returns: int (success/failure)
 * Effects: opens file, writes data to disk
 ***********************************************************************/

int
Image::write_png(char *file) const
{
   
   if (_width == 0 || _height == 0 || _data == 0) {
      fprintf(stderr,"Image::write_png: image has no data");
      return 0;
   } else if (_bpp < 1 || _bpp > 4) {
      fprintf(stderr,"Image::write_png: unsupported number of bytes/pixel (%d)",
              _bpp);
      return 0;
   }

   FILE* fp;
   if ((fp = fopen(file, "wb")) == 0) {
      fprintf(stderr,"Image::write_png: can't open file %s", file);
      return 0;
   }

   // Create and initialize the png_struct with the desired error handler
   // functions.  If you want to use the default stderr and longjump method,
   // you can supply NULL for the last three parameters.  We also check that
   // the library version is compatible with the one used at compile time,
   // in case we are using dynamically linked libraries.  REQUIRED.
   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
   if (!png_ptr) {
      fclose(fp);
      fprintf(stderr,"Image::write_png: png_create_write_struct() failed");
      return 0;
   }

   // Allocate/initialize the image information data.  REQUIRED
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      fprintf(stderr,"Image::write_png: png_create_info_struct() failed");
      return 0;
   }

   // Set error handling
   if (setjmp(png_ptr->jmpbuf)) {
      // jump here from error encountered inside PNG code...
      // free all memory associated with the png_ptr and info_ptr
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      fprintf(stderr,"Image::write_png: error writing file %s", file);
      return 0;
   }

   // Set up the input control (using standard C streams)
   //
   //   see note re: C streams in read_png() above
   //
   png_init_io(png_ptr, fp);

   // set the image information:
   png_set_IHDR(png_ptr,
                info_ptr,
                _width,
                _height,
                8,                              // bit depth
                ((_bpp==4) ? PNG_COLOR_TYPE_RGB_ALPHA :
                 (_bpp==3) ? PNG_COLOR_TYPE_RGB :
                 (_bpp==2) ? PNG_COLOR_TYPE_GRAY_ALPHA :
                 PNG_COLOR_TYPE_GRAY),
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE,
                PNG_FILTER_TYPE_BASE);

   // set gamma
   double gamma = getenv("JOT_GAMMA") ? atof(getenv("JOT_GAMMA")) : 0.45;
   png_set_gAMA(png_ptr, info_ptr, gamma);
                
   // write the file header information.  REQUIRED
   png_write_info(png_ptr, info_ptr);

   // write the image data (inverted vertically):
   for (int y=_height-1; y>=0; y--)
      png_write_row(png_ptr, row(y));

   // It is REQUIRED to call this to finish writing
   png_write_end(png_ptr, info_ptr);

   // clean up after the write, and free any memory allocated
   png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

   // close the file
   fclose(fp);

   return 1;
}
