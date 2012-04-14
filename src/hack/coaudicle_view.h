#include "coaudicle.h"
#include "coaudicle_avatar.h"

class CoCamera { 
    CoCamera();
    Point3D pos;
    Point3D at;
    Point3D up;

    double angle;
    double aspect;
    double near;
    double far;

    void setProjection();
    void setModelView();
}

class CoSessionRendererImp() extends CoSessionRenderer { 
public: 
    CoSessionRendererImp() { 
        m_camera = new Camera();
    }
    CoSession* m_session;
    CoSession*& session() { return m_session; }

    Camera * m_camera;
    std::vector < CoAvatar * > m_avatars;

    void set_session( CoSession * session ); 
    void render();
};



