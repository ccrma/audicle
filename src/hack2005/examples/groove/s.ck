// value of 8th
4096::samp => dur T;

// send object
OscSend xmit;

// aim the transmitter at our local port 6449
xmit.setHost( "localhost", 6449 );

// get pane
AudicleGroove.pane() @=> AudiclePane @ pane;

int x;
int y;

T/2 => now;

// infinite time loop
while( true )
{
    for( 0 => y; y < pane.height(); y++ )
        for( 0 => x; x < pane.width(); x++ )
        {
            // start the message...
            xmit.startMsg( "/plork/synch/clock", "i i" );

            // a message is kicked as soon as it is complete 
            // - type string is satisfied and bundles are closed
            x => xmit.addInt; y => xmit.addInt;
            // <<< "sent (via OSC):", x, y >>>;

            // advance time
            T => now;
        }
}
