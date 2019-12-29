# AMPi

AMPi is a bare metal HDMI audio driver for the Raspberry Pi
written in C. It was ported from the 40th release of the
awesome [Circle](https://github.com/rsta2/circle) library.

## Interface

AMPi requires support from the bare metal environment through
functions defined in [_ampi/ampienv.h_](ampi/ampienv.h). These
include memory allocation, timer, interrupt handler, mailbox,
and logging. See comments there for details.

AMPi's functionalities are exposed through 6 functions:

- **AMPiInitialize(sample_rate, chunk_size)** — Initialization
- **AMPiSetChunkCallback(callback)** — Set audio data callback
- **AMPiStart()**, **AMPiStop()**, **AMPiIsActive()** — As the names suggest
- **AMPiPoke()** — Call this periodically to keep audio running

See [_sample/_](_sample/_) for a demonstration.

## Usage

With the proper [toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
and GNU Make version 4.0 or higher, run

```sh
cd ampi
RASPPI=<1|2|3|4> AARCH=<32|64> make -j4
```

If omitted, parameters default to `RASPPI=1` and `AARCH=32`
respectively.

Add the _ampi/_ directory to the include path and pass
the produced file _libampi.a_ to the linker. Include _ampi.h_
to access functionalities provided by the library.

### Troubleshooting

**"failed to set channelbase"**: Make sure data is correctly
transferred through the mailbox in **EnableVCHIQ()**.

**Data abort at an ldrex/strex instruction**: Make sure that
function **ampi_malloc()** returns Normal memory. See the ARM
Architectural Reference Manual for details. For ARMv6, relevant
information resides on page B4-12.

**Initialization succeeded, but no audio output**: Check that
**AMPiPoke()** is indeed being called periodically. Also, make
sure that calls to AMPi functions are surrounded with memory
barrier instructions like is done for any other peripheral.

## Caveats

Although the library is expected to work on all models of Pi,
models other than the Zero have not been tested. If interested,
please help by sending your success or failure report!

## Licence and Credits

AMPi is released under GPLv3+.

Portions of the effort goes to [Circle](https://github.com/rsta2/circle)
and consequently its dependencies and authors, including but not
limited to Linux (Linus Torvalds and the kernel contributors) and
VideoCore userland driver (Broadcom and Raspberry Pi Foundation).
