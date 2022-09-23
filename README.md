# TTL Debugging Tools Plugin

## Overview

This plugin offers a set of widgets to make TTL event debugging easier:

* A toggle panel manually generates TTL events.

* A front panel display shows the current state of TTL inputs.

* **(NYI)** A pattern generator allows sequences of pulses or words to be
generated.

* **(NYI)** A logic analyzer display allows TTL waveforms or code sequences
to be viewed on a strip-chart.


## Limitations

* The toggle panel and front panel are not visualizers; they're limited to
a small number of channels (32), to allow display in the plugin bar.

* The number of bits per bank in the toggle panel and front panel is limited
to 64, due to internally storing bank state as a uint64 integer.

* **(NYI)** The pattern generator only supports up to 64 outputs, due to
internally storing output state as a uint64 integer.


## Notes

### Toggle Panel

* The toggle panel creates one TTL event channel per bank, with one virtual
channel per bit.

### Front Panel

* The front panel concatenates all input TTL channels and maps the resulting
unified bit series to the front panel display.



_(This is the end of the file.)_
