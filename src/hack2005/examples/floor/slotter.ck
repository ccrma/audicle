// slotter.ck // looping slot machine sound simulator v.16a 
// by Scott Smallwood, Oct-March 2006 or so

AudicleFloor.message( "loading..." );

// model bar init
ModalBar bell1 => dac;
std.mtof( 84 ) => bell1.freq;
.8 => bell1.masterGain;
1 => bell1.preset;
.9 => bell1.stickHardness;
   
ModalBar bell2 => dac;
std.mtof( 84 ) => bell2.freq;
.8 => bell2.masterGain;
1 => bell2.preset;
.9 => bell2.stickHardness;
   
ModalBar bell3 => dac;
std.mtof( 96 ) => bell3.freq;
.8 => bell3.masterGain;
1 => bell3.preset;
.8 => bell3.stickHardness;

//triosc inits
triosc b1 => Envelope ew1 => dac; //bell osc1
triosc b2 => Envelope ew2 => dac; //bell osc2
triosc s => Envelope e => dac; //rolling osc


// seed trick
std.rand2f(.01,.09) => float seed;
seed::second => now;

// other inits
(1 + (1. / std.rand2(5,50))) => float fract;	//time factor of randomization
20 => int odds;    	//init for odds
30 => int CR;		//beginning credits
1 => int CRbet;		//number of credits to bet
15 => int thresh1;      //threshold for changing sounds
5 => int thresh2;       //another threshold

//OSC stuff
OscSend xmit;
xmit.setHost("THINGFORIT.local", 6449);

// set credits
CR => AudicleFloor.credits;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MAIN MAIN MAIN
// --------------

//always look for new odds from network
spork ~ getOddsAdjust();

<<<" ", " ">>>;
<<<"WELCOME!  You have ", CR, "credits.">>>;
<<<" ", " ">>>;

// message
AudicleFloor.message( "WELCOME!" );
int audicle_rotate;

while ( CR > 0) // big outer loop
{
  <<<"[ ", CR, " ] credits">>>;
  <<<" ", " ">>>;
  <<<"Hit keys 1-3", " to bet credits">>>;

  AudicleFloor.message( "ready..." );
  AudicleFloor.bgcolor(0,0,0 );

  if( AudicleFloor.event() == null ) getCreditBet(CR) => CRbet;
  else getCreditBet2(CR) => CRbet; //call function to get keyboard input

  // comment out the next few lines to NOT rotate
  if( audicle_rotate ) AudicleFloor.angle( -360 * std.rand2(1,4), std.rand2f(.99,.995) );
  else AudicleFloor.angle( 360 * std.rand2(1,4), std.rand2f(.99,.995) );
  !audicle_rotate => audicle_rotate;

  bellRing(CRbet, .2, .5, .5, 84, 96, 1); //call credit ding-ding function

  CRbet -=> CR; //subtract bet from total
  // audicle make sure
  CR => AudicleFloor.credits;

  //send credits and odds data to the monitor
  xmit.startMsg("bet", "i i i s");
  CR => xmit.addInt;
  odds => xmit.addInt;
  CRbet => xmit.addInt;
  std.getenv("USER") => xmit.addString;

  <<<" ", " ">>>;
  <<<"Betting ", CRbet, " credits... ">>>;
  if( CRbet == 1 ) AudicleFloor.message( "betting 1..." );
  else if( CRbet == 2 ) AudicleFloor.message( "betting 2..." );
  else if( CRbet == 3 ) AudicleFloor.message( "betting 3..." );
  else AudicleFloor.message( "betting..." );

  if (CR > thresh1)
    rollingTones(3, fract); //call triad arp. rolling function
  else if (CR > thresh2)
    rollingTones(4, fract); //call triad arp. rolling function
  else
    caStream(); //call caStream function

  500::ms => now;
  
  // WINNINGS...

  std.rand2(0,odds) => int w;
  0 => int x;

  if (w == 0)
  {
    (std.rand2(1,10) * CRbet) => x;
    <<<"YOU WIN ", x, " CREDITS">>>;
    AudicleFloor.message( "YOU WIN!!!" );
    x +=> CR;

   //send info to monitor
   xmit.startMsg("win", "i i i s");
   x => xmit.addInt;
   0 => xmit.addInt;
   0 => xmit.addInt;
   std.getenv("USER") => xmit.addString;

   bellRing(x, .3, .4, .5, 72, 96, 0);
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
	  x => odds;
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
// Function bellRing:  creates bell sound. 
//   args: iterations, bell strike vel, triosc frequencies
// ---------------------

fun void bellRing ( int x, float bs1, float bs2, float bs3, int triFreq1, int
triFreq2, int status ) {

  std.mtof( triFreq1 ) => b1.freq;
  std.mtof( triFreq2 ) => b2.freq;

  while ( x > 0 )
  {

   bs1 => bell1.strike;
   bs2 => bell2.strike;
   bs3 => bell3.strike;
   std.rand2f(.3, .35) => bell1.stickHardness;
   std.rand2f(.4, .45) => bell2.stickHardness;
   std.rand2f(.3, .36) => bell3.stickHardness;


	// tri envel attack-sus
   1 => ew1.keyOn;
   .3 => ew1.target;
   .002 => ew1.time;
   1 => ew2.keyOn;  
   .3 => ew2.target;
   .002 => ew2.time;
   25::ms => now;
	
	// tri envel release
   0. => ew1.target;
   .25 => ew1.time;
   0. => ew2.target;
   .25 => ew2.time;
   25::ms => now;

	// done
   1 => ew1.keyOff;
   1 => ew2.keyOff;

   //check for win or bet
   if (status == 1) //bet
   {
   	AudicleFloor.addCredits( -1 );
   	AudicleFloor.bgcolor(1,1,1);
   }
   else //win
   {
        AudicleFloor.addCredits( 1 );
        AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );
   }


   50::ms => now;

   1 -=> x;

  }

  500::ms => now;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function rollingTones:  creates arpeggiated rolling triad sound
// ---------------------

fun void rollingTones(int pitchExp, float fract)
{

  // c major triad array
  int c[5];

  60 => c[0];
  64 => c[1];
  67 => c[2];
  72 => c[3];
  62 => c[4];

  20 => int x; //number of iterations
  .5 => s.gain; //volume
  std.mtof( c[std.rand2(0,3)] ) => s.freq;

  // envelope ramp up
  1 => e.keyOn;
  .8 => e.target;
  .01 => e.time;
  100::ms => now;

  <<<"   ...rolling...   ", " ">>>;

  while( x > 0 )
  {
    std.mtof( c[std.rand2(0,pitchExp)] ) => s.freq;
    1 -=> x;
	// audicle color shifts
    AudicleFloor.bgcolor( std.rand2f(0,1), std.rand2f(0,1), std.rand2f(0,1) );
    (140 * fract)::ms => now;
  }

  // envelope ramp down
  0. => e.target;
  .01 => e.time;
  20::ms => now;
  1 => e.keyOff;
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

  2::second => now; //wait a sec

  //sample buffer init
  sndbuf casBuf => Envelope ce => dac;

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
  1 => ce.keyOn;
  .8 => ce.target;
  10::second => ce.duration;

  // sustain
  20::second => now;
      
  //close envelope
  1 => ce.keyOff;
  0 => ce.target;
  10::second => ce.duration;

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

   .3 => g.gain; // MAIN GAIN
   .5 => r.mix;  // mixture of reverb
   .1 => r.gain;

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

   // move it away
   AudicleFloor.translate( -10, .999 );

   // open envelope (attack)
   1 => e.keyOn;
   1 => e.target;
   5::second => e.duration;
   5::second => now;

   // sustain
   1::second => now;

   <<<"Press any key to END", " ">>>;
   AudicleFloor.message( "any key..." );
   if( AudicleFloor.event() != null ) AudicleFloor.event() => now;
   else KBHit kb => now;  // wait for keystroke
   AudicleFloor.message( "ending..." );

   // move back
   AudicleFloor.translate( 0, .999 );

   //close envelope (release)
   1 => e.keyOff;
   0 => e.target;
   10::second => e.duration;
   10::second => now;

   AudicleFloor.message( "stopped..." );
}
