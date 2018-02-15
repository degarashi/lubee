from sys import argv
if(len(argv) < 4):
    print("too few arguments: (required=3, actual={})".format(len(argv)-1))
    print("usage: make_gdbinit.py [source-dir] [library-name] [dest-file]")
    quit()

from os import path
srcpath = path.abspath(argv[1])
srcpath = path.split(srcpath)[0]
lib_name = argv[2]
destfile = argv[3]

result = "\
catch throw\n\
catch catch\n\
python\n\
from sys import path\n\
path.insert(1, \"{}\")\n\
import gdb\n\
from {} import printer\n\
printer.buildPrinter(gdb)\n\
end\n\
".format(srcpath, lib_name)

with open(destfile, "w") as wf:
    wf.write(result)
