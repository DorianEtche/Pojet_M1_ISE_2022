# nom : capteur.py
# Créé par : Dorian ETCHEBER
# Date : 11/01/2023
# Description : Affichage des données capteurs du shield Sense HAT
# Capteurs :
#   Humidité
#   Température
#   Pression


from sense_hat import SenseHat

sense = SenseHat()

# capteur d'humidité
humidity = sense.get_humidity()
print("Humidity: %s %%rH" % humidity)

# capteur de température sur capteur d'humidité
temp = sense.get_temperature()    # --> alternatif : temp = sense.get_temperature_from_humidity()
print("Temperature (capteur d'humidité): %s °C" % temp)

# capteur de température sur capteur de pression
temp = sense.get_temperature_from_pressure()
print("Temperature (capteur de pression): %s C" % temp)

# capteur de pression
pressure = sense.get_pressure()
print("Pressure: %s Millibars" % pressure)

