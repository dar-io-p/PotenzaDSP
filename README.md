# PotenzaDSP
PotenzaDSP JUCE Projects, gui library, etc.

## File Structure

/modules        #custom JUCE modules that are reused in the different projects 

/SimpleClip     #JUCE Project Folders 

/SimpleSub 

...            # other projects

/JUCE          # JUCE added as a submodule, tracking develop.

## Builds

### CMake

To use *CMake* to construct a configuration (using your preferred generator, i.e. Make, Ninja, Xcode, Visual Studio, etc.), use

`cmake -B Builds/<plugin> -G <generator> -DCMAKE_BUILD_TYPE=<type> <plugin>`,

in the root directory. For example, to construct a release Ninja configuration for *Dubber*, use

`cmake -B Builds/Dubber -G Ninja -DCMAKE_BUILD_TYPE=Release Dubber`,

and use

`ninja -CBuilds/Dubber`,

to build the plugin.

## Downloads

To download the latest binaries, head to my website

www.PotenzaDSP.com

## Plugins

- SimpleSub - Harmonic synthesiser for making sub basses. 
- SimpleClip - Variable soft/hard clipper with an easy to use interface
- PullUpMachine - stores a buffer that can be cycled through in order to get a vinyl wheel up effect
- Dubber - Dub siren with 2 pitch LFOs that can be blended together for interesting siren noises.
