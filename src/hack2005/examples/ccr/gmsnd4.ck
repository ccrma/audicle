// gmsound.ck

// Audio Outs

PRCRev mainVerb => gain mainAmp => dac;
.00 => mainVerb.mix;
.00 => mainAmp.gain;

// Oscillators (three groups of five)

5 => int numOsc;
3 => int numStream;

sinosc sOsc[numStream][numOsc];
Envelope env[numStream][numOsc];

for (int x; x < numStream; x++)
  for (int y; y < numOsc; y++)
     sOsc[x][y] => env[x][y] => mainVerb;

// Keyboard controls

117 => int keyVolup;                    // [r] vol down
114 => int keyVoldown;                  // [u] vol up
106 => int keyNotedown;                // [j] start adjust down
107 => int keyNoteup;                  // [k] start pitch adjust up
102 => int keyStepdown;                // [j] start step adjust down
100 => int keyStepup;                  // [k] start step adjust up
118 => int keyCompdown;                // [j] start component adjust down
109 => int keyCompup;                  // [k] start component adjust up

// KNOBS initialization
1.0 => float noteAdjust => float stepAdjust => float compAdjust;

// look for keyboard input
spork ~ getKeys();

// steams of sound...
spork ~ glissUp(0, 3000, 304, 6, .33, 15::ms);
spork ~ glissUp(1, 300, 30, 6, .33, 30::ms);
spork ~ glissUp(2, 200, 100, 4, .33, 60::ms);

while (true) 1::ms => now;


////////////////////////////////////////////////////////////////
//// Function: glissUp
////		stream is the group number of osc to use
////		Takes a startNote (frequency) and generates a
////		  step-wise gliss.  
////		stepSize (frequency) is the size of next step
////		stepNum is the number of steps
////		startGain is init vol, Q is time chunk

fun void glissUp (int stream, float startNote, float stepSize, int stepNum, float startGain, dur Q)
{

	[1, 7, 14, 21, 28] @=> int component[];

	for (int y; y < numOsc; y++)
	{
  	  startGain * (1 / component[y]) => sOsc[stream][y].gain;
	  1 => env[stream][y].keyOn;
	}
	
	while (true)
	{
	  startNote * noteAdjust => float note;
	  for ( int cnt; cnt < stepNum * stepAdjust; cnt++ ) 
	  {
	    for (int y; y < numOsc; y++)	
  	      note * (component[y] * compAdjust) => sOsc[stream][y].freq;
	    Q => now;
	    stepSize +=> note;
	  }
	}
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Function getKeys:  keyboard controls
// ---------------------

fun void getKeys()
{

  KBHit kb;

  while( true )
  {
          kb => now;
    // potentially more than 1 key at a time
    while( kb.more() )
    {
        kb.getchar() => int key;

        //** check for down/up keys **//

        if (key == keyVolup)
          if (mainAmp.gain() < 1.0)
                mainAmp.gain() + .005 => mainAmp.gain;;

        if (key == keyVoldown)
          if (mainAmp.gain() > 0.005)
                mainAmp.gain() - .005 => mainAmp.gain;

        if (key == keyNoteup)
          if (noteAdjust < 2000)
                .005 +=> noteAdjust;

        if (key == keyNotedown)
          if (noteAdjust > 1.0)
                .005 -=> noteAdjust;

        if (key == keyStepup)
          if (stepAdjust < 2000)
                .005 +=> stepAdjust;

        if (key == keyStepdown)
          if (stepAdjust > 1.0)
                .005 -=> stepAdjust;

        if (key == keyCompup)
          if (stepAdjust < 2000)
                .005 +=> compAdjust;

        if (key == keyCompdown)
          if (stepAdjust > 1.0)
                .005 -=> compAdjust;

    }
  }
}



