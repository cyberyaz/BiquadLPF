Biquad LPF

Stereo lowpass filter plugin in JUCE. Wrote the coefficients myself from Will
Pirkle's Designing Audio Effect Plugins in C++ instead of using the dsp module
to understand the math.

Controls:
- cutoff (20 hz - 20 khz) 
- resonance (Q 0.5 - 10)

Math
y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2] - (a1/a0)*y[n-1] - (a2/a0)*y[n-2]

w0    = 2π * (cutoff / sampleRate)
alpha = sin(w0) / (2 * Q)

b0 = (1 - cos(w0)) / 2;
b1 =  1 - cos(w0);
b2 = (1 - cos(w0)) / 2;
a0 =  1 + alpha;
a1 = -2 * cos(w0);
a2 =  1 - alpha;

separate state variables per channel, reset in prepareToPlay

To Build:
- Get JUCE. open the .jucer file, set your JUCE path, save and open in Xcode or Visual Studio.

- macOS: output goes to ~/Library/Audio/Plug-Ins/VST3/ or ~/Library/Audio/Plug-Ins/Components/ for AU
- Windows: output goes to C:\Program Files\Common Files\VST3\

- rescan in your DAW after building

References:
Will Pirkle, Designing Audio Effect Plugins in C++ (Focal Press, 2nd ed.)
