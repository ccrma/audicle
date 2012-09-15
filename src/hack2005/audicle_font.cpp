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
// name: audicle_font.cpp
// desc: audicle font interface 
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: July 2005
//-----------------------------------------------------------------------------

// FTGL / Freetype library for font rendering 

#include "audicle_gfx.h"
#include "audicle_font.h"

AudicleFont* monoFont;
AudicleFont* varFont;

AudicleFont* AudicleFont::m_varfont = NULL;
AudicleFont* AudicleFont::m_monofont = NULL;

AudicleFont::AudicleFont() : m_ref(0) {}


std::vector < std::string >   AudicleFont::m_available_fonts;
std::vector < AudicleFont * > AudicleFont::m_font_pool;

std::vector < std::string > & 
AudicleFont::available_fonts( ) { return m_available_fonts; }

void
AudicleFont::releaseFont ( AudicleFont * f ) { 

    for ( int i = 0; i < m_font_pool.size() ; i++ ) { 
        if ( f == m_font_pool[i] ) { 
            //sorting doesn't matter...
            f->m_ref--;
            if ( f->m_ref == 0 ) { 
                fprintf ( stderr, "refct empty for font %d, removing from pool\n", i, f->m_ref );
                m_font_pool[i] = m_font_pool[m_font_pool.size()-1];
                m_font_pool.pop_back();
                delete f; 

            }
        }
    }

}

AudicleFont *
AudicleFont::check_pool ( const char * name ) { 
    for ( int i = 0 ; i < m_font_pool.size() ; i++ ) 
        if ( m_font_pool[i]->isNamed ( name ) ) return m_font_pool[i];
    return NULL;
}

AudicleFont *& 
AudicleFont::monofont() { return m_monofont; } 

AudicleFont *& 
AudicleFont::varfont() { return m_varfont; }

void 
AudicleFont::draw_centered ( const std::string & str ) 
{ 
    glTranslated( - 0.5 * length (str), 0 , 0 );
    draw ( str );
}

char
AudicleFont::draw_sub ( const std::string & str, char last ) 
{ 
    if ( str.length() == 0 ) return last; //empty string
    if ( last ) { //add kerning for the last character pair, if supplied
        glTranslated ( kerning ( last, str[0] ), 0, 0 );  
    }
    glPushMatrix();
    draw( str );
    glPopMatrix();
    glTranslated( length (str), 0 , 0 );        
    return *str.end();
}

double  
AudicleFont::height()       { return m_height ; } 

double 
AudicleFont::line_height()  { return m_line_height; } 

void 
AudicleFont::scale ( double h, double aspect ) 
{ 
    glScaled ( h * aspect * m_height_unit_scale, h * m_height_unit_scale , 1 );
}


//OpenGL is supported 
//or else. 

class AudicleOpenGLFont : public AudicleFont { 
protected: 
    bool m_mono;
public :
    AudicleOpenGLFont ( bool mono ) : 
        m_mono( mono )
    {
        m_height = 100.0;
        m_line_height = 133.0;
        m_mono_width = 104.76;
        m_height_unit_scale = 1.0 / m_height ;
        m_line_unit_scale = 1.0 / m_line_height ;
        if ( mono ) m_name = "mono";
        else m_name = "variable";
    }
    
    ~AudicleOpenGLFont() {} ;
    
    void draw ( const std::string & str ) { 
        int n = str.size();
        if ( m_mono ) { 
            for ( int i = 0; i < n ; i++ ) {
                glutStrokeCharacter(  GLUT_STROKE_MONO_ROMAN , str[i] ); 
            }
        } 
        else { 
            for ( int i = 0; i < n ; i++ ) {
                glutStrokeCharacter(  GLUT_STROKE_ROMAN , str[i] ); 
            }
        }
    }
    
    double length ( const std::string & str ) { 
        if ( m_mono )
            return m_mono_width * str.length();
        else 
            return glutStrokeLength( GLUT_STROKE_ROMAN, (const unsigned char* ) str.c_str() );
    }

    bool isNamed ( const char * name ) { 
        if ( strncmp ( name, "OpenGL:", 7 ) == 0 ) 
            if ( m_name == name+7 )
                return true;
        return false;
    }
    
};

void addOpenGLFontList () { 
    AudicleFont::available_fonts().push_back ( "OpenGL:mono");
    AudicleFont::available_fonts().push_back ( "OpenGL:variable");
}

AudicleOpenGLFont * varOpenGLFont = NULL;
AudicleOpenGLFont * monoOpenGLFont = NULL;


#ifdef _USE_FTGL_FONTS_

#ifdef __PLATFORM_WIN32__
//#define FTGL_LIBRARY_STATIC
//use pragma here? 

#endif

#include "FTGLTextureFont.h"

char samplestring[] = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*()-=_+qwertyuiopasdfghjklzxcvbnm[]{}\\|;':\",./<>?";


#ifdef __MACOSX_CORE__ 
char fontpath[] = "../ftgl_lib/fonts/";

#else
#ifdef __PLATFORM_WIN32__
char fontpath[] = "C:\\WINDOWS\\FONTS\\";
#else
char fontpath[] = "/define/this/directory/";

#endif
#endif

char kernedfontfile[] = "FTGL:verdana.TTF";
char monospacedfontfile[] = "FTGL:LUCON.TTF";


class AudicleFTGLFont : public AudicleFont { 

protected :
    FTFont * m_font;
public: 
    AudicleFTGLFont( char * name ) { 
        
        glEnable ( GL_TEXTURE_2D );
        
        char fontlocation[512];
        strncpy ( fontlocation, fontpath, 512 );
        strncat ( fontlocation, name, 512 - strlen ( fontlocation ) );
        
        m_font = new FTGLTextureFont ( fontlocation );
        
        if ( m_font->Error() ) { 
            fprintf(stderr, "AudicleFTGLFont: font load error %d - exiting\n", m_font->Error() );
            exit(1);
        }
        else { 
            
        if ( !m_font->FaceSize(18) ) { 
            fprintf(stderr, "AudicleFTGLFont: font size error  %d - exiting\n", m_font->Error() );
            exit(1);
        }

        m_name = name;
        m_font->Depth(2);
        m_font->CharMap(ft_encoding_unicode);
    
        glDisable ( GL_TEXTURE_2D );
        
        float x1, y1, z1, x2, y2, z2;
        
        m_font->BBox( samplestring , x1, y1, z1, x2, y2, z2);
        m_height = y2;
        m_line_height = m_font->LineHeight();
        
        m_height_unit_scale = 1.0 / m_height ;
        m_line_unit_scale = 1.0 / m_line_height ;
        m_mono_width = m_height; 

        }
    }
    
    ~AudicleFTGLFont() { delete m_font; }
    
    void draw ( const std::string & str ) { 
        glEnable(GL_TEXTURE_2D);
        m_font->Render( str.c_str() );
        glDisable(GL_TEXTURE_2D);
    }
    
    double length ( const std::string & str ) { 
        return m_font->Advance ( str.c_str() ); 
    }

    bool isNamed ( char * name ) { 
        if ( strncmp ( name, "FTGL:", 5 ) == 0 ) 
            if ( m_name == name+5 )
                return true;
        return false;
    }

    double kerning ( char a, char b ) { 
        //silly hack.
        //kerning = advance ( ab ) - ( advance ( a ) + advance ( b ) )
        static char st[7];
        double r; 
        // st = 'ab0a0b0'
        st[2] = st[4] = st[6] = 0;
        st[0] = st[3] = a;
        st[1] = st[5] = b;
        r  = m_font->Advance ( st );
        r -= m_font->Advance ( st+3 );
        r -= m_font->Advance ( st+5 );
        return r;
    }

};


AudicleFTGLFont * varFTGLFont = NULL;
AudicleFTGLFont * monoFTGLFont = NULL;

void addFTGLFontList() {
#ifdef __MACOSX_CORE__
    AudicleFont::available_fonts().push_back( "FTGL:HelveticaNeue.dfont" );
#endif

    // we should do a directory scan here...
    AudicleFont::available_fonts().push_back( "FTGL:ARIAL.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:ARIBLK.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:BYTE.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:COUR.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:LUCON.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:TIMES.TTF" );
    AudicleFont::available_fonts().push_back( "FTGL:verdana.TTF" );

}; 

#endif


//includes for initializing fonts
extern void init_UI_Fonts();
extern void init_Buffer_Font();

void setupFonts() { 
    
    addOpenGLFontList();

#ifdef _USE_FTGL_FONTS_

    addFTGLFontList();
    varFont = AudicleFont::varfont() = AudicleFont::loadFont ( kernedfontfile ); 
    monoFont = AudicleFont::monofont() = AudicleFont::loadFont ( monospacedfontfile );

#else 

    varFont = AudicleFont::varfont() = AudicleFont::loadFont ( "OpenGL:variable" );
    monoFont = AudicleFont::monofont() = AudicleFont::loadFont ( "OpenGL:mono" );

#endif 

    init_UI_Fonts();
    init_Buffer_Font();

}


AudicleFont * 
AudicleFont::loadFont ( const char * name ) { 

    AudicleFont * font = check_pool ( name );

    if ( !font ) { 
        
        if ( strncmp ( name, "OpenGL:", 7 ) == 0 ) { 
            bool mono = ( strcmp ( name+7, "mono" ) == 0 ); 
            font = new AudicleOpenGLFont ( mono );
        }
        
#ifdef _USE_FTGL_FONTS_ 
        else if ( strncmp ( name, "FTGL:", 5 ) == 0 ) { 
            font = new AudicleFTGLFont ( name + 5 );
        }
#endif
        else { 
            fprintf( stderr, "Audicle::loadFont - font not found, using OpenGL mono\n" );
            font = check_pool( "OpenGL:mono" ); 
            if ( !font ) font = new AudicleOpenGLFont ( true );
        }
        
        if ( font ) m_font_pool.push_back( font );
    }
    
    if ( font ) { 
        font->m_ref++;
        return font;
    }
    return NULL;
}



// font rendering 
void drawString( const std::string & str ) { 
    varFont->draw ( str );
}

void drawString_mono( const std::string & str ) { 
    monoFont->draw ( str );
} 


double drawString_line_height ( ) { 
    return varFont->line_height( );
}

double drawString_line_height_mono ( ) { 
    return monoFont->line_height( );
}


double drawString_height ( ) { 
    return varFont->height( );
}

double drawString_height_mono ( ) { 
    return monoFont->height( );
}

double drawString_length ( const std::string & str ) { 
    return varFont->length( str );
}

double drawString_length_mono ( const std::string & str ) { 
    return monoFont->length( str );
}

void drawString_centered ( const std::string & str ) { 
    varFont->draw_centered( str );
}

void drawString_centered_mono ( const std::string & str ) { 
    monoFont->draw_centered( str );
}

void scaleFont(double h, double aspect) { 
    varFont->scale( h, aspect );
}

void scaleFont_mono(double h, double aspect) { 
    monoFont->scale( h, aspect );
}

/* CODE GRAVEYARD


#ifdef _USE_FTGL_FONTS_

#endif

double font_height = 1.0;
double font_line_height = 1.0;
double font_height_unit_scale = 1.0;
double font_line_unit_scale = 1.0;

double font_height_mono = 1.0;
double font_line_height_mono = 1.0;
double font_height_unit_scale_mono = 1.0;
double font_line_unit_scale_mono = 1.0;

double glut_mono_width = 104.76; 
double font_mono_width = 1.0;


void setupFonts() { 

#ifdef _USE_FTGL_FONTS_
    glEnable(GL_TEXTURE_2D);

    if ( !kernedFont  ) { 

        kernedFont = new FTGLTextureFont( kernedfontfile );
        fprintf(stderr, "building kerned font from %s\n", kernedfontfile );

        if ( kernedFont->Error() ) { 
            fprintf(stderr, "font load error %d - exiting\n", kernedFont->Error() );
            exit(1);
        }
        if ( !kernedFont->FaceSize(18) ) { 
            fprintf(stderr, "font size error  %d - exiting\n", kernedFont->Error() );
            exit(1);
        }
        kernedFont->Depth(2);
        kernedFont->CharMap(ft_encoding_unicode);
    }

    if ( !monospacedFont ) { 
        monospacedFont = new FTGLTextureFont( monospacedfontfile );
        fprintf(stderr, "building monospaced font from %s\n", monospacedfontfile );

        if ( monospacedFont->Error() ) {
            fprintf(stderr, "font load error %d - exiting\n", monospacedFont->Error() );
            exit(1);
        }
        if ( !monospacedFont->FaceSize(18) ) { 
            fprintf(stderr, "font size error %d - exiting\n", monospacedFont->Error() );
            exit(1);
        }

        monospacedFont->Depth(2);
        monospacedFont->CharMap(ft_encoding_unicode);

    }

    glDisable(GL_TEXTURE_2D);

    char samplestring[] = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*()-=_+qwertyuiopasdfghjklzxcvbnm[]{}\\|;':\",./<>?";

    float x1, y1, z1, x2, y2, z2;

    kernedFont->BBox( samplestring , x1, y1, z1, x2, y2, z2);
    font_height = y2;
    font_line_height = kernedFont->LineHeight();


    monospacedFont->BBox( samplestring , x1, y1, z1, x2, y2, z2);
    font_height_mono = y2;
    font_line_height_mono = monospacedFont->LineHeight();

#else

    font_height = 100.0;
    font_height_mono = 100.0;

    font_line_unit_scale = 133.0;
    font_line_unit_scale_mono = 133.0;

#endif

    font_height_unit_scale = 1.0 / font_height ;
    font_line_unit_scale = 1.0 / font_line_height ;

    font_height_unit_scale_mono = 1.0 / font_height_mono;
    font_line_unit_scale_mono = 1.0 / font_line_height_mono; 

    fprintf(stderr, "loading fonts - height scale %f/%f mono %f/%f \n", font_height_unit_scale, font_line_unit_scale, font_height_unit_scale_mono, font_line_unit_scale_mono );
}

// font rendering 
void drawString( const std::string & str ) { 

#ifdef _USE_FTGL_FONTS_
    glEnable(GL_TEXTURE_2D);
    kernedFont->Render( str.c_str() );
    glDisable(GL_TEXTURE_2D);
#else
    int n = str.size();
    for ( int i = 0; i < n ; i++ ) { 
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i] );}
#endif
}

void drawString_mono( const std::string & str ) { 
#ifdef _USE_FTGL_FONTS_
    glEnable(GL_TEXTURE_2D);
    monospacedFont->Render( str.c_str() );
    glDisable(GL_TEXTURE_2D);
#else
    int n = str.size();
    for ( int i = 0; i < n ; i++ ) {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, str[i] );
    } 
#endif
} 


double drawString_line_height ( ) { 
    return font_line_height;
}

double drawString_line_height_mono ( ) { 
    return font_line_height_mono;
}


double drawString_height ( ) { 
    return font_height;
}

double drawString_height_mono ( ) { 
    return font_height_mono;
}


double drawString_length ( const std::string & str ) { 
    //this underestimates...
#ifdef _USE_FTGL_FONTS_
    return kernedFont->Advance( str.c_str() );
#else
    return glutStrokeLength( GLUT_STROKE_ROMAN, (const unsigned char* ) str.c_str() );
#endif
}

double drawString_length_mono ( const std::string & str ) { 
    //okay, so the characters are 104.76, but glutStrokeLength returns an 
    //int, and it's not even rounded properly, it's just 104 x blah.. stupid!  
#ifdef _USE_FTGL_FONTS_
    return monospacedFont->Advance ( str.c_str() ); 
#else
    return glut_mono_width * (double)str.size();
#endif
}

void drawString_centered ( const std::string & str ) { 
    glTranslated( -drawString_length(str)*0.5, 0, 0);
    drawString(str);
}

void drawString_centered_mono ( const std::string & str ) { 
    glTranslated( -drawString_length_mono(str)*0.5, 0, 0);
    drawString_mono(str);
}

void scaleFont(double h, double aspect) { 
    glScaled ( h * font_height_unit_scale * aspect, h * font_height_unit_scale , 1 );
}

void scaleFont_mono(double h, double aspect) { 
    glScaled ( h * font_height_unit_scale_mono * aspect, h * font_height_unit_scale_mono , 1 );
}




*/
