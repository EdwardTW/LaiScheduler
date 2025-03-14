# Copyright (c) 2013-2022, Lai Yin Chuan. All rights reserved.
import os, pygame
from pygame.locals import *
#from pygame.compat import geterror

if not pygame.font: print('Warning, fonts disabled')
if not pygame.mixer: print('Warning, sound disabled')

#import RPi.GPIO as GPIO
import LaiScheduler as LaiS
from enum import Enum
import threading
import ctypes
import sys,copy
import getopt
import time,sys
import math

#from pygame.compat import unichr_, unicode_

import locale

#import keyboard as k

global  BASICFONT

class Mode(int,Enum):
    A_pass_state,B_pass_state,A_ready_stop,B_ready_stop,Clean_up_state_A,Clean_up_state_B,RGL_Debug_Mode,RGL_IDAL_mode = range(8)

class LaiST(int,Enum):
    _1024Hz,_512Hz,_256Hz,_128Hz,_64Hz,_32Hz,_16Hz,_8Hz,_4Hz,_2Hz,_1Hz,_2S,_4S,_8S,_16S,_32S,_64S,_128S,_256S,_512S,_1024S,_2048S,_4096S,_8192S,_16384S,_32768S,_65536S =range(27)

class RGL:
    class text:
        Debug = 'RGL_Debug_Mode'
        Idal = 'RGL_IDAL_mode'
        A_pass = 'A_pass_state'
        B_pass = 'B_pass_state'
        A_ready_stop = 'A_ready_stop'
        B_ready_stop = 'B_ready_stop'
        Clean_Up = 'Clean_up_state'
    class ShowTime:
        A_pass_state_Second = 10 
        A_ready_stop_Second = 5 
        Clean_up_state_Second = 3
        B_pass_state_Second = 10
        B_ready_stop_Second = 5
        RGL_IDAL_mode_Second = 20
        RGL_Debug_Mode_Second = 10 
    A_Red_light = 0
    A_Green_light = 0
    A_Yellow_light = 0
    B_Red_light = 0
    B_Green_light = 0
    B_Yellow_light = 0
    Dark=0
    Light=1
    state=Mode.RGL_Debug_Mode
    Tmr=ShowTime.RGL_Debug_Mode_Second
    text_tmp=text.Debug

# functions to create our resources
main_dir = os.path.split(os.path.abspath(__file__))[0]
data_dir = os.path.join(main_dir, 'data')

def load_image(name):
    fullname = os.path.join(data_dir, name)
    try:
        image = pygame.image.load(fullname)
    except :
        print('Cannot load image:', fullname)
#        raise SystemExit(str(geterror()))
    image = image.convert()
    return image

def load_sound(name):
    class NoneSound:
        def play(self): pass
    if not pygame.mixer or not pygame.mixer.get_init():
        return NoneSound()
    fullname = os.path.join(data_dir, name)
    try:
        sound = pygame.mixer.Sound(fullname)
    except :
        print('Cannot load sound: %s' % fullname)
#        raise SystemExit(str(geterror()))
    return sound

def ChangeMode():
    if RGL.state == Mode.RGL_IDAL_mode:
        RGL.state = Mode.Clean_up_state_B
        RGL.Tmr = RGL.ShowTime.Clean_up_state_Second
        RGL.text_tmp=RGL.text.Clean_Up
        print(RGL.text_tmp +'\r\n')	
        return

    if RGL.state == Mode.RGL_Debug_Mode:
        RGL.state = Mode.RGL_IDAL_mode
        RGL.Tmr = RGL.ShowTime.RGL_IDAL_mode_Second
        RGL.text_tmp=RGL.text.Idal
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.Clean_up_state_B:
        RGL.state = Mode.A_pass_state
        RGL.Tmr = RGL.ShowTime.A_pass_state_Second
        RGL.text_tmp=RGL.text.A_pass
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.A_pass_state:
        RGL.state = Mode.A_ready_stop
        RGL.Tmr = RGL.ShowTime.A_ready_stop_Second
        RGL.text_tmp=RGL.text.A_ready_stop
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.A_ready_stop:
        RGL.state = Mode.Clean_up_state_A
        RGL.Tmr = RGL.ShowTime.Clean_up_state_Second
        RGL.text_tmp=RGL.text.Clean_Up
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.Clean_up_state_A:
        RGL.state = Mode.B_pass_state
        RGL.Tmr = RGL.ShowTime.B_pass_state_Second
        RGL.text_tmp=RGL.text.B_pass
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.B_pass_state:
        RGL.state = Mode.B_ready_stop
        RGL.Tmr = RGL.ShowTime.B_ready_stop_Second
        RGL.text_tmp=RGL.text.B_ready_stop
        print(RGL.text_tmp +'\r\n')
        return

    if RGL.state == Mode.B_ready_stop:
        RGL.state = Mode.Clean_up_state_B
        RGL.Tmr = RGL.ShowTime.Clean_up_state_Second
        RGL.text_tmp=RGL.text.Clean_Up
        print(RGL.text_tmp +'\r\n')
        return
              
def proc_RGL():
    if RGL.state ==  Mode.RGL_Debug_Mode: 
        RGL.A_Red_light = LaiS.Blink(LaiST._1Hz)
        RGL.B_Red_light = LaiS.Blink(LaiST._1Hz)
        RGL.A_Green_light = LaiS.Blink(LaiST._1Hz)
        RGL.A_Yellow_light = LaiS.Blink(LaiST._1Hz)
        RGL.B_Green_light = LaiS.Blink(LaiST._1Hz)
        RGL.B_Yellow_light = LaiS.Blink(LaiST._1Hz)         
        return

    if RGL.state ==  Mode.A_pass_state:  
        RGL.A_Red_light = RGL.Dark
        RGL.A_Yellow_light = RGL.Dark
        RGL.A_Green_light = RGL.Light
        RGL.B_Red_light = RGL.Light
        RGL.B_Yellow_light = RGL.Dark
        RGL.B_Green_light = RGL.Dark
        return

    if RGL.state ==  Mode.A_ready_stop:   
        RGL.A_Red_light = RGL.Dark
        RGL.A_Yellow_light = LaiS.Blink(LaiST._1Hz)
        RGL.A_Green_light = RGL.Dark
        RGL.B_Red_light = RGL.Light
        RGL.B_Yellow_light = RGL.Dark
        RGL.B_Green_light = RGL.Dark
        return

    if RGL.state ==  Mode.Clean_up_state_A:
        RGL.A_Red_light=RGL.Light
        RGL.A_Yellow_light=RGL.Dark
        RGL.A_Green_light=RGL.Dark
        RGL.B_Red_light=RGL.Light
        RGL.B_Yellow_light=RGL.Dark
        RGL.B_Green_light=RGL.Dark
        return   

    if RGL.state ==  Mode.Clean_up_state_B:     
        RGL.A_Red_light=RGL.Light
        RGL.A_Yellow_light=RGL.Dark
        RGL.A_Green_light=RGL.Dark
        RGL.B_Red_light=RGL.Light
        RGL.B_Yellow_light=RGL.Dark
        RGL.B_Green_light=RGL.Dark
        return

    if RGL.state ==  Mode.B_pass_state:
        RGL.A_Red_light=RGL.Light
        RGL.A_Yellow_light=RGL.Dark
        RGL.A_Green_light=RGL.Dark
        RGL.B_Red_light=RGL.Dark
        RGL.B_Yellow_light=RGL.Dark
        RGL.B_Green_light=RGL.Light
        return

    if RGL.state ==  Mode.B_ready_stop:         
        RGL.A_Red_light=RGL.Light
        RGL.A_Yellow_light=RGL.Dark
        RGL.A_Green_light=RGL.Dark
        RGL.B_Red_light=RGL.Dark
        RGL.B_Yellow_light = LaiS.Blink(LaiST._1Hz) 
        RGL.B_Green_light=RGL.Dark
        return

    if RGL.state ==  Mode.RGL_IDAL_mode:        
        RGL.A_Red_light=RGL.Dark
        RGL.A_Green_light=RGL.Dark    
        RGL.A_Yellow_light = LaiS.Blink(LaiST._2S)
        RGL.B_Red_light = LaiS.Blink(LaiST._2S)
        RGL.B_Yellow_light=RGL.Dark
        RGL.B_Green_light=RGL.Dark
        return  
       
def main():
    pygame.init()
    # FPS = 8 # frames per second setting
    WINDOWWIDTH = 640
    WINDOWHEIGHT = 480
    # fpsClock = pygame.time.Clock()
    #BASICFONT = pygame.font.Font('Times New Roman', 18)
    BASICFONT = pygame.font.SysFont('freesansbold.ttf', 50)
    DISPLAYSURF = pygame.display.set_mode((WINDOWHEIGHT, WINDOWWIDTH), 0, 32)
    pygame.display.set_caption("Traffic Light Demo use Lai's Scheduler")

    # GRAY        = (185, 185, 185)
    BLACK       = (  0,   0,   0)
    # RED         = (155,   0,   0)
    # LIGHTRED    = (175,  20,  20)

    WHITE = (255, 255, 255)
    TEXTCOLOR = BLACK
    RGLdarkImg = load_image('RGL_dark.png')
    RLImg =load_image('red_light.png')
    GLImg =load_image('green_light.png')
    YLImg =load_image('yellow_light.png')
    
    print(time.strftime("%H:%M:%S"))
    print(RGL.text.Debug +'\r\n')
    RGL.state = Mode.RGL_Debug_Mode
    RGL.Tmr = RGL.ShowTime.RGL_Debug_Mode_Second

    LaiS.Start()
                       
    going  = True

    while going : # the main game loop
            if LaiS.CheckTSMf(LaiST._1Hz):
                if RGL.Tmr:
                    RGL.Tmr -= 1 
                if RGL.Tmr == 0:
                    print(time.strftime("%H:%M:%S"))
                    ChangeMode()   
      
            if LaiS.CheckTSMf(LaiST._4Hz):
                proc_RGL()
                DISPLAYSURF.fill(WHITE)
                DISPLAYSURF.blit(RGLdarkImg, (60, 30))
                DISPLAYSURF.blit(RGLdarkImg, (250, 30))
                if RGL.A_Red_light==RGL.Light:
                    DISPLAYSURF.blit(RLImg, (268-190, 45))
                if RGL.A_Yellow_light==RGL.Light:    
                    DISPLAYSURF.blit(YLImg, (268-190, 170))
                if RGL.A_Green_light==RGL.Light:
                    DISPLAYSURF.blit(GLImg, (268-190, 290))
                if RGL.B_Red_light==RGL.Light:
                    DISPLAYSURF.blit(RLImg, (268, 45))
                if RGL.B_Yellow_light==RGL.Light:
                    DISPLAYSURF.blit(YLImg, (268, 170))
                if RGL.B_Green_light==RGL.Light:
                    DISPLAYSURF.blit(GLImg, (268, 290))
                scoreSurf = BASICFONT.render(RGL.text_tmp, True, TEXTCOLOR)
                DISPLAYSURF.blit( scoreSurf , (60,440))       
                scoreSurf = BASICFONT.render('Second : %s' %RGL.Tmr, True, TEXTCOLOR)
                DISPLAYSURF.blit( scoreSurf , (60,480))
                pygame.display.update()
        #   k.add_hotkey("ctrl+x",lambda: sys.exit())   
            for event in pygame.event.get():
                if event.type == QUIT:
                    going = False
                    LaiS.Stop()
                    pygame.quit()
                    sys.exit()                    
                elif event.type == KEYDOWN and event.key == K_ESCAPE:
                    going = False
                    LaiS.Stop()
                    pygame.quit()
                    sys.exit()
            time.sleep(0.0000001)      

    LaiS.Stop()       
    pygame.quit()
    sys.exit()
if __name__ == '__main__':
    main()
#    fpsClock.tick(FPS)
