#include "windowing.h"

enum cubeFace_name { FACE_EDITOR = 0, FACE_TIMING, FACE_COMPILER, FACE_VM, FACE_SHREDULING, FACE_TR };

class AudiFace;
class AudiConsole;

class AudiCube {

protected:


    AudiConsole *   _console;
    AudiFace *      _foremost;
    AudiFace *      _faces[6];
    InputState      _inp;

    double      _omatrix[16];
    bool        _switching;
    Quaternion  _lastOrient;
    double      _switchTime;
    Quaternion  _cubeOrient;

public:

    void draw();
    void selectFace(cubeFace_name n);
    void selectFace(char * name);

    virtual void handleKey (char c, Point2D pt);
	virtual void handleSpec(int c, Point2D pt);
	virtual void handleMouse(int button, int state, Point2D pt);
	virtual void handleMotion( Point2D pt);

};

class AudiConsole { 

protected:

   
    AudiCube * _cube;
    InputState _inp;
    UIRectangle _window;

public:

    void draw();
    virtual void handleKey (char c, Point2D pt);
    virtual void handleSpec(int c, Point2D pt);
    virtual void handleMouse(int button, int state, Point2D pt);
    virtual void handleMotion( Point2D pt);

};

class AudiFace { 

protected:
    WindowManager _w;
    InputState _inp;
    Quaternion _orient;
    double     _omatrix[16];
//    Point3D  _origin;
//    Vec3D    _up;

    AudiFace *_up;
    AudiFace *_down;
    AudiFace *_left;
    AudiFace *_right;
    AudiFace *_back;

    string _label;
public:
    void draw(); 
    void drawFace();
    void drawLabel();

    void setLabel(char * p) { _label = p; } 

    Quaternion orient() { return _orient; } 
    void setOrient(Quaternion q) { _orient = q; _orient.genMatrix( (double*)_omatrix ); } 

    virtual void handleKey (char c, Point2D pt);
    virtual void handleSpec(int c, Point2D pt);
	virtual void handleMouse(int button, int state, Point2D pt);
	virtual void handleMotion( Point2D pt);

};