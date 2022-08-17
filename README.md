# TTL Debugging Tools Plugin

## Overview

This plugin offers a set of widgets to make TTL event debugging easier:

* A toggle panel manually generates TTL events.

* A front panel display shows the current state of TTL inputs.

* **(NYI)** A pattern generator allows sequences of pulses or words to be
generated.

* **(NYI)** A logic analyzer display allows TTL waveforms or code sequences
to be viewed on a strip-chart.


## Limitations:

* The minimized versions of the toggle panel and front panel show only a
small number of TTL channels (16).

* The pattern generator only supports up to 64 outputs, due to internally
storing output state as a uint64_t integer.



_(This is the end of the file.)_
