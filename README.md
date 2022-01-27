# What is this repo ??

This is the code for a realtime convolutionall reverb (VST for linux and windows) that I wrote for fun.

# How can I use it ?
Well you shouldn't, even though the dsp/realtim part is working fine ther might still be few crashes when loading/unloading impulse responses. The state of the plugin isn't saved when closing reopenning your daw...

If you still want to give this plugin a go there's a vst3 under the build folder it should work fine on your device.

# How does it work ?

I plan on writing a few blogposts explaining the developpement of such plugins because there are not much documentation on working implementations of Conv-Reverbs online.

For now the quick explenation is : this is an implementation of a variation of the overlapp and add method for convolution using ffts for convolving. the partition currently have fixed sizes but this could very be easily changed.

# Licensing
I should add licensing to the project since I also use some code that isn't mine for now the license will be :
The code isn't for you to use (if you're part of a company) though I won't be running after you if you do.
If it is for the sole purpose of you having fun and you wont be using this code to be used in a product of any company or firm, then why tf are you reading this ???
