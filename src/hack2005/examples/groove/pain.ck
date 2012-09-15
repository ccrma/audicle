// Groove face test

// get pane
AudicleGroove.pane() @=> AudiclePane @ pane;

int x;
int y;

// make things glow
while( true )
{
    for( 0 => y; y < pane.height(); y++ )
        for( 0 => x; x < pane.width(); x++ )
        {
            <<< x, y, "value:", pane.getvalue( x, y ) >>>;
            pane.setglow( x, y, 1 );
            100::ms => now;
        }
}
