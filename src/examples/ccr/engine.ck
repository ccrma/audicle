public class Engine
{
    fun void play( float note, float volume ) { }

    fun void stop() { }

    fun static void set( int index, Engine e )
    {
        if( index >= 0 && index < bank.cap() )
        {
            e @=> bank[index];
        }
        else
        {
            <<< "cannot set Engine on index:", index, "..." >>>;
        }
    }

    fun static void doplay( int i, float note, float volume )
    {
        if( i >= 0 && i < bank.cap() )
        {
            if( bank[i] != NULL ) bank[i].play( note, volume );
        }
    }

    fun static void dostop( int i )
    {
        if( i >= 0 && i < bank.cap() )
        {
            if( bank[i] != NULL ) bank[i].stop();
        }
    }

    static Engine @ bank[];
}

// create bank
Engine @ bank[8] @=> Engine.bank;

// create our OSC receiver
OscRecv recv;
// use port 6449 (or whatever)
6449 => recv.port;
// start listening (launch thread)
recv.listen();

// create an address in the receiver, store in new variable
recv.event( "/engine/play, i f f" ) @=> OscEvent oe;
recv.event( "/engine/stop, i" ) @=> OscEvent oe_stop;

spork ~ stop();

// infinite event loop
while( true )
{
    // wait for event to arrive
    oe => now;

    // grab the next message from the queue. 
    while( oe.nextMsg() )
    {
        // play
        Engine.doplay( oe.getInt(), oe.getFloat(), oe.getFloat() );
    }
}

// stop
fun void stop()
{
    // infinite event loop
    while( true )
    {
        // wait for event to arrive
        oe_stop => now;

        // grab the next message from the queue. 
        while( oe_stop.nextMsg() )
        {
            // play
            Engine.dostop( oe_stop.getInt() );
        }
    }
}

