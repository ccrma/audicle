#include "coaudicle_view.h"

CoCamera::CoCamera() { 
    //projection
    double angle = 45.0;
    double aspect = 1.2;  //guess for now 
    double near = 1.0;
    double far = 500.0;

    //lookat
    Point3D pos = Point3D (0,20,-40);
    Point3D at  = Point3D (0,10, 0);
    Point3D up  = Point3D (0, 1, 0);
}

void 
CoCamera setProjection() { 
    gluPerspective ( angle, aspect, near, far );
}

void
CoCamera setModelView() { 
    gluLookAt( pos[0], pos[1], pos[2], 
               at[0],  at[1],  at[2], 
               up[0],  up[1],  up[2] );
}
