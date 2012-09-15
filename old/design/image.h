#ifndef __IMAGE_H_INCLUDED__
#define __IMAGE_H_INCLUDED__


#include "png.h"

enum { PNG_BYTES_TO_CHECK = 8 };
enum { AND, NAND, OR, XOR};

//typedef unsigned int uint;
//typedef unsigned char uchar;


class Image 
{ 
protected:

   uint         _width;         // width
   uint         _height;        // height
   uint         _bpp;           // bytes per pixel
   uchar*       _data;          // pixel data
   bool         _no_delete;

   FILE* open_png(char* file);

public:
	
	Image() : _width(0), _height(0), _bpp(0), _data(0), _no_delete(0) {}

   Image(char* file) :
      _width(0),
      _height(0),
      _bpp(0),
      _data(0),
      _no_delete(0) {
      if (file)
         read_png(file);
   }
   Image(uint w, uint h, uint bpp, uchar* data, bool nd=1) {
      set(w,h,bpp,data,nd);
   }
   Image(uint w, uint h, uint bpp) :
      _width(0), _height(0), _bpp(0), _data(0), _no_delete(0) {
      resize(w,h,bpp);
   }

   void set(int w, int h, uint bpp, uchar* data, bool nd=1) {
      if (bpp < 1 || bpp > 4) {
         fprintf(stderr,"Image::Image: %d bytes/pixel not supported", bpp);
      } else {
         clear();
         _width = w;
         _height = h;
         _bpp = bpp;
         _data = data;
         _no_delete = nd;
      }
   }

   void clear() {
      if (!_no_delete)
         delete [] _data;
      _width = _height = _bpp = 0;
      _data  = 0;
      _no_delete = 0;
   }

   int resize(uint w, uint h, uint b) {
      // if current size is different from
      // desired size:
      //   let go current data (if any)
      //   set new width, height and bpp
      //   allocate new data
      //   but don't initialize it
      if (_width != w || _height != h || _bpp != b) {
         _width = w;
         _height = h;
         _bpp = b;
         if (!_no_delete)
            delete [] _data;
         _data = 0;
         _no_delete = 0;
         if ((_data = new uchar [ size() ]) == 0) {
            fprintf(stderr, "Image::resize: can't allocate data");
            return 0;
         }
      }
      return 1;
   }

   int copy_tile(const Image& tile, uint i, uint j);

   virtual ~Image() { clear(); }

   // accessors:
   int       width()    const { return _width; }
   int       height()   const { return _height; }
   uint      bpp()      const { return _bpp; }
   int       size()     const { return _width*_height*_bpp; }
   int       row_size() const { return _width*_bpp; }
   uchar*    data()     const { return _data; }
   uchar*    row(int k) const { return _data + k*row_size(); }
   uchar*    copy();

   bool empty() const { return !(_width && _height && _bpp && _data); }

   void expand_power2();
   int  resize_rows_mult_4();
   
   int  read_png(char* file);
	int  write_png(char* file) const;
   

};

#endif