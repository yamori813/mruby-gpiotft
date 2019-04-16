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
p GpioTft.hi
#=> "hi!!"
t = GpioTft.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the BSD License:
- see LICENSE file
