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
// name: coaudicle_view.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle_net_session.h"
#include "coaudicle_chat.h"
#include "coaudicle_view.h"
#include "coaudicle_ui_coding.h"

CoSessionRendererImp::CoSessionRendererImp () : 
    m_session(NULL),
    m_chat_render(NULL),
    m_chat(NULL),
    m_camera(NULL),
    m_usr_avatar(NULL)
{
    EM_log( CK_LOG_FINER, "(audicle) initializing renderer..." );
    EM_pushlog();
    m_camera = new CoCamera();

    m_chat = new CoChatImp();
    EM_log( CK_LOG_FINER, "(audicle) initializing chat renderer..." );
    m_chat_render = new CoChatRenderer();
    m_chat_render->set_chat ( m_chat );
    EM_log( CK_LOG_FINER, "(audicle) initializing dude..." );
    m_usr_avatar = new CoAvatar();

    dtime = 0;
    last = AudicleWindow::main()->get_current_time();
    EM_log( CK_LOG_FINER, "(audicle) SessionRenderImp: finished " );
    EM_poplog();
}

void
CoSessionRendererImp::render_pre() { 

    static t_CKUINT pflag = GL_LIGHTING_BIT \
        | GL_COLOR_BUFFER_BIT \
        | GL_DEPTH_BUFFER_BIT \
        | GL_POINT_BIT \
        | GL_LINE_BIT \
        | GL_LIGHTING_BIT ;
    

    glPushAttrib( pflag );

    bool m_lighting = true; 

    if ( m_lighting) { 
        float width = 100.0;
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float ambient[4] = {0.2f, 0.2f, 0.2f, 0.2f};
        float diffuse[4] = {0.9f, 0.9f, 0.9f, 0.2f};
        float specular[4] = {0.2f, 0.2f, 0.2f, 0.1f};
        float position[4] = {40, 80, 200.0 , 0.0f};
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
    }

    glEnable(GL_NORMALIZE);

}

void
CoSessionRendererImp::render_post() { 
    glPopAttrib();
}

void
CoSessionRendererImp::render() { 
    render_pre();
    if ( m_session ) render_session();
    else render_default();
    render_post();
}

void 
CoSessionRendererImp::set_session( CoSession * session ) { 
    // DO SOMETHING 
    m_session = session;
    m_chat_render->set_chat( session->server()->chat() );
    
}

void
CoSessionRendererImp::render_session() { 
    //we only get here if we have a session.  
    assert ( m_session->id() != 0 );
    glPushName( m_session->id() );

    //the lair of el cidua!

    dtime = AudicleWindow::main()->get_current_time() - last;
    last += dtime;
    
    // walkin' doin' his thing.
    m_usr_avatar->animate( dtime );

    static float yy= 0;
    yy += ( 360.0 / 16.0 ) * dtime;
    
    //demo-ish turntable
    std::vector < CoSession * > users = m_session->server()->sessions();    
    EM_log( CK_LOG_FINEST, "(audicle) rendering - %d user(s)", users.size() );
    //text ( window-oriented );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();  //session title
        glTranslated ( 0, 0.4 , 0);
        scaleFont( 0.12, 1.0 );
        drawString_centered("bienvenidos a elCidua");
    glPopMatrix();   //session title

    ClientUser * u= (ClientUser *)m_session->user();
    Agent * a = u->agent();
    
    m_usr_avatar->push_agent( a );
    // m_camera->follow( m_usr_avatar );

    glMatrixMode (GL_PROJECTION);
    glPushMatrix(); //stage projection

    m_camera->setProjection();
    
    glMatrixMode (GL_MODELVIEW);
    
    glPushMatrix(); //camera view       
    glLoadIdentity();
    m_camera->setModelView();
    
    m_usr_avatar->pop_agent();

    //render_unit_axes();

    glPushMatrix(); //turntable
    // glRotated( yy, 0, 1, 0 );
    
    glEnable(GL_LIGHTING);
    
    render_room();
    
    for ( int i =0 ; i < users.size(); i++) { 
        render_user( users[i] );
    }

    glPopMatrix(); //turntable
        
    glPopMatrix(); //camera view

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix(); //stage projection


    glPopName();
}

void
CoSessionRendererImp::render_room() { 

    render_chat();
    
    render_shreds();
    
    render_resources();
    
    render_control();
    
}

//global
void 
CoSessionRendererImp::render_chat() {
    glPushMatrix();
    m_chat_render->render();
    glPopMatrix();      
}

void 
CoSessionRendererImp::render_shreds(){
    
}

void 
CoSessionRendererImp::render_resources(){
    
}

void 
CoSessionRendererImp::render_control(){
    
}


void 
CoSessionRendererImp::render_user( CoSession *s ) { 

    ClientUser * u= (ClientUser *)s->user();
    Agent * a = u->agent();

    m_usr_avatar->push_agent( a );

    m_usr_avatar->animate_state( dtime );
    EM_log( CK_LOG_FINEST, "(audicle) rendering user: %d", s->id() );
    glPushMatrix(); //center user
                    //center him for now...
    Point3D apos = m_usr_avatar->pos(); 
    glTranslated ( -apos[0], -apos[1], -apos[2] ) ;
        
    // render using skin
    render_character( s );

    render_windows( s );
    
    render_bindle( s );

    render_halo ( s );

    glPopMatrix();

    render_portal( s );

    m_usr_avatar->pop_agent();
    
    
}

//per user(session)
void 
CoSessionRendererImp::render_character(CoSession* s){
    glPushMatrix(); //user frame    
    m_usr_avatar->render(); // this needs to be a customized avatar on the top
    glPopMatrix(); //user frame 
}

void 
CoSessionRendererImp::render_windows(CoSession* s){
    glPushMatrix(); //window frame 
    
    m_usr_avatar->apply_window_transform();
    
    
    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT);
    
    glDisable(GL_LIGHTING); //no lighting on our windows
    
    window_render_mode defmode = WindowManager::getRenderMode();
    
    glPolygonOffset( 1.0, 1.0 );
    glEnable(GL_POLYGON_OFFSET_FILL);
    
    WindowManager::setRenderMode( WINDOW_RENDER_DEPTH );
    s->wm()->draw();
    WindowManager::setRenderMode( defmode );
    s->wm()->drawCursor();
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glPopAttrib();
    
    
    glPopMatrix(); //window frame;  
}

void
CoSessionRendererImp::render_halo( CoSession *s ) { 
    glPushAttrib( GL_LIGHTING_BIT | GL_POLYGON_BIT );
    glDisable(GL_LIGHTING);

    m_usr_avatar->render_halo();

    glPopAttrib();
}

void 
CoSessionRendererImp::render_portal(CoSession* s){
    //the door they come from, on entry
    glPushAttrib( GL_LIGHTING_BIT | GL_POLYGON_BIT );
    
    glDisable(GL_LIGHTING);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset( 1.0, 1.0 );
    
    glPushMatrix();

    static UIButton b;
    b.setCols( UI_BASE, Color4D( 0.8, 1, 0.95 ), Color4D( 0.3,0.3,0.3 ) );
    if ( s ) b.setLabel( s->user()->host() );
    else b.setLabel( "servername" );
    b.fitLabel();

    static UIRect door;
    door.setw( 4 );
    door.seth( 9 ); 
    
    
    /*
    glColor3d(0,0,0);
    glBegin( GL_LINES);
    glVertex3dv( m_usr_avatar->home().data() );
    glVertex3dv( m_usr_avatar->pos().data() );
    glEnd();
     
     
     glColor3d(0,0,0);  
     glBegin(GL_QUADS);
     glVertex2d( -2, 0 );
     glVertex2d( -2, 9 );
     glVertex2d(  2, 9 );
     glVertex2d(  2, 0 );
     glEnd();
     
    */
    
    Point3D home;
        home = m_usr_avatar->home();
    glTranslated( home[0], home[1], home[2] - 20  );
    
    glScaled ( 1.3, 1.3, 1.3 );

    
    door.setCols( UI_BASE, Color4D( 0.3,0.3,0.3 ), Color4D( 0,0,0 ) );
    glPushMatrix();
    glTranslated( -0.5 * door.w(), door.h() , 0 );
    door.filledTab(true, 0.5);
    glPopMatrix();
    

    glPushMatrix();
    glTranslated( -b.w() * 0.5 , door.h() + b.h() + 1.0 , 0 );
    b.drawRounded();
    glPopMatrix();

    
    glPushMatrix();
    
    glTranslated ( 2, 0 , 0 );
    glRotated ( 40, 0 ,1, 0 );
    glTranslated ( -2, 0, 0 );
    

    door.setCols( UI_BASE, Color4D( 0.75, 1, 0.90 ), Color4D( 0.3,0.3,0.3 ));
    glPushMatrix();
    glTranslated( -0.5 * door.w(), door.h() , 0 );
    door.filledTab(true, 0.5);
    door.outlineTab(true, 0.5 );
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
    glPopAttrib();
}

void 
CoSessionRendererImp::render_bindle(CoSession* s) {
    //right now it's in-character..might stay there
}


void
CoSessionRendererImp::render_default() { 
    //the lair of el cidua!

    dtime = AudicleWindow::main()->get_current_time() - last;
    last += dtime;

    // walkin' doin' his thing.
    // over all avatars
    m_usr_avatar->animate( dtime );

    //text ( window-oriented );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();  //session title
        glTranslated ( 0, 0.4 , 0);
        scaleFont( 0.12, 1.0 );
        drawString_centered("introducing elCidua");
    glPopMatrix();   //session title


    glMatrixMode (GL_PROJECTION);
    glPushMatrix(); //stage projection

    m_camera->setProjection();

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix(); //camera view    

    glLoadIdentity();
    m_camera->setModelView();

    // modelview for isometrix
    /*
    // simple rotation of the model
    static int xx= 0;
    xx += 5;
    //semi-isometric world view ( this should use more of a camera model ) 

    glScaled( 0.09, 0.09, 0.09 );
    glTranslated ( 0, -7, 0 );

    glRotated ( 20, 1, 0, 0 );
    */

    //demo table

    glPushMatrix(); //turntable

    static float yy= 0;
    yy += ( 360.0 / 16.0 ) * dtime;
    //glRotated ( yy, 0, 1, 0 );

    glPushMatrix(); //center user
    //center him for now...
    Point3D apos = m_usr_avatar->pos(); 
    glTranslated ( -apos[0], -apos[1], -apos[2] ) ;

    // render using skin
    glEnable(GL_LIGHTING);
    glPushMatrix(); //user frame    
    m_usr_avatar->render();
    glPopMatrix(); //user frame

    glPushMatrix(); //window frame 
    m_usr_avatar->apply_window_transform();
    m_usr_avatar->render_windowmanager();
    glPopMatrix(); //window frame;

    glPopMatrix(); //center user

    glPushMatrix();
    m_chat_render->render();
    glPopMatrix();

    glPopMatrix(); //turntable
    
    //render the chat
    
    glPopMatrix(); //camera view

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix(); //stage projection
}

void
CoSessionRendererImp::handleEvent ( const InputEvent &e ) {
    m_session->wm()->handleEvent(e);
}

void 
CoSessionRendererImp::handleKey ( const InputEvent &e ) {
    m_session->wm()->handleKey(e);
}

void 
CoSessionRendererImp::handleSpec( const InputEvent &e ) {
    m_session->wm()->handleSpec(e);
}

void 
CoSessionRendererImp::handleMouse( const InputEvent &e ) {
    m_session->wm()->handleMouse(e);
}

void 
CoSessionRendererImp::handleMotion( const InputEvent &e ) { 
    m_session->wm()->handleMotion(e);
}



CoCamera::CoCamera() { 
    //projection
    angle = 30.0;
//    aspect = 1.2;  //guess for now  
    aspect = 1.0;
    //this is already handled by the window transform
    near_limit = 1.0;
    far_limit = 500.0;
    
    //world scale
    m_scale = 1.0 ;

    //lookat
    pos = Point3D (0,23,-42);
    at  = Point3D (0, 9, 0);
    up  = Point3D (0, 1, 0);

    following_dist = 30.0;
    m_following = NULL;
}

void 
CoCamera::setProjection() { 
    gluPerspective ( angle, aspect, near_limit, far_limit );
}

void
CoCamera::follow( CoAvatar * co ) { m_following = co; }
 
void
CoCamera::setModelView() { 
    if ( m_following == NULL ) { 
        gluLookAt( pos[0], pos[1], pos[2], 
                   at[0],  at[1],  at[2], 
                   up[0],  up[1],  up[2] );
    }
    else { 

        double base_height = 15.0;
    double following_dist = -30.0;
    double char_height_pt = 7.0;
    /*
        Point3D follow_pt = m_following->pos() + Vec3D ( 0, base_height, 0 ) ;
        Point3D at_pt = follow_pt; 
        //following_hang is offset angle for direction ( from behind char );
        //following_vang is offset angle vertically...
        Vec3D vel = m_following->vel(); 
        double hang = EAT_PI + atan2( vel[2], vel[0] ) + following_hang ;
        double xzdist  = following_dist * cos ( following_vang );
        double dx = xzdist * cos ( hang );
        double dz = xzdist * sin ( hang );
        double dy = following_dist * sin ( following_vang );
        follow_pt += Point3D( dx, dy, dz);
    */
    Point3D follow_pt = m_following->pos() + Vec3D ( 0, char_height_pt + base_height, following_dist );
    at = m_following->pos() + Vec3D( 0 , char_height_pt , 0 );

        gluLookAt( follow_pt[0], follow_pt[1], follow_pt[2],
                   at[0], at[1], at[2], 
                   up[0], up[1], up[2] );
    }
    glScaled( m_scale, m_scale, m_scale );
}

ChatContent::ChatContent() { 
    m_chat = NULL;
    m_chat_last_size = 0;
    _prompt = "message";
    _fontColor = Color4D( 1.0, 1.0, 1.0, 1.0 );
}

void
ChatContent::setChat( CoChat * chat ) { 
    m_chat = chat;
    m_chat_last_size = 0;
}

CoChat * 
ChatContent::chat() { 
    return m_chat;
}

void
ChatContent::processLine() { 

    CoChatMesg * mesg = new CoChatMesgImp("me", _entry.str() );
    
    //temporary
    if ( m_chat )
        m_chat->send_mesg( mesg );
    else 
        fprintf (stderr, "error chatcontent -- chat is unassigned\n" );
    
    //temporary 
//    _output->addTextLine( _entry );
    
    _entry = TextLine();
    _loc.chr = 0;
    
}

const char * 
ChatContent::title()   { return "server chat log"; }

const char * 
ChatContent::posMesg()   { return ""; }

const char * 
ChatContent::mesg()   { return ""; }

void
ChatContent::sync_chat() { 

    if ( !m_chat || m_chat->messages().size() == 0 ) return; 
    
    // m_chat_last_id should be the id of the last chat message
    if ( m_chat_last_size != m_chat->messages().size() ) { 
        int n = m_chat->messages().size();
        int i = m_chat_last_size;
        //find the message that has correct id
        for ( ; i < n ; i++ ) { 
            TextLine newline( m_chat->message(i)->user() + ": " + m_chat->message(i)->data()  );
            _output->addTextLine ( newline );
        }
        m_chat_last_size = n;
    }
}

void
ChatContent::drawPromptLine( TextLine & s ) { 
    //switch to user's color.. or as background? 
    bufferFont->draw_sub( s.str() );
}

void
ChatWindow::drawUILayer() { 
    // nothin!
}

void
ChatWindow::drawScrollBars() { 
    //and resize button!
    glLineWidth ( 1 );
    //vscroll
    _vScrollBar.setRange (  _content->getY(), _content->getYSpan(), _content->getYRange() );
    _vScrollBar.draw();
        
}
ChatWindow::ChatWindow() { 
    setContent( new ChatContent() );
    initShapes();
    _id = IDManager::instance()->getPickID();
}

void
ChatWindow::set_chat( CoChat * chat ) { 
    ChatContent * cc = (ChatContent * )_content; 
    EM_log( CK_LOG_FINER, "(audicle) set chat %x %x", (t_CKUINT)_content, (t_CKUINT ) chat );
    cc->setChat( chat );
}

void
ChatWindow::initShapes() { 

    DisplayWindow::initShapes();

    Color4D border (0.5, 0.5, 0.5, 1.0  );

    _baseWindow.setCols   ( UI_BASE, Color4D (0.7, 0.7, 0.7, 1.0), border ); 
    _contentWindow.setCols( UI_BASE, Color4D (0.2, 0.4, 0.2, 0.9 ), border ); 

}

void 
ChatWindow::clean() { 
    if ( _content ) { 
        _titleBox.setLabel( (char*)_content->title() );
        ((ChatContent*)_content)->sync_chat();
    }
}

void
ChatWindow::reshape() { 

    DisplayWindow::reshape();

    UIRect &bw = _baseWindow;
    
    //these are all relative to basewindow coordinates! 
    double pad = _marginSize;
    double pad2 = 2.0 * _marginSize;

    double cspan =  max ( 1.5 * bw.h() - pad2, min ( 1.2, 0.4 * bw.w() ) );

    _contentWindow.moveto ( bw.left() + pad2 , bw.top() - ( pad2 ) );
    _contentWindow.resize ( bw.w() - ( pad2 * 2 ) , bw.h() - (pad2 * 2) );

    if ( _content ) setContentArea();

    _vScrollBar.moveto ( _contentWindow.right(), _contentWindow.top() );
    _vScrollBar.resize ( pad2, _contentWindow.h() );

}

CoChatRenderer::CoChatRenderer() { 
    m_dir = Vec3D( 0, -0.5, -1.0 ).unit();
    m_pos = Point3D( 0.0 , 18.0 , 12.0 );
    m_scale = 8.0;
    window = new ChatWindow();
}

CoChatRenderer::~CoChatRenderer() {}

void
CoChatRenderer::set_chat( CoChat * chat ) 
{   
    window->set_chat( chat );
}

void
CoChatRenderer::move( Point3D pt ) { 
    m_pos = pt;
}

void
CoChatRenderer::resize( double w, double h ) { 
    window->resize( w, h );
}

void
CoChatRenderer::setdir( Vec3D dir ) { 
    m_dir = dir.unit();
}   

void
CoChatRenderer::render()
{ 
    //m_dir should point downward...
    //we'll make our 'forward' down as well, so that it hangs vertical by default. 
    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT); 
    glDisable(GL_LIGHTING); //no lighting on our windows
    
    //m_dir should point downward...
    //we'll make our 'forward' down as well, so that it hangs vertical by default. 
    GLdouble mmtx[16];
    m_dir.fillMatrix( mmtx, Vec3D( 0, 1, 0 ), m_pos, SQUASH_IDENT );
    glMultMatrixd ( mmtx );
    glScaled ( m_scale, m_scale, m_scale ) ;
    glRotated ( 180, 0, 0, 1 );
    glRotated ( 90, 1, 0, 0 );
    
    glPolygonOffset( 1.0, 1.0 );
    glEnable(GL_POLYGON_OFFSET_FILL);
    
    window_render_mode defmode = WindowManager::getRenderMode();
    WindowManager::setRenderMode( WINDOW_RENDER_DEPTH );
    window->drawWindow();
    WindowManager::setRenderMode( defmode );
    
    glPopAttrib();
}


void 
CoChatRenderer::handleEvent ( const InputEvent &e ) {}

void 
CoChatRenderer::handleKey ( const InputEvent &e ) {}

void 
CoChatRenderer::handleSpec( const InputEvent &e ) {}

void 
CoChatRenderer::handleMouse( const InputEvent &e ) {}

void 
CoChatRenderer::handleMotion( const InputEvent &e ) {}


DemoRenderImp::DemoRenderImp() : 
show_army(true),
show_character(true),
show_bindle(false),
show_chat(false),
show_controls(false),
show_portal(false),
show_resources(false),
show_shreds(false),
show_windows(false),
show_halo(false)
{ 
    m_leader = NULL;
}

t_CKBOOL DemoRenderImp::add_agent()
{
    Agent * a = new Agent();
    a->pos = m_leader->pos + Point3D( Rand_in( -30, 30),   0, Rand_in( -30, 30)   );
    return add_agent( a );
}


t_CKBOOL DemoRenderImp::add_agent( Agent * a )
{
    // find
    vector<Agent *>::size_type i;
    for( i = 0; i < agents.size(); i++ )
        if( agents[i] == a )
            break;

    // already there
    if( i < agents.size() ) return FALSE;

    a->calc_feet ( a->pos );
    agents.push_back( a ); 

    return TRUE;
}


t_CKBOOL DemoRenderImp::rem_agent()
{
    if ( agents.size() == 0 ) return FALSE;

    Agent * a;
    rem_agent( a = agents.back() );

    // delete here
    delete a;

    return TRUE;
}


t_CKBOOL DemoRenderImp::rem_agent( Agent * a )
{
    vector<Agent *>::size_type i;

    // find
    for( i = 0; i < agents.size(); i++ )
        if( agents[i] == a )
            break;

    // not in vector
    if( i >= agents.size() ) return FALSE;

    // shift
    for( ++i; i < agents.size(); i++ )
        agents[i-1] = agents[i];

    // pop
    agents.pop_back();

    // TODO delete

    return TRUE;
}


void
DemoRenderImp::do_move( char a ) { 
    switch( a ) { 
    case 'e' :
        m_leader->do_lean_back();
        break;
    case 'r':
        m_leader->do_arms_up();
        break;
    case 't':
        m_leader->do_arm_up();
        break;
    case 'y':
        m_leader->do_head_bang();
        break;
    case 'u':
        m_leader->do_head_bang_once();
        break;
    case 'i' :
        m_leader->do_walk_tall();
        break;
    case ' ' :
        m_leader->do_jump();
        break;
    }
}

void 
DemoRenderImp::copy_move( float var ) { 

    bool lean = ( m_leader->lean_back > 0 );
    bool arms = ( m_leader->arms_up > 0 );
    bool arm = ( m_leader->arm_up > 0 );
    bool headbang = ( m_leader->head_bang > 0 );
    bool jump = (m_leader->jump > 0 );
    bool tall = (m_leader->walk_tall > 0 );

    for ( int i=0 ; i < agents.size(); i++ ) { 
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( lean ) agents[i]->do_lean_back();
            if ( arms ) agents[i]->do_arms_up();
            if ( arm ) agents[i]->do_arm_up();
            if ( headbang ) agents[i]->do_head_bang();
            if ( jump ) agents[i]->do_jump();
            if ( tall ) agents[i]->do_walk_tall();
        }
    }
}


void 
DemoRenderImp::copy_some( float var ) { 

    bool lean = ( m_leader->lean_back > 0 );
    bool arms = ( m_leader->arms_up > 0 );
    bool arm = ( m_leader->arm_up > 0 );
    bool headbang = ( m_leader->head_bang > 0 );
    bool jump = (m_leader->jump > 0 );

    for ( int i=0 ; i < agents.size(); i++ ) { 
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( lean ) agents[i]->do_lean_back();
        }
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( arms ) agents[i]->do_arms_up();
        }
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( arm ) agents[i]->do_arm_up();
        }
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( headbang ) agents[i]->do_head_bang();
        }
        if ( Rand_in( 0.0, 1.0) < var ) { 
            if ( jump ) agents[i]->do_jump();
        }
    }

}


Agent * DemoRenderImp::get_leader()
{
    m_leader = m_usr_avatar->agent();
    return m_leader;
}

void DemoRenderImp::render_session()
{
    dtime = AudicleWindow::main()->get_current_time() - last;
    last += dtime;
    
    m_leader = m_usr_avatar->agent();

    // walkin' doin' his thing.
    // over all avatars
    m_usr_avatar->animate( dtime );
    
    //text ( window-oriented );
    glMatrixMode(GL_MODELVIEW);
/*
    glPushMatrix();  //session title
    glTranslated ( 0, 0.4 , 0);
    scaleFont( 0.12, 1.0 );
    drawString_centered("introducing elCidua");
    glPopMatrix();   //session title
*/
    
    glMatrixMode (GL_PROJECTION);
    glPushMatrix(); //stage projection
    
    m_camera->setProjection();

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix(); //camera view    
    
    glLoadIdentity();
    m_camera->setModelView();
    //render_unit_axes();
    
    // modelview for isometrix
    /*
     // simple rotation of the model
     static int xx= 0;
     xx += 5;
     //semi-isometric world view ( this should use more of a camera model ) 
     
     glScaled( 0.09, 0.09, 0.09 );
     glTranslated ( 0, -7, 0 );
     
     glRotated ( 20, 1, 0, 0 );
     */
    
    //demo table
    
    glPushMatrix(); //turntable
    
    static float yy= 0;
    yy += ( 360.0 / 16.0 ) * dtime;
    glRotated ( yy, 0, 1, 0 );
    
    glPushMatrix(); //center user
                    //center him for now...
    Point3D apos = m_usr_avatar->pos(); 
    glTranslated ( -apos[0], -apos[1], -apos[2] );

    for ( int i = 0; i < agents.size() ; i++ ) { 
      agents[i]->update ( dtime );
      if ( show_army ) { 
          m_usr_avatar->push_agent( agents[i] );
          m_usr_avatar->render_character();
          m_usr_avatar->pop_agent();
      }
    }

    // render using skin

    glEnable(GL_LIGHTING);
    if ( show_character ) { 
      glPushMatrix(); //user frame    
      m_usr_avatar->render_character();
      glPopMatrix(); //user frame
    }

    std::string say = ClientSessionManagerImp::instance()->say();
    if( say.length() > 0 )
    { 
        static UIButton saybox;
        saybox.setCols(UI_BASE, Color4D( 1,1,1,1), Color4D(0,0,0,0) );
        saybox.setLabel( say );
        saybox.fitLabel();
        glPushMatrix();
        Agent * a= m_usr_avatar->agent();
        glMultMatrixd( a->transform);
        glTranslated( 0, 10 + a->jump_height, 0 );
        double sfact = 20.0 / saybox.w();
        glScaled ( sfact, sfact, sfact );

        glPushAttrib( GL_LINE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT );
        glLineWidth(3.0);
        glDisable(GL_LIGHTING);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset( 1.0, 1.0 );

        glTranslated( -saybox.w() * 0.5 , saybox.h() , 0 );
        saybox.drawRounded();
        glPopAttrib();

        glPopMatrix();
    }
    
    if( show_bindle)
    {
        glPushMatrix(); //user frame    
        m_usr_avatar->render_bindle();
        glPopMatrix(); //user frame
    }

    if( show_halo )
    { 
        glPushMatrix();
        m_usr_avatar->render_halo();
        glPopMatrix();
    }
                       
/*
    if( show_windows ) { 
      glPushMatrix(); //window frame 
      //      m_usr_avatar->apply_window_transform();
      render_windows ( m_session );
      //      m_usr_avatar->render_windowmanager();
      glPopMatrix(); //window frame;
    }
*/
    
    if( show_portal ) { 
        render_portal(NULL);
    }
    
    glPopMatrix(); //center user
/*  
    if( show_chat ) { 
        glPushMatrix();
        m_chat_render->render();
        glPopMatrix();
    }
*/
    if( show_shreds ){ 
        render_shreds();
    }
    
    if( show_controls ) { 
        render_control();
    }
    
    
    glPopMatrix(); //turntable
    
    //render the chat
    
    glPopMatrix(); //camera view
    
    glMatrixMode ( GL_PROJECTION );
    glPopMatrix(); //stage projection   
}
