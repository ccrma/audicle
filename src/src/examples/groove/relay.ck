// number of targets
2 => int targets;
// port
6449 => int port;

// send objects
OscSend xmit[16];

// create our OSC receiver
OscRecv recv;
// set port
port => recv.port;
// start listening (launch thread)
recv.listen();

// create an address in the receiver, store in new variable
recv.event( "/plork/synch/clock, i i" ) @=> OscEvent oe;

// aim the transmitter at port
xmit[0].setHost( "scapha", port );
xmit[1].setHost( "anti-tragus", port );

int x;
int y;
int z;

// infinite time loop
while( true )
{
    // wait
    oe => now;

    // get
    while( oe.nextMsg() != 0 )
    {
        // store
        oe.getInt() => x;
        oe.getInt() => y;

        // send
        for( 0 => z; z < targets; z++ )
        {
            // start the message...
            xmit[z].startMsg( "/plork/synch/clock", "i i" );

            // a message is kicked as soon as it is complete 
            x => xmit[z].addInt; y => xmit[z].addInt;
        }
    }
}

