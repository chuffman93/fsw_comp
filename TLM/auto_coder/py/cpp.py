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

  def using_namespace(self):
    print "using namespace std;"
    print "using namespace AllStar::Core;"
    print

  def fmg_server(self):
    print "FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));"
    print

  def print_func(self):
    print "void %s%s(%s) {" % (self.gen_func_name, self.name, self.args)
    print "  uint8_t * buf = (uint8_t *) malloc(%s);" % (self.buf_size)
    print "  Serialize ser(buf, %s);" % (self.buf_size)
    print "  ser.serializeUInt16(%s);" % (self.ID)
    for i in range(self.list_len):
        print "  ser.serialize%s(%s);" % (self.type_list[i], self.arg_list[i])
    print "  fmgServer->Log(DESTINATION_%s, buf, %s);" % (self.dest, self.buf_size)
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
    self.using_namespace()
    self.namespace_start()
    self.fmg_server()
    self.autocode_all_tlm()
    self.namespace_end()



if len(sys.argv) < 2:
  print "ERROR: No argument for filename given"
  quit()

input_file = sys.argv[1]

with CPP_AutoCoder(input_file) as coder:
  coder.autocode()
