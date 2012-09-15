// groove receiver

// patch
SndBuf s[9];
JCRev r => dac;
for( int i; i < s.cap(); i++ ) 
{ s[i] => r; 0 => s[i].gain; }
.025 => r.mix;

"../data/kick.wav" => s[0].read;
"../data/kick.wav" => s[1].read;
"../data/snare-chili.wav" => s[2].read;
"../data/snare-hop.wav" => s[3].read;
"../data/snare.wav" => s[4].read;
"../data/hihat.wav" => s[5].read;
"../data/hihat-open.wav" => s[6].read;
"../data/hihat-open.wav" => s[7].read;

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
    if( value == 0 ) return;

    // <<< "playing:", value >>>;
    1.0 => s[value].gain;
    0 => s[value].pos;
}
