100::ms => dur T;

6 => int N;
int note;

"localhost" @=> string from;
"localhost" @=> string to;

// patch
ModalBar bar1 => JCRev r => dac;
ModalBar bar2 => r;
ModalBar bar3 => r;
ModalBar bar4 => r;

Wurley w => r;

gain oscAmp => r;

// set mix
.08 => r.mix;
// set preset
1 => bar1.preset;
2 => bar2.preset;
3 => bar3.preset;
4 => bar4.preset;

// Oscillators

5 => int numOsc;
2 => int numStream;
1 => float noteAdjust;
100. => float noteBase;
(3. / 2.) => float noteFactor;
0 => float oscAmpAdjust;

sinosc sOsc[numStream][numOsc];
Envelope env[numStream][numOsc];

for (int x; x < numStream; x++)
  for (int y; y < numOsc; y++)
     sOsc[x][y] => env[x][y] => oscAmp;

// create our OSC receiver
OscRecv recv;
// use port 6449
6449 => recv.port;
// start listening (launch thread)
recv.listen();
 
// create an address in the receiver, store in new variable
recv.event( "/plork/synch/ccr, f" ) @=> OscEvent oe;    

// send object
OscSend teleport_in;
OscSend teleport_out;

// aim the transmitter at our local port 6449
teleport_in.setHost( from, 6449 );
teleport_out.setHost( to, 6449 );

spork ~ clock();
spork ~ add_mouse();
spork ~ glissUp(0, noteBase, 1);
spork ~ volumes();

while( true )
{
    1::second => now;
/*
    AudicleMouseMania.go( T / second );

    for( int i; i < N; i++ )
    {
        AudicleMouseMania.get_item( i ) => note;
        if( note ) play( i, note );
    }

    AudicleMouseMania.get_teleport_in( ) => note;
    if( note ) <<< "teleport in:", note >>>;

    AudicleMouseMania.get_teleport_out( ) => note;
    if( note ) <<< "teleport out:", note >>>;

    T => now;
*/
}

fun void play0( int note )
{
    noteBase => w.freq => noteAdjust;
    .7 => bar1.noteOn;
}

fun void play1( int note )
{
    noteBase * noteFactor => w.freq => noteAdjust;
    .7 => bar1.noteOn;
}

fun void play2( int note )
{
    noteBase * 2 => w.freq => noteAdjust;
    .7 => bar2.noteOn;
}

fun void play3( int note )
{
    noteBase * 2 * noteFactor => w.freq => noteAdjust;
    .7 => bar3.noteOn;
}

fun void play4( int note )
{
    noteBase * 3 => w.freq => noteAdjust;
    .7 => bar4.noteOn;
}

fun void play5( int note )
{
    noteBase * noteFactor * 3 => w.freq => noteAdjust;
    .7 => w.noteOn;
}

fun void play( int which, int note )
{
    if( which == 0 ) play0( note );
    else if( which == 1 ) play1( note );
    else if( which == 2 ) play2( note );
    else if( which == 3 ) play3( note );
    else if( which == 4 ) play4( note );
    else if( which == 5 ) play5( note );
}

// receiver
fun void clock()
{
    // count
    0 => int count;
    float f;

    // infinite event loop
    while ( true )
    {
        // wait for event to arrive
        oe => now;

        // grab the next message from the queue.
        while( oe.nextMsg() != 0 )
        {
            // get x and y
            oe.getFloat() => f;
            // go
            AudicleMouseMania.go( f );

            for( int i; i < N; i++ )
            {
                AudicleMouseMania.get_item( i ) => note;
                if( note ) play( i, note );
            }

            AudicleMouseMania.get_teleport_in( ) => note;
            if( note ) send_to_low( note );

            AudicleMouseMania.get_teleport_out( ) => note;
            if( note ) send_to_high( note );
        }
    }
}

fun void add_mouse()
{
    // create our OSC receiver
    OscRecv recv;
    // use port 6449
    6449 => recv.port;
    // start listening (launch thread)
    recv.listen();
 
    // create an address in the receiver, store in new variable
    recv.event( "/plork/synch/ccr/add", "i i" ) @=> OscEvent oe;

    // listen
    while( true )
    {
        oe => now;
        while( oe.nextMsg() )
        {
            AudicleMouseMania.add_mouse( oe.getInt(), oe.getInt() );
        }
    }
}

fun void send_to_low( int note )
{
    // start the message...
    teleport_in.startMsg( "/plork/synch/ccr/add", "i i" );

    // a message is kicked as soon as it is complete
    // - type string is satisfied and bundles are closed
    1 => teleport_in.addInt;
    note => teleport_in.addInt;
}

fun void send_to_high( int note )
{
    // start the message....
    teleport_out.startMsg( "/plork/synch/ccr/add", "i i" );

    0 => teleport_out.addInt;
    note => teleport_out.addInt;
}



////////////////////////////////////////////////////////////////
//// Function: glissUp
////            stream is the group number of osc to use
////            Takes a startNote (frequency) and generates a
////              step-wise gliss.
////            stepSize (frequency) is the size of next step
////            stepNum is the number of steps

fun void glissUp (int stream, float startNote, int stepNum)
{

        [1, 7, 14, 21, 28] @=> int component[];
	.2 => float startGain;

        for (int y; y < numOsc; y++)
        {
          startGain * (1 / component[y]) => sOsc[stream][y].gain;
          1 => env[stream][y].keyOn;
        }

	startNote => noteAdjust;

        while (true)
        {
          noteAdjust  => float note;
	  AudicleMouseMania.get_num_mice() + 1 => stepNum;
          for ( int cnt; cnt < stepNum; cnt++ )
          {
            for (int y; y < numOsc; y++)
              note * component[y] => sOsc[stream][y].freq;
            oe => now;
            noteAdjust * noteFactor +=> note;
            if (stepNum < 2) 0 => oscAmp.gain; else oscAmpAdjust => oscAmp.gain;

          }
        }
}

fun void volumes ()
{
	while (true)
	{
	  AudicleMouseMania.get_volume() => r.gain;
	  AudicleMouseMania.get_volume2() => oscAmpAdjust;
	  1::ms => now;
	}
}
	
