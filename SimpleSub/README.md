# SimpleSub
 
## A basic sine wave synthesiser for making big sub basses.

## Architecture:

The synth consists of 
 - Envelope
  - ADSR + curve shape control for attack, decay, and release.
 - FX Section
  - Width - applies a chorus to all harmonics except for the fundamental
  - Distortion - Sine, Cubic, and Tanh clipping functions for harmonic distortion
 - Harmonics
  - Gain for the first 5 harmonics of input frequency.
 - Glide
 - Master Gain Out
   
