# TTL Debugging Tools - Unsorted To-Do List

## Toggle Panel

* Add controls to select positive pulse, negative pulse, or toggle when
clicking bit buttons.

  _Note: This will involve making the plugin schedule future events._

* Get nicer colours working for the buttons.

* Maybe allow a large windowed version?


## Front Panel

* Redo the front panel display to use boxes instead of buttons for bits.

* Maybe allow a large windowed version?


## Pattern Generator

* NYI.

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

* NYI.

* This should be able to group bits arbitrarily, and to parse grouped bits
into signed or unsigned words.

* It should be easy for the user to specify "this range, big-endian" and
"this range, little-endian".

* This should give a nice strip chart of TTL bits and code words.

* This should emit the shuffled bits and words as channels to let the user
easily save decoded data.


_(This is the end of the file.)_
