# AMPi Sample

A sample program demonstrating AMPi usage. Only works on
Raspberry Pi 1/0/0W.

To build, run:

```sh
make -j4
```

This will produce the image _kernel.img_.

The program plays a sine wave at 344.5 Hz for 2.97 seconds,
sleeps for 2 seconds, and repeats indefinitely. (These
numbers are not completely arbitrary and the reason behind
them is left to the reader as an exercise.)

### Ogg mode!

Find a **stereo** Ogg file with a length not exceeding 380
seconds and not too large in size — 16 MiB is completely safe.
Copy it to this directory and build with a flag:

```sh
cp path/to/music.ogg music.ogg
OGG=1 make -j4
```

On startup, the Act LED is turned on while the data is being
decoded, and when the LED goes off the audio will start to play.

## Credits

Parts of the environment is taken from Circle and simplified
a lot.

If you find this interesting and are willing to help port
this to other models, please get in touch through an issue
or an email!
