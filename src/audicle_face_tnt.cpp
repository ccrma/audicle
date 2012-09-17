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
// name: audicle_face_tnt.cpp
// desc: interface for audicle face TnT
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_tnt.h"
#include "audicle_gfx.h"
#include "audicle_nexus.h"
#include "audicle.h"
#include "audicle_ui_base.h"
#include "chuck_stats.h"
#include "digiio_rtaudio.h"

#include "audicle_face_shredder.h"
#include "chuck_instr.h"

#include <typeinfo>



//-----------------------------------------------------------------------------
// name: AudicleFaceTnT()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceTnT::AudicleFaceTnT( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceTnT()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceTnT::~AudicleFaceTnT( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceTnT::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_current_shred = NULL;
    m_name = "Time 'n' Timing (TnT)";
    m_max = 0;
    m_max_drift = 0;

    m_rotx = 0;
    m_roty = 0;

    m_item_height  = 0.07;
    m_item_padding = 0.01;
    m_item_spacing = m_item_height + m_item_padding;

    m_active_scale = 1.0;
    m_done_scale = 1.0;

    double swin = 2.0;
    m_srate  = Digitalio::sampling_rate();
    m_window = m_window_target = ( swin * m_srate );
    m_log_scale = true;
    m_wrapmode = 1;
    m_fit_mode = FIT_ACTIVE;
    m_show_mode = SHOW_TIMING_HISTORY;
    
    // more init
    m_init = TRUE;
    m_now = 0;
    m_aspt = 1.0; // set later
    m_earliest = 0;
    m_mheight = 1.0;
    m_drawing_active_set = FALSE;
    m_drawing_scale = FALSE;
    m_active_accum = 0;
    m_done_accum = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceTnT::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


//t_CKBOOL drawing_done_set = false;

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceTnT::render( void * d )
{
    EM_log( CK_LOG_FINEST, "AudicleFaceTNT: rendering objects..." );
    EM_pushlog();

    int i;
    map<Shred_Stat *, Shred_Stat *>::iterator iter;

    Chuck_Stats::instance()->get_shreds( m_shreds, m_done );
    Shred_Stat * stat = NULL;    

    m_now = the()->shreduler()->now_system;
    m_earliest = m_now - m_window; // set a now time for the check_active test

    // log
    EM_log( CK_LOG_FINEST, "num shreds: %d", m_shreds.size() );

    // get activations
    m_drawing_active_set = true; //hack
    for( i = 0; i < m_shreds.size(); i++ )
        fetch_shred_activations ( m_shreds[i] );

    m_drawing_active_set = false; //hack
    for( iter = m_done.begin(); iter != m_done.end(); iter++ )
    {
        stat = (*iter).second;
        fetch_shred_activations ( stat );
    }

    m_now = the()->shreduler()->now_system; 
    m_earliest = m_now - m_window; 

    EM_poplog();

    if ( m_show_mode == SHOW_TIMING_HISTORY )
        return render_timing_history();
    else
        return render_cycle_heatmap ();
        

    return 0;
}





//-----------------------------------------------------------------------------
// name: render_cycle_heatmap()
// desc: ...
//-----------------------------------------------------------------------------

t_CKUINT
AudicleFaceTnT::render_cycle_heatmap()
{ 
    int i;
    map<Shred_Stat *, Shred_Stat *>::iterator iter;
    Shred_Stat * stat;
    t_CKUINT total = 0;

    vector<Shred_Stat *> top;

    for( i = 0; i < m_shreds.size(); i++ )
        if ( m_shreds[i]->parent == 0 ) 
            top.push_back( m_shreds[i]);
  
    for( iter = m_done.begin(); iter != m_done.end(); iter++ )
    {
        stat = (*iter).second;
        if ( stat->parent == 0 ) 
            top.push_back( stat );
    }    

    for ( i = 0; i < top.size(); i++ )
        fetch_group_cycles( top[i] );

    glPushMatrix();
    glTranslated( -m_aspt, 0.98, 0 );
    render_cycle_shred_group( top, 0, top.size(), (float)(m_aspt * 2.0), 1.9f );  
    glPopMatrix();

    return top.size();
}

t_CKUINT
AudicleFaceTnT::render_cycle_shred( Shred_Stat * stat, float w, float h ) { 

    Color4D me;
    if ( stat->state != 3 ) 
        me = stat->data->color.interp( Color4D( 0.7, 1.0, 0.7, 0.7 ), 0.2 );
    else 
        me = stat->data->color.interp( Color4D( 0.5, 0.5, 0.5, 0.2 ), 0.8 );
    
    if ( stat == m_current_shred ) 
        me = me.interp ( Color4D (0.8,1.0,0.8,1.0), 0.5 );

    glPushName(stat->data->name);
    if ( stat->children.size() > 0 ) {  
       glPushMatrix();
       float self = h * (float)stat->cycles / (float)stat->time->group_cycles;
       glColor4dv( stat->data->color.interp( Color4D( 0, 0.8, 0.8, 0.8), 0.2 ).data()  );
       glBegin(GL_QUADS);
       glVertex2f ( 0, 0 );
       glVertex2f ( w, 0 );
       glVertex2f ( w, -self );
       glVertex2f ( 0, -self );
       glEnd();

       glPushMatrix();
        glColor4d ( 0,0,0,0.5 );
        glTranslated( 0.5 * w, -self * 0.5 - 0.025, 0);
        labelFont->scale( 0.05, 1.0 );
        labelFont->draw_centered ( stat->name.c_str() );
//        scaleFont( 0.05, 1.0 );
//        drawString_centered( stat->name.c_str() );
       glPopMatrix();

       glTranslated ( 0, -self, 0 );
       render_cycle_shred_group ( stat->children, 0, stat->children.size(), w, h-self );
       glPopMatrix();
    }

    glColor4dv ( me.data() );
    glBegin(GL_QUADS);
    glVertex2f ( 0, 0 );
    glVertex2f ( w, 0 );
    glVertex2f ( w, -h );
    glVertex2f ( 0, -h );
    glEnd();
    if ( m_current_shred  == stat ) { 
        glPushMatrix();
        int inum = stat->shred_ref->code->num_instr;
        float scale = ( inum == 0 ) ? 1.0 : h / ( 0.05 * (float)inum ); 
        glScaled ( scale , scale, 1.0 );
        for ( int j = 0; j < inum; j++ ) {
            glTranslated ( 0, -0.05, 0 );
            glPushMatrix();
            labelFontMono->scale ( 0.05 );
            labelFontMono->draw ( (char*)( typeid( *(stat->shred_ref->instr[j]) ).name() + 18 ) );
//            scaleFont_mono(0.05, 1.0);
            //plus 18 to skip 'class Chuck_Instr_'
//            drawString_mono( (char*)( typeid( *(stat->shred_ref->instr[j]) ).name() + 18 ) );
            glPopMatrix();
        }
        glPopMatrix();
    }
    if ( stat->state != 3 ) { 
        glColor4dv ( me.interp( Color4D ( 1,1,1,1 ), 0.5 ).data() ) ;
//        fprintf(stderr, "pc : %u of %u ", stat->shred_ref->next_pc, stat->shred_ref->code->num_instr );
        float pc_pos = (float)stat->shred_ref->pc / (float)stat->shred_ref->code->num_instr ; 
        glBegin(GL_LINES);
        glVertex2f ( 0, pc_pos * -h );
        glVertex2f ( w, pc_pos * -h );
        glEnd();
    }

    glColor4d ( 0,0,0,0.5 );
    glBegin(GL_LINE_LOOP);
    glVertex2f ( 0, 0 );
    glVertex2f ( w, 0 );
    glVertex2f ( w, -h );
    glVertex2f ( 0, -h );
    glEnd();

    char buf[256];
    sprintf(buf, "(%ld)", stat->xid );
    glLineWidth( (stat==m_current_shred ) ? 3.0 : 2.0);
    glPushMatrix();
        glTranslated( 0.5 * w, -0.5 * h , 0);
        labelFont->scale ( 0.05 );
        labelFont->draw_centered ( buf );
//        scaleFont( 0.05, 1.0 );
//        drawString_centered( buf );
    glPopMatrix();
    glLineWidth(1.0);
    glPushMatrix();
        glTranslated( 0.5 * w, -0.5 * h - 0.045, 0);
        labelFont->scale ( 0.03 );
        labelFont->draw_centered ( stat->name.c_str() );
//        scaleFont( 0.03, 0.8 );
//        drawString_centered( stat->name.c_str() );
    glPopMatrix();

    glPopName();
    return 1;
}

t_CKUINT
AudicleFaceTnT::render_cycle_shred_group( vector < Shred_Stat* > & shreds, int begin, int num, float w, float h) {
    t_CKUINT total = 0;
    int i ;
    if ( num == 1 ) { 
        render_cycle_shred ( shreds[begin], w, h ); 
        return 1; 
    } 

    for ( i = 0; i < num ; i++ )
        total += shreds[begin + i]->time->group_cycles ;
    
    t_CKUINT half_total  = shreds[begin]->time->group_cycles;
    t_CKUINT middle_size = total / 2;
    i = 0;
    
    while ( i < num-1 && half_total + shreds[(i+1) + begin]->time->group_cycles < middle_size ) { 
        half_total += shreds[++i + begin]->time->group_cycles;
    }

    float div = (float)half_total / (float)total; 
    int next = i+1;
    glPushMatrix();
    
    if ( w < h ) { 
        render_cycle_shred_group ( shreds, begin, next, w, div * h );
        glTranslated ( 0, -div * h , 0);
        render_cycle_shred_group ( shreds, begin+next, num - next, w, h - ( div * h ) );
    }
    else { 
        render_cycle_shred_group ( shreds, begin, next, div * w, h );
        glTranslated ( div * w, 0 ,0  );
        render_cycle_shred_group ( shreds, begin+next, num - next , w - ( div * w ), h );
    }
    glPopMatrix();
    return num;
}

t_CKUINT
AudicleFaceTnT::render_timing_grid() { 

    if ( m_wrapmode != 0 ) return 0 ; 

    glPushMatrix();
    
    double grid_display_alpha = 0.05;

    glTranslated ( -m_aspt + 0.5 , 0, 0 );
    glScaled (  ( m_aspt * 2.0 - 0.5 )  / m_window , 1.0 , 1.0 );

    float logdiv = log ( (double)m_window  * 0.125 / (double)m_srate ) / log(2.0); 
    float logb = floor( logdiv );
    float mdiv = pow ( (double)2, (double)logb ) * m_srate;

    float beg = fmod ( (float)m_now, mdiv ) ;

    float alph = logdiv - logb;
    int c = (int) floor ( ((float)m_now) / mdiv  );
    float f;
    float df;
    glBegin(GL_LINES);
    for ( f = beg; f < m_window; c++, f += mdiv ) { 
        if ( c % 2 == 0 ) glColor4d (0,1,1,grid_display_alpha);
        else glColor4d( 0,1,1, grid_display_alpha * ( 1.0 - alph ) );
        df = ( m_wrapmode == 0 ) ? f : fmod ( (float)m_now - f,  (float)m_window ) ;
        glVertex2d ( df, 0.0 );
        glVertex2d ( df, -2.0 );
    }
    glEnd();
    glPopMatrix();

    return 0;
}

//-----------------------------------------------------------------------------
// name: render_timing_history()
// desc: ...
//-----------------------------------------------------------------------------

t_CKUINT
AudicleFaceTnT::render_timing_history() { 
    EM_log( CK_LOG_FINEST, "AudicleFaceTNT: rendering timing history..." );

    int i;
    map<Shred_Stat *, Shred_Stat *>::iterator iter;
    Shred_Stat * stat;


    glPushMatrix();

    glRotated ( m_rotx, 0, 1.0, 0.0 );
    glRotated ( m_roty, 1.0, 0.0, 0.0 );
    glTranslated ( 0 , 1.0, 0 );

    render_timing_grid();
    
    
    glPushMatrix(); 

    glScaled ( 1.0, m_active_scale, 1.0 ); 
    glTranslated ( 0.0, -0.025, 0.0 );
    

    m_drawing_active_set = true; 
    m_drawing_scale = m_active_scale;
    m_active_accum = 0.0;

    if ( m_fit_mode == FIT_NESTED ) { 
        for ( i = 0; i < m_shreds.size(); i++ ) { 
            render_shred_nested ( m_shreds[i] , 0 );
        }
    }
    else { 
        for( i = 0; i < m_shreds.size(); i++ ) 
            render_shred ( m_shreds[i] );
    }
    glPopMatrix();


    m_done_accum = 0.0;
    m_drawing_active_set = false;

    if ( m_fit_mode == FIT_ALL || m_fit_mode == LIST_ALL ) 
    { 
        glTranslated ( 0,  -( 0.05 + m_active_accum) * m_active_scale , 0 );
        
        glPushMatrix();
        
        glScaled ( 1.0, m_done_scale, 1.0 ); 
        glTranslated ( 0.0, -0.025, 0.0 );
        
        m_drawing_scale = m_done_scale;
        
        for( iter = m_done.begin(); iter != m_done.end(); iter++ )
        {
            stat = (*iter).second;
            render_shred ( stat );
        }        
        glPopMatrix();
    }
       
    glPopMatrix();
            
    recalculate_scaling_factors();

    return 0;
}





//-----------------------------------------------------------------------------
// name: recalculate_scaling_factors()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT
AudicleFaceTnT::recalculate_scaling_factors () { 

    //...okay
    //we want active shred area, with 0.025 borders

    //we want inactive shred area, with 0.025 borders.
    //we worry about active area 1st...
    //if there's room we squeeze
    double actbase = 0.05 + m_active_accum;
    double donebase = 0.05 + m_done_accum;

    double active_maxh, active_minh, done_maxh, done_minh;
    double screen_h = 1.9 ;

    double tmp_active_scale = 1.0;
    double tmp_done_scale = 1.0;

    switch ( m_fit_mode ) { 

    case FIT_NESTED:
    case FIT_ACTIVE:
        active_maxh = screen_h;
        active_minh = min ( screen_h * 0.90 , 2.5 * actbase ) ;
        break;
    case LIST_ALL:
    case LIST_ACTIVE:
        active_minh = 0.05 ;
        active_maxh = actbase;
        break;
    case FIT_ALL:
        active_minh = 0.05;
        active_maxh = screen_h * max ( 0.6, actbase / ( actbase + donebase) );
        break;
    }

    if ( active_minh < actbase &&  actbase < active_maxh )
        tmp_active_scale = 1.0;
    else if ( actbase > active_maxh ) 
        tmp_active_scale = active_maxh / actbase ;
    else if ( actbase < active_minh )
        tmp_active_scale = active_minh / actbase ;
    else { 
        tmp_active_scale = 1.0;
    }

    //calculate spacing for done variables..

    double inactive_h  =  max ( 0.0, ( screen_h - actbase * tmp_active_scale ) ); 

    switch ( m_fit_mode ) { 
    case FIT_NESTED:
    case FIT_ACTIVE:
    case LIST_ACTIVE:
        done_minh = 0.05;
        done_maxh = 0.05;
        break;
    case FIT_ALL:
        done_maxh = inactive_h; // active_max is set to leave some rooom...
        done_minh = min ( inactive_h , donebase * tmp_active_scale );
        break;
    case LIST_ALL:
        done_minh = 0.05;
        done_maxh = donebase;
        break;
    }

    if ( done_minh < donebase &&  donebase < done_maxh) 
        tmp_done_scale = 1.0;
    else if ( donebase > done_maxh ) 
        tmp_done_scale = done_maxh / donebase;
    else if ( donebase < done_minh ) 
        tmp_done_scale = done_minh / donebase;
    else { 
        tmp_done_scale = 1.0;
    }

    if ( m_fit_mode == FIT_NESTED ) { 
        tmp_done_scale = 1.0;
        tmp_active_scale = 1.0;
    }


//    m_active_scale = tmp_active_scale;
//    m_done_scale = tmp_done_scale;
    m_active_scale += 0.06 * ( tmp_active_scale - m_active_scale );
    m_done_scale += 0.06 * (tmp_done_scale - m_done_scale  );
    
//transitions for other values

    m_max_drift += 0.03  * ( (t_CKFLOAT)m_max - m_max_drift );
    if ( m_wrapmode == 0 ) m_window += 0.03 * ( (t_CKDUR) m_window_target - m_window );
    else m_window = m_window_target;
    m_mheight = 1.0 / ( ( m_log_scale ) ? log( m_max_drift ) : m_max_drift );

    return 0;
}



//-----------------------------------------------------------------------------
// name: fetch_group_cycles()
// desc: ...
//-----------------------------------------------------------------------------

t_CKUINT
AudicleFaceTnT::fetch_group_cycles( Shred_Stat * stat) { 
    Shred_Time * t = stat->time;

    if ( m_show_mode == SHOW_CYCLE_HEATMAP_CUMULATIVE ) { 
        t->group_cycles = stat->cycles;
    }
    else if ( m_show_mode == SHOW_CYCLE_HEATMAP_RUNNING ) { 
        t->group_cycles = (t_CKUINT) ( (float)stat->average_cycles * (float)stat->average_ctrl );
    }

    for ( int i = 0 ; i < stat->children.size(); i++ ) { 
        t->group_cycles += fetch_group_cycles( stat->children[i] );
    }

    return t->group_cycles;
}



//-----------------------------------------------------------------------------
// name: fetch_shred_activations()
// desc: ...
//-----------------------------------------------------------------------------

t_CKUINT
AudicleFaceTnT::fetch_shred_activations( Shred_Stat * stat) { 

    Shred_Time * time = stat->time;
    if ( !stat->data ) 
        AudicleFaceShredder::initialize_stat_data ( stat );

    if( !time )
    {
        time = new Shred_Time;
        stat->time = time;
        time->active = false;
        time->group_cycles = 0;
    }
    
    stat->get_activations( m_activations );

    for ( int j = 0 ; j < m_activations.size(); j++ ) 
    {  
        time->history.push_back(m_activations[j]);
        m_max = max ( m_max, m_activations[j].cycles );
    }
    return 0;    
}



t_CKUINT 
AudicleFaceTnT::render_shred_nested(Shred_Stat *stat, int deep  ) { 
    if ( deep == 0 && stat->parent ) return 0; 
    render_shred ( stat );
    glTranslated( 0.05, 0, 0 );
    for ( int i = 0; i < stat->children.size() ; i++ ) { 
        render_shred_nested ( stat->children[i] , deep+1 );
    }
    glTranslated( -0.05, 0, 0 );
    return 0;
}




//-----------------------------------------------------------------------------
// name: render_shred()
// desc: ...
//-----------------------------------------------------------------------------

t_CKUINT
AudicleFaceTnT::render_shred( Shred_Stat * stat) { 

    int j;

    static UIButton nameButton; //plug this into time, potentially

    Shred_Time * time = stat->time;

    check_active_stat( time );

    Color4D scol( 1,1,1,1 );    

    if ( stat->data ) 
        scol = stat->data->color;

    if ( !time->active || !m_drawing_active_set ) 
        scol = scol.interp (Color4D ( 1.0, 1.0, 1.0, 0.2), 0.50);
    else 
        scol = scol.interp ( Color4D ( 1.0, 1.0, 1.0, 1.0 ) , 0.25 );

    static char buf[512];
    sprintf(buf, " (%03ld):", stat->xid );

    double sleading = 1.0;
    double ssize = 1.0;
    if ( time->switching ) { 
        double anim_d = ( m_now - time->switch_time ) / time->switch_span ; 
        if (anim_d > 1.0 ) { 
            time->switching = false;
            anim_d = 1.0;
        }
        if ( time->active ) { 

            sleading = anim_d ;
            ssize = anim_d ;
        }
        else { 
            sleading =  1.0 - anim_d ; 
            ssize    =  1.0 - anim_d ; 
        }
    }

    else if ( !time->active ) { 
        ssize = 0.0;
        sleading = 0.0;
        return 0;
    }


    glTranslated ( 0, -m_item_spacing * sleading , 0.0 );

    if ( m_drawing_active_set ) m_active_accum += m_item_spacing * sleading ;
    else m_done_accum += m_item_spacing * sleading;
    
    glPushMatrix();
    
    glTranslated ( -m_aspt + 0.475 , 0, 0 );

    nameButton.resize( ssize * m_item_height, ssize * m_item_height );
    nameButton.setLabel( stat->name + buf );
    nameButton.fitLabel();
    nameButton.setw ( nameButton.w() * 0.65 );

    nameButton.fillCol( scol );
    nameButton.lineCol( Color4D ( 0.3, 0.3, 0.3, 1.0 ) );
    
    glLineWidth( 2.0 );

    glPushMatrix();


    if ( m_drawing_scale > 1.0 ) glScaled (  1.0, 1.0 / m_drawing_scale , 1.0 );
    if ( nameButton.w() > 0.45 ) glScaled ( 0.45 / nameButton.w(), 1.0, 1.0 );
    nameButton.moveto ( -nameButton.w(), nameButton.h() );
    nameButton.drawRounded( 0.80f );

    glPopMatrix();

    glPopMatrix();


    //view for the shred histories...
    glPushMatrix();
    glTranslated ( -m_aspt + 0.5 , 0, 0 );
    glScaled (  ( m_aspt * 2.0 - 0.5 )  / m_window , ssize * m_item_height *  m_mheight , 1.0 );


    glLineWidth ( 1.0 );
    glBegin( GL_LINES);    

    Color4D linebase ( 0.0, 0.8, 0.0 , ( !time->active || !m_drawing_active_set ) ? 0.20 : 1.0 );
    glColor4dv  ( linebase.data() );

    glVertex2d ( 0.0, 0.0 );
    glVertex2d ( m_window, 0.0 );
  
    glEnd();

    linebase = linebase.interp ( scol, 0.5 );

    double x, y, dt, emph;

    t_CKDUR emph_window = min ( 0.5 * m_srate , 0.125 * m_window ) ;
    Color4D emph_col = Color4D ( 1.0, 1.0, 1.0, 1.0);


    glLineWidth(5.0 );
    glBegin(GL_LINES);

    for ( j = time->history.size() -1  ; j > 0 && time->history[j].when > m_now - emph_window ; j-- ) 
    { 
        dt = m_now - time->history[j].when;
        if ( dt < 0 ) fprintf(stderr, "dt < 0 : %f\n", dt );
        x = ( m_wrapmode == 0 ) ?  dt : fmod ( time->history[j].when, m_window ) ;
        y = ( m_log_scale ) ? log ( (double) time->history[j].cycles ) : time->history[j].cycles; 
        emph = ( dt <  emph_window ) ? 1.0 - ( dt / emph_window )  : 0.0 ;
        glColor4dv  ( linebase.interp( emph_col, emph * 0.75).alpha(emph).data() );
        glVertex2d( x , y  );
        glVertex2d( x , 0  );
  
    }
    glEnd();


    glLineWidth (1.0 );
    glColor4dv  ( linebase.data() );
    glBegin(GL_LINES);
    for ( j = time->history.size() -1  ; j > 0 && time->history[j].when > m_earliest ; j-- ) 
    { 

        x = ( m_wrapmode == 0 ) ?  m_now - time->history[j].when : fmod ( time->history[j].when, m_window ) ;
        y = ( m_log_scale ) ? log ( (double) time->history[j].cycles ) : time->history[j].cycles; 

        glVertex2d( x , y  );
        glVertex2d( x , 0  );

        
    }


    glEnd();

/*
    if ( time->history.size() > 0 && stat->wake_time != 0 ) { 
        glPushMatrix();
        glScaled(   1.0, \
                    ( m_log_scale ) ? log ( (double)time->history.back().cycles ) : time->history.back().cycles , \
                    ( m_log_scale ) ? log ( (double)time->history.back().cycles ) : time->history.back().cycles );
        double sleeping = ( stat->wake_time - time->history.back().when );

        //comment this out - old code 
        if ( sleeping > 0 ) { 

            glPushMatrix();
            glTranslated ( (m_wrapmode == 0) ? 0 : fmod ( m_now, m_window )  , 0 , 0  );
            double rev = 90.0 * ( m_now - time->history.back().when ) / sleeping; 
            glRotated( rev, 1.0, 0.0, 0.0 );        
            glColor4d( 1,0.7,0.7,0.8);
            glLineWidth(2);
            glBegin(GL_LINES);
            glVertex3d(0,0.0,0.0 );
            glVertex3d(0,1.0,0.0 );
            glEnd();
            glPopMatrix();
        }
        //end old

        if ( sleeping > 0 ) { 

            double f_to_time =  ( m_now - time->history.back().when ) / sleeping;  
            double xn = (m_wrapmode == 0) ? 0 : fmod ( m_now, m_window );
            double xw = (m_wrapmode == 0) ? stat->wake_time : fmod ( stat->wake_time , m_window );
            glBegin( GL_LINES );
            glColor4d ( 0.25 + 0.75 * f_to_time, 1.0 , f_to_time, 0.4 );
            if ( m_wrapmode != 0 && xw < xn ) { //we're over the page wrap... 
                glVertex3d( xn , -0.1, 0 );
                glVertex3d( m_window, -0.1 , 0 );
                glVertex3d( 0 , -0.1, 0 );
                glVertex3d( xw, -0.1, 0 );
            }
            else { 
                glVertex3d( xn , -0.1, 0  );
                glVertex3d( xw , -0.1, 0 );
            }
            glEnd();
        }

        glPointSize(8);
        glBegin(GL_POINTS);
        glVertex3d( (m_wrapmode == 0 )? stat->wake_time : fmod ( stat->wake_time , m_window )  ,-0.1,0.0 );
        glEnd();
        glPopMatrix();
    }

*/


    glPopMatrix();
 
    return 0;
}



t_CKUINT
AudicleFaceTnT::check_active_stat ( Shred_Time * time  ) 
{
    if ( time->history.size() == 0 ) return 0; 

    double last_update = m_now - time->history.back().when; 
    double active_window = max ( 0.75 * m_srate , m_window * 1.25 );

    if ( !time->active && last_update <  active_window) 
    { 
        time->active = true;
        time->switching = true;
        time->switch_time = m_now;
        time->switch_span = 0.50 * m_srate;
    }
    else if ( !m_drawing_active_set && time->active && last_update > active_window * 1.25 ) 
    {  //only shreds in done list can be deactivated... ( drawing_active is a hack )
        time->active = false;
        time->switching = true;
        time->switch_time = m_now;
        time->switch_span = 0.75 * m_srate;
    }

    return 0;
}



//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceTnT::render_pre()
{
    // log
    EM_log( CK_LOG_FINEST, "AudicleFaceTNT: pre-render..." );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    AudicleFace::render_pre();

    glPushAttrib( GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
                  | GL_POINT_BIT  | GL_LINE_BIT ); 

    // enable depth
    glDisable( GL_DEPTH_TEST );

    glEnable( GL_BLEND ) ;

    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if ( AudicleWindow::main()->m_antialiased ) { 
        glHint   (GL_POINT_SMOOTH_HINT, GL_NICEST );
        glHint   (GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable (GL_POINT_SMOOTH);
        glEnable (GL_LINE_SMOOTH);
    }
    else { 
        glDisable (GL_POINT_SMOOTH);
        glDisable (GL_LINE_SMOOTH);
    }
}




//-----------------------------------------------------------------------------
// name: render_post()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceTnT::render_post()
{
    EM_log( CK_LOG_FINEST, "AudicleFaceTNT: post-render..." );

    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceTnT::render_view( )
{
    EM_log( CK_LOG_FINEST, "AudicleFaceTNT: rendering view..." );

    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    // load the identity matrix XXX
    // this is handled by AudicleWindow, in order to set up pick matrices...
    // you can assume that LoadIdentity has been called already
    //glLoadIdentity( ); 
    // create the viewing frustum
    AudicleWindow * aW = AudicleWindow::main();    
    m_aspt = aW->m_hsize / aW->m_vsize;

    glOrtho(-m_aspt, m_aspt, -1.0, 1.0 , -10, 10 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point

    gluLookAt( 0.0f, 3.5f * sin( m_eye_y ), 3.5f * cos( m_eye_y ), 
               0.0f, 0.0f, 0.0f, 
               0.0f, ( cos( m_eye_y ) < 0 ? -1.0f : 1.0f ), 0.0f );

    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, m_light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, m_light1_pos );
}




//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceTnT::on_activate()
{
    EM_log( CK_LOG_FINE, "AudicleFaceTNT: on activate..." );
    Chuck_Stats::activations_yes = TRUE;
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceTnT::on_deactivate( t_CKDUR dur )
{
    EM_log( CK_LOG_FINE, "AudicleFaceTNT: on deactivate..." );
    Chuck_Stats::activations_yes = FALSE;
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceTnT::on_event( const AudicleEvent & event )
{
    EM_log( CK_LOG_FINER, "AudicleFaceTNT: on event..." );

    if ( event.type == ae_event_INPUT ) { 

        InputEvent * e = (InputEvent * ) event.data;
        inp.setEvent(*e);
        if ( e->type == ae_input_MOUSE ) { 
            InputEvent sub = *e;
            sub.popStack();
            int i;
            map<Shred_Stat *, Shred_Stat *>::iterator iter;
            Shred_Stat * stat;

            m_current_shred = NULL;

            for( i = 0; i < m_shreds.size(); i++ )
                if ( sub.checkID ( m_shreds[i]->data->name ) )
                    m_current_shred = m_shreds[i];
            for( iter = m_done.begin(); iter != m_done.end(); iter++ )
            {
                stat = (*iter).second;
                if ( sub.checkID ( stat->data->name ) )
                    m_current_shred = stat;
            }    


        }
        if ( e->type == ae_input_MOTION ) { 
            // wtf? we don't need random rotations in here...or do we ? //PLD 
            /*
            if ( inp.isDepressed ) { 
                if ( fabs( inp.dPos[0] ) > fabs( inp.dPos[1] ) )  {
                    m_rotx += inp.dPos[0];
                }   
                else { 
                    m_roty += inp.dPos[1];
                }
            }
            */
        }
        if ( e->type == ae_input_KEY ) { 
            switch ( e->key ) { 
            case 'a':
                m_window_target = 0.5 * m_window_target;
                break;
            case 'd':
                m_window_target = 2.0 * m_window_target;
                break;
            case 's':
                m_log_scale = !m_log_scale;
                break;
            case 'w':
                m_wrapmode = !m_wrapmode;
                break;
            case 'f':
                m_fit_mode = ( m_fit_mode + 1 ) % FIT_NTYPES;
                break;
            case 'h':
                m_show_mode = ( m_show_mode + 1 ) % SHOW_NTYPES;
                break;
            }

        } 

    }

    return AudicleFace::on_event( event );
}
