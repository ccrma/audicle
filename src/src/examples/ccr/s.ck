// value of 8th
100::ms => dur T;

// send object
OscSend xmit;

// aim the transmitter at our local port 6449
xmit.setHost( "localhost", 6449 );

spork ~ midi( 1 );

// infinite time loop
while( true )
{
        {
            // start the message...
            xmit.startMsg( "/plork/synch/ccr", "f" );

            // a message is kicked as soon as it is complete 
            // - type string is satisfied and bundles are closed
            T / second => xmit.addFloat;

            // advance time
            T => now;
        }
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
