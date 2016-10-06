# coding: utf-8
# QtCreator Debugger用のpretty-printer
from dumper import *

# ------------------------------------------------------------
# Range
def qdump__lubee__Range(d, value):
    d.putValue("[" + str(value["from"]) + " | " + str(value["to"]) + "]")
    d.putType("Range")
    d.putNumChild(0)
