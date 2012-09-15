#include "audicle_ui.h"

void
AudiCube::draw() { 

    glPushMatrix();
    if ( _switching ) { 
        double w =   ( GetTime() - _switchTime ) / 0.5 ;
        if ( w >= 1.0 ) { 
            _cubeOrient = _foremost->orient();
            _switching = false;
        }
        else { 
            _cubeOrient = _lastOrient.slerp( _foremost->orient(), GetTime() - _switchTime ).inv();
        }
        _cubeOrient.genMatrix((double*)_omatrix);
        glMultMatrixd( (double*)_omatrix );
        for ( int i = 0; i < 6; i++ ) { 
            if ( _faces[i] != _foremost ) 
                _faces[i]->drawFace();
        }
        _foremost->drawFace();
    }
    else { 
        _foremost->draw();
    }
    glPopMatrix();

}

void
AudiFace::drawFace() { 
    glPushMatrix();
        glMultMatrixd( (double *)_omatrix );
        glTranslated( 0, 0, -1.0 );
        draw();
    glPopMatrix();
}

void
AudiFace::draw() { 

}

void
AudiFace::drawLabel() { 
    glPushMatrix();
        glMultMatrixd( (double *)_omatrix );
        glTranslated( 0, 0, -1.0 );
        scaleFont(0.2, 1.0);
        draw();

    glPopMatrix();    
}