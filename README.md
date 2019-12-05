# AMPi

AMPi is a bare metal HDMI audio driver for the Raspberry Pi
written in C. It was ported from the 40th release of the
awesome [Circle](https://github.com/rsta2/circle) library.

## Interface

:construction: For specific usage, see _sample/_ for now.

AMPi requires support from the bare metal environment through
functions defined in _ampienv.h_. These include memory
allocation, co-operative scheduler, timer, interrupt handler,
and logging. See comments there for details.

To build the sample, run

```sh
cd sample
make -j4        # To play a sine wave note, or
cp path/to/music.ogg music.ogg
OGG=1 make -j4  # To play an Ogg audio file
```

## Usage

With the proper [toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
and GNU Make version 4.0 or higher, run

```sh
cd ampi
RASPPI=<1|2|3|4> AARCH=<32|64> make -j4
```

If omitted, parameters default to `RASPPI=1` and `AARCH=32`
respectively.

Then add the _ampi_ directory to the include path and pass
the produced file _libampi.a_ to the linker. Include _ampi.h_
to access functionalities provided by the library.

## Licence and Credits

AMPi is released under GPLv3+.

Portions of the effort goes to [Circle](https://github.com/rsta2/circle)
and consequently its dependencies and authors, including but not
limited to Linux (Linus Torvalds and the kernel contributors) and
VideoCore userland driver (Broadcom and Raspberry Pi Foundation).
