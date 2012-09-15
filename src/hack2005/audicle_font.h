/*----------------------------------------------------------------------------
    Audicle Context-sensitive, On-the-fly Audio Programming Environment
      for the ChucK Programming Language

    Copyright (c) 2005 Ge Wang, Perry R. Cook, Ananya Misra, Philip Davidson.
      All rights reserved.
      http://audicle.cs.princeton.edu/
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
    U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// name: audicle_font.h
// desc: audicle font support
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: July 2005
//-----------------------------------------------------------------------------

#ifndef __AUDICLE_FONT_H_INCLUDED__
#define __AUDICLE_FONT_H_INCLUDED__

class AudicleFont { //Audicle Font rendering interface 

public:

    static std::vector < std::string > & available_fonts();
    static AudicleFont *& monofont();
    static AudicleFont *& varfont();
    static AudicleFont * loadFont( const char * );
    static void releaseFont ( AudicleFont * f );

protected:

    static std::vector < std::string > m_available_fonts;
    static std::vector < AudicleFont* > m_font_pool;
    static AudicleFont* m_monofont;
    static AudicleFont* m_varfont; 

protected: 

    double m_height;
    double m_line_height;
    double m_height_unit_scale;
    double m_line_unit_scale;
    double m_mono_width; 
    std::string m_name;
    int m_ref;

protected:

    static AudicleFont * check_pool ( const char * name ) ;    
    virtual double kerning ( char first, char last ) { return 0; }
    
public:
    AudicleFont();
    virtual ~AudicleFont() {}
    virtual void   draw         ( const std::string & str ) {} 
    void   draw_centered        ( const std::string & str );
    char   draw_sub             ( const std::string & str, char last = 0 );
    virtual double length       ( const std::string & str ) { return 0; }
    void   scale        ( double h, double aspect = 1.0 );
    virtual bool isNamed        ( const char * name ) { return false; }
    double line_height();
    double height();

};

extern AudicleFont * monoFont;
extern AudicleFont * varFont; 
extern AudicleFont * bufferFont;

void setupFonts();
void drawString ( const std::string & str );
void drawString_mono ( const std::string & str ) ;
double drawString_length ( const std::string & str ) ;
double drawString_length_mono ( const std::string & str ) ;
double drawString_line_height ( ) ;
double drawString_line_height_mono ( ) ;
double drawString_height ( ) ;
double drawString_height_mono ( ) ;
void drawString_centered ( const std::string & str ) ;
void drawString_centered_mono ( const std::string & str ) ;
void scaleFont ( double h, double aspect = 1.0 );
void scaleFont_mono ( double h, double aspect = 1.0 );

#endif 
