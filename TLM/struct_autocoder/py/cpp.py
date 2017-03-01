import sys
import re
from auto_code import *

class CPP_Autocoder(STRUCT_Autocoder):
  def __init__(self, input_file):
    STRUCT_Autocoder.__init__(self, input_file)

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    STRUCT_Autocoder.__exit__(self, exc_type, exc_value, traceback)

  def print_serialize(self):
    print "%s::serialize() {" % (self.name)
    for field in range(len(self.types)):
      split_arg = re.split("\[|\]", self.args[field])
      if ( len(split_arg) > 1 ):
        print "  for (int iter = 0; iter < %s; iter++) {" % (split_arg[1])
        print "    this->serialize%s(this->%s[iter]);" % (self.types[field], split_arg[0])
        print "  }"
      else:
        print "  this->serialize%s(this->%s);" % (self.types[field], self.args[field])
    print "}"
    print

  def print_deserialize(self):
    print "%s::deserialize() {" % (self.name)
    for field in range(len(self.types)):
      split_arg = re.split("\[|\]", self.args[field])
      if ( len(split_arg) > 1 ):
        print "  for (int iter = 0; iter < %s; iter++) {" % (split_arg[1])
        print "    this->%s[iter] = this->deserialize%s();" % (split_arg[0], self.types[field])
        print "  }"
      else:
        print "  this->%s = this->deserialize%s();" % (self.args[field], self.types[field])
    print "}"
    print

  def autocode(self):
    self.header()
    self.include()
    self.namespace_start()

    while (not self.EOF):
      self.get_struct()

      if (self.EOF):
        break

      self.print_serialize()
      self.print_deserialize()
      self.reset_struct()



if len(sys.argv) < 2:
  print "ERROR: No argument for filename given"
  quit()

input_file = sys.argv[1]

with CPP_Autocoder(input_file) as coder:
  coder.autocode()
