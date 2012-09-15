// groove receiver

// patch
SndBuf s[9];
JCRev r => dac;
for( int i; i < s.cap(); i++ ) 
{ s[i] => r; 0 => s[i].gain; }
.01 => r.mix;

"data/silence.wav" => s[0].read;
"data/q.wav" => s[1].read;
"data/u.wav" => s[2].read;
"data/i.wav" => s[3].read;
"data/a.wav" => s[4].read;
"data/r.wav" => s[5].read;
"data/k.wav" => s[6].read;
"data/g.wav" => s[7].read;
"data/l.wav" => s[8].read;

// create our OSC receiver
OscRecv recv;
// use port 6449
6449 => recv.port;
// start listening (launch thread)
recv.listen();

// create an address in the receiver, store in new variable
recv.event( "/plork/synch/clock, i i" ) @=> OscEvent oe;

// get pane
AudicleGroove.pane() @=> AudiclePane @ pane;

// <<< pane.width(), pane.height() >>>;

int x;
int y;
int value;

// infinite event loop
while ( true )
{
    // wait for event to arrive
    oe => now;

    // grab the next message from the queue. 
    while( oe.nextMsg() != 0 )
    {
        // get x and y
        oe.getInt() => x;
        oe.getInt() => y;
        // set glow
        pane.setglow( x, y, 1 );
        // get value
        pane.getvalue( x, y ) => value;
        // play the thing
        play( value );
    }
}

// do something
fun void play( int value )
{
[[.5, 0.2, 0.3, 0.2, 0.4, 0.1, 0.2, 0.1],
[0.5, 0.1, 0.3, 0.2, 0.4, 0.1, 0.2, 0.1],
[0.5, 0.1, 0.3, 0.2, 0.5, 0.1, 0.2, 0.1],
[0.4, 0.1, 0.3, 0.2, 0.3, 0.1, 0.2, 0.1]] @=> float mygains[][];
float temp;

    if( value == 0 ) return;

    0.2 + std.rand2f(0.0,mygains[y][x]*2) => temp;
    temp => s[value].gain;
    0 => s[value].pos;
}
