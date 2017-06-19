import sys
from auto_code import *

class HPP_AutoCoder(TLM_AutoCoder):
  def __init__(self, input_file):
    TLM_AutoCoder.__init__(self, input_file)

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    TLM_AutoCoder.__exit__(self, exc_type, exc_value, traceback)

  def guards_start(self):
    print "#ifndef TLM_H_"
    print "#define TLM_H_"
    print

  def guards_end(self):
    print "#endif"

  def print_func(self):
    print "void %s%s(%s);" % (self.gen_func_name, self.name, self.args)

  def autocode_single_tlm(self):
      self.build_args_string()
      self.print_func()

  def autocode_all_tlm(self):
    while True:
      self.parse_data()
      if self.eof(): break
      self.autocode_single_tlm()

  def autocode(self):
    self.header()
    self.guards_start()
    self.include()
    self.autocode_all_tlm()
    self.guards_end()



if len(sys.argv) < 2:
  print "ERROR: No argument for filename given"
  quit()

input_file = sys.argv[1]

with HPP_AutoCoder(input_file) as coder:
  coder.autocode()
