# ad9850-arduino

This is a fork of the AD9850 Arduino library which I found at 
[http://code.google.com/p/ad9850-arduino](http://code.google.com/p/ad9850-arduino).

I've hacked it around a reasonable amount since then, incompatibly changing the API
to make it easier to control multiple chips which share the same clock.

The reason behind all this was to generate signals for displaying Lissajous
figures on a scope.

See the examples/Dual sketch for a simple way to do that, or read about it
at [http://www.mjoldfield.com/atelier/2015/06/ad9850-lissajous.html](http://www.mjoldfield.com/atelier/2015/06/ad9850-lissajous.html).
