# coding: utf-8
# GDB用のpretty-printer
import gdb
import re

def buildPrinter(obj):
    obj.pretty_printers.append(Lookup)

Re_Lubee = re.compile("^lubee::(.+)$")
def Lookup(val):
    name = val.type.strip_typedefs().name
    if name == None:
        return None
    obj = Re_Lubee.match(name)
    if obj:
        return LookupLubee(obj.group(1), val)
    return None

Re_Range = re.compile("^Range<[^>]+>")
def LookupLubee(name, val):
    if(Re_Range.match(name)):
        return Range(val)
    return None

class Iterator:
    def next(self):
        return self.__next__()

class Range:
    def __init__(self, val):
        self._from = val["from"]
        self._to = val["to"]
        code = val["from"].type.strip_typedefs().code
        if code == gdb.TYPE_CODE_INT:
            self._makestr = self.makeInt
        elif code == gdb.TYPE_CODE_FLT:
            self._makestr = self.makeFloat
        else:
            self._makestr = self.makeOther
    def makeInt(self):
        return "{%d -> %d}" % (int(self._from), int(self._to))
    def makeFloat(self):
        return "{%.4f -> %.4f}" % (float(self._from), float(self._to))
    def makeOther(self):
        return None
    def to_string(self):
        return self._makestr()
