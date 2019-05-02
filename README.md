# mruby-gpiotft   [![Build Status](https://travis-ci.org/yamori813/mruby-gpiotft.svg?branch=master)](https://travis-ci.org/yamori813/mruby-gpiotft)
GpioTft class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'yamori813/mruby-gpiotft'
end
```
## example
```ruby
def lcdCopy(g, c)
  g.transfer2(c.get_data)
end

g = GpioTft.new(0)
lcdInit(g)
lcdReset(g)
lcdSetWriteDir(g)
lcdSetup(g)

c = Cairo.new(240, 320)
c.set_source_rgb(1 , 0, 0)
c.move_to(0, 0)
c.line_to(100, 100)
c.stroke()

lcdCopy(g, c)
```

## License
under the BSD License:
- see LICENSE file
