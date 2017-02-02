import time
import os

#os.system("csplit dict.txt '/^\^\^\^\^\^\^\ BEGIN TELEMETRY\ \^\^\^\^\^\^/' > /dev/null")
fname0 = "xx00"
fname1 = "xx01"

type_sizes = {
  "Int8"  : 1,
  "Int16" : 2,
  "Int32" : 4,
  "Int64" : 8,
  "UInt8" : 1,
  "UInt16": 2,
  "UInt32": 4,
  "UInt64": 8,
  "Float" : 4,
  "Double": 8,
  "Bool"  : 1
}

with open("dict.txt") as f:
  while f.readline().strip() != "##### BEGIN TELEMETRY #####": pass

  while True:
    # Skip first line
    #f.readline()

    name = f.readline().strip()
    ID = f.readline().strip()
    dest = f.readline().strip()
    fmt_str = f.readline().strip()
    param_types = f.readline().strip()
    f.readline()

    if not param_types: break

    type_list = []
    arg_list = []
    length = 0

    if param_types != "None":
      type_list = param_types.split(",")
      length = len(type_list);
      for i in range(length):
        arg_list.append("arg" + str(i))

    buf_size = 2;
    for t in type_list:
      buf_size += type_sizes[t]

    args = ""
    for i in range(length):
      args += type_list[i].lower() + " " + arg_list[i] + ", "
    args = args[0:-2]


    print "void log_tlm_%s(%s) {" % (name, args)
    print "  uint8_t buf[%s];" % (buf_size)
    print "  ser.Serialize ser(buf, %s);" % (buf_size)
    print "  ser.serializeUInt16(%s);" % (ID)
    for i in range(length):
        print "  ser.serialize%s(%s);" % (type_list[i], arg_list[i])
    print "  fmgServer->Log(DESTINATION_%s, buf);" % (dest)
    print "}"
    print

