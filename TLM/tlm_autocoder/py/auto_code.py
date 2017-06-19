from ctypes import *

class TLM_AutoCoder():

  def __init__(self, input_file):
    # Open file
    self.f = open(input_file)

    # Function name root
    self.gen_func_name = "TLM_"

    # Parsed variables
    self.name = ""
    self.ID = ""
    self.dest = ""
    self.fmt_str = ""
    self.param_types = ""

    # Buffer size to calculate
    self.buf_size = 0

    # Args variables
    self.type_list = []
    self.arg_list = []
    self.list_len = 0
    self.args = ""

    # Skip to the proper place in the file
    self.skip_to_begin_tlm()


  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    self.close_file()

  def close_file(self):
    self.f.close

  type_sizes = {
    "int8"  : sizeof(c_int8),
    "int16" : sizeof(c_int16),
    "int32" : sizeof(c_int32),
    "int64" : sizeof(c_int64),
    "uint8" : sizeof(c_uint8),
    "uint16": sizeof(c_uint16),
    "uint32": sizeof(c_uint32),
    "uint64": sizeof(c_uint64),
    "float" : sizeof(c_float),
    "double": sizeof(c_double),
    "bool"  : sizeof(c_bool)
  }

  def skip_to_begin_tlm(self):
    # Skip until begin tlm marker
    while self.f.readline().strip() != "##### BEGIN TELEMETRY #####": pass

  def header(self):
    print "/*******************************************************"
    print " *"
    print " * THIS FILE IS AUTO-GENERATED, DO NOT EDIT THIS FILE!"
    print " * To edit this file, edit the auto-coder located"
    print " * in TLM/auto_code/py"
    print " *"
    print " ******************************************************/"
    print

  def include(self):
    print "#include <stdint.h>"
    print "#include \"util/serialize.h\""
    print "#include \"servers/FMGServer.h\""
    print

  def parse_data(self):
    self.name = self.f.readline().strip()
    self.ID = self.f.readline().strip()
    self.dest = self.f.readline().strip()
    self.fmt_str = self.f.readline().strip()
    self.param_types = self.f.readline().strip()
    self.f.readline()

  def eof(self):
    return True if not self.param_types else False

  def parse_params(self):
    self.arg_list = []
    self.type_list = []
    if self.param_types != "None":
      self.type_list = self.param_types.split(",")
      self.list_len = len(self.type_list);
      for i in range(self.list_len):
        self.arg_list.append("arg" + str(i))

  def get_buf_size(self):
    self.buf_size = 2;
    for t in self.type_list:
      self.buf_size += self.type_sizes[t]

  def build_args_string(self):
    self.parse_params()
    self.args = ""

    if self.list_len == 0:
      self.args = "void"
    else:
      for i in range(self.list_len):
        self.args += self.type_list[i].lower() + " " + self.arg_list[i] + ", "

      # Remove trailing comma and space
      self.args = self.args[0:-2]

