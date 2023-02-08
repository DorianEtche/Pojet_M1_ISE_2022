# Créé par : Dorian ETCHEBER
# Date : 11/01/2023
# Description : Utilisation du joystick du shield Sense HAT. Le joystick déplace un pixel rouge
# sur la matrice de led et si on apuie sur le joystick, un pixel rouge reset sur la position actuel.


from sense_hat import SenseHat

sense = SenseHat()
global black
global red
global position
global mapping

black = (0,0,0)
red = (255,0,0)
position = [0,0]

mapping = [black]*64



def change_position(event):
    if event.direction == "left" and event.action == "pressed" :
        if position[0] != 0:
            position[0] -= 1
    elif event.direction == "right" and event.action == "pressed" :
        if position[0] != 7:
            position[0] += 1
    elif event.direction == "up" and event.action == "pressed" :
        if position[1] != 0:
            position[1] -= 1
    elif event.direction == "down" and event.action == "pressed" :
        if position[1] != 7:
            position[1] += 1
    elif event.direction == "middle" and event.action == "pressed" :
        mapping[position[0]+position[1]*8] = red

def set_screen():
    sense.set_pixels(mapping)
    sense.set_pixel(position[0],position[1],red)







set_screen()
while True :
    # attente d'un event sur joystick
    event = sense.stick.wait_for_event() # autre possiblité : sense.stick.get_events() --> pas d'attente
    print("The joystick was {} {}".format(event.action, event.direction))
    change_position(event)
    set_screen()