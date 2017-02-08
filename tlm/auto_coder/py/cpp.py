import sys
from auto_code import *

class CPP_AutoCoder(TLM_AutoCoder):
  def __init__(self, input_file):
    TLM_AutoCoder.__init__(self, input_file)
    self.buf_size = 0

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    TLM_AutoCoder.__exit__(self, exc_type, exc_value, traceback)

  def print_func(self):
    print "void %s%s(%s) {" % (self.gen_func_name, self.name, self.args)
    print "  uint8_t buf[%s];" % (self.buf_size)
    print "  ser.Serialize ser(buf, %s);" % (self.buf_size)
    print "  ser.serializeUInt16(%s);" % (self.ID)
    for i in range(self.list_len):
        print "  ser.serialize%s(%s);" % (self.type_list[i], self.arg_list[i])
    print "  fmgServer->Log(DESTINATION_%s, buf);" % (self.dest)
    print "}"
    print

  def autocode_single_tlm(self):
      self.build_args_string()
      self.get_buf_size()
      self.print_func()

  def autocode_all_tlm(self):
    while True:
      self.parse_data()
      if self.eof(): break
      self.autocode_single_tlm()

  def autocode(self):
    self.header()
    self.include()
    self.autocode_all_tlm()



if len(sys.argv) < 2:
  print "ERROR: No argument for filename given"
  quit()

input_file = sys.argv[1]

with CPP_AutoCoder(input_file) as coder:
  coder.autocode()
