// value of 8th
100::ms => dur T;
// number of targets
2 => int targets;
// port
6449 => int port;

// send objects
OscSend xmit[16];

// aim the transmitter at port
xmit[0].setHost ( "localhost", port );
xmit[1].setHost ( "hiss.local", port );

int z;

// spork ~ midi( 1 );

// infinite time loop
while( true )
{
            for( 0 => z; z < targets; z++ )
            {
                // start the message...
                xmit[z].startMsg( "/plork/synch/ccr", "f" );

                // a message is kicked as soon as it is complete 
                T / second => xmit[z].addFloat;
            }

            // advance time
            T => now;
}

fun void midi( int device )
{
    // devices to open (try: chuck --probe)
    MidiIn min;

    // open the device
    if( !min.open( device ) ) return;

    // the message 
    MidiMsg msg;

    // infinite event loop
    while( true )
    {
        // wait on event
        min => now;

        // print message
        while( min.recv( msg ) )
        {
            if( msg.data1 == 144 && msg.data3 != 0 )
                AudicleMouseMania.add_mouse( 0, msg.data2 );
            else if( msg.data1 == 176 && msg.data2 == 11 )
                ( 1 + 99 * msg.data3 / 64.0 )::ms => T;
        }
    }
}
