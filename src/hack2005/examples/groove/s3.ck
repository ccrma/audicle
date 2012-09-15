// value of 8th
8192::samp => dur T;
// number of targets
1 => int targets;
// port
6449 => int port;

// send objects
OscSend xmit[16];

// aim the transmitter at port
// xmit[0].setHost( "tux", port );
// xmit[1].setHost( "anti-tragus", port );

xmit[0].setHost ( "kerplunk.local", port );
xmit[1].setHost ( "boom.local", port );
xmit[2].setHost ( "snarl.local", port );
xmit[3].setHost ( "squeak.local", port );
xmit[4].setHost ( "mumble.local", port );
xmit[5].setHost ( "purr.local", port );
xmit[6].setHost ( "clang.local", port );
xmit[7].setHost ( "whirr.local", port );
xmit[8].setHost ( "buzz.local", port );
xmit[9].setHost ( "moan.local", port );
xmit[10].setHost ( "gush.local", port );
xmit[11].setHost ( "splash.local", port );
xmit[12].setHost ( "snap.local", port );
xmit[13].setHost ( "hiss.local", port );
xmit[14].setHost ( "beep.local", port );
xmit[15].setHost ( "hush.local", port );

// get pane
AudicleGroove.pane() @=> AudiclePane @ pane;

int x;
int y;
int z;

// infinite time loop
while( true )
{
    for( 0 => y; y < pane.height(); y++ )
        for( 0 => x; x < pane.width(); x++ )
        {
            for( 0 => z; z < targets; z++ )
            {
                // start the message...
                xmit[z].startMsg( "/plork/synch/clock", "i i" );

                // a message is kicked as soon as it is complete 
                x => xmit[z].addInt; y => xmit[z].addInt;
            }

            // advance time
            T => now;
        }
}

