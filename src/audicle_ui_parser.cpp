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
// name: audicle_ui_parser.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_ui_editor.h"
#include "audicle_ui_parser.h"
#include "audicle_utils.h"

#include "chuck_absyn.h"

#ifdef __PLATFORM_WIN32__
//'C' specification necessary for windows to link properly
extern "C" a_Program g_program;
extern "C" int yyparse( void );
extern "C" FILE * yyin;
extern "C" int EM_lineNum;
#else
extern a_Program g_program;
extern "C" int yyparse(void);
extern FILE * yyin;
extern int EM_lineNum;
#endif

#ifdef PARSEDEBUG 
#define TREEDEBUG 1
#define DRAWDEBUG 1
#else
#define TREEDEBUG 0 
#define DRAWDEBUG 0
#endif

a_Program
parse_buffer( TextBuffer * t )
{
    bool ret = FALSE;
    FILE * f = NULL;

    // parse
    f = fopen ( "tmp_editor.txt", "w" ) ;
    if ( f ) { 
        t->print(f);
        fclose(f);
    }
    else return NULL; 
    yyin = fopen("tmp_editor.txt", "r" );
    EM_lineNum = 1;
    if (yyin) { 
        ret = (yyparse( ) == 0);
    }
    return ( ret ) ? g_program : NULL;
}


tree_Node_::tree_Node_() { 
    open = true;
    isValue = false;
    firstline = 0;
    lastline = 0;
    boxID = IDManager::instance()->getPickID();
    structID = IDManager::instance()->getPickID();
}
 
tree_Node_::~tree_Node_() { 
    for ( int i = 0; i < (int)subNodes.size() ; i++ ) { 
        if ( subNodes[i] ) delete subNodes[i];
    }
}

void
tree_Node_::draw_tree() { 

    glPushMatrix();


    draw();

    glPushName(structID);

    double top = f_line() - 1.0 ;
    double bottom  = l_line();
    double midpoint = 0.5 * (top + bottom);

    if ( subNodes.size() > 1 ) {

//      glBegin(GL_LINE_STRIP); 
//      glVertex2d( 0, top );
//      glVertex2d(-1, top );
//      glVertex2d(-0.8, midpoint );
//      glVertex2d( 0, bottom );
//      glEnd();

        glBegin(GL_QUADS); 
        glVertex2d(-1.0, top );
        glVertex2d( 0.0, top );
        glVertex2d( 0.0, bottom );
        glVertex2d(-1.0, bottom );
        glEnd();


        glPushName ( boxID ); 
        glPushMatrix();
        
            glTranslated(-1, top , 0 ); 
            glScaled ( 0.3, 0.3, 1 );
            glBegin(GL_QUADS);
            glColor3d ( 1.0 , 0.0 , 0.0 );
            if ( open ) glColor3d ( 0.0 , 1.0, 0.0 );
            glVertex2d ( 1.0 , 1.0 );
            glVertex2d ( -1.0 , 1.0 );
            glVertex2d ( -1.0 , -1.0 );
            glVertex2d ( 1.0 , -1.0 );
            glEnd();
        
        glPopMatrix();
        glPopName();

        glTranslated ( 1.0, 0.0, 0.0 );

    }

    else if ( open &&  subNodes.size() == 1 ) { 
        if ( subNodes[0]->firstline != firstline ||  subNodes[0]->lastline != lastline ) { 
            //glBegin(GL_LINE_STRIP);
            //glVertex2d( 0, top );
            //glVertex2d(-1, top);
            //glVertex2d( 0, bottom );
            //glEnd();
            glBegin(GL_QUADS); 
            glVertex2d(-1.0, top );
            glVertex2d( 0.0, subNodes[0]->f_line() - 1.0 );
            glVertex2d( 0.0, subNodes[0]->l_line());
            glVertex2d(-1.0, bottom );
            glEnd();
            glTranslated ( 1.0, 0.0, 0.0 );
        }
    }

    if ( open ) { 
        for ( int i = 0 ; i < (int)subNodes.size(); i++ ) { 
            switch ( i % 3 ) { 
                case 0 : glColor4d ( 1,0,0,0.2 ); break;
                case 1 : glColor4d ( 0,1,0,0.2 ); break;
                case 2 : glColor4d ( 0,0,1,0.2 ); break;
            }
            if ( subNodes[i] ) { 
                
//                glBegin(GL_LINES);
//                    glVertex2d(-1.0, top);
//                    glVertex2d(0.0, subNodes[i]->f_line() - 1.0);
//                glEnd();

                subNodes[i]->draw_tree();
            }
        }
    }

    glPopName();
    glPopMatrix();
}

int
tree_Node_::checkIDList( t_CKUINT * stack, t_CKUINT n ) { 

    if ( n <= 0 ) return 0;
    if ( stack[0] == structID ) { 
        fprintf(stderr, "stack match %ld\n", stack[0]);
        if ( n > 1 )  { 
            if ( stack[1] == boxID ) { 
                open = !open;
                fprintf(stderr, "flipped open %d\n", open );
                return 1;
            }
            else { 
                int hit = 0;
                for ( t_CKUINT i = 0 ; i < subNodes.size(); i++ ) { 
                    if ( subNodes[i] ) { 
                        hit += subNodes[i]->checkIDList( stack+1, n-1 );
                    }
                }   
                return hit;
            }
        }
        else { 
            fprintf(stderr, "hit my struct %ld!\n", structID);
            return 1;
        }
    }
    return 0;
}

void
tree_Node_::draw_tree_buffer( TextBuffer * b) { 
    draw();
    int last = f_line();
    tree_Node sub = NULL;
    for ( int i = 0 ; i < (int)subNodes.size(); i++ ) { 
//      glPushMatrix();
        switch ( i % 3 ) {         
        case 0 : glColor3d ( 0.5,0,0 ); break;   
        case 1 : glColor3d ( 0,0.5,0 ); break; 
        case 2 : glColor3d ( 0,0,0.5 ); break;
        }
        sub = subNodes[i];         
        if ( sub && (  multiline() || sub->multiline() ) ) { 
            drawTextRange(b, last, sub->f_line() - 1 );
            sub->draw_tree_buffer(b);
            last = sub->l_line() + 1;
        }
//      glPopMatrix();
    }
    drawTextRange(b, last, l_line());
}

void
tree_Node_::drawTextRange(TextBuffer * b, int start, int end ) { 
    for ( int i = start; i <= end; i++ ) { 
//      fprintf(stderr, "drawing %i\n", i);
        glTranslated( 0, -1.0, 0 );
        glPushMatrix();
        scaleFont_mono( 1.0, 0.8 );
        drawString_mono( b->line(i-1).cstr() );
        glPopMatrix();
    }
}

void
tree_Node_::changelines(int pos, int change ) { 
    //using the firstline, lastline notation, modifications
    //to the line array mean we must modify every
    //node which contains or falls below the change location

    //we could later change this to an offset, range notation
    //which would limit modifications to higher level elements
    //but would make the math that much less pleasant

    //oh ,and lines start at 1, to keep with the parser's notation

    if ( change == 0 ) return;

    for ( int i = 0 ; i < (int)subNodes.size(); i++ )
        if ( subNodes[i] ) 
            subNodes[i]->changelines( pos, change );
    
    // change must be + or -
    if ( change > 0 ) { 
        //inserted n lines at pos
        //5, 1 adds one, shifting 5 -> 6
        //5, 3 add three, shifting 5 -> 8
        if ( firstline >= pos ) firstline += change;
        if ( lastline  >= pos ) lastline += change;
    }
    else if ( change < 0 ) { 
        //removed n lines, starting at pos
        // 5, -1 removes line 5.  4 no change, 5 gone, 6 -> 5
        // 5, -3 removes 5,6,7. 4 no change, 5,6,7 out, 8 -> 5
        //this may have invalidated you!
        int nix = - change;
        if ( firstline >= pos ) { 
            if ( firstline >= pos + nix ) firstline -= nix;
            else { 
                firstline = 0; //
            }
        }
        if ( lastline >= pos ) { 
            if ( lastline >= pos + nix ) firstline -= nix;
            else { 
                firstline = 0;
            }
        }
    }
}

void
tree_Node_::draw() { 
        
}

int
tree_Node_::findfirst() {
    firstline = lastline;
    for ( int i =0; i < (int)subNodes.size() ; i++ )
        if ( subNodes[i] ) firstline = min ( firstline, subNodes[i]->findfirst() );
    return firstline;
}

tree_a_File_::tree_a_File_(a_Program arg, int first, int last) { //list container
    root = arg;
    firstline = first;
    lastline = last;
    for ( ; arg != NULL ; arg = arg->next ) { 
        subNodes.push_back ( (tree_Node) new tree_a_Program_(arg) );
    }
}

void
tree_a_File_::draw() { 
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Program_:: class draw call\n"); } 

}

tree_a_Program_::tree_a_Program_(a_Program arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->section) ? new tree_a_Section_( arg->section ) : NULL );
}

void
tree_a_Program_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Program_:: class draw call\n"); } 
}


tree_a_Section_::tree_a_Section_(a_Section arg) { 
    sub = arg;
    lastline = arg->linepos;

    if ( arg->s_type == ae_section_stmt ) 
        subNodes.push_back ( new tree_a_Stmt_List_( arg->stmt_list) );
    else if ( arg->s_type == ae_section_func ) 
        subNodes.push_back ( new tree_a_Func_Def_( arg->func_def) );
    else if ( arg->s_type == ae_section_class ) 
        subNodes.push_back ( new tree_a_Class_Def_( arg->class_def) );

    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Section_ constructed\n"); } 
}

void
tree_a_Section_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Section_:: class draw call\n"); } 
}


tree_a_Stmt_List_::tree_a_Stmt_List_(a_Stmt_List arg) { //container
    sub = arg;  //collapse statement list tree to a multinode
    for (  ; arg ; arg = arg->next ) { 
        subNodes.push_back ( (arg->stmt) ? new tree_a_Stmt_( arg->stmt ) : NULL );
        lastline = arg->linepos;
    }
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_List_ constructed\n"); } 
}


void
tree_a_Stmt_List_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_List_:: class draw call\n"); } 
}


tree_a_Class_Def_::tree_a_Class_Def_(a_Class_Def arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->ext) ? new tree_a_Class_Ext_( arg->ext ) : NULL );
    subNodes.push_back ( (arg->body) ? new tree_a_Class_Body_( arg->body ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Class_Def_ constructed\n"); } 
}


void
tree_a_Class_Def_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Class_Def_:: class draw call\n"); } 
}


tree_a_Func_Def_::tree_a_Func_Def_(a_Func_Def arg) { 
    sub = arg;

    //we don't care about members that are not nodes
    subNodes.push_back ( new tree_a_Type_Decl_(arg->type_decl) );
    subNodes.push_back ( ( arg->arg_list ) ? new tree_a_Arg_List_(arg->arg_list) : NULL );
    subNodes.push_back ( ( arg->code ) ? new tree_a_Stmt_(arg->code) : NULL );
    lastline = arg->linepos;

}

void
tree_a_Func_Def_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Func_Def_:: class draw call\n"); } 
}


tree_a_Stmt_::tree_a_Stmt_(a_Stmt arg) { 
    sub = arg;

    switch ( arg->s_type ) { 
    case ae_stmt_exp:
        subNodes.push_back ( (arg->stmt_exp) ? new tree_a_Exp_( arg->stmt_exp ) : NULL );
        break;
    case ae_stmt_while:
        subNodes.push_back ( (&arg->stmt_while) ? new tree_a_Stmt_While_( &arg->stmt_while ) : NULL );
        break;
    case ae_stmt_until:
        subNodes.push_back( new tree_a_Stmt_Until_( &arg->stmt_until ) );
        break;
    case ae_stmt_for:
        subNodes.push_back( new tree_a_Stmt_For_( &arg->stmt_for ) );
        break;
    case ae_stmt_if:
        subNodes.push_back( new tree_a_Stmt_If_( &arg->stmt_if ) );
        break;
    case ae_stmt_code:
        subNodes.push_back( new tree_a_Stmt_Code_( &arg->stmt_code ) );
        break;
    case ae_stmt_switch:
        subNodes.push_back( new tree_a_Stmt_Switch_( &arg->stmt_switch) );
        break;
//  case ae_stmt_break:
//      subNodes.push_back( new tree_a_Stmt_Break_( arg->stmt_break ) );
//      break;
//  case ae_stmt_continue:
//      subNodes.push_back( new tree_a_Stmt_Continue_( arg->stmt_cont ) );
//      break;
    case ae_stmt_return:
        subNodes.push_back( new tree_a_Stmt_Return_( &arg->stmt_return ) );
        break;
    case ae_stmt_case:
        subNodes.push_back( new tree_a_Stmt_Case_( &arg->stmt_case ) );
        break;
    case ae_stmt_gotolabel:
        subNodes.push_back( new tree_a_Stmt_GotoLabel_( &arg->stmt_gotolabel ) );
        break;
    }
    lastline = arg->linepos;
    fprintf(stderr, "tree_a_Stmt_ constructed!\n");
}

void
tree_a_Stmt_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_:: class draw call\n"); } 
}


tree_a_Exp_::tree_a_Exp_(a_Exp arg) { 
    sub = arg;
    switch (arg->s_type){ 
    case ae_exp_binary:
        subNodes.push_back( new tree_a_Exp_Binary_( &arg->binary) );
        break;
    case ae_exp_unary:
        subNodes.push_back( new tree_a_Exp_Unary_( &arg->unary) );
        break;
    case ae_exp_cast:
        subNodes.push_back( new tree_a_Exp_Cast_( &arg->cast) );
        break;
    case ae_exp_postfix :
        subNodes.push_back( new tree_a_Exp_Postfix_( &arg->postfix) );
        break;
    case ae_exp_dur :
        subNodes.push_back( new tree_a_Exp_Dur_( &arg->dur) );
        break;
    case ae_exp_primary :
        subNodes.push_back( new tree_a_Exp_Primary_( &arg->primary) );
        break;
    case ae_exp_array :
        subNodes.push_back( new tree_a_Exp_Array_( &arg->array) );
        break;
    case ae_exp_func_call :
        subNodes.push_back( new tree_a_Exp_Func_Call_( &arg->func_call) );
        break;
    case ae_exp_dot_member :
        subNodes.push_back( new tree_a_Exp_Dot_Member_( &arg->dot_member) );
        break;
    case ae_exp_if :
        subNodes.push_back( new tree_a_Exp_If_( &arg->exp_if) );
        break;
    case ae_exp_decl :
        subNodes.push_back( new tree_a_Exp_Decl_( &arg->decl) );
        break;
// v2    case ae_exp_namespace :
//        subNodes.push_back( new tree_a_Exp_Namespace_( &arg->name_space ) );
//        break;
    }
    lastline = arg->linepos;
    fprintf(stderr, "tree_a_Exp_ constructed!\n");
}

void
tree_a_Exp_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_:: class draw call\n"); } 
}

tree_a_Exp_Binary_::tree_a_Exp_Binary_(a_Exp_Binary arg) { 
    sub = arg;

    subNodes.push_back ( (arg->lhs) ? new tree_a_Exp_( arg->lhs ) : NULL );
    subNodes.push_back ( (arg->rhs) ? new tree_a_Exp_( arg->rhs ) : NULL );
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Binary_ constructed\n"); } 
}

void
tree_a_Exp_Binary_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Binary_:: class draw call\n"); } 
}


tree_a_Exp_Cast_::tree_a_Exp_Cast_(a_Exp_Cast arg) { 
    sub = arg;
    subNodes.push_back ( (arg->exp) ? new tree_a_Exp_( arg->exp ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Cast_ constructed\n"); } 
    lastline = arg->linepos;
}

void
tree_a_Exp_Cast_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Cast_:: class draw call\n"); } 
}


tree_a_Exp_Unary_::tree_a_Exp_Unary_(a_Exp_Unary arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->exp) ? new tree_a_Exp_( arg->exp ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Unary_ constructed\n"); } 
}

void
tree_a_Exp_Unary_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Unary_:: class draw call\n"); } 
}


tree_a_Exp_Postfix_::tree_a_Exp_Postfix_(a_Exp_Postfix arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->exp) ? new tree_a_Exp_( arg->exp ) : NULL );
//  if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Postfix_ constructed\n"); } 
}

void
tree_a_Exp_Postfix_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Postfix_:: class draw call\n"); } 
}


tree_a_Exp_Primary_::tree_a_Exp_Primary_(a_Exp_Primary arg) { 
    sub = arg;
    lastline = arg->linepos;
//  if ( arg->s_type = ae_primary_exp ) 
//      subNodes.push_back ( (arg->exp) ? new tree_a_Exp_( arg->exp ) : NULL );
//  if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Primary_ constructed\n"); } 
}

void
tree_a_Exp_Primary_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Primary_:: class draw call\n"); } 
}


tree_a_Exp_Dur_::tree_a_Exp_Dur_(a_Exp_Dur arg) { 
    sub = arg;
    subNodes.push_back ( (arg->base) ? new tree_a_Exp_( arg->base ) : NULL );
    subNodes.push_back ( (arg->unit) ? new tree_a_Exp_( arg->unit ) : NULL );
    lastline = arg->linepos;
}

void
tree_a_Exp_Dur_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Dur_:: class draw call\n"); } 
}


tree_a_Exp_Array_::tree_a_Exp_Array_(a_Exp_Array arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->base) ? new tree_a_Exp_( arg->base ) : NULL );
//    subNodes.push_back ( (arg->index) ? new tree_a_Exp_( arg->index ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Array_ constructed\n"); } 
}

void
tree_a_Exp_Array_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Array_:: class draw call\n"); } 
}


tree_a_Exp_Func_Call_::tree_a_Exp_Func_Call_(a_Exp_Func_Call arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->func) ? new tree_a_Exp_( arg->func ) : NULL );
    //or do we make a node with a NULL sub?
    subNodes.push_back( (arg->args ) ? new tree_a_Exp_( arg->args ) : NULL);
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Func_Call_ constructed\n"); } 
}

void
tree_a_Exp_Func_Call_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Func_Call_:: class draw call\n"); } 
}


tree_a_Exp_Dot_Member_::tree_a_Exp_Dot_Member_(a_Exp_Dot_Member arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->base) ? new tree_a_Exp_( arg->base ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Dot_Member_ constructed\n"); } 
}

void
tree_a_Exp_Dot_Member_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Dot_Member_:: class draw call\n"); } 
}


tree_a_Exp_If_::tree_a_Exp_If_(a_Exp_If arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->cond) ? new tree_a_Exp_( arg->cond ) : NULL );
    subNodes.push_back ( (arg->if_exp) ? new tree_a_Exp_( arg->if_exp ) : NULL );
    subNodes.push_back ( (arg->else_exp) ? new tree_a_Exp_( arg->else_exp ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_If_ constructed\n"); } 
}

void
tree_a_Exp_If_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_If_:: class draw call\n"); } 
}


tree_a_Exp_Decl_::tree_a_Exp_Decl_(a_Exp_Decl arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->var_decl_list) ? new tree_a_Var_Decl_List_( arg->var_decl_list ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Decl_ constructed\n"); } 
}

void
tree_a_Exp_Decl_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Decl_:: class draw call\n"); } 
}


// v2 tree_a_Exp_Namespace_::tree_a_Exp_Namespace_(a_Exp_Namespace arg) { 
//    sub = arg;
//    lastline = arg->linepos;
//    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Exp_Namespace_ constructed\n"); } 
//}

// v2 void
//tree_a_Exp_Namespace_::draw() {
//    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Exp_Namespace_:: class draw call\n"); } 
//}


tree_a_Stmt_Code_::tree_a_Stmt_Code_(a_Stmt_Code arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->stmt_list) ? new tree_a_Stmt_List_( arg->stmt_list ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Code_ constructed\n"); } 
}

void
tree_a_Stmt_Code_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Code_:: class draw call\n"); } 
}


tree_a_Stmt_If_::tree_a_Stmt_If_(a_Stmt_If arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->cond) ? new tree_a_Exp_( arg->cond ) : NULL );
    subNodes.push_back( (arg->if_body ) ? new tree_a_Stmt_( arg->if_body ) : NULL );
    subNodes.push_back( (arg->else_body ) ? new tree_a_Stmt_( arg->else_body ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_If_ constructed\n"); } 
}

void
tree_a_Stmt_If_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_If_:: class draw call\n"); } 
}


tree_a_Stmt_While_::tree_a_Stmt_While_(a_Stmt_While arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->cond) ? new tree_a_Exp_( arg->cond ) : NULL );
    subNodes.push_back( (arg->body ) ? new tree_a_Stmt_( arg->body ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_While_ constructed\n"); } 
}

void
tree_a_Stmt_While_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_While_:: class draw call\n"); } 
}


tree_a_Stmt_Until_::tree_a_Stmt_Until_(a_Stmt_Until arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->cond) ? new tree_a_Exp_( arg->cond ) : NULL );
    subNodes.push_back ( (arg->body) ? new tree_a_Stmt_( arg->body ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Until_ constructed\n"); } 
}

void
tree_a_Stmt_Until_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Until_:: class draw call\n"); } 
}


tree_a_Stmt_For_::tree_a_Stmt_For_(a_Stmt_For arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back( (arg->c1) ? new tree_a_Stmt_( arg->c1 ) : NULL );
    subNodes.push_back( (arg->c2) ? new tree_a_Stmt_( arg->c2 ): NULL  );
    subNodes.push_back( (arg->c3) ? new tree_a_Exp_( arg->c3 ) : NULL );
    subNodes.push_back( (arg->body) ? new tree_a_Stmt_( arg->body ): NULL  );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_For_ constructed\n"); } 
}

void
tree_a_Stmt_For_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_For_:: class draw call\n"); } 
}


tree_a_Stmt_Switch_::tree_a_Stmt_Switch_(a_Stmt_Switch arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->val) ? new tree_a_Exp_( arg->val ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Switch_ constructed\n"); } 
}

void
tree_a_Stmt_Switch_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Switch_:: class draw call\n"); } 
}

/*  // POISON!
    tree_a_Stmt_Break_::tree_a_Stmt_Break_(a_Stmt_Break arg) { 
    sub = arg;
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Break_ constructed\n"); } 
    }

    void
    tree_a_Stmt_Break_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Break_:: class draw call\n"); } 
    }

    tree_a_Stmt_Continue_::tree_a_Stmt_Continue_(a_Stmt_Continue arg) { 
    sub = arg;
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Continue_ constructed\n"); } 
    }

    void
    tree_a_Stmt_Continue_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Continue_:: class draw call\n"); } 
    }
*/

tree_a_Stmt_Return_::tree_a_Stmt_Return_(a_Stmt_Return arg) { 
    sub = arg;
    subNodes.push_back ( (arg->val) ? new tree_a_Exp_( arg->val) : NULL );
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Return_ constructed\n"); } 
}

void
tree_a_Stmt_Return_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Return_:: class draw call\n"); } 
}


tree_a_Stmt_Case_::tree_a_Stmt_Case_(a_Stmt_Case arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->exp) ? new tree_a_Exp_( arg->exp) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_Case_ constructed\n"); } 
}

void
tree_a_Stmt_Case_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_Case_:: class draw call\n"); } 
}


tree_a_Stmt_GotoLabel_::tree_a_Stmt_GotoLabel_(a_Stmt_GotoLabel arg) { 
    sub = arg;
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Stmt_GotoLabel_ constructed\n"); } 
}

void
tree_a_Stmt_GotoLabel_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Stmt_GotoLabel_:: class draw call\n"); } 
}


tree_a_Var_Decl_::tree_a_Var_Decl_(a_Var_Decl arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( ( arg->var_decl  ) ? new tree_a_Var_Decl_( arg->var_decl ) : NULL );
// v2    subNodes.push_back ( ( arg->exp  ) ? new tree_a_Exp_( arg->exp) : NULL );
    fprintf(stderr, "tree_a_Var_Decl_ constructed!\n");
}

void
tree_a_Var_Decl_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Var_Decl_:: class draw call\n"); } 
}


tree_a_Var_Decl_List_::tree_a_Var_Decl_List_(a_Var_Decl_List arg) { 
    sub = arg;
    lastline = arg->linepos;
    for ( ; arg ; arg = arg->next ) 
        subNodes.push_back ( new tree_a_Var_Decl_( arg->var_decl) );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Var_Decl_List_ constructed\n"); } 
}

void
tree_a_Var_Decl_List_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Var_Decl_List_:: class draw call\n"); } 
}


tree_a_Type_Decl_::tree_a_Type_Decl_(a_Type_Decl arg) { 
    sub = arg;
    lastline = arg->linepos;
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Type_Decl_ constructed\n"); } 
}

void
tree_a_Type_Decl_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Type_Decl_:: class draw call\n"); } 
}


tree_a_Arg_List_::tree_a_Arg_List_(a_Arg_List arg) { 
    sub = arg;
    lastline = arg->linepos;
    for ( ; arg ; arg = arg->next ) 
        subNodes.push_back ( (arg->type_decl) ? new tree_a_Type_Decl_( arg->type_decl ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Arg_List_ constructed\n"); } 
}

void
tree_a_Arg_List_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Arg_List_:: class draw call\n"); } 
}


tree_a_Id_List_::tree_a_Id_List_(a_Id_List arg) { 
    sub = arg;
    lastline = arg->linepos;

    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Id_List_ constructed\n"); } 
}

void
tree_a_Id_List_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Id_List_:: class draw call\n"); } 
}


tree_a_Class_Ext_::tree_a_Class_Ext_(a_Class_Ext arg) { 
    sub = arg;
    lastline = arg->linepos;
    subNodes.push_back ( (arg->impl_list) ? new tree_a_Id_List_( arg->impl_list ) : NULL );
    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Class_Ext_ constructed\n"); } 
}

void
tree_a_Class_Ext_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Class_Ext_:: class draw call\n"); } 
}


tree_a_Class_Body_::tree_a_Class_Body_(a_Class_Body arg) { 
    sub = arg;
    for ( ; arg; arg = arg->next ) 
        subNodes.push_back ( new tree_a_Section_( arg->section) );
    lastline = arg->linepos;

    if ( TREEDEBUG ) { fprintf(stderr, "tree_a_Class_Body_ constructed\n"); } 
}

void
tree_a_Class_Body_::draw() {
    if ( DRAWDEBUG ) { fprintf(stderr, "tree_a_Class_Body_:: class draw call\n"); } 
}



