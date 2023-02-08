Fiche d'utilisation du sheild Sense hat

Réalisé par : Dorian ETCHEBER
date        : 11/01/2023

*************************************************************
Installation : sudo apt install sense-hat

*************************************************************
Modules présent sur la sense hat :
    * Gyroscope
    * Accélérometre
    * Magnétomètre
    * Capteur de température
    * Capteur d'humidité
    * Capteur de pression barométrqiue

*************************************************************
Importation :
from sense-hat import SenseHat
sense = SenseHat()

**************************************************************
Référence API

référence : https://pythonhosted.org/sense-hat/api/


                <Matrice LEDS 8x8>
________________________________________________________________

void sense.set_rotation(integer r,Boolean redraw)

description : If you're using the Pi upside down or sideways you 
can use this function to correct the orientation of the image 
being shown.
________________________________________________________________

void sense.flip_h(Boolean redraw)

description : Flips the image on the LED matrix horizontally.
________________________________________________________________

void sense.flip_v(Boolean redraw)

description : Flips the image on the LED matrix vertically.
________________________________________________________________

void sense.set_pixels(list pixel_list)

description : Updates the entire LED matrix based on a 64 length 
list of pixel values.
________________________________________________________________

list sense.get_pixels(void)

description : return list from get_pixels
________________________________________________________________

void sense.set_pixel(integer x, integer y, list pixel, integer r, integer g, integer b)

description : Sets an individual LED matrix pixel at the specified 
X-Y coordinate to the specified colour.

exemple : sense.set_pixel(0,0,255,0,0) --> Sets the top left LED 
to the colour red
________________________________________________________________

list sense.get_pixel(integer x, integer y)

description : return list color from set_pixel
________________________________________________________________

list sense.load_image(string file_path, boolean redraw)

description : Loads an image file, converts it to RGB format and
displays it on the LED matrix. The image must be 8 x 8 pixels in size.
________________________________________________________________

void sense.clear(list colour) or void sense.clear(integer r, integer g, integer b)

description : Sets the entire LED matrix to a single colour, 
defaults to blank / off.
________________________________________________________________

void sense.show_message(string text_string, float scroll_speed, list text_colour, list back_colour)

description :Scrolls a text message from right to left across 
the LED matrix and at the specified speed, in the specified colour 
and background colour.
________________________________________________________________

void sense.show_letter(string s, list text_colour, list back_colour)

description : Displays a single text character on the LED matrix.
________________________________________________________________

sense.low_light  --> boolean

description : Toggles the LED matrix low light mode, 
useful if the Sense HAT is being used in a dark environment.
________________________________________________________________

sense.gamma

description :
For advanced users. Most users will just need the low_light Boolean 
property above. The Sense HAT python API uses 8 bit (0 to 255) colours 
for R, G, B. When these are written to the Linux frame buffer they're bit 
shifted into RGB 5 6 5. The driver then converts them to RGB 5 5 5 before 
it passes them over to the ATTiny88 AVR for writing to the LEDs.

The gamma property allows you to specify a gamma lookup table for the 
final 5 bits of colour used. The lookup table is a list of 32 numbers 
that must be between 0 and 31. The value of the incoming 5 bit colour is 
used to index the lookup table and the value found at that position is then
written to the LEDs.
________________________________________________________________

void sense.gamma_reset(void)

description : A function to reset the gamma lookup table to default, 
ideal if you've been messing with it and want to get it back to a default state.