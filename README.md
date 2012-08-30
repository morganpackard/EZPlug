EZPlug
======

EZPlug is a baby C++ library designed to make audio synthesis in C++ faster, easier, and more fun. Right now it's used by pretty much only Morgan Packard, but he'd love for you to use it too and to help him make it better. Morgan realizes that in its current state it's going to be very hard for anyone to understand. Please encourage him to document it and make tutorials.

EZPlug is built on top of the STK library, a collection of low-level tools for audio synthesis. While STK is great with the nuts and bolts of synthesis, it isn't designed for easy modularity between unit generators. EZPlug aims to be a slightly higher-level "glue" which can be used to easily connect STK classes.

Another thing about all of this: it's my first foray in to C++, and I've taken what many people may consider a weird approach. I suspect there are many ways to do what I'm doing better. I'm very open to hearing what those ways are.

The plan is for EZPlug to have a very permissive license. I personally will be using it for commercial development and would be delighted to have others doing the same. 

#Who is EZPlug For?
EZPlug is for people who: want to build high performance synthesizers in to their software, but don't like patching, so don't want to use PD.

#Why use EZPlug rather than straight STK?

EZPlug is designed for efficiency. It assumes that you want to calculate your samples in batches, rather than one sample at a time. This can lead to dramatic increases in performance, but can be a little clumsy to code. EZPlug hides the clumsiness and lets you focus on synthesis.

EZPlug is designed for fast development. EZPlug will manage your synthesis network for you. No need to create instance variables for generator objects -- EZPlug will keep track of them for you. No need to create getter and setter methods for parameters. EZPlug's FixedValue and Triggerable objects allow you to set parameters and send trigger messages based on parameter names.

#Where to Start
The best way to get an idea of how EZPlug works is to look at [EZPlugDemoSynth.h](https://github.com/morgancodes/EZPlug/blob/master/EZPlugDemo/EZPlgDemoSynth.h)




#Classes:

###EZPlugBase

The base class for an EZPlug synth. The main purpose of EZPlugBase is to provide convenience methods for synth building. The most significant of these methods are the storage methods *a* and *f*. Both the *a* and *f* methods take and return a pointer as an argument. "A" is an abbreviation for "add", and can be used when creating an object which won't need to be accessed while the synth is running. For example, a Panner object can often be created, configured, then forgotten about. In that case you can use a(new Panner())



###Generator

The fundamental building block of an EZPlug synthesizer. Everything that generates samples is a subclass of EZPlugGenerator. EZPlugGenerator can be plugged in to EZPlugProcessors. This class is part of the stk library.

###EZPlugProcessor
 
An EZPlugGenerator that can accept one or more EZPlugGenerators as input.

###Mixer

Adds the output of two or more Generators together.

###Multiplier 

Multiples the output of two or more Generators together

###Panner

Takes a Generator whose output is mono and outputs a stereo signal.

###Enveloper

Apply an envelope to a signal. Restart the envelope using the *trigger* method.

