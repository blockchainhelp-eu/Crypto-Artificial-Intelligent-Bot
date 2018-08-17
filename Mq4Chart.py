# -*-mode: python; py-indent-offset: 4; indent-tabs-mode: nil; encoding: utf-8-dos; coding: utf-8 -*-

"""
A Mq4Chart object is a simple abstraction to encapsulate a Mt4 chart.
"""

import sys, logging
import time
import Queue
from SimpleFormat import sFormatMessage

oLOG = logging

# For convenience, define true and false so we can use the MQ4 variables
try:
    __builtins__.true
except AttributeError:
    __builtins__['true'] = True
    __builtins__['false'] = False

def sSafeSymbol(s):
    for elt in ['!', '-', '#', '.']:
        s = s.replace(elt, '')
    return s

class Mq4Chart(object):
    # should these be module level?
    _dCharts = dict()
    _dChartNames = dict()

    def __init__(self, sChartId, **dParams):
        self.sChartId = sChartId

        self.dParams = dParams

        # FixMe: see if it's already there
        self.vAdd(id(self))
        self.oQueue = Queue.Queue()

    def zMq4PopQueue(self, sIgnored=""):
        """
        The PopQueue is usually called from the Mt4 OnTimer.
        We use is a queue of things for the ProcessCommand in Mt4.
        """
        if self.oQueue.empty():
            return ""

        # while we are here flush stdout so we can read the log file
        # whilst the program is running
        sys.stdout.flush()
        sys.stderr.flush()

        return self.oQueue.get()

    def eMq4PushQueue(self, sMessage):
        """
        """
        # FixMe; dispatch to the right chart
        self.oQueue.put(sMessage)
        return ""

    def eMq4Retval(self, sMark, sType, sValue):
        sTopic = 'retval'
        # FixMe: replace with sChartId
        sChartId = self.iChartId
        sMess = sFormatMessage(sTopic, sChartId, sMark, sType, sValue)
        self.eMq4PushQueue(sMess)

    def vAdd(self, iId):
        self.iId = iId
        # see if it's already there
        if iId not in self._dCharts:
            self._dCharts[iId] = self
            self._dChartNames[self.sChartId] = self

    # FxiMe: put this on a __del__?
    def vRemove(self, iId=None):
        if not iId: iId = id(self)
        if iId in self._dCharts:
            del self._dCharts[iId]
            del self._dChartNames[self.sChartId]

    def vReInit(self, **dKeys):
        self.dParams.update(**dKeys)


def oMakeChart(sChartId, dParams):
    """
    Make an instance of a Mq4Chart object to encapsulate a Mt4 chart.
    It will reuse an existing chart or create it needed.
    """
    if sChartId in sys.modules['__main__'].__dict__:
        oLOG.info("Reusing "+sChartId)
        return sys.modules['__main__'].__dict__[sChartId]
    oLOG.info("Creating "+sChartId)
    return Mq4Chart(sChartId, **dParams)


def iFindChartByName(sChartId):
    """
    This is old code that doesnt make sense to me.
    Why use is iId?-+

    """
    oRetval = oFindChartByName(sChartId)
    if not oRetval: return 0
    return oRetval.iId

def oFindChartByName(sChartId):
    """
    This is old code that doesnt make sense to me.
    Why use _dChartNames and _dCharts?
    """
    oLOG.info("Looking for "+sChartId)
    if sChartId in Mq4Chart._dChartNames.keys():
        oLOG.info("Found "+sChartId)
        return Mq4Chart._dChartNames[sChartId]
    oLOG.info("Couldn't find "+sChartId+" in "+
              str(Mq4Chart._dChartNames.keys()))
    return None

def lFindAllChartNames():
    return Mq4Chart._dChartNames.keys()

def lFindAllCharts():
    lRetval = Mq4Chart._dChartNames.values()
    oLOG.info("Found " +repr(lRetval))
    return lRetval

# unused
def iFindExpert(sSymbol, iPeriod):
    """old code"""
    iRetval = -1
    sChartId = "oChart"+"_"+sSafeSymbol(sSymbol) + "_"+str(iPeriod)+"_"
    oLOG.info("Looking for "+sChartId)
    iLen = len(sChartId)
    for sElt in Mq4Chart._dChartNames.keys():
        if sElt.startswith(sChartId):
            iRetval = int(sElt[iLen:])
            if iRetval > 0:
                return iRetval
    oLOG.info("Couldn't find "+sChartId+" in "+
              str(Mq4Chart._dChartNames.keys()))
    return iRetval

def iChartExists(sChartId):
    oLOG.info("Looking for "+sChartId)
    if sChartId in sys.modules['__main__'].__dict__:
        oLOG.info("Found "+sChartId)
        return 1
    oLOG.info("Couldn't find "+sChartId+" in "+
              str(sys.modules['__main__'].__dict__.keys()))
    return 0
