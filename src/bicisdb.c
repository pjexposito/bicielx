#include "bicisdb.h"
#include "pebble.h"

static int paradas[]={265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 398, 399, 400, 401, 402, 403, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420};  
  
char *array_nombre_parada[] = {
"Estación de autobuses",
"Oficina de Turismo",
"Plaza de la lonja",
"Plaza del Raval",
"Centro de congresos",
"Ayuntamiento",
"Pabellón Municipal",
"Polivalente Carrús",
"Biblioteca San José",
"Escorxador",
"Plaza Crevillente",
"Ambulatorio San Fermín",
"Plaza 1 de Mayo",
"Comisaría",
"Avda. Libertad-R. Victoria",
"Jardín de Aspe",
"Plaza Castilla",
"Polideportivo Altabix",
"Jesuitinas",
"Toni Mena - Fray J. Torres",
"C.P. Mariano Benlliure",
"Avda. Llauradors - Eslava",
"Conrado Campo - Trinquete",
"Policía Local",
"Corazón de Jesús",
"UMH -Escuela O. de Idiomas",
"UMH - Pabellón Deportes",
"Passeig de Germanies",
"Casablanca",
"Squash",
"PRUEBAS",
"Centro salud Altabix",
"Avda. Libertad - Aljub",
"Hospital general",
"Hospital del Vinalopó"
};
  

int total_paradas = 35;

char * devuelve_nombre_parada(int parada)
  {
  return array_nombre_parada[parada];
}

int devuelve_parada(int parada)
  {
  return paradas[parada];
}

