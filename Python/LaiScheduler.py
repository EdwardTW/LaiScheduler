# Copyright (c) 2013-2022, Lai Yin Chuan. All rights reserved.
import threading
from threading import Thread
import sys
import time
import math

class LaiS:
    TSM=0
    TSMp=0
    TSMf=0  
    running = True

def Threadfun(string, sleeptime):
    #timestamp=(time.time()*1000000)
    while LaiS.running:
    #    if (time.time()*1000000)-timestamp>=368:
    #       timestamp=(time.time()*1000000)
            LaiS.TSMp = LaiS.TSM                              
            LaiS.TSM += 1                                
            LaiS.TSMf |= LaiS.TSM ^ LaiS.TSMp                     
            time.sleep(sleeptime) 
    sys.exit()  
    
def CheckTSMf(x):
    if LaiS.TSMf & (0x1<<x):
        LaiS.TSMf &= ~(0x1<<x)
        return True
    return False

def Blink(x):
        if LaiS.TSM & (0x1<<(x-1)):
            return True
        return False

def BlinkTimes(x):
    if ((LaiS.TSM%0x4000)>(x*0x7ff))|((LaiS.TSM%0x800)>0x80)>0:
        return  True
    return False        

def Start():
    Thread(target=Threadfun, args=("often run",0.00051)).start()
    return True

def ResetTSM():
    LaiS.TSM=0
    LaiS.TSMp=0
    LaiS.TSMf=0
    return True

def Stop():
    LaiS.running=False
    return True

