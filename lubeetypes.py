# coding: utf-8
# QtCreator Debugger用のpretty-printer
from dumper import *

# ------------------------------------------------------------
# Range
def qdump__lubee__Range(d, value):
    d.putValue("[" + str(value["from"].value()) + " | " + str(value["to"].value()) + "]")
    d.putType("Range")
    d.putNumChild(0)
