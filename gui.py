import serial.tools.list_ports,pygame,threading,math,time

#Initilization of serial port
serialinit = serial.Serial()

#Initilization of pygame
pygame.init()
WIDTH = 800
HEIGHT = 1000
WIN = pygame.display.set_mode((WIDTH,HEIGHT))


def display(dots):
    WIN.fill((0,0,0))
    for degree,distance in dots.items():
        x = math.cos(math.radians(degree))*distance
        y = math.sin(math.radians(degree))*distance
        pygame.draw.rect(WIN,(244,244,244),pygame.Rect((WIDTH/2)+x,(HEIGHT/2)+y, 10,10))
    
    pygame.display.update()



run = True
FPS = 60
dots = {}

#Setting up pico connection
connected = False
port = input("Enter port:")
while not connected:
    try:
        serialinit.baudrate = 115200
        serialinit.port = port
        serialinit.open()
        connected = True
    except:
        print("waiting for pico connection")
print("Pico connected")

#main loop
while run:
    pygame.time.Clock().tick(FPS)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
            pygame.quit()

    if serialinit.in_waiting:
        packet = serialinit.readline()
        deg,dis = packet.decode("utf-8").strip("\n").split(":")
        deg = float(deg)
        dis = float(dis)
        dots[deg] = dis
    display(dots)

    


