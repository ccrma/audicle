#include "geometry.h"

/* These are the implementations of the promised methods for Point2D */
Flt& Point2D::operator[] (int i){
  if(ASSERT_ARRAY){assert(0<=i && i<2);}
  return p[i];
}
Point2D Point2D::unit(void){
  Flt l=length();
  assert(l!=0.0);
  return scale(1.0/l);
}
Point2D Point2D::negate(void){return scale(-1.0);}
Point2D Point2D::operator -(void){return scale(-1.0);}

Point2D Point2D::scale(Flt s){return Point2D(p[0]*s,p[1]*s);}
Point2D Point2D::operator* (Flt s){return scale(s);}
Point2D Point2D::operator/ (Flt s){return scale(1.0/s);}

Point2D::Point2D(Point3D p3, Matrix * xf ) { 
   if ( xf ) { 
      Point3D tran = xf->multPosition(p3);
      p[0] = tran[0];
      p[1] = tran[1];
   }
   else { 
      //identity
      p[0] = p3[0];
      p[1] = p3[1];
   }
}

  
Flt     Point2D::dot(Point2D q){return p[0]*q.p[0]+p[1]*q.p[1];}
Flt     Point2D::perp(Point2D q) { return(p[0] * q[1] - p[1] * q[0]); }
int     Point2D::clock(Point2D a, Point2D c) { 
   return ( (c-*this).perp(*this-a) >= 0 ) ? 1 : -1 ;
}
Flt     Point2D::length(void){return sqrt(dot(*this));}

Point2D Point2D::add(Point2D q){
  return Point2D(p[0]+q.p[0],p[1]+q.p[1]);
}
Point2D Point2D::operator+ (Point2D q){return add(q);}

Point2D Point2D::subtract(Point2D q){
  return Point2D(p[0]-q.p[0],p[1]-q.p[1]);
}
Point2D Point2D::operator- (Point2D q){return subtract(q);}

Point2D Point2D::mult(Point2D q){
  return Point2D(p[0]*q[0],p[1]*q[1]);
}
Point2D Point2D::div(Point2D q){
  return Point2D(p[0]/q[0],p[1]/q[1]);
}

Flt Point2D::normalize() { 
	Flt len = length();
	if ( len == 0 ) return 0;
	p[0] /= len ;
	p[1] /= len ;
	return len;

}


bool
Point2D::intersect_rad( Point2D q, double dist, Point2D &out ) { 
    if ( (*this-q).length() < dist ) { out = q.interp( *this, 0.5 ); return true; }
    return false;    
}

bool
Point2D::inSeg(Point2D x1, Point2D x2) { 

    if ( x1[0] != x2[0] ) { 
        if ( x1[0] <= p[0] && p[0] <= x2[0] ) return true;
        if ( x1[0] >= p[0] && p[0] >= x2[0] ) return true;
    }
    else { 
        if ( x1[1] <= p[1] && p[1] <= x2[1] ) return true;
        if ( x1[1] >= p[1] && p[1] >= x2[1] ) return true;
    }

    return false;
}

bool 
Point2D::intersect_lines(Point2D xyz1, Point2D xyz2, Point2D abc1, Point2D abc2, Point2D &out ) { 
   
   double eps = 0.0001;

   Point2D u = xyz2-xyz1;
   Point2D v = abc2-abc1;
   Point2D w = xyz1-abc1;

   double D = u.perp(v);
   
   if ( fabs(D) > eps ) { 
      //skew
      double sI = v.perp(w) / D;
      if ( sI < 0 || sI > 1 ) return false;
      double tI = u.perp(w) / D;
      if ( tI < 0 || tI > 1 ) return false;
      out = xyz1 + u*sI;

      return true;
   }
   
   if ( u.perp(w) != 0 || v.perp(w) != 0 ) return 0; // not collinear
   
   float du = u.dot(u);
   float dv = v.dot(v);
   
   if ( du==0 && dv==0 ) { 
      if ( !( xyz1==abc1) ) return false;
      out = xyz1;
   }
   if ( du==0 ) { 
      if ( xyz1.inSeg(abc1, abc2) ) { 
         out = xyz1;
         return true;
      }
   }
   if ( dv==0 ) { 
      if ( abc1.inSeg(xyz1, xyz2) ) { 
         out = abc1;
         return true;
      }
   }
   
   float t1, t0;
   Point2D w2;
   w2 = xyz2 - abc1; 
   if ( v[0] != 0 ) { 
      t0 = w [0] / v[0];
      t1 = w2[0] / v[0];
   }
   else { 
      t0 = w [1] / v[1];
      t1 = w2[1] / v[1];
   }
   if ( t0 > t1 ) { 
      float t= t0; t0=t1; t1=t;
   }
   if ( t0 > 1 || t1 < 0 ) return false;
   t0 = t0<0? 0 : t0;              // clip to min 0
   t1 = t1>1? 1 : t1;              // clip to max 1
   if (t0 == t1) {  // intersect is a point
      out = abc1 + v * t0;
      return true;
   }
   
   // return midpoint of subsegment
   out = abc1 + v * (t0+t1) * 0.5;   
   return true;
   // parallel test
}




/* These are the implementations of the promised methods for Point3D */
Point3D::Point3D(Color4D pt) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2];}

Flt&    Point3D::operator[] (int i){
  if(ASSERT_ARRAY){assert(0<=i && i<3);}
  return p[i];
}

Point3D Point3D::unit(void){
  Flt l=length();
//  assert(l!=0.0);
  return (l != 0 ) ? scale(1.0/l) : Point3D(0,0,0);
}
Point3D Point3D::negate(void){return scale(-1.0);}
Point3D Point3D::operator -(void){return scale(-1.0);}

Point3D Point3D::scale(Flt s){return Point3D(p[0]*s,p[1]*s,p[2]*s);}
Point3D Point3D::operator* (Flt s){return scale(s);}
double  Point3D::operator* (Point3D p) { return dot(p);}
Point3D Point3D::operator/ (Flt s){return scale(1.0/s);}

Flt     Point3D::dot(Point3D q){return p[0]*q.p[0]+p[1]*q.p[1]+p[2]*q.p[2];}
Flt     Point3D::length(void){return sqrt(dot(*this));}
Flt     Point3D::len2(void) { return dot(*this); }

Flt     Point3D::nearest_to_line(Point3D x1, Point3D dir, Point3D * ret) { 
                  Vec3D rel = *this-x1;
                  Vec3D d   = dir.norm();
                  Vec3D x   = d * d.dot(rel);
                  if( ret ) *ret = x1+x;
                  return ( rel-x ).length();
                  }

Flt     Point3D::nearest_to_ray(Point3D x1, Point3D dir, Point3D * ret) { 
                  Vec3D rel = *this-x1;
                  Vec3D d   = dir.norm();
                  double dp = d.dot(rel);
                  Vec3D x = ( dp > 0 ) ? d * dp : Point3D (0,0,0);
                  if( ret ) *ret = x1 + x;
                  return ( rel-x ).length();
                  }

Flt     Point3D::nearest_to_segment(Point3D x1, Point3D x2, Point3D * ret) { 
                  Vec3D rel   = *this-x1;
                  Vec3D dir   = x2 - x1;
                  Vec3D d     = dir.norm();
                  double dp   = d.dot(rel);
                  Vec3D x     = ( dp < 0 ) ? Point3D (0,0,0) : ( ( dp > dir.length() ) ? dir : d * dp );
                  if( ret ) *ret = x1 + x;
                  return ( rel-x ).length();
                  }

Point3D Point3D::add(Point3D q){
  return Point3D(p[0]+q.p[0],p[1]+q.p[1],p[2]+q.p[2]);
}
Point3D Point3D::operator+  (Point3D q){return add(q);}


Point3D Point3D::subtract(Point3D q){
  return Point3D(p[0]-q.p[0],p[1]-q.p[1],p[2]-q.p[2]);
}
Point3D Point3D::operator- (Point3D q){return subtract(q);}

Point3D Point3D::crossProduct(Point3D q){
  return Point3D(p[1]*q[2]-p[2]*q[1],-p[0]*q[2]+p[2]*q[0],p[0]*q[1]-p[1]*q[0]);
}
Point3D Point3D::mult(Point3D q){
  return Point3D(p[0]*q[0],p[1]*q[1],p[2]*q[2]);
}

Flt Point3D::normalize() { 
	Flt len = length();
	if ( len == 0 ) return 0;
	p[0] /= len ;
	p[1] /= len ;
	p[2] /= len ;
	return len;

}


Point3D 
Point3D::norm() { 
	Flt len = length();
	Point3D pt = *this;
   if ( len == 0 ) return pt;
	pt[0] /= len ;
	pt[1] /= len ;
	pt[2] /= len ;
	return pt;

}

Flt
Point3D::dot_xz( Point3D p2 ) { 
   return(p[0] * p2[0] + p[2] * p2[2]);
}
Flt
Point3D::perp_xz( Point3D p2 ) { 
   return(p[0] * p2[2] - p[2] * p2[0]);
}

Point3D 
Point3D::project( Point3D pt ) { 
   return norm() * norm().dot(pt);
}

double fract( double x ) { return x - floor(x);}

void
Point3D::wrap( double xmin, double xspan, double ymin, double yspan, double zmin, double zspan) { 
   if ( p[0] < xmin || xmin + xspan < p[0] ) p[0] = xmin + xspan * fract ( ( p[0]  - xmin ) / (xspan) );
   if ( p[1] < ymin || ymin + yspan < p[1] ) p[1] = ymin + yspan * fract ( ( p[1]  - ymin ) / (yspan) );
   if ( p[2] < zmin || zmin + zspan < p[2] ) p[2] = zmin + zspan * fract ( ( p[2]  - zmin ) / (zspan) );
}

bool
Point3D::inSeg2D ( Point3D x1, Point3D x2) { 

    if ( x1[0] != x2[0] ) { 
        if ( x1[0] <= p[0] && p[0] <= x2[0] ) return true;
        if ( x1[0] >= p[0] && p[0] >= x2[0] ) return true;
    }
    else { 
        if ( x1[2] <= p[2] && p[2] <= x2[2] ) return true;
        if ( x1[2] >= p[2] && p[2] >= x2[2] ) return true;
    }

    return false;

}
 
bool
Point3D::isect2d_rad( Point3D xyz2, Point3D abc2, double dist, Point3D &out ) { 
    if ( (xyz2-abc2).length() < dist ) { out = xyz2.interp( abc2, 0.5 ); return true; }
    return false;    

}

//intersection in xz plane

bool 
Point3D::isect2d(Point3D xyz1, Point3D xyz2, Point3D abc1, Point3D abc2, Point3D &out ) { 
   
   double eps = 0.0001;
   Point3D u = xyz2-xyz1;
   Point3D v = abc2-abc1;
   Point3D w = xyz1-abc1;

   double D = u.perp_xz(v);
   
   if ( fabs(D) > eps ) { 
      //skew
      double sI = v.perp_xz(w) / D;
      if ( sI < 0 || sI > 1 ) return false;
      double tI = u.perp_xz(w) / D;
      if ( tI < 0 || tI > 1 ) return false;
      out = xyz1 + u*sI;

//    out[0] = xyz1[0] + sI * u[0];
//    out[1] = xyz1[1] + sI * u[1];
//    out[2] = xyz1[2] + sI * u[2];
      return true;
   }
   
   if ( u.perp_xz(w) != 0 || v.perp_xz(w) != 0 ) return 0; // not collinear
   
   float du = u.dot_xz(u);
   float dv = v.dot_xz(v);
   
   if ( du==0 && dv==0 ) { 
      if ( !( xyz1==abc1) ) return false;
      out = xyz1;
   }
   if ( du==0 ) { 
      if ( xyz1.inSeg2D(abc1, abc2) ) { 
         out = xyz1;
         return true;
      }
   }
   if ( dv==0 ) { 
      if ( abc1.inSeg2D(xyz1, xyz2) ) { 
         out = abc1;
         return true;
      }
   }
   
   float t1, t0;
   Point3D w2;
   w2 = xyz2 - abc1; 
   if ( v[0] != 0 ) { 
      t0 = w [0] / v[0];
      t1 = w2[0] / v[0];
   }
   else { 
      t0 = w [2] / v[2];
      t1 = w2[2] / v[2];
   }
   if ( t0 > t1 ) { 
      float t= t0; t0=t1; t1=t;
   }
   if ( t0 > 1 || t1 < 0 ) return false;
   t0 = t0<0? 0 : t0;              // clip to min 0
   t1 = t1>1? 1 : t1;              // clip to max 1
   if (t0 == t1) {  // intersect is a point
      out = abc1 + v * t0;
      return true;
   }
   
   // return midpoint of subsegment
   out = abc1 + v * (t0+t1) * 0.5;   
   return true;
   // parallel test
}


void
Point3D::fillMatrix( double *m, Point3D fwd, Point3D base, int squash) { 
   //this vector is up ( y ) 
   //fwd is the direction that ( z ) faces 

   //base is the root position...maybe we ignore this?
   
   // make this an orthogonal matrix
   // uniform scale
   Point3D u_y = *this;
   double len = u_y.normalize();
   Point3D u_z = fwd - (u_y * u_y.dot(fwd));
   u_z.normalize();
   Point3D u_x = u_y.crossProduct(u_z);

   double invol = (squash==SQUASH_VOLUME) ? sqrt( 1.0 / len ) : ( ( squash == SQUASH_CONST ) ? 1.0 : len );
   u_x *= invol;
   u_y *= len;
   u_z *= invol;
/*
   0  4  8  12
   1  5  9  13
   2  6  10 14
   3  7  11 15
*/
   //x
   m[0]  = u_x[0];
   m[1]  = u_x[1];
   m[2]  = u_x[2];
   m[3]  = 0;

   //y
   m[4]  = u_y[0];
   m[5]  = u_y[1];
   m[6]  = u_y[2];
   m[7]  = 0;

   //z
   m[8]  = u_z[0];
   m[9]  = u_z[1];
   m[10] = u_z[2];
   m[11] = 0;

   //t
   m[12] = base[0];
   m[13] = base[1];
   m[14] = base[2];
   m[15] = 1;



}

//KINDA STUPID
//KEEP A TABLE OF ALL COMBINATORICS

int comb( int n, int k) { 

   static int** comb = NULL;
   static int  csize = 1;
   
   static int cmax=0;
   
   int i,j;

   if ( n < 0 || k < 0 || k > n ) return 0 ;   
   
   if ( n > cmax ) { 
      if ( cmax == 0 ) {
         comb = new int*[n];
         //base case..
         comb[0]     =   new int[1];
         comb[0][0]  =   1;
      }
      if ( n >= csize ) { 
         //resize table
         while ( csize <= n ) csize *= 2;
         if ( ( comb = (int**) realloc(comb, csize * sizeof(int*) ) ) == NULL )  {
            cmax = 0; csize = 1;
            fprintf(stderr, "ack!  could not realloc!\n");
            return 0;
         }
      }
      for ( j=cmax+1; j <= n; j++ ) { 
         //build the table up to (n)
         comb[j]     = new int[j+1];
         comb[j][0]  = 1;
         comb[j][j]  = 1;
         for ( i=1; i < j ; i++ ) { 
            comb[j][i] = comb[j-1][i-1] + comb[j-1][i];
         }
      }
      cmax = n;
   }

   return comb[n][k];

}


Point3D
Point3D::bezier_interp_pt(Point3D** v1, int n, double u) { 

   //pretty formula here -> http://astronomy.swin.edu.au/~pbourke/curves/bezier/

   
   Point3D** v = v1;
   if ( n < 1 ) return Point3D(0,0,0);
   if ( n == 1 ) { 
      //identity
      return *v[0];
   }
   else if ( n == 2 ) { 
      //linear
      
      return v[0]->interp( *v[1] , u);
   }
   else if ( n == 3 ) { 
      //quadratic   
      //pu =  p1 * (1-u)^2 + p2 * 2 * u * ( 1-u ) + p3 * u^2;
      double u2   = u * u;
      double ru   = 1 - u;
      double ru2  = ru * ru;
      return *v[0] * ru2 + *v[1] * 2 * ru * u + *v[2] * u2;
   }
   else if ( n == 4 ) { 
      //cubic
      //pu = p1 * (1-u)^3  + p2 * 3 * u * ( 1-u )^2 + p3 * 3 * u^2 * (1-u) + p4 * u^3;
      double u2  = u*u;
      double u3  = u*u2;
      double r   = 1-u;
      double r2  = r*r;
      double r3  = r*r2;
      return *v[0] * r3  + *v[1] * 3.0 * u * r2   + *v[2] * 3.0 * u2 * r   + *v[3] * u3;
   }
   else { // n > 4
      //general form!

//      fprintf(stderr, "bezier_interp - no general form!  barfing!\n");
//      return v[0]->interp(*v[n-1], u);

      int i;
      double* powu = new double[n]; // u to various powers
      double* powr = new double[n]; // 1-r to various powers

      powu[0]  = 1;
      powr[0]  = 1;
      powu[1]  = u;
      powr[1]  = 1-u;

      //set up arrays
      //2n mult
      for ( i = 1; i < n ; i++ ) { 
         powu[i] = powu[i-1] * u;
         powr[i] = powr[i-1] * (1-u);
      }

      Point3D p(0,0,0);

      //3n mult
      for ( i = 0 ; i < n; i++ ) { //summation of terms
         p += *v[i] * comb(n-1, i) * powu[i] * powr[n-(1+i)];
      }

      delete [] powu;
      delete [] powr;

      return p;
      
   }
}

Point3D
Point3D::bezier_interp_pt_dir(Point3D** v1, int n, double u, Point3D &dir) { 

   //pretty formula here -> http://astronomy.swin.edu.au/~pbourke/curves/bezier/

   
   Point3D** v = v1;
   if ( n < 1 ) return Point3D(0,0,0);
   if ( n == 1 ) { 
      //identity
      dir = Point3D(1,0,0);
      return *v[0];
   }
   else if ( n == 2 ) { 
      //linear
      dir = *v[1] - *v[0];  //linear derivative
      return v[0]->interp( *v[1] , u);
   }
   else if ( n == 3 ) { 
      //quadratic   
      //pu =  p1 * (1-u)^2 + p2 * 2 * u * ( 1-u ) + p3 * u^2;
      double u2   = u * u;
      double ru   = 1 - u;
      double ru2  = ru * ru; 
      
      dir =   *v[0] * (-2 + 2*u) \
            + *v[1] * (2 - 4 * u) \
            + *v[2] * (2 * u);  //quad derivative 
      
      return  *v[0] * ru2 \
            + *v[1] * (2 * ru * u) \
            + *v[2] * u2;
   }
   else if ( n == 4 ) { 
      //cubic
      //pu = p1 * (1-u)^3  + p2 * 3 * u * ( 1-u )^2 + p3 * 3 * u^2 * (1-u) + p4 * u^3;
      double u2  = u*u;
      double u3  = u*u2;
      double r   = 1-u;
      double r2  = r*r;
      double r3  = r*r2;
      
      dir =  *v[0] * (-3 * r2)   \
            +*v[1] * (3 * r * (-2*u + r)) \
            +*v[2] * (3 * u * ( 2*r + u)) \
            +*v[3] * (3 * u2);  //quad derivative 

      return  *v[0] * r3  \
            + *v[1] * (3.0 * u  * r2) \
            + *v[2] * (3.0 * u2 * r) \
            + *v[3] * u3;
   }
   else { // n > 4
      //general form!

//      fprintf(stderr, "bezier_interp - no general form!  barfing!\n");
//      return v[0]->interp(*v[n-1], u);

      int i;
      double* powu = new double[n]; // u to various powers
      double* powr = new double[n]; // 1-r to various powers

      powu[0]  = 1;
      powr[0]  = 1;
      powu[1]  = u;
      powr[1]  = 1-u;

      //set up arrays
      //2n mult
      for ( i = 1; i < n ; i++ ) { 
         powu[i] = powu[i-1] * u;
         powr[i] = powr[i-1] * (1-u);
      }

      Point3D p(0,0,0);

      //3n mult
      for ( i = 0 ; i < n; i++ ) { //summation of terms
         p += *v[i] * comb(n-1,i) * powu[i] * powr[n-(1+i)];
      }

      delete [] powu;
      delete [] powr;

      return p;
      
   }
}

Point3D
Point3D::spline_interp_pt(Point3D** vl, double* pl,  double u ) { 

   //vl is an array of pointers to Point3D 
   //dl is an array with the cumulative distance of the polyline
   
   double  _6u;
   
   double   u2;
   double _3u2;
   double _6u2;
   
   double   u3;
   double _2u3;
   
   
   //We do this as lazy as possible. 
   //Only relevant quantities are interpolated. 
   //Only the position is splined, while others
   //are linearly interpolated.
   
   
 //XXX - We've assumed that vertices are unique
   
   assert(pl[2] != pl[0]);
   assert(pl[3] != pl[1]);
   
   Point3D m1, m2; //actually vectors
   
   double di = (pl[2] - pl[1]);
   
   // If the endpoints are replicated pointers, then
   // we use the parabola-slope enpoint scheme
   // Note, if (vl[0] == vl[1]) && (vl[2] == vl[3])
   // then we do the first case (which ends up with
   // m1=m2=(vl[2]->pos() - vl[1]->pos())
   if (vl[0] == vl[1]) 
      {
         m2 = (*vl[3] - *vl[1]) * (di/(pl[3] - pl[1]));
         m1 = (*vl[2] - *vl[1]) * 2.0 - m2;
      }
   // If the endpoints are replicated pointers, then
   // we use the parabola-slope enpoint scheme
   else if (vl[2] == vl[3])
      {
         m1 = (*vl[2] - *vl[0]) * (di/(pl[2] - pl[0]));
         m2 = (*vl[2] - *vl[3]) * 2.0 - m1;
      }
   // In general, the slopes are taken as the
   // chords between the adjacent points
   else
      {
         m1 = (*vl[2] - *vl[0]) * (di/(pl[2] - pl[0]));
         m2 = (*vl[3] - *vl[1]) * (di/(pl[3] - pl[1]));
      }

   // If u is outside [0,1] then we must be trying
   // to apply an offset that extends beyond the
   // end of the stroke.  That's cool, but let's
   // make sure that we didn't screw up by asserting
   // that we're extending beyond the t=_min_t or t=_max_t
   // endpoints of the stroke. Then just use the
   // slope at the end to extend outward...
   if (u>1.0)
      {
         // This function is sometimes called while generating
         // the offsets, before they have been set.
         //assert(_offsets != NULL);
         assert( vl[2]==vl[3]);
//         assert( vl[2]->_t == _max_t);
         return *vl[2] + m2 * (u - 1.0);
         //v->_dir = m2;

      }
   else if (u<0.0)
      {
         // This function is sometimes called while generating
         // the offsets, before they have been set.
         //assert(_offsets != NULL);
         assert( vl[0]==vl[1]);
  //       assert( vl[1]->_t == _min_t);
         return *vl[1] + m1 * u;
         //v->_dir = m1;
      }
   else
      {

         _6u =  6*u;
   
         u2 =  u*u;
         _3u2 = 3*u2;
         _6u2 = 6*u2;
   
         u3 = u*u2;
         _2u3 = 2*u3;


         return (
            *vl[1] * (1     - _3u2 + _2u3) +
            *vl[2] * (        _3u2 - _2u3) +
            m1               * (    u - 2*u2 +   u3) +
            m2               * (      -   u2 +   u3)
            );

         // This is the tangent (not the normal)
         /* v->_dir =(
            ((vl[1]->pos() * (      - _6u  + _6u2) -
              vl[2]->pos() * (      - _6u  + _6u2)) +
             m1 * (    1 - 4*u  + _3u2)  +
             m2 * (      - 2*u  + _3u2)));
         */
       
   }

}


Point3D
Point3D::spline_interp_pt_dir(Point3D**vl, double* pl , double u, Point3D &dir ) { 

    //vl is an array of pointers to Point3D 
   //dl is an array with the cumulative distance of the polyline
   
   double  _6u;
   
   double   u2;
   double _3u2;
   double _6u2;
   
   double   u3;
   double _2u3;
   
   
   //We do this as lazy as possible. 
   //Only relevant quantities are interpolated. 
   //Only the position is splined, while others
   //are linearly interpolated.
   
   
   //XXX - We've assumed that vertices are unique
   
   assert(pl[2] != pl[0]);
   assert(pl[3] != pl[1]);
   
   Point3D m1, m2; //actually vectors
   
   double di = (pl[2] - pl[1]);
   
   // If the endpoints are replicated pointers, then
   // we use the parabola-slope enpoint scheme
   // Note, if (vl[0] == vl[1]) && (vl[2] == vl[3])
   // then we do the first case (which ends up with
   // m1=m2=(vl[2]->pos() - vl[1]->pos())
   if (vl[0] == vl[1]) 
      {
         m2 = (*(vl[3]) - *(vl[1])) * (di/(pl[3] - pl[1]));
         m1 = (*(vl[2]) - *(vl[1])) * 2.0 - m2;
      }
   // If the endpoints are replicated pointers, then
   // we use the parabola-slope enpoint scheme
   else if (vl[2] == vl[3])
      {
         m1 = (*(vl[2]) - *(vl[0])) * (di/(pl[2] - pl[0]));
         m2 = (*(vl[2]) - *(vl[3])) * 2.0 - m1;
      }
   // In general, the slopes are taken as the
   // chords between the adjacent points
   else
      {
         m1 = (*(vl[2]) - *(vl[0])) * (di/(pl[2] - pl[0]));
         m2 = (*(vl[3]) - *(vl[1])) * (di/(pl[3] - pl[1]));
      }

   // If u is outside [0,1] then we must be trying
   // to apply an offset that extends beyond the
   // end of the stroke.  That's cool, but let's
   // make sure that we didn't screw up by asserting
   // that we're extending beyond the t=_min_t or t=_max_t
   // endpoints of the stroke. Then just use the
   // slope at the end to extend outward...

   if (u>1.0)
      {
         // This function is sometimes called while generating
         // the offsets, before they have been set.
         //assert(_offsets != NULL);
         assert( vl[2]==vl[3]);
//         assert( vl[2]->_t == _max_t);
         dir = m2;
         return *(vl[2]) + m2 * (u - 1.0);
         

      }
   else if (u<0.0)
      {
         // This function is sometimes called while generating
         // the offsets, before they have been set.
         //assert(_offsets != NULL);
         assert( vl[0]==vl[1]);
  //       assert( vl[1]->_t == _min_t);
         dir = m1;    
         return *(vl[1]) + m1 * u;
       
      }
   else
      {

         _6u =  6*u;
   
         u2 =  u*u;
         _3u2 = 3*u2;
         _6u2 = 6*u2;
   
         u3 = u*u2;
         _2u3 = 2*u3;


    
         // This is the tangent (not the normal)
         dir =(
            (( *(vl[1]) * (      - _6u  + _6u2) -
              *(vl[2]) * (      - _6u  + _6u2)) +
             m1 * (    1 - 4*u  + _3u2)  +
             m2 * (      - 2*u  + _3u2)));
         
         return (
            *(vl[1]) * (1     - _3u2 + _2u3) +
            *(vl[2]) * (        _3u2 - _2u3) +
            m1               * (    u - 2*u2 +   u3) +
            m2               * (      -   u2 +   u3)
            );

   }

}

// for Color ( 4D ! ) 
Flt&    Color4D::operator[] (int i){
  if(ASSERT_ARRAY){assert(0<=i && i<4);}
  return p[i];
}
Color4D::Color4D(Point3D pt) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; p[3]=1.0;}


Color4D Color4D::unit(void){
  Flt l=length();
  assert(l!=0.0);
  return scale(1.0/l);
}
Color4D Color4D::negate(void){return scale(-1.0);}
Color4D Color4D::operator -(void){return scale(-1.0);}

Color4D Color4D::scale(Flt s){return Color4D(p[0]*s,p[1]*s,p[2]*s, p[3]*s);}
Color4D Color4D::operator* (Flt s){return scale(s);}
Color4D Color4D::operator/ (Flt s){return scale(1.0/s);}

Flt     Color4D::dot(Color4D q){return p[0]*q.p[0]+p[1]*q.p[1]+p[2]*q.p[2]+p[3]*q.p[3];}
Flt     Color4D::length(void){return sqrt(dot(*this));}

Color4D Color4D::add(Color4D q){
  return Color4D(p[0]+q.p[0],p[1]+q.p[1],p[2]+q.p[2], p[3]+q.p[3]);
}
Color4D Color4D::operator+ (Color4D q){return add(q);}

Color4D Color4D::subtract(Color4D q){
  return Color4D(p[0]-q.p[0],p[1]-q.p[1],p[2]-q.p[2], p[3]-q.p[3]);
}
Color4D Color4D::operator- (Color4D q){return subtract(q);}

Color4D Color4D::crossProduct(Color4D q){
  return Color4D(p[1]*q[2]-p[2]*q[1],-p[0]*q[2]+p[2]*q[0],p[0]*q[1]-p[1]*q[0], 1);
}
Color4D Color4D::mult(Color4D q){
  return Color4D(p[0]*q[0],p[1]*q[1],p[2]*q[2],p[3]*q[3]);
}
Color4D Color4D::alpha(double a){
  return Color4D(p[0],p[1],p[2],p[3]*a);
}

/* These are the implementations of the promised methods for Ray */
Ray Ray::translate(Point3D q){return Ray(p+q,d);}
Point3D Ray::position(Flt s){
  return Point3D(p[0]+s*d[0],p[1]+s*d[1],p[2]+s*d[2]);
}
Point3D Ray::operator() (Flt s){return position(s);}

bool Circle::inside(Point3D pt ) { 
   return ( (pt-p).length() < rad ) ? true : false;
}

Circle::Circle(Point3D p1, Point3D p2, Point3D p3 ) { 
   float num;
	float cp;

	cp = ((p2 - p1).crossProduct( p3 - p1 ))[2];
	if (cp != 0.0)
	    {
   		float p1Sq, p2Sq, p3Sq;
   		float cx, cy;

	   	p1Sq = p1[0] * p1[0] + p1[1] * p1[1];
	   	p2Sq = p2[0] * p2[0] + p2[1] * p2[1];
	   	p3Sq = p3[0] * p3[0] + p3[1] * p3[1];
	   	num = p1Sq*(p2[1] - p3[1]) + p2Sq*(p3[1] - p1[1]) + p3Sq*(p1[1] - p2[1]);
	   	cx = num / (2.0f * cp);
	   	num = p1Sq*(p3[0] - p2[0]) + p2Sq*(p1[0] - p3[0]) + p3Sq*(p2[0] - p1[0]);
		   cy = num / (2.0f * cp); 

   		p[0] = cx; p[1] = cy; p[2] = 0;
	    }
    
	// Radius 
	rad = (p1-p).length();

}

/* These are the implementations of the promised methods for Matrix */
Flt& Matrix::operator() (int i,int j){
  if(ASSERT_ARRAY){assert(0<=i && 0<=j && i<4 && j<4);}
  return m[i][j];
}
Flt  Matrix::det(void){
  Flt d=0.0;
  int i;
  for(i=0;i<4;i++){
    if((i%2)==0){d+=subDet(i,0)*this->m[i][0];}
    else{d-=subDet(i,0)*this->m[i][0];}
  }
  return d;
}
Flt Matrix::subDet(int c1,int r1,int c2,int r2){
  return this->m[c1][r1]*this->m[c2][r2]-this->m[c1][r2]*this->m[c2][r1];
}
Flt Matrix::subDet(int c,int r){
  int i;
  int c1,r1,c2,r2,row;
  Flt d=0,sgn=1.0;
  row=0;
  if(row==r){row++;}
  for(i=0;i<4;i++){
    if(i==c){continue;}
    c1=0;
    while(c1==i || c1==c){c1++;}
    c2=c1+1;
    while(c2==i || c2==c){c2++;}
    r1=0;
    while(r1==row || r1==r){r1++;}
    r2=r1+1;
    while(r2==row || r2==r){r2++;}

    d+=sgn*this->m[i][row]*subDet(c1,r1,c2,r2);
    sgn*=-1.0;
  }
  return d;
}

void Matrix::print(void){
  int i,j;
  for(j=0;j<4;j++){
    for(i=0;i<4;i++){printf("%5.3f ",this->m[j][i]);}
    printf("\n");
  }
}

Matrix Matrix::mult(Matrix m){
  int i,j,k;
  Matrix n;
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      n(i,j)=0.0;
      for(k=0;k<4;k++){n(i,j)+=this->m[k][j]*m(i,k);}
    }
  }
  return n;
}

Matrix Matrix::sum(Matrix b) { 
   Matrix r;
   int i,j;
   for ( i=0;i<4;i++) { 
      for ( j=0;j<4;j++) { 
         r(i,j) = m[i][j] + b(i,j);
      }
   }
   return r;
}

Matrix Matrix::operator* (Matrix m){return mult(m);}

Matrix Matrix::transpose(void){
  int i,j;
  Matrix n;
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){n(i,j)=this->m[j][i];}
  }
  return n;
}
Matrix Matrix::invert(void){
  int i,j;
  Matrix m;
  Flt d;
  
  d=det();
  assert(d!=0.0);
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      if((i+j)%2==0){m(j,i)=subDet(i,j)/d;}
      else{m(i,j)=-subDet(j,i)/d;}
    }
  }
  return m;
}

void
Matrix::fill3x3mat(float n[3][3]) { 
   for ( int i=0; i < 3;i++ ) { 
      for ( int j=0; j < 3; j++ ) { 
         n[i][j] = m[j][i];
      }
   }
}

Matrix
Matrix::scale(double f) { 
   Matrix n;
   for ( int i=0; i<4; i++ ) { 
      for ( int j=0; j<4; j++ ) { 
         n(i,j) = m[i][j] * f;
      }
   }
   return n;
}
void
Matrix::cscale(float r, float g, float b, float a) { 
   Matrix n = IdentityMatrix();
   n(0,0) = r;
   n(1,1) = g;
   n(2,2) = b;
   n(3,3) = a;
   *this = mult(n);
}

void
Matrix::luminance() { 
   *this = mult(LuminanceMatrix());
}

void
Matrix::saturate(float val) { 
   Matrix lum = LuminanceMatrix();
   Matrix col = IdentityMatrix();
   Matrix sat = (lum * (1.0-val)) + (col * val);
   *this = sat.mult(*this);
}

void
Matrix::rotate(int axis, float rs, float rc) {
   //right hand rule-like... ixj=k;
   int a0 = axis;
   int a1 = (axis + 1) % 3;
   int a2 = (axis + 2) % 3;

   Matrix rot = IdentityMatrix().scale(0);
   rot(4,4)=1.0;     //fix at 1.0
  
   rot(a0,a0) = 1.0; //self remains fixed;
   
   rot(a1,a1) = rc;  
   rot(a1,a2) = rs;

   rot(a2,a2) = rc;
   rot(a2,a1) = -rs;
   
   *this = rot.mult(*this);
}

void 
Matrix::zshear( float dx, float dy ) { 
   Matrix shr = IdentityMatrix();

   shr(0,2)=dx;
   shr(1,2)=dy;

   *this = shr.mult(*this);
}

void
Matrix::huerotate(float angle) { 


    Matrix huexform;
    Matrix rot;

    static bool precalc = false;

    static Matrix huespace;
    static Matrix invhuespace;
    static float r2mag = sqrt(2.0);
    static float r3mag = sqrt(3.0);
    static float xrs = 1.0 / r2mag;
    static float xrc = 1.0 / r2mag;
    static float yrs = -1.0  / r3mag;
    static float yrc = r2mag / r3mag;
    float zrs, zrc;
    static float zsx=0;
    static float zsy=0;

    if ( !precalc ) {
      //loop invariables...

      huespace = IdentityMatrix();
      /* rotate the grey vector into positive Z */
      huespace.rotate(0,xrs,xrc);
      
      huespace.rotate(1,yrs,yrc);
      
      /* shear the space to make the luminance plane horizontal */
      Point3D pos(RLUM, GLUM, BLUM);
      pos = huespace.multPosition(pos);
      zsx = pos[0]/pos[2];
      zsy = pos[1]/pos[2];
      huespace.zshear(zsx,zsy);

      invhuespace = IdentityMatrix();
      /* unshear the space to put the luminance plane back */
      invhuespace.zshear(-zsx, -zsy);
      /* rotate the grey vector back into place */
      invhuespace.rotate(1, -yrs, yrc);
      invhuespace.rotate(0, -xrs, xrc);

      precalc = true;
    }

    /* rotate the hue */
    zrs = sin(angle*PI/180.0);
    zrc = cos(angle*PI/180.0);
    rot = IdentityMatrix();
    rot.rotate(2, zrs, zrc);

    huexform = invhuespace * (rot * huespace) ; //perform
    *this = mult(huexform);

}

Point3D Matrix::multPosition(Point3D p){
  Point3D q;
  int i,j;
  for(i=0;i<3;i++){
    q[i]=this->m[3][i];
    for(j=0;j<3;j++){q[i]+=this->m[j][i]*p[j];}
  }
  return q;
}

Point3D Matrix::multDirection(Point3D p){
  Point3D q;
  int i,j;
  for(i=0;i<3;i++){
    q[i]=0.0;
    for(j=0;j<3;j++){q[i]+=this->m[j][i]*p[j];}
  }
  return q;
}

Point3D Matrix::multNormal(Point3D p){
  return transpose().invert().multDirection(p);
}

Ray Matrix::mult(Ray r){
  Ray q;
  q.p=multPosition(r.p);
  q.d=multDirection(r.d);
  return q;
}
Ray Matrix::operator* (Ray r){return mult(r);}
int Matrix::isIdentity(void){
  int i,j;
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      if(i==j && m[i][j]!=1.0){return 0;}
      else if(i!=j && m[i][j]!=0.0){return 0;}
    }
  }
  return 1;
}

Flt det(Matrix m){return m.det();}
Matrix IdentityMatrix(void){
  Matrix m;
  int i,j;
  for(i=0;i<4;i++){
     for(j=0;j<4;j++){
        m(i,j)=(i==j)?1.0:0.0;
     }
  }
  return m;
}

Matrix LuminanceMatrix(void) { 
   Matrix m = IdentityMatrix();
   for ( int i=0; i < 3; i++ ) { 
      m(0,i) = RLUM;
      m(1,i) = GLUM;
      m(2,i) = BLUM;
   }
   return m;
}

double
gRectangle::douter(Point2D p) { 
	fprintf (stderr, "points ");
	p.print(); tl.print(); br.printnl();

	double dx = ( tl[0] < p[0] && p[0] < br[0] ) ? 0 : min ( br[0] - p[0], p[0] - tl[0] );  
	double dy = ( br[1] < p[1] && p[1] < tl[1] ) ? 0 : min ( tl[1] - p[1], p[1] - br[1] );  
	fprintf (stderr, "points %f %f\n", dx, dy);
	p.print(); tl.print(); br.printnl();
	return sqrt ( dx*dx + dy*dy );
}
