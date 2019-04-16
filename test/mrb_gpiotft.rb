##
## GpioTft Test
##

assert("GpioTft#hello") do
  t = GpioTft.new "hello"
  assert_equal("hello", t.hello)
end

assert("GpioTft#bye") do
  t = GpioTft.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("GpioTft.hi") do
  assert_equal("hi!!", GpioTft.hi)
end
