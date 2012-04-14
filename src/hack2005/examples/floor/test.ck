sinosc s => JCRev r => dac;
.1 => r.mix;
50 => AudicleFloor.credits;

fun void crazy()
{
    while( true )
    {
        std.rand2(-1,1) => AudicleFloor.addCredits;
        if( AudicleFloor.credits() < 0 ) 0 => AudicleFloor.credits;
        100::ms => now;
    }
}

spork ~ crazy();

while( true )
{
    AudicleFloor.event() => now;
    500 * AudicleFloor.which() => s.freq;
    AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );
}
