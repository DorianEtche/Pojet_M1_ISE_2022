# nom : IMU.py
# Créé par : Dorian ETCHEBER
# Date : 11/01/2023
# Description : Affichage des données capteurs du IMU du shield Sense HAT

# capteur IMU (inertial measurement unit)

# 3 capteurs :
#    * Gyroscope
#    * Accéléromètre
#    * Magnétomètre

from sense_hat import SenseHat

sense = SenseHat()


####################################################
# global

sense.set_imu_config(True, True, True) # On active tout les capteurs

# donne l'orientation actuelle en radiants
orientation_rad = sense.get_orientation_radians()
print("Orientation en radiant :\n   p: {pitch} rad, r: {roll} rad, y: {yaw} rad".format(**orientation_rad))
# donne l'orientation actuelle en degrées
orientation = sense.get_orientation_degrees()  # --> alternatif : orientation = sense.get_orientation()
print("Orientation en degrée :\n   p: {pitch}°, r: {roll}°, y: {yaw}°".format(**orientation))

####################################################
# Magnétomètre

north = sense.get_compass() # donne le résultat en degrée en fonction du nord
print("Compas :\n   North: %s" % north)

raw = sense.get_compass_raw() # données brut du magnétomètre
print("Données brut du compas :\n   x: {x}, y: {y}, z: {z}".format(**raw))

####################################################
# Gyroscope

#gyro_only = sense.get_gyroscope() # fait appele à set_imu_config pour n'activer que le gyroscope
#print("p: {pitch}, r: {roll}, y: {yaw}".format(**gyro_only))

# Donne le mouvement de la carte en radiants pas secondes
raw = sense.get_gyroscope_raw()
print("Gyroscope :\n   x: {x} rad/s, y: {y} rad/s, z: {z} rad/s".format(**raw))


####################################################
# accéléromètre

#accel_only = sense.get_accelerometer() fait appele à set_imu_config pour n'activer que l'accelerometre
#print("p: {pitch}, r: {roll}, y: {yaw}".format(**accel_only))

raw = sense.get_accelerometer_raw()
print("Accélérometre :\n   x: {x} Gs, y: {y} Gs, z: {z} Gs".format(**raw))

####################################################
