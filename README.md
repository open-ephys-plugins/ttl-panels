# TTL Debugging Tools Plugin

## Overview

This plugin offers a set of widgets to make TTL event debugging easier:

* A toggle panel manually generates TTL events.

* A front panel display shows the current state of TTL inputs.

Planned but not implemented yet:

* _(NYI)_ A pattern generator allows sequences of pulses or words to be
generated.

* _(NYI)_ A logic analyzer display allows TTL waveforms or code sequences
to be viewed on a strip-chart.


## Limitations

* The toggle panel and front panel are not visualizers; they're limited to
a small number of TTL channels (32), to allow display in the plugin bar.

* The number of bits per bank in the toggle panel and front panel is limited
to 64, due to internally storing bank state as a uint64 integer.

* _(NYI)_ The pattern generator only supports up to 64 outputs, due to
internally storing output state as a uint64 integer.


## Notes

### Toggle Panel

* The toggle panel creates one physical TTL event channel per bank, with one
virtual channel per bit.

### Front Panel

* Any given bank shows data from only one physical input channel.

  For example, if there are two physical input channels with two and three
virtual bit channels (respectively), the first bank will show the first
channel's bits as its 2 least-significant bits, with the remaining positions
off, and the second bank will show the second channel's bits as its 3
least-significant bits, with the remaining positions off.



_(This is the end of the file.)_
