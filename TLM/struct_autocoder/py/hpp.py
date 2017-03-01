import sys
from auto_code import *

class HPP_Autocoder(STRUCT_Autocoder):
  def __init__(self, input_file):
    STRUCT_Autocoder.__init__(self, input_file)

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    STRUCT_Autocoder.__exit__(self, exc_type, exc_value, traceback)

  def guards_start(self):
    print "#ifndef HSTStructs_H_"
    print "#define HSTStructs_H_"
    print

  def guards_end(self):
    print "#endif"

  def include(self):
    print "#include \"util/serialize.h\""
    print

  def print_struct(self):
    print "class %s : public Serialize {" % (self.name)
    print "public:"
    for field in range(len(self.types)):
      print "  %s %s; %s" % (self.types[field], self.args[field], self.comments[field])
    print "  void serialize(void);"
    print "  void deserialize(void);"
    print "};"
    print
    
  def autocode(self):
    self.header()
    self. guards_start()
    self.include()
    self.namespace_start()

    while (not self.EOF):
      self.get_struct()

      if (self.EOF):
        break

      self.print_struct()
      self.reset_struct()

    self.namespace_end()
    self.guards_end()



if len(sys.argv) < 2:
  print "ERROR: No argument for filename given"
  quit()

input_file = sys.argv[1]

with HPP_Autocoder(input_file) as coder:
  coder.autocode()
