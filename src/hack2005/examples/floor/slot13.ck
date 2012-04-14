
// slot13.ck // looping slot machine sound simulator v.13 
// by Scott Smallwood, Oct-March 2006 or so

// model bar init

ModalBar bell1 => dac;
std.mtof( 96 ) => bell1.freq;
.3 => bell1.masterGain;
1 => bell1.preset;
.2 => bell1.stickHardness;
   
ModalBar bell2 => dac;
std.mtof( 79 ) => bell2.freq;
.3 => bell2.masterGain;
1 => bell2.preset;
.5 => bell2.stickHardness;
   
ModalBar bell3 => dac;
std.mtof( 84 ) => bell2.freq;
.3 => bell2.masterGain;
1 => bell2.preset;
.4 => bell3.stickHardness;

// seed trick
std.rand2f(.01,.09) => float seed;
seed::second => now;

// other inits

(1 + (1. / std.rand2(5,50))) => float fract;	
//time factor of randomization

20 => int initOdds;	//odds default
0 => int oddsAdjust;    //adjustment from outside
30 => int CR;		//beginning credits
1 => int CRbet;		//number of credits to bet
15 => int thresh1;       //threshold for changing sounds
5 => int thresh2;       //another threshold
OscSend xmit;
xmit.setHost("THINGFORIT.local", 6449);

// set credits
CR => AudicleFloor.credits;

3234535 => std.srand;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MAIN MAIN MAIN
// --------------

<<<"waiting for host contact...", " ">>>;
<<<".", " ">>>;
// checkforHost();

spork ~ getOddsAdjust();

<<<" ", " ">>>;
<<<"WELCOME!  You have ", CR, "credits.">>>;
<<<" ", " ">>>;

// message
AudicleFloor.message( "WELCOME!" );

while ( CR > 0) // big outer loop
{
  <<<"[ ", CR, " ] credits">>>;
  <<<" ", " ">>>;
  <<<"Hit keys 1-3", " to bet credits">>>;

  AudicleFloor.message( "ready..." );
  AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );

  oddsAdjust +=> initOdds;       //add adjustment
  if (initOdds < 1) 1 => initOdds; //not below 1 !!!
  <<<oddsAdjust, initOdds>>>;
  initOdds => int odds;  	//initials odds w/ default

  if( AudicleFloor.event() == null ) getCreditBet(CR) => CRbet;
  else getCreditBet2(CR) => CRbet; //call function to get keyboard input

  //adjust odds based on bet  
  if (CRbet == 1) 2 -=> odds; 
  if (CRbet == 3) 2 +=> odds;
  if (initOdds < 1) 1 => initOdds; //not below 1 !!!

  bellBetSound(CRbet); //call credit ding-ding function

  CRbet -=> CR; //subtract bet from total
  // audicle make sure
  CR => AudicleFloor.credits;

  //send credits and odds data to the monitor
  /*
  xmit.startMsg("slotter", "i i s");
  CR => xmit.addInt;
  odds => xmit.addInt;
  std.getenv("HOSTNAME") => xmit.addString;
  */

  <<<" ", " ">>>;
  <<<"Betting ", CRbet, " credits... ">>>;
  if( CRbet == 1 ) AudicleFloor.message( "betting 1..." );
  else if( CRbet == 2 ) AudicleFloor.message( "betting 2..." );
  else if( CRbet == 3 ) AudicleFloor.message( "betting 3..." );
  else AudicleFloor.message( "betting..." );

  if (CR > thresh1)
    rollingTriad(); //call triad arp. rolling function
  else if (CR > thresh2)
    rollingTriad(); //call triad arp. rolling function
  else
    caStream(); //call caStream function

  500::ms => now;
  
  // WINNINGS...

  std.rand2(0,odds) => int w;
  0 => int x;

  if (w == 1)
  {
    (std.rand2(1,10) * CRbet) => x;
    <<<"YOU WIN ", x, " CREDITS">>>;
    AudicleFloor.message( "YOU WIN!!!" );
    x +=> CR;
    bellWinCredits(x);
    1::second => now;  
  }

} // end of big loop

<<<"[ ", CR, " ] credits">>>;
<<<"                         ", " ">>>;
<<<"     -- finished --      ", " ">>>;
<<<" ", " ">>>;

1::second => now;

<<<"...distilling....", " ">>>;
<<<" ", " ">>>;

ccloud();

<<<"* * * * * * * * * * * * * * * *   D O N E", " ">>>;
<<<" ", " ">>>;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function checkforHost:  wait for prompt from host (remote machine)
// ---------------------     

fun void checkforHost( )
{
  OscRecv recv;
  6449 => recv.port;
  recv.listen();
  recv.event( "ready, i" ) @=> OscEvent oe;

  while (true)
  {

        oe => now;

        while (oe.nextMsg() != 0)
        {
	  if (oe.getInt() == 1) 
		return;
        }
  }   
}
 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function getOddsAdjust:  listen over network for odds adjustment   
// ---------------------

fun void getOddsAdjust( )
{
  OscRecv recv;
  6449 => recv.port;
  recv.listen();
  recv.event( "adjustment, i" ) @=> OscEvent oe;

  while (true)
  {

	oe => now;
	
	while (oe.nextMsg() != 0)
	{
	  oe.getInt() => int x;
	  x => oddsAdjust;
	}
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function getCreditBet:  get num of credits to bet
// ---------------------

fun int getCreditBet( int CR )
{ 
  
  // keyboard input

  49 => int a1;  // ascii value of "1" on keyboard, map to one credit
  50 => int a2;  // etc....
  51 => int a3;

  KBHit kb; 		//keyboard (user) in var


  // KEY KEYBOARD INPUT
  
  while( true )
  {
    // wait on kbhit event
     kb => now;
    
    // potentially more than 1 key at a time
    while( kb.more() )
    {
        kb.getchar() => int key;
        if (key > (a1 - 1) && key < (a3 + 1))
        {
                if (key == a1 && CR >= 1)
                {
                  return 1;
                }
                if (key == a2 && CR >= 2)
                {
                  return 2;
                }
                if (key == a3 && CR >= 3)
                {
                  return 3;
                }
                
        }
    }
  }  
}


fun int getCreditBet2( int CR )
{ 
  while( true )
  {
    // wait on audicle event
    AudicleFloor.event() => now;
    
    // get which
    AudicleFloor.which() => int key;
    
    if (key == 1 && CR >= 1)
    {
      return 1;
    }
    else if (key == 2 && CR >= 2)
    {
      return 2;
    }
    else if (key == 3 && CR >= 3)
    {
      return 3;
    }
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function bellBetSound:  ring bell for each credit bet
// ---------------------

fun void bellBetSound( int x )
{

  while ( x > 0 )
  {

   triosc b1 => Envelope ec1 => dac;
   triosc b2 => Envelope ec2 => dac;
   std.mtof( 84 ) => float f1;
   std.mtof( 96 ) => float f2;
   f1 => b1.freq;
   f2 => b2.freq;

   .3 => bell2.strike;
   .3 => bell3.strike;

   1 => ec1.keyOn;
   .3 => ec1.target;
   .002 => ec1.time;
   15::ms => now;
   0. => ec1.target;
   .10 => ec1.time;

   1 => ec2.keyOn;  
   .3 => ec2.target;
   .002 => ec2.time;
   30::ms => now;
   0. => ec2.target;
   .10 => ec2.time;

   AudicleFloor.addCredits( -1 );   
   (50 * fract)::ms => now;
   1 => ec1.keyOff;
   b1 =< ec1;
   b2 =< ec2;

   ec1 =< dac;
   ec2 =< dac;

   1 -=> x;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function rollingTriad:  creates arpeggiated rolling triad sound
// ---------------------

fun void rollingTriad()
{

  // c major triad array
  int c[4];

  60 => c[0];
  64 => c[1];
  67 => c[2];
  72 => c[3];

  20 => int x; //number of iterations
  triosc s => Envelope e => dac;
  std.mtof( c[std.rand2(0,3)] ) => s.freq;

  // envelop settings
  1 => e.keyOn;
  .8 => e.target;
  .01 => e.time;
  150::ms => now;

  <<<"   ...rolling...   ", " ">>>;

  while( x > 0 )
  {
    std.rand2(0,3) => int ff;
    std.mtof( c[ff] ) => float freq;
    freq => s.freq;
    1 -=> x;
    // audicle
    AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );
    (140 * fract)::ms => now;
  }

  0. => e.target;
  .01 => e.time;
  1 => e.keyOff;
  200::ms => now;
  s =< e;
  e =< dac;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function bellWinCredits:  creates winning bell sound
// ---------------------

fun void bellWinCredits ( int x )
{

  while ( x > 0 )
  {

   triosc b1 => Envelope ew1 => dac;
   triosc b2 => Envelope ew2 => dac;
   std.mtof( 84 ) => float f1;
   std.mtof( 96 ) => float f2;
   f1 => b1.freq;
   f2 => b2.freq;

   .4 => bell1.strike;
   .4 => bell2.strike;
   .4 => bell3.strike;

   1 => ew1.keyOn;
   .3 => ew1.target;
   .002 => ew1.time;
   25::ms => now;
   0. => ew1.target;
   .10 => ew1.time; 

   1 => ew2.keyOn;  
   .3 => ew2.target;
   .002 => ew2.time;
   25::ms => now;
   0. => ew2.target;
   .10 => ew2.time; 

   AudicleFloor.addCredits( 1 );
   AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );
   50::ms => now;

   1 => ew1.keyOff;
   b1 =< ew1;
   b2 =< ew2;

   ew1 =< dac;
   ew2 =< dac;

   1 -=> x;

  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function ostTriad:  creates a looped arp segment
// ---------------------
                  
fun void ostTriad()
{   
         
  // c major triad array
  int c[4];
   
  60 => c[0];
  64 => c[1];
  67 => c[2];
  72 => c[3];

  // random c member array
  int m[5];

  c[std.rand2(0,3)] => m[0];
  c[std.rand2(0,3)] => m[1];
  c[std.rand2(0,3)] => m[2];
  c[std.rand2(0,3)] => m[3];
  c[std.rand2(0,3)] => m[4];

  std.rand2(1,4) => int length;

  20 => int loops; //number of loops
  triosc s => Envelope e => dac;
  std.mtof( c[std.rand2(0,3)] ) => s.freq;
  
  // envelop settings
  1 => e.keyOn;
  .4 => e.target;
  .01 => e.time;
  150::ms => now;
   
  <<<"   ...looping...   ", " ">>>;
   
  while ( loops > 0 )
  {
   for( 0 => int x; x < length; x++ )
   {
     std.mtof( m[x] ) => float freq;
     freq => s.freq;
     1 +=> x;
     (140 * fract)::ms => now;
   }
   1 -=> loops;
  }
   
  0. => e.target;  
  .01 => e.time;
  1 => e.keyOff;  
  200::ms => now;   
  s =< e;
  e =< dac;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function caStream:  plays segments of field recordings (soundfiles)
// ---------------------
  

fun void caStream()
{

  // filename array
  string fName[15];

  "sounds/s1.aif" => fName[0];  
  "sounds/s2.aif" => fName[1];  
  "sounds/s3.aif" => fName[2];
  "sounds/s4.aif" => fName[3];
  "sounds/s5.aif" => fName[4];
  "sounds/s6.aif" => fName[5];
  "sounds/s7.aif" => fName[6];
  "sounds/s8.aif" => fName[7];
  "sounds/s9.aif" => fName[8];
  "sounds/s10.aif" => fName[9];
  "sounds/s11.aif" => fName[10];
  "sounds/s12.aif" => fName[11];
  "sounds/s13.aif" => fName[12];
  "sounds/s14.aif" => fName[13];
  "sounds/s15.aif" => fName[14];

  5::second => now; //wait a sec

  // audio chain
  sndbuf casBuf => Envelope e => dac;

  // seed trick
  std.rand2f(.01,.09) => float seed;
  seed::second => now;
 
  // read in file, randomly chosen from array
  std.rand2(0,14) => int fNum;
  fName[fNum] => casBuf.read;
  <<<"...playing file number ", fNum>>>;
      
  // set rate, gain, and find length
  1.0 => casBuf.rate;
  .8 => casBuf.gain;
  casBuf.length() => dur casBufLength;

  spork ~ loopBuf(casBuf, casBufLength);

  // open envelope
  1 => e.keyOn;
  .8 => e.target;
  10::second => e.duration;

  // sustain
  20::second => now;
      
  //close envelope
  1 => e.keyOff;
  0 => e.target;
  10::second => e.duration;

  // release
  10::second => now;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function loopBuf:  loop through the file
// ---------------------

fun void loopBuf(sndbuf sbuffer, dur slength)
{   
      
    while (true)
    {
      0 => sbuffer.pos;
      slength => now;
    }
  
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function c-cloud:  generates subtly random clouds of c major pitch memebers
// ---------------------

//  . ramps on, waits for keystroke, then fades
//  . 10 oscillators, harmonically close to each other

fun void ccloud()
{

   // seed trick for randomization
   std.rand2f(.01,.09) => float seed;
   seed::second => now;

   // c major triad array
   float c[4];

   std.mtof(60) => c[0];
   std.mtof(64) => c[1];
   std.mtof(67) => c[2];
   std.mtof(72) => c[3];

   // pick random c-major pitch member (c, e, g, c')
   std.rand2(0,3) => int noteChoose;

   // audio chain (gain to reverb to envelope)
   gain g => PRCRev r => Envelope e => dac;

   .1 => g.gain; // MAIN GAIN
   .5 => r.mix;  // mixture of reverb

   // frequencies for added sine tones
   // members of c-major, with beatings...

   sinosc o1 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) / 4 => o1.freq;
   sinosc o2 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) / 2 => o2.freq;
   sinosc o3 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) => o3.freq;
   sinosc o4 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 2 => o4.freq;
   sinosc o5 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 3 => o5.freq;
   sinosc o6 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 4 => o6.freq;
   sinosc o7 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) / 4 => o7.freq;
   sinosc o8 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) / 2 => o8.freq;
   sinosc o9 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) => o9.freq;
   sinosc o10 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 2 => o10.freq;
   sinosc o11 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 3 => o11.freq;
   sinosc o12 => g;
   std.rand2f(c[noteChoose] - 2, c[noteChoose] + 2) * 4 => o12.freq;

   // open envelope (attack)
   1 => e.keyOn;
   1 => e.target;
   5::second => e.duration;
   5::second => now;

   // sustain
   1::second => now;

   <<<"Press any key to END", " ">>>;
   AudicleFloor.message( "press any key to end..." );
   if( AudicleFloor.event() != null ) AudicleFloor.event() => now;
   else KBHit kb => now;  // wait for keystroke

   //close envelope (release)
   1 => e.keyOff;
   0 => e.target;
   10::second => e.duration;
   10::second => now;
}
