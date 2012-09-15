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
// name: audicle_ui_parser.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __PARSE_GUI_H_INCLUDED__
#define __PARSE_GUI_H_INCLUDED__

#include "audicle_def.h"

//chuck defs
#include "chuck_absyn.h"
#include "chuck_type.h"

class TextBuffer;
a_Program parse_buffer(TextBuffer * t);

typedef struct tree_a_File_ * tree_a_File;

typedef struct tree_a_Program_ * tree_a_Program;
typedef struct tree_a_Section_ * tree_a_Section;
typedef struct tree_a_Stmt_List_ * tree_a_Stmt_List;
typedef struct tree_a_Class_Def_ * tree_a_Class_Def;
typedef struct tree_a_Func_Def_ * tree_a_Func_Def;
typedef struct tree_a_Stmt_ * tree_a_Stmt;
typedef struct tree_a_Exp_ * tree_a_Exp;
typedef struct tree_a_Exp_Binary_ * tree_a_Exp_Binary;
typedef struct tree_a_Exp_Cast_ * tree_a_Exp_Cast;
typedef struct tree_a_Exp_Unary_ * tree_a_Exp_Unary;
typedef struct tree_a_Exp_Postfix_ * tree_a_Exp_Postfix;
typedef struct tree_a_Exp_Primary_ * tree_a_Exp_Primary;
typedef struct tree_a_Exp_Dur_ * tree_a_Exp_Dur;
typedef struct tree_a_Exp_Array_ * tree_a_Exp_Array;
typedef struct tree_a_Exp_Func_Call_ * tree_a_Exp_Func_Call;
typedef struct tree_a_Exp_Dot_Member_ * tree_a_Exp_Dot_Member;
typedef struct tree_a_Exp_If_ * tree_a_Exp_If;
typedef struct tree_a_Exp_Decl_ * tree_a_Exp_Decl;
typedef struct tree_a_Exp_Namespace_ * tree_a_Exp_Namespace;
typedef struct tree_a_Stmt_Code_ * tree_a_Stmt_Code;
typedef struct tree_a_Stmt_If_ * tree_a_Stmt_If;
typedef struct tree_a_Stmt_While_ * tree_a_Stmt_While;
typedef struct tree_a_Stmt_Until_ * tree_a_Stmt_Until;
typedef struct tree_a_Stmt_For_ * tree_a_Stmt_For;
typedef struct tree_a_Stmt_Switch_ * tree_a_Stmt_Switch;
typedef struct tree_a_Stmt_Break_ * tree_a_Stmt_Break;
typedef struct tree_a_Stmt_Continue_ * tree_a_Stmt_Continue;
typedef struct tree_a_Stmt_Return_ * tree_a_Stmt_Return;
typedef struct tree_a_Stmt_Case_ * tree_a_Stmt_Case;
typedef struct tree_a_Stmt_GotoLabel_ * tree_a_Stmt_GotoLabel;
typedef struct tree_a_Decl_ * tree_a_Decl;
typedef struct tree_a_Var_Decl_ * tree_a_Var_Decl;
typedef struct tree_a_Var_Decl_List_ * tree_a_Var_Decl_List;
typedef struct tree_a_Type_Decl_ * tree_a_Type_Decl;
typedef struct tree_a_Arg_List_ * tree_a_Arg_List;
typedef struct tree_a_Id_List_ * tree_a_Id_List;
typedef struct tree_a_Class_Ext_ * tree_a_Class_Ext;
typedef struct tree_a_Class_Body_ * tree_a_Class_Body;


/*


a_Program
a_Section
a_Stmt_List
a_Class_Def
a_Func_Def
a_Code_Segment
a_Stmt
a_Exp
a_Exp_Chuck
a_Exp_Binary
a_Exp_Cast
a_Exp_Unary
a_Exp_Postfix
a_Exp_Primary
a_Exp_Dur
a_Exp_Array
a_Exp_Func_Call
a_Exp_Dot_Member
a_Exp_If
a_Exp_Decl
a_Exp_Namespace
a_Stmt_Code
a_Stmt_If
a_Stmt_While
a_Stmt_Until
a_Stmt_For
a_Stmt_Switch
a_Stmt_Break
a_Stmt_Continue
a_Stmt_Return
a_Stmt_Case
a_Stmt_GotoLabel
a_Decl
a_Var_Decl
a_Var_Decl_List
a_Type_Decl
a_Arg_List
a_Id_List
a_Class_Ext
a_Class_Body

ae_Exp_Meta 
ae_Exp_Primary_Type 
ae_Exp_Type 
ae_Func_Type 
ae_Keyword 
ae_Operator 
ae_Section_Type 
ae_Stmt_Type

*/
typedef struct tree_Node_ * tree_Node;
enum {ADJUST_ADD, ADJUST_REMOVE};
struct tree_Node_ {

    bool open;          //elision

    bool isValue;
    bool isList;

    t_CKUINT structID;
    t_CKUINT boxID;

    int firstline;      //node boundaries
    int lastline;
    
    bool valid;         //is the node still valid?  this occurs 
                        //when its region fails to parse ( modified, added ) 
                        //or when the region's boundaries are broken ( deletion )

    std::vector < tree_Node > subNodes;
    tree_Node_();
    ~tree_Node_();
    virtual void draw_tree(); //base recursive call
    virtual void draw_tree_buffer(TextBuffer * b);
    virtual void drawTextRange(TextBuffer * b, int start, int end );
    virtual void draw(); //specific draw behavior
    virtual void changelines(int pos, int change);
    virtual int checkmodified(int pos, int range ) { return 0;}
    virtual int checkIDList( t_CKUINT * stack, t_CKUINT n );
    virtual int findfirst();
    virtual int f_line() { return firstline; }
    virtual int l_line()  { return (open) ? lastline : firstline ; }
    bool multiline() { return ( open &&  firstline != lastline ); } 
};

struct tree_a_File_ : public tree_Node_ {
    a_Program root;
    tree_a_File_( a_Program arg, int firstline, int lastline);
    void draw();
};

struct tree_a_Program_ : public tree_Node_ { 
    a_Program sub;
    tree_a_Program_(a_Program arg);
    void draw();
};

struct tree_a_Section_ : public tree_Node_ { 
    a_Section sub;
    tree_a_Section_(a_Section arg);
    void draw();
};

struct tree_a_Stmt_List_ : public tree_Node_ { 
    a_Stmt_List sub;
    tree_a_Stmt_List_(a_Stmt_List arg);
    void draw();
};

struct tree_a_Class_Def_ : public tree_Node_ { 
    a_Class_Def sub;
    tree_a_Class_Def_(a_Class_Def arg);
    void draw();
};

struct tree_a_Func_Def_ : public tree_Node_ { 
    a_Func_Def sub;
    tree_a_Func_Def_(a_Func_Def arg);
    void draw();
};

struct tree_a_Code_Segment_ : public tree_Node_ { 
    a_Code_Segment sub;
    tree_a_Code_Segment_(a_Code_Segment arg);
    void draw();
};

struct tree_a_Stmt_ : public tree_Node_ { 
    a_Stmt sub;
    tree_a_Stmt_(a_Stmt arg);
    void draw();
};

struct tree_a_Exp_ : public tree_Node_ { 
    a_Exp sub;
    tree_a_Exp_(a_Exp arg);
    void draw();
};

struct tree_a_Exp_Binary_ : public tree_Node_ { 
    a_Exp_Binary sub;
    tree_a_Exp_Binary_(a_Exp_Binary arg);
    void draw();
};

struct tree_a_Exp_Cast_ : public tree_Node_ { 
    a_Exp_Cast sub;
    tree_a_Exp_Cast_(a_Exp_Cast arg);
    void draw();
};

struct tree_a_Exp_Unary_ : public tree_Node_ { 
    a_Exp_Unary sub;
    tree_a_Exp_Unary_(a_Exp_Unary arg);
    void draw();
};

struct tree_a_Exp_Postfix_ : public tree_Node_ { 
    a_Exp_Postfix sub;
    tree_a_Exp_Postfix_(a_Exp_Postfix arg);
    void draw();
};

struct tree_a_Exp_Primary_ : public tree_Node_ { 
    a_Exp_Primary sub;
    tree_a_Exp_Primary_(a_Exp_Primary arg);
    void draw();
};

struct tree_a_Exp_Dur_ : public tree_Node_ { 
    a_Exp_Dur sub;
    tree_a_Exp_Dur_(a_Exp_Dur arg);
    void draw();
};

struct tree_a_Exp_Array_ : public tree_Node_ { 
    a_Exp_Array sub;
    tree_a_Exp_Array_(a_Exp_Array arg);
    void draw();
};

struct tree_a_Exp_Func_Call_ : public tree_Node_ { 
    a_Exp_Func_Call sub;
    tree_a_Exp_Func_Call_(a_Exp_Func_Call arg);
    void draw();
};

struct tree_a_Exp_Dot_Member_ : public tree_Node_ { 
    a_Exp_Dot_Member sub;
    tree_a_Exp_Dot_Member_(a_Exp_Dot_Member arg);
    void draw();
};

struct tree_a_Exp_If_ : public tree_Node_ { 
    a_Exp_If sub;
    tree_a_Exp_If_(a_Exp_If arg);
    void draw();
};

struct tree_a_Exp_Decl_ : public tree_Node_ { 
    a_Exp_Decl sub;
    tree_a_Exp_Decl_(a_Exp_Decl arg);
    void draw();
};

/* v2 struct tree_a_Exp_Namespace_ : public tree_Node_ { 
    a_Exp_Namespace sub;
    tree_a_Exp_Namespace_(a_Exp_Namespace arg);
    void draw();
};*/

struct tree_a_Stmt_Code_ : public tree_Node_ { 
    a_Stmt_Code sub;
    tree_a_Stmt_Code_(a_Stmt_Code arg);
    void draw();
};

struct tree_a_Stmt_If_ : public tree_Node_ { 
    a_Stmt_If sub;
    tree_a_Stmt_If_(a_Stmt_If arg);
    void draw();
};

struct tree_a_Stmt_While_ : public tree_Node_ { 
    a_Stmt_While sub;
    tree_a_Stmt_While_(a_Stmt_While arg);
    void draw();
};

struct tree_a_Stmt_Until_ : public tree_Node_ { 
    a_Stmt_Until sub;
    tree_a_Stmt_Until_(a_Stmt_Until arg);
    void draw();
};

struct tree_a_Stmt_For_ : public tree_Node_ { 
    a_Stmt_For sub;
    tree_a_Stmt_For_(a_Stmt_For arg);
    void draw();
};

struct tree_a_Stmt_Switch_ : public tree_Node_ { 
    a_Stmt_Switch sub;
    tree_a_Stmt_Switch_(a_Stmt_Switch arg);
    void draw();
};

struct tree_a_Stmt_Break_ : public tree_Node_ { 
    a_Stmt_Break sub;
    tree_a_Stmt_Break_(a_Stmt_Break arg);
    void draw();
};

struct tree_a_Stmt_Continue_ : public tree_Node_ { 
    a_Stmt_Continue sub;
    tree_a_Stmt_Continue_(a_Stmt_Continue arg);
    void draw();
};

struct tree_a_Stmt_Return_ : public tree_Node_ { 
    a_Stmt_Return sub;
    tree_a_Stmt_Return_(a_Stmt_Return arg);
    void draw();
};

struct tree_a_Stmt_Case_ : public tree_Node_ { 
    a_Stmt_Case sub;
    tree_a_Stmt_Case_(a_Stmt_Case arg);
    void draw();
};

struct tree_a_Stmt_GotoLabel_ : public tree_Node_ { 
    a_Stmt_GotoLabel sub;
    tree_a_Stmt_GotoLabel_(a_Stmt_GotoLabel arg);
    void draw();
};

struct tree_a_Var_Decl_ : public tree_Node_ { 
    a_Var_Decl sub;
    tree_a_Var_Decl_(a_Var_Decl arg);
    void draw();
};

struct tree_a_Var_Decl_List_ : public tree_Node_ { 
    a_Var_Decl_List sub;
    tree_a_Var_Decl_List_(a_Var_Decl_List arg);
    void draw();
};

struct tree_a_Type_Decl_ : public tree_Node_ { 
    a_Type_Decl sub;
    tree_a_Type_Decl_(a_Type_Decl arg);
    void draw();
};

struct tree_a_Arg_List_ : public tree_Node_ { 
    a_Arg_List sub;
    tree_a_Arg_List_(a_Arg_List arg);
    void draw();
};

struct tree_a_Id_List_ : public tree_Node_ { 
    a_Id_List sub;
    tree_a_Id_List_(a_Id_List arg);
    void draw();
};

struct tree_a_Class_Ext_ : public tree_Node_ { 
    a_Class_Ext sub;
    tree_a_Class_Ext_(a_Class_Ext arg);
    void draw();
};

struct tree_a_Class_Body_ : public tree_Node_ { 
    a_Class_Body sub;
    tree_a_Class_Body_(a_Class_Body arg);
    void draw();
};

#endif
