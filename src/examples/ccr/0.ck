// (1) ------> this number chooses where to store <------------
0 => int index;

// (2) --> change the class name below to reflect filename <--
public class S0 extends Engine
{

// (3) -------> put global data, including ugens below <------

//------------------------------------------------------------

    // (4) -----------> implement this function <-------------
    fun void play( float freq, float volume )
    {    <<< freq, volume >>>;
    }


    // (5) -------> implement this function too <-------------
    fun void stop()
    {    <<< "stop!" >>>;
    }
}


// (6) ---> change the name below to reflect class name <-----
Engine.set( index, new S0 );

// advance time
while( true ) 1::second => now;
