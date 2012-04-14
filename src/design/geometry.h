#ifndef GEOMETRY_INCLUDED
#define GEOMETRY_INCLUDED

#include "sys.h"

#if defined ( __PLATFORM_WIN32__ ) 
static int isinf (double d) {
    int expon = 0;
    double val = frexp (d, &expon);
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

static int isnan (double d) {
    int expon = 0;
    double val = frexp (d, &expon);
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

/* *LEAVE* this as double!*/
typedef double Flt;


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

  Flt& operator[] (int index);
  const double* data() { return p; }  
  double * pdata()      { return p; }
  void print(){printf("(%f,%f)",p[0],p[1]);}
  void printnl(){print();printf("\n");}
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
  Point2D interp ( Point2D pt, double w)		{return scale(1.0-w) + pt.scale(w); } 
  bool    inSeg( Point2D x1, Point2D x2 ) ;
  bool    intersect_rad( Point2D q, double dist, Point2D &out);
  bool    intersect_lines( Point2D a1, Point2D a2, Point2D b1, Point2D b2, Point2D &out );
  int     clock( Point2D a, Point2D c );
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
  Flt& operator[] (int index);
  bool operator== (Point3D pt) { return (p[0]==pt[0] && p[1]==pt[1] && p[2] == pt[2] ); } 
  const double* data() { return p; }  
  double * pdata()		{ return p; }
  void print(void){printf("(%f,%f,%f)",p[0],p[1],p[2]);}
  void printnl(void){print();printf("\n");}
   
  void fprint(FILE* fp){fprintf( fp, "%f %f %f",p[0],p[1],p[2]);}
  void fprintnl(FILE * fp){fprint(fp);fprintf(fp, "\n");}

  void wrap ( double xmin = -1.0, double xspan = 2.0, double ymin = -1.0, double yspan = 2.0, double zmin = -1.0, double zspan = 2.0 ); 

  Flt dot(Point3D pt);

  Flt len2(void);
  Flt length(void);
  Flt dist(Point3D x) { return (*this - x).length(); }
  bool inSeg2D ( Point3D x1, Point3D x2 ) ;
  static bool isect2d ( Point3D a1, Point3D a2, Point3D b1, Point3D b2, Point3D &out ) ;
  static bool isect2d_rad ( Point3D a2, Point3D b2, double dist, Point3D &out ) ;
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
  double  operator* (Point3D pt);
  Point3D operator+ (Point3D pt);

  Point3D subtract(Point3D pt);
  Point3D operator- (Point3D pt);

  void    operator +=(Point3D v)	  { p[0] += v[0]; p[1] += v[1]; p[2] += v[2]	;}
  void    operator -=(Point3D v)	  { p[0] -= v[0]; p[1] -= v[1]; p[2] -= v[2]	;}
  
  void    fillMatrix ( double* m, Point3D fwd, Point3D base, int squash=SQUASH_IDENT);
  Point3D project (Point3D pt );
  Point3D crossProduct(Point3D pt);

  Point3D interp ( Point3D pt, double w)		{return scale(1.0-w) + pt.scale(w); } 

  Point3D mult(Point3D pt);


  Point3D      bezier_interp_pt(Point3D** v1, int n, double u );
  Point3D      bezier_interp_pt_dir(Point3D** v1, int n, double u, Point3D &dir);
  Point3D      spline_interp_pt(Point3D** vl, double * pl, double u);
  Point3D      spline_interp_pt_dir(Point3D** vl, double * pl, double u, Point3D &dir);

};

class Color4D{
  Flt p[4];
 public:
  Color4D(){p[0]=p[1]=p[2]=0;p[3]=1;}
  Color4D(Flt x,Flt y,Flt z){p[0]=x;p[1]=y;p[2]=z;p[3]=1;}
  Color4D(Flt x,Flt y,Flt z, Flt a){p[0]=x;p[1]=y;p[2]=z;p[3]=a;}
  Color4D(unsigned char* px)  {     p[0] = ((double)px[0]) / 255.0; 
                                    p[1] = ((double)px[1]) / 255.0; 
                                    p[2] = ((double)px[2]) / 255.0;
                                    p[3] = 1;                        }
  Color4D(Point3D pt);
  Flt& operator[] (int index);

  const double* data() { return p; }  
  double* pdata() { return p; }  
  void print(void)			{printf("(%f,%f,%f,%f)",p[0],p[1],p[2],p[3]);}
  void printnl(void)		   {print();printf("\n");}
  void fprint (FILE* fp)	{fprintf(fp,"%f %f %f %f",p[0], p[1], p[2], p[3] );} 
  void fprintnl(FILE* fp)	{fprint(fp);fprintf(fp, "\n");}
  void fill_float(float *dbg) { dbg[0]=p[0]; dbg[1]=p[1]; dbg[2]=p[2]; dbg[3]=p[3];  }
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
  Color4D interp ( Color4D pt, double w)		{return scale(1.0-w) + pt.scale(w); }

  Color4D mult(Color4D pt);
  Color4D alpha(double a);
};

typedef class Quaternion Quat;
class Quaternion { 
protected:
   
   //  i*i = j*j = k*k = -1;
   //  i*j = k = -j*i;
   //  j*k = i = -k*j;
   //  k*i = j = -i*k;

   double _r;  //real

   Point3D _im;

public:

   Quaternion(Point3D axis , double angle ) {
                                           _r = cos ( angle/2.0 );
                                           _im = axis.norm() * sin ( angle/2.0 );
                                          }
   Quaternion(double w, double x, double y, double z) { _r = w; _im[0]=x; _im[1]=y; _im[2]=z;}
   Quaternion(double w, Point3D im) : _r (w), _im(im) {}
   Quaternion() { _r = 1; _im = Point3D(0,0,0);}

   void print() { printf("%lf, %lf i, %lf j, %lf k", _r, _im[0], _im[1], _im[2]); }
   void printnl() { print(); printf("\n"); }
   
   double      r()  {return _r;}
   Point3D     im() {return _im;}
   Quaternion scale(double d)   { return Quaternion( _r * d , _im * d ); }
   Quaternion negate()  { return Quaternion(-_r, -_im); } 
   Quaternion conj()    { return Quaternion( _r, -_im); }
   Quaternion inv()     { return conj().scale(1.0 / dot(conj()) );} 
   double     mag()     { return sqrt(dot(conj()));}
   Quaternion unit()    { return (mag() > 0 ) ? scale(1.0 / mag()) : Quaternion(1,0,0,0); } 

   double     dot(Quaternion p) { return _r * p._r - (_im.dot(p._im)); }
   Quaternion add(Quaternion p) { return Quaternion ( _r+p._r ,  _im + p._im ); }
   Quaternion prod(Quaternion p) { //  *this x p
                                    return Quaternion ( dot(p), p._im*_r + _im*p._r + _im.crossProduct(p._im)); 
                                 }

   void fprint( FILE * fp)       { fprintf( fp, "%f %f %f %f",_r, _im[0],_im[1],_im[2]); }

   double*    pr()               { return &_r;}
   double*    pim()              { return _im.pdata(); } 

   Point3D pointmul(Point3D p) { 
                                   Quaternion r = Quaternion( 0, p );
                                   return prod( r.prod(inv()) )._im;
                               }

   Quaternion interp(Quaternion r, double w) { return scale(1-w).add(r.scale(w)); }
   Quaternion slerp(Quaternion r, double w) { return interp(r,w).unit(); }

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

   void genMatrix(double *m) { 
      
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
         for ( int i = 0 ; i < 3 ; i++ ) { 
            _min[i] = min( p[i], _min[i] );
            _max[i] = max( p[i], _max[i] );
         }   
      }
   }
   void add2d(Point2D p ) { //let's be picky!
      if ( !_hasContent ) set(p);
      else { 
         for ( int i = 0 ; i < 2 ; i++ ) { 
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
   double x() {return tl[0];   }
   double y() {return tl[1];   }
   double w() {return br[0] - tl[0];     }
   double h() {return tl[1] - br[1];     }
   
   void   setx(double x) { tl[0] = x; }
   void   sety(double y) { tl[1] = y; }
   void   setw(double w) { br[0] = tl[0]+w; }
   void   seth(double h) { br[1] = tl[1]-h; }
   
   double left()              { return tl[0]; }
   double right()             { return br[0]; }
   double top()               { return tl[1]; }
   double bottom()            { return br[1]; }

   void   setPos(Point2D p)   { moveto(p[0],p[1]); }
   Point2D pos()              { return tl; }
   Point2D center()			  { return ( tl + br )* 0.5 ; }
   void   setl(double l)      { tl[0] = l; }
   void   setr(double r)      { br[0] = r; }
   void   sett(double t)      { tl[1] = t; }
   void   setb(double b)      { br[1] = b; }
   
   void moveto(double x, double y) { br += Point2D(x,y) - tl; tl = Point2D(x,y); }
   void resize(double w, double h) { br = tl + Point2D(w, -h); }
   
   bool   inside (Point2D pt) { return ( tl[0] < pt[0] && pt[0] < br[0] && br[1] < pt[1] && pt[1] < tl[1] ) ? true : false; }  
   
   void print() { tl.printnl(); br.printnl(); }
   gRectangle() : tl(Point2D(0,0)), br(Point2D(1,-1)) { } 
   gRectangle(double x, double y, double w, double h) : tl(Point2D(x,y)), br(Point2D(x+w,y-h)) {} 
   double douter(Point2D p); 
};

class Circle { 
public:
   Point3D p;
   double rad;
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
 *    float  f;
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
  Flt subDet(int col1,int row1,int col2,int row2);
  Flt subDet(int col,int row);
  Flt m[4][4];
 public:
  Flt& operator() (int col,int row);

  Flt det(void);

  void  fill3x3mat(float n[3][3]);
  void  saturate(float sat);
  void  huerotate(float angle);
  void  luminance();
  void  cscale( float r, float g, float b, float a=1);

  void  rotate(int axis, float rs, float rc);
  void  zshear(float dx, float dy);

  void print(void);
  void printnl(void){print();printf("\n");}

  void readgl(double *mt) { 
     for ( int col = 0; col < 4 ; col++) { 
        for( int row = 0 ; row < 4 ; row++ ) { 
            m[col][row] = mt[col*4 + row];
        }
     }
  }

  void fillgl(double *mt, int dim ) { 
     for ( int col = 0; col < 4 ; col++) { 
        for ( int row = 0 ; row < 4 ; row++ ) { 
            mt[col*4 + row] = m[col][row];;
        }
     }
  }

  Matrix scale(double f);
  Matrix mult(Matrix m);
  Matrix sum(Matrix b);
  Matrix operator* (Matrix m);

  Matrix operator* (double d) {return scale(d);}
  Matrix operator+ (Matrix m) {return sum(m); } 

  Matrix transpose(void);

  Matrix invert(void);

  Point3D multPosition(Point3D position);
  Point3D multDirection(Point3D direction);
  Point3D multNormal(Point3D normal);

  Ray mult(Ray ray);
  Ray operator* (Ray ray);

  int isIdentity(void);
};
Matrix IdentityMatrix(void);
Matrix LuminanceMatrix(void);
#endif /* GEOMETRY_INCLUDED */
