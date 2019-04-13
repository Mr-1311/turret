import pygame, sys, time, random, serial
from threading import Thread
from random import randint
from pygame.locals import *

try:
    ser = serial.Serial('/dev/ttyUSB0')
except:
    ser = serial.Serial('/dev/ttyUSB1')
    
pygame.init()

windowSurface = pygame.display.set_mode((975, 650), 0, 32)
pygame.display.set_caption("Paint")

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

DATA = [0,0]
line = 0
dot = 0

isTargetSpotted = 0

def init():
    pygame.mixer.music.load('activated.wav')
    pygame.mixer.music.play(0)


def targetSearching():
    while True:
        time.sleep(3)
        if isTargetSpotted == 0:
            print("where.wav")
            x = randint(0, 2)
            if x == 0:
                pygame.mixer.music.load('are_you_still_there.wav')
                pygame.mixer.music.play(0)
            elif x == 1:
                pygame.mixer.music.load('whos_there.wav')
                pygame.mixer.music.play(0)
            else:
                pygame.mixer.music.load('is_anyone_there.wav')
                pygame.mixer.music.play(0)


def targetFound():
    pygame.mixer.music.load('target_acquired.wav')
    pygame.mixer.music.play(0)
    
    
def targetLost():
    pygame.mixer.music.load('target_lost.wav')
    pygame.mixer.music.play(0)


init()

search = Thread(target=targetSearching)
search.start()
    
while True:

    
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()


    try:
        line = ser.readline().decode('ascii').replace('\r', '').replace('\n', '')
        DATA = line.split(':')
    except:
        print('Data could not be read')

    searchCount = 0
        
    try:
        line = int(DATA[0])
        dot = int(DATA[1])
        if dot == -1:
            dot = -100
            if isTargetSpotted == 1:
                searchCount = 0
                isTargetSpotted = 0
                targetLost()

        elif isTargetSpotted == 0:
            searchCount = 0
            isTargetSpotted = 1
            targetFound()
         
    except:
        line = 0
        dot = -100
        
    line = line * 10 + 25
    dot = 650 - (dot * 20 + 25)
    
    windowSurface.fill((0,0,0)) # fill the screen with white

    pygame.draw.line(windowSurface,WHITE,(line, 600), (line+25, 600), 8)
    pygame.draw.circle(windowSurface, GREEN , (line + 12, dot), 15, 0)

    pygame.display.update() # update the screen
