
import pygame, random

filename = "test.tri"

drawlist = []

with open(filename) as f:
    lines = f.read().splitlines()
    width = int(lines[0])
    height = int(lines[1])
    triangle = []
    for i, line in enumerate(lines):
        if i<2:
            continue
        coord = line.split(", ")
        x = float(coord[0])
        y = float(coord[1])
        triangle.append([x, y])

        if i%3==1 and i!=1:
            drawlist.append(triangle)
            triangle = []


pygame.init();

screen = pygame.display.set_mode((width, height))

colors = ['red', 'blue', 'green', 'cyan', 'magenta', 'yellow', 'white']

mapcolor = []

for i, triangle in enumerate(drawlist):
    mapcolor.append(colors[i%7])

i = 0
run = 1
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            run = 0
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_SPACE:
                random.shuffle(drawlist)
    screen.fill('black')

    i = 0
    for triangle in drawlist:
        pygame.draw.polygon(screen, mapcolor[i], triangle)
        i += 1

    pygame.display.flip()
