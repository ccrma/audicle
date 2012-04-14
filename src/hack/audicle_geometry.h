//-----------------------------------------------------------------------------
// name: audicle_geometry.h
// desc: ...
//
// authors: 
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_GEOMETRY_H__
#define __AUDICLE_GEOMETRY_H__

#include "audicle_def.h"


#if defined ( __PLATFORM_WIN32__ ) 

static t_CKINT isinf( t_CKFLOAT d )
{
    int expon = 0;
    t_CKFLOAT val = frexp (d, &expon);
    if (expon == 1025) {
        if (val == 0.5) {
            return 1;
        } else if (val == -0.5) {
            return -1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static t_CKINT isnan (t_CKFLOAT d)
{
    int expon = 0;
    t_CKFLOAT val = frexp (d, &expon);
    if (expon == 1025) {
        if (val == 0.5) {
            return 0;
        } else if (val == -0.5) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

#endif


#define ASSERT_ARRAY 0
#define RAYEPS 1e-5

/* *LEAVE* this as t_CKFLOAT!*/
typedef t_CKFLOAT Flt;


/* The Point2D and Point3D classes are just a 2D and 3D vectors with methods 
 * supporting indexing, printing, taking the dot product with another vector, 
 * obtaining length, returning a unit-length scaled version of itself,
 * negation, scaling, adding, subtracting, and doing component-wise 
 * multiplication. Additionally, the 3D vector supports taking the cross-
 * product. */

/* Note that coordinates of the vector are accessed using square brackets in 
 * the same way that an array is so that:
 *   Point3D p(0.0,1.0,2.0);
 *   printf("%f\n",p[1]);
 * should print out "1.0" */

class Point3D;
class Color4D;
class Matrix;

typedef class Point2D Vec2D;

class Point2D{
  Flt p[2];
 public:
  Point2D()             {p[0]=p[1]=0.0;}
  Point2D(Flt x,Flt y)  {p[0]=x; p[1]=y;}
  Point2D(Point3D p3, Matrix * xf = NULL );

  Flt& operator[] (t_CKINT index);
  const t_CKFLOAT* data() { return p; }  
  t_CKFLOAT * pdata()      { return p; }
  void print() const {printf("(%f,%f)",p[0],p[1]);}
  void printnl() const {print();printf("\n");}
  bool operator== (Point2D pt) { return (p[0]==pt[0] && p[1]==pt[1] ); } 
  Flt dot(Point2D pt);
  Flt perp(Point2D pt);

  Flt length(void);

  Point2D unit(void);

  Point2D negate(void);
  Point2D operator- (void);

  Point2D scale(Flt scl);
  Point2D operator* (Flt scale);
  Point2D operator/ (Flt scale);

  Point2D add(Point2D pt);
  Point2D operator+ (Point2D pt);

  void  operator +=(Point2D v)  { p[0] += v[0]; p[1] += v[1]	; }
  void  operator -=(Point2D v)  { p[0] -= v[0]; p[1] -= v[1]	; }
  void  operator *=(Flt scale)   { p[0] *= scale; p[1] *= scale; }
  void  operator /=(Flt scale)   { p[0] /= scale; p[1] /= scale; }

  Flt normalize(); 
  Point2D subtract(Point2D pt);
  Point2D operator- (Point2D pt);
  Point2D interp ( Point2D pt, t_CKFLOAT w)		{return scale(1.0-w) + pt.scale(w); } 
  bool    inSeg( Point2D x1, Point2D x2 ) ;
  bool    intersect_rad( Point2D q, t_CKFLOAT dist, Point2D &out);
  bool    intersect_lines( Point2D a1, Point2D a2, Point2D b1, Point2D b2, Point2D &out );
  t_CKINT     clock( Point2D a, Point2D c );
  Point2D mult(Point2D pt); //pairwise
  Point2D div(Point2D pt);  //pairwise
};

enum {   SQUASH_IDENT, SQUASH_VOLUME, SQUASH_CONST }; 

typedef class Point3D Vec3D;

class Point3D{
  Flt p[3];
 public:
  Point3D(){p[0]=p[1]=p[2]=0;}
  Point3D(Flt x, Flt y )		{	p[0]=x;	p[1]=y;	p[2]=0;}
  Point3D(Flt x,Flt y,Flt z)	{	p[0]=x;	p[1]=y;	p[2]=z;}
  Point3D(Flt* pf)	{	p[0]=pf[0];	p[1]=pf[1];	p[2]=pf[2];}
  Point3D(Point2D pt)	{	p[0]=pt[0];	p[1]=pt[1];	p[2]=0;}
  Point3D(Color4D pt);	
  Flt& operator[] (t_CKINT index);
  bool operator== (Point3D pt) { return (p[0]==pt[0] && p[1]==pt[1] && p[2] == pt[2] ); } 
  const t_CKFLOAT* data() { return p; }  
  t_CKFLOAT * pdata()		{ return p; }
  void print(void){printf("(%f,%f,%f)",p[0],p[1],p[2]);}
  void printnl(void){print();printf("\n");}
   
  void fprint(FILE* fp){fprintf( fp, "%f %f %f",p[0],p[1],p[2]);}
  void fprintnl(FILE * fp){fprint(fp);fprintf(fp, "\n");}

  void wrap ( t_CKFLOAT xmin = -1.0, t_CKFLOAT xspan = 2.0, t_CKFLOAT ymin = -1.0, t_CKFLOAT yspan = 2.0, t_CKFLOAT zmin = -1.0, t_CKFLOAT zspan = 2.0 ); 

  Flt dot(Point3D pt);

  Flt len2(void);
  Flt length(void);
  Flt dist(Point3D x) { return (*this - x).length(); }
  bool inSeg2D ( Point3D x1, Point3D x2 ) ;
  static bool isect2d ( Point3D a1, Point3D a2, Point3D b1, Point3D b2, Point3D &out ) ;
  static bool isect2d_rad ( Point3D a2, Point3D b2, t_CKFLOAT dist, Point3D &out ) ;
  Flt nearest_to_line     (Point3D x1, Point3D dir, Point3D* ret = NULL);
  Flt nearest_to_segment  (Point3D x1, Point3D dir, Point3D* ret = NULL);
  Flt nearest_to_ray      (Point3D x1, Point3D dir, Point3D* ret = NULL);
  Flt perp_xz (Point3D p2);
  Flt dot_xz (Point3D p2);
  Flt normalize();
  Point3D norm();
  Point3D unit(void);

  Point3D negate(void);
  Point3D operator- (void);

  Point3D scale(Flt);
  Point3D operator* (Flt scale);
  Point3D operator/ (Flt scale);
  void	 operator *=(Flt scale)   { p[0] *= scale; p[1] *= scale; p[2] *= scale; }
  void    operator /=(Flt scale)   { p[0] /= scale; p[1] /= scale; p[2] /= scale; }

  Point3D add(Point3D pt);
  t_CKFLOAT  operator* (Point3D pt);
  Point3D operator+ (Point3D pt);

  Point3D subtract(Point3D pt);
  Point3D operator- (Point3D pt);

  void    operator +=(Point3D v)	  { p[0] += v[0]; p[1] += v[1]; p[2] += v[2]	;}
  void    operator -=(Point3D v)	  { p[0] -= v[0]; p[1] -= v[1]; p[2] -= v[2]	;}
  
  void    fillMatrix ( t_CKFLOAT* m, Point3D fwd, Point3D base, t_CKINT squash=SQUASH_IDENT);
  Point3D project (Point3D pt );
  Point3D crossProduct(Point3D pt);

  Point3D interp ( Point3D pt, t_CKFLOAT w)		{return scale(1.0-w) + pt.scale(w); } 

  Point3D mult(Point3D pt);


  Point3D      bezier_interp_pt(Point3D** v1, t_CKINT n, t_CKFLOAT u );
  Point3D      bezier_interp_pt_dir(Point3D** v1, t_CKINT n, t_CKFLOAT u, Point3D &dir);
  Point3D      spline_interp_pt(Point3D** vl, t_CKFLOAT * pl, t_CKFLOAT u);
  Point3D      spline_interp_pt_dir(Point3D** vl, t_CKFLOAT * pl, t_CKFLOAT u, Point3D &dir);

};

class Color4D{
  Flt p[4];
 public:
  Color4D(){p[0]=p[1]=p[2]=0;p[3]=1;}
  Color4D(Flt x,Flt y,Flt z){p[0]=x;p[1]=y;p[2]=z;p[3]=1;}
  Color4D(Flt x,Flt y,Flt z, Flt a){p[0]=x;p[1]=y;p[2]=z;p[3]=a;}
  Color4D(unsigned char* px)  {     p[0] = ((t_CKFLOAT)px[0]) / 255.0; 
                                    p[1] = ((t_CKFLOAT)px[1]) / 255.0; 
                                    p[2] = ((t_CKFLOAT)px[2]) / 255.0;
                                    p[3] = 1;                        }
  Color4D(Point3D pt);
  Flt& operator[] (t_CKINT index);

  const t_CKFLOAT* data() { return p; }  
  t_CKFLOAT* pdata() { return p; }  
  void print(void)			{printf("(%f,%f,%f,%f)",p[0],p[1],p[2],p[3]);}
  void printnl(void)		   {print();printf("\n");}
  void fprint (FILE* fp)	{fprintf(fp,"%f %f %f %f",p[0], p[1], p[2], p[3] );} 
  void fprintnl(FILE* fp)	{fprint(fp);fprintf(fp, "\n");}
  void fill_float(t_CKFLOAT *dbg) { dbg[0]=p[0]; dbg[1]=p[1]; dbg[2]=p[2]; dbg[3]=p[3];  }
  void fill_ubyte(unsigned char *dbg) {   dbg[0]=(unsigned char) ( min(1.0, max(0,p[0]))*255 ); 
                                          dbg[1]=(unsigned char) ( min(1.0, max(0,p[1]))*255 ); 
                                          dbg[2]=(unsigned char) ( min(1.0, max(0,p[2]))*255 );  }

  Flt dot(Color4D pt);

  Flt length(void);

  Color4D unit(void);

  Color4D negate(void);
  Color4D operator- (void);

  Color4D scale(Flt);
  Color4D operator* (Flt scale);
  Color4D operator/ (Flt scale);

  Color4D add(Color4D pt);
  Color4D operator + (Color4D pt);
  void    operator +=(Color4D pt)	  { p[0] += pt[0]; p[1] += pt[1]; p[2] += pt[2]; p[3] += pt[3];}

  Color4D subtract(Color4D pt);
  Color4D operator- (Color4D pt);

  Color4D crossProduct(Color4D pt);
  Color4D interp ( Color4D pt, t_CKFLOAT w)		{return scale(1.0-w) + pt.scale(w); }

  Color4D mult(Color4D pt);
  Color4D alpha(t_CKFLOAT a);
};

typedef class Quaternion Quat;
class Quaternion { 
protected:
   
   //  i*i = j*j = k*k = -1;
   //  i*j = k = -j*i;
   //  j*k = i = -k*j;
   //  k*i = j = -i*k;

   t_CKFLOAT _r;  //real

   Point3D _im;

public:

   Quaternion(Point3D axis , t_CKFLOAT angle ) {
                                           _r = cos ( angle/2.0 );
                                           _im = axis.norm() * sin ( angle/2.0 );
                                          }
   Quaternion(t_CKFLOAT w, t_CKFLOAT x, t_CKFLOAT y, t_CKFLOAT z) { _r = w; _im[0]=x; _im[1]=y; _im[2]=z;}
   Quaternion(t_CKFLOAT w, Point3D im) : _r (w), _im(im) {}
   Quaternion() { _r = 1; _im = Point3D(0,0,0);}

   void print() { printf("%lf, %lf i, %lf j, %lf k", _r, _im[0], _im[1], _im[2]); }
   void printnl() { print(); printf("\n"); }
   
   t_CKFLOAT      r()  {return _r;}
   Point3D     im() {return _im;}
   Quaternion scale(t_CKFLOAT d)   { return Quaternion( _r * d , _im * d ); }
   Quaternion negate()  { return Quaternion(-_r, -_im); } 
   Quaternion conj()    { return Quaternion( _r, -_im); }
   Quaternion inv()     { return conj().scale(1.0 / dot(conj()) );} 
   t_CKFLOAT     mag()     { return sqrt(dot(conj()));}
   Quaternion unit()    { return (mag() > 0 ) ? scale(1.0 / mag()) : Quaternion(1,0,0,0); } 

   t_CKFLOAT     dot(Quaternion p) { return _r * p._r - (_im.dot(p._im)); }
   Quaternion add(Quaternion p) { return Quaternion ( _r+p._r ,  _im + p._im ); }
   Quaternion prod(Quaternion p) { //  *this x p
                                    return Quaternion ( dot(p), p._im*_r + _im*p._r + _im.crossProduct(p._im)); 
                                 }

   void fprint( FILE * fp)       { fprintf( fp, "%f %f %f %f",_r, _im[0],_im[1],_im[2]); }

   t_CKFLOAT*    pr()               { return &_r;}
   t_CKFLOAT*    pim()              { return _im.pdata(); } 

   Point3D pointmul(Point3D p) { 
                                   Quaternion r = Quaternion( 0, p );
                                   return prod( r.prod(inv()) )._im;
                               }

   Quaternion interp(Quaternion r, t_CKFLOAT w) { return scale(1-w).add(r.scale(w)); }
   Quaternion slerp(Quaternion r, t_CKFLOAT w) { return interp(r,w).unit(); }

   void test() {
      Point3D x(1,0,0);
      Point3D y(0,1,0);
      Point3D z(0,0,1);

      Point3D px = pointmul(x);
      Point3D py = pointmul(y);
      Point3D pz = pointmul(z);
      printf("Quaternion :");
      printnl();
      printf("test unit axis\n");
      x.print(); printf(" to "); px.printnl();
      y.print(); printf(" to "); py.printnl();
      z.print(); printf(" to "); pz.printnl();
      printf("end unit axis\n");
   } 

   void genMatrix(t_CKFLOAT *m) { 
      
   }//fill a rotation matrix
};

class BBox { 
protected:
   bool    _hasContent;
   Point3D _min;
   Point3D _max;
public:
   BBox() { reset(); }
   BBox(Point3D p) { set(p); }
   BBox(Point3D p1, Point3D p2) { set(p1,p2); }
   void reset() { _hasContent = false; } 
   void set(Point3D p) { _hasContent=true; _min = p; _max = p;} 
   void set(Point3D p1, Point3D p2) { set(p1); add(p2); } 
   void add(Point3D p) {
      if ( !_hasContent ) set(p);
      else { 
         for ( t_CKINT i = 0 ; i < 3 ; i++ ) { 
            _min[i] = min( p[i], _min[i] );
            _max[i] = max( p[i], _max[i] );
         }   
      }
   }
   void add2d(Point2D p ) { //let's be picky!
      if ( !_hasContent ) set(p);
      else { 
         for ( t_CKINT i = 0 ; i < 2 ; i++ ) { 
            _min[i] = min( p[i], _min[i] );
            _max[i] = max( p[i], _max[i] );
         }   
      }
   }
   bool     is_set()    { return _hasContent;   } 
   Point3D& pmin()      { return _min;          } 
   Point3D& pmax()      { return _max;          } 

};


//for normalized display in cartesian space
class gRectangle { 
protected:
   Point2D tl;
   Point2D br;
   
public:
   t_CKFLOAT x() {return tl[0];   }
   t_CKFLOAT y() {return tl[1];   }
   t_CKFLOAT w() {return br[0] - tl[0];     }
   t_CKFLOAT h() {return tl[1] - br[1];     }
   
   void   setx(t_CKFLOAT x) { tl[0] = x; }
   void   sety(t_CKFLOAT y) { tl[1] = y; }
   void   setw(t_CKFLOAT w) { br[0] = tl[0]+w; }
   void   seth(t_CKFLOAT h) { br[1] = tl[1]-h; }
   
   t_CKFLOAT left()              { return tl[0]; }
   t_CKFLOAT right()             { return br[0]; }
   t_CKFLOAT top()               { return tl[1]; }
   t_CKFLOAT bottom()            { return br[1]; }

   void   setPos(Point2D p)   { moveto(p[0],p[1]); }
   Point2D pos()              { return tl; }
   Point2D center()			  { return ( tl + br )* 0.5 ; }
   void   setl(t_CKFLOAT l)      { tl[0] = l; }
   void   setr(t_CKFLOAT r)      { br[0] = r; }
   void   sett(t_CKFLOAT t)      { tl[1] = t; }
   void   setb(t_CKFLOAT b)      { br[1] = b; }
   
   void moveto(t_CKFLOAT x, t_CKFLOAT y) { br += Point2D(x,y) - tl; tl = Point2D(x,y); }
   void resize(t_CKFLOAT w, t_CKFLOAT h) { br = tl + Point2D(w, -h); }
   
   bool   inside (Point2D pt) { return ( tl[0] < pt[0] && pt[0] < br[0] && br[1] < pt[1] && pt[1] < tl[1] ) ? true : false; }  
   
   void print() { tl.printnl(); br.printnl(); }
   gRectangle() : tl(Point2D(0,0)), br(Point2D(1,-1)) { } 
   gRectangle(t_CKFLOAT x, t_CKFLOAT y, t_CKFLOAT w, t_CKFLOAT h) : tl(Point2D(x,y)), br(Point2D(x+w,y-h)) {} 
   t_CKFLOAT douter(Point2D p); 
};

class Circle { 
public:
   Point3D p;
   t_CKFLOAT rad;
   Circle() { p = Point3D(0.0,0.0,0.0); rad = 1.0;}
   Circle(Point3D p1, Point3D p2, Point3D p3);
   bool inside(Point3D p);
};


/* A ray is a directed line segment which is defined by a pair of 3D
 * coordinates (or points). The first point specifies the starting point of the
 * ray and the second specifies its direction. It supports methods for
 * translating, finding the paramtetrized distance along a ray, and taking
 * the dot product. */

/* In terms of the position operators are equivalent to:
 *  r(s)=r.position(s)=r.p+r.q*s;
 *  r.dot(m)=(r.q-r.p).dot(m)
 * where "s" is floating point number and "m" is Point3D. (You don't need them
 * but they may help.) */



class Ray{
 public:
  Point3D p;
  Point3D d;

  Ray(){;}
  Ray(Point3D p1,Point3D d1){p=p1;d=d1;}

  void print(void){p.print();printf(":");d.print();}
  void printnl(void){print();printf("\n");}

  Ray translate(Point3D pt);

  Point3D operator() (Flt param);
  Point3D position(Flt param);
};

/* The Matrix class is a 4x4 matrix and supports: taking of determinant,
 * multiplying by another matrix on the right, transposing, inverting,
 * transforming a position, transforming a direction, transforming a normal,
 * and transforming a ray */

/* Its elements are indexed using parentheses and are indexed first by column
 * and the by row. Thus:
 *    Matrix m;
 *    t_CKFLOAT  f;
 *    ...
 *    f=m(i,j);
 * Sets "f" equal to the value in the matrix stored in column "i" and row "j".
 * (Indexing starts at zero.) */

/* Note that since the matrix is actually a 4x4 matrix it is also used to
 * represent translations. For this reason it supports multiplication by the
 * three different kind of points:
 *  1) Multiplying a positional point performs the full transformation
 *  2) Multiplying a direction does not perform the translation, and
 *  3) Multiplying a normal returns a unit-vector which corresponds to the
 *     transformed normal. (i.e. If "n" is a vector normal to a plane then
 *     Matrix.multNormal(n) returns a unit-vector that is normal to the 
 *     transformed plane.) */

/* Indexed via Matrix(col,row)*/

#define RLUM    (0.3086)
#define GLUM    (0.6094)
#define BLUM    (0.0820)


class Matrix{
  Flt subDet(t_CKINT col1,t_CKINT row1,t_CKINT col2,t_CKINT row2);
  Flt subDet(t_CKINT col,t_CKINT row);
  Flt m[4][4];
 public:
  Flt& operator() (t_CKINT col,t_CKINT row);

  Flt det(void);

  void  fill3x3mat(t_CKFLOAT n[3][3]);
  void  saturate(t_CKFLOAT sat);
  void  huerotate(t_CKFLOAT angle);
  void  luminance();
  void  cscale( t_CKFLOAT r, t_CKFLOAT g, t_CKFLOAT b, t_CKFLOAT a=1);

  void  rotate(t_CKINT axis, t_CKFLOAT rs, t_CKFLOAT rc);
  void  zshear(t_CKFLOAT dx, t_CKFLOAT dy);

  void print(void);
  void printnl(void){print();printf("\n");}

  void readgl(t_CKFLOAT *mt) { 
     for ( t_CKINT col = 0; col < 4 ; col++) { 
        for( t_CKINT row = 0 ; row < 4 ; row++ ) { 
            m[col][row] = mt[col*4 + row];
        }
     }
  }

  void fillgl(t_CKFLOAT *mt, t_CKINT dim ) { 
     for ( t_CKINT col = 0; col < 4 ; col++) { 
        for ( t_CKINT row = 0 ; row < 4 ; row++ ) { 
            mt[col*4 + row] = m[col][row];;
        }
     }
  }

  Matrix scale(t_CKFLOAT f);
  Matrix mult(Matrix m);
  Matrix sum(Matrix b);
  Matrix operator* (Matrix m);

  Matrix operator* (t_CKFLOAT d) {return scale(d);}
  Matrix operator+ (Matrix m) {return sum(m); } 

  Matrix transpose(void);

  Matrix invert(void);

  Point3D multPosition(Point3D position);
  Point3D multDirection(Point3D direction);
  Point3D multNormal(Point3D normal);

  Ray mult(Ray ray);
  Ray operator* (Ray ray);

  t_CKINT isIdentity(void);
};
Matrix IdentityMatrix(void);
Matrix LuminanceMatrix(void);


/*
//Mesh class from veldt 
//don't know if we need it yet .

class Mesh : public Geom {
protected:
	


    Field *			      _field;

    Mesh *              _subd[2];


    VECTOR(Mesh *)      _children;

    VECTOR(Geom *)      _trash;
    VECTOR(MeshOp)      _ops_list;

    VECTOR(Vert*)	_verts;
    VECTOR(Edge*)       _edges;
    VECTOR(Poly*)       _faces;
 
    bool                _haslist;

    uint                _dlist;
    uint                _dflags;

    bool                _hasTex;
    GLuint              _tex;
    bool                _toontex;
	
    bool                _connectivity_dirty;
    bool                _deformation_dirty;
    bool                _clearflag;

    Matrix              _xform;
    Point3D             _center;
    double              _radius;
    
public:

	Mesh() :
		_field(NULL),
		_sf(new ScalarField()),
    	_haslist(false),
		_dlist(1),
		_dflags(0),
		_hasTex(false),
		_tex(0),
      _toontex(false),
		_clearflag(false)
   {
      _xform = IdentityMatrix();
      _subd[0]=NULL;
      _subd[1]=NULL;
		_verts.clear();
		_edges.clear();
		_faces.clear();
      _children.clear();
	}

	Mesh(Field* f) :
		_field(f),
		_sf(new ScalarField()),
		_haslist(false),
      _dlist(1),
		_dflags(0),
		_hasTex(false),
		_tex(0),
      _toontex(false),
		_clearflag(false)
   {
      _subd[0]=NULL;
      _subd[1]=NULL;
      _xform = IdentityMatrix();
		_verts.clear();
		_edges.clear();
		_faces.clear();
      _children.clear();
	}


	Mesh(Field* f, char* filename) :
		_field(f),
		_sf(new ScalarField()),
		_haslist(false),
		_dlist(1),
		_dflags(0),
      _hasTex(false),
		_tex(0),
      _toontex(false),
		_clearflag(false)
   {
      _xform = IdentityMatrix();
      _subd[0]=NULL;
      _subd[1]=NULL;
		_verts.clear();
		_edges.clear();
		_faces.clear();
      _children.clear();
	   load_mesh(filename);
   }

   ~Mesh() { 
      remove_self();
   }

	Field*&					field()				{return _field;			}
	
	ScalarField*&			sf()					{return _sf;				}
   
   void                 assert_consistency();
   
   void                 remove_self();
   void                 notify_changed(Mesh* m);
   int                  find_vert(Vert *v);
   int                  find_edge(Edge *e);
   int                  find_poly(Poly *p);
   bool                 has_geom(Geom* g);

   void                 add_child(Mesh *m);

   void                 subdivide();
   void                 unsubdivide();
   void                 regen_child_mesh();
   void                 recalc_child_positions();
   int                  subNumChild();
   int                  subNumParent();
   Mesh*&               subchild()        {return _subd[1];}
   Mesh*&               subparent()       {return _subd[0];} 

   void                 remove_element(Geom* f);
   bool&                clearflag()             {return _clearflag; }    
   bool&                connectivity_dirty()    {return _connectivity_dirty; }
   bool                 fix_winding();
   

   VECTOR(Vert*)&       verts()	{return _verts;}
   Vert*&               vert(int i) {return _verts[i];		}
   int                  nverts() {return _verts.size();	}
   
   Particle*            particle(int i)   {return (Particle*)_verts[i];}
   
   VECTOR(Edge*)&       edges()           {return _edges; }
   Edge*&					edge(int i)			{return _edges[i];		}
   int						nedges()				{return _edges.size();	}
   
   VECTOR(Poly*)&			faces()				{return _faces;			}
   Poly*&					face(int i)			{return _faces[i];		}
   int						nfaces()				{return _faces.size();	}
   
   VECTOR(ContourSet*)& csets()			   {return _csets;		      }
   ContourSet*&			cset(int i)		   {return _csets[i];	      }
   int						ncsets()			   {return _csets.size();     }
   
   VECTOR(Strand*)&	   strands()			{return _strands;			   }
   Strand*&				   strand(int i)		{return _strands[i];		   }
   int						nstrands()			{return _strands.size();	}
   int                  find_strand(Strand* s);

   void update_vert ( Vert*  p_old,	Vert* p_new );
   void update_edge ( Edge*		e_old,	Edge*		 e_new );
   void update_face ( Poly*		f_old,	Poly*		 f_new );

   double mass();
   double pe();
   double ke();

	void new_vert (Vert* p);
	void new_edge (Edge*		 e);
	void new_face (Poly*		 f);
   void tri_from_edge (Edge * e, Vert * p );

   void load_mesh    (char * filename);
	void load_mesh_sm (char * filename);
	void load_mesh_obj(char * filename);
   
   void load_mesh_vrml_block(char * filename, char *blockname);
   void load_mesh_dxf_block(char * filename, char *blockname);


   void write_mesh      (char * filename);
   void write_mesh_obj  (char * filename);
   void write_mesh_sm   (char * filename);

   virtual  gtype isa()    { return MESH;  }
	
   Matrix  xform()     {return _xform;  }
   Point3D center()    {return _center; } 
   double  radius()    {return _radius; } 

	void find_range ( double& min, double&max	  );
	void add_contour( double value );
	void add_blank_contours( int n );
   
   void fix_connectivity();
   void fix_positions();


   void cleanup();
   void rebuild_strands();
   void empty_trash();
   bool in_trash(Geom* f);
   void discard(Geom* g);

   void add_op(Geom *g, mop op)        {MeshOp m; m.g=g; m.op=op; add_op(m);} 
   void add_op(MeshOp op);
   bool in_ops(Geom* g);
   void run_ops();

   void clear();

   bool hasTex()     { return _hasTex; }
   virtual void draw(uint flags =0);
	void draw_points();
   void draw_normals();
	void draw_edges();

   void draw_faces(uint flags = 0);
	void draw_faces_smooth();
	void draw_faces_textured();
   void draw_faces_spline();
	void draw_faces_toon();
   void draw_contours();
   void draw_rescaled();

};

*/

#endif
