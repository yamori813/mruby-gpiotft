class GpioTft < BsdGpio
  
  LCD_RST = 20
  LCD_CS = 172
  LCD_RS = 156
  LCD_WR = 131
  LCD_RD = 171
  
  LCD_D8 = 132
  LCD_D9 = 133
  LCD_D10 = 134
  LCD_D11 = 135
  LCD_D12 = 136
  LCD_D13 = 137
  LCD_D14 = 138
  LCD_D15 = 139
  
  LOW = 0
  HIGH = 1
  
  def lcdSetWriteDir
    self.setflags(LCD_D8, BsdGpio::OUTPUT)
    self.setflags(LCD_D9, BsdGpio::OUTPUT)
    self.setflags(LCD_D10, BsdGpio::OUTPUT)
    self.setflags(LCD_D11, BsdGpio::OUTPUT)
    self.setflags(LCD_D12, BsdGpio::OUTPUT)
    self.setflags(LCD_D13, BsdGpio::OUTPUT)
    self.setflags(LCD_D14, BsdGpio::OUTPUT)
    self.setflags(LCD_D15, BsdGpio::OUTPUT)
  end
  
  def lcdReset
    self.set(LCD_RST, LOW)
    sleep 1
    self.set(LCD_RST, HIGH)
    sleep 1
  end
  
  def lcdInit
    self.setflags(LCD_CS, BsdGpio::OUTPUT)
    self.set(LCD_CS, HIGH)
    self.setflags(LCD_RS, BsdGpio::OUTPUT)
    self.set(LCD_RS, HIGH)
    self.setflags(LCD_WR, BsdGpio::OUTPUT)
    self.set(LCD_WR, HIGH)
    self.setflags(LCD_RD, BsdGpio::OUTPUT)
    self.set(LCD_RD, HIGH)
    self.setflags(LCD_RST, BsdGpio::OUTPUT)
    self.set(LCD_RST, HIGH)
  end
  
  def lcdSetup
    self.writereg(0x00e5,0x8000)
    self.writereg(0x0000,0x0001)
    self.writereg(0x0001,0x0100)
    self.writereg(0x0002,0x0700)
    self.writereg(0x0003,0x1030)
    self.writereg(0x0004,0x0000)
    self.writereg(0x0008,0x0202)
    self.writereg(0x0009,0x0000)
    self.writereg(0x000a,0x0000)
    self.writereg(0x000c,0x0000)
    self.writereg(0x000d,0x0000)
    self.writereg(0x000f,0x0000)
    self.writereg(0x0010,0x0000)
    self.writereg(0x0011,0x0000)
    self.writereg(0x0012,0x0000)
    self.writereg(0x0013,0x0000)
    self.writereg(0x0010,0x17b0)
    self.writereg(0x0011,0x0037)
    self.writereg(0x0012,0x0138)
    self.writereg(0x0013,0x1700)
    self.writereg(0x0029,0x000d)
    self.writereg(0x0020,0x0000)
    self.writereg(0x0021,0x0000)
    self.writereg(0x0030,0x0001)
    self.writereg(0x0031,0x0606)
    self.writereg(0x0032,0x0304)
    self.writereg(0x0033,0x0202)
    self.writereg(0x0034,0x0202)
    self.writereg(0x0035,0x0103)
    self.writereg(0x0036,0x011d)
    self.writereg(0x0037,0x0404)
    self.writereg(0x0038,0x0404)
    self.writereg(0x0039,0x0404)
    self.writereg(0x003c,0x0700)
    self.writereg(0x003d,0x0a1f)
    self.writereg(0x0050,0x0000)
    self.writereg(0x0051,0x00ef)
    self.writereg(0x0052,0x0000)
    self.writereg(0x0053,0x013f)
    self.writereg(0x0060,0x2700)
    self.writereg(0x0061,0x0001)
    self.writereg(0x006a,0x0000)
    self.writereg(0x0090,0x0010)
    self.writereg(0x0092,0x0000)
    self.writereg(0x0093,0x0003)
    self.writereg(0x0095,0x0101)
    self.writereg(0x0097,0x0000)
    self.writereg(0x0098,0x0000)
    self.writereg(0x0007,0x0021)
    self.writereg(0x0007,0x0031)
    self.writereg(0x0007,0x0173)
  end
  
  def init
    lcdInit
    lcdReset
    lcdSetWriteDir
    lcdSetup
  end
  
  def lcdCopy(c)
    self.transfer2(c)
  end
end
