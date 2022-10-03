# TTL Debugging Tools - Unsorted To-Do List

## Toggle Panel

* Add controls to select positive pulse, negative pulse, or toggle when
clicking bit buttons. Add a combo box or cycling button for pulse length
(e.g. 1/3/10/30/100/300/1000/3000 ms).

  _Note: This will involve making the plugin schedule future events._

  _The easy way to do this is to add a "countdown" array. Clicking in pulse
mode toggles the state, then sets a countdown for toggling the state a second
time._

* Let the user group banks, and have each group of banks start at bit 0.
Bank groups get letters to identify them.

  Right now, we have one bank per row, treated as a separate TTL event
stream for each bank. With 8-bit banks, we can't get 16-bit output, for
example.

* Move everything that's _not_ shared into the child class, rather than
having a do-everything parent class.


## Front Panel

* Visually indicate which banks are grouped together (i.e. which banks
correspond to the same input stream). Have each group of banks start at
bit 0, and give bank groups letters to identify them.

* Move everything that's _not_ shared into the child class, rather than
having a do-everything parent class.


## Pattern Generator

* NYI; planned future widget.

* This should be able to group bits arbitrarily, and to schedule repeating
patterns of output words for each group.

* Possibly specify a global repetition period instead of making the user
add up delays. This makes it easier to synchronize groups.

* Should be able to trigger a one-shot pattern, or to keep repeating while
an input is high (toggle held down). The sequence should always still
complete.

* Manual control is pushbutton vs toggle button for the previous point.
Automated control is a TTL input (active-high, active-low, rising-edge,
falling-edge for two continuous and two one-shot cases).


## Logic Analyzer

* NYI; planned future widget.

* This should be able to group bits arbitrarily, and to parse grouped bits
into signed or unsigned words.

* It should be easy for the user to specify "this range, big-endian" and
"this range, little-endian".

* This should give a nice strip chart of TTL bits and code words.

* This should emit the shuffled bits and words as channels to let the user
easily save decoded data.


_(This is the end of the file.)_
