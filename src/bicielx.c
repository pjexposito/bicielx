// POR HACER
// Meter las funciones de BD en un único archivo. 
// Quitarle roña al código


#include <pebble.h>
#include "bicielx.h"
#include "bicisdb.h"
  
  
Window* window;
// Barra de opciones
ActionBarLayer *action_bar;

// Capas del programa
TextLayer *dig1_layer, *mensaje_layer, *txtbici_layer, *txtcandado_layer, *textoparada_layer;


// Variables de imágenes
GBitmap *arriba_bitmap, *abajo_bitmap, *pulsar_bitmap, *play_bitmap, *buscar_bitmap, *bici_bitmap, *candado_bitmap;

BitmapLayer *bici_layer, *candado_layer;

// Resto de variables
char texto[1024], tiempo1[1024], tiempo2[1024], tiempo3[1024];
static int numero1, cargando=0, favorito=0;
int tiempo4;

// Asignación para recibir datos
enum {
	KEY_T1 = 0,
	KEY_T2 = 1,
  KEY_T3 = 2,
  KEY_T4 = 3
};


void pinta_datos(void)
{

  static char buffer1[]="123";

  snprintf(buffer1, sizeof(buffer1), "%d", devuelve_parada(numero1));
	text_layer_set_text(dig1_layer, buffer1);
    
  text_layer_set_text(mensaje_layer, devuelve_nombre_parada(numero1)); 
  layer_set_hidden((Layer *)bici_layer, true);
  layer_set_hidden((Layer *)candado_layer, true);
  layer_set_hidden((Layer *)txtcandado_layer, true);
  layer_set_hidden((Layer *)txtbici_layer, true);
  layer_set_hidden((Layer *)mensaje_layer, false);

  
}

void process_tuple(Tuple *t)
{

  int key = t->key;
	switch(key) 
    {
		case KEY_T1:
      strcat(tiempo1, t->value->cstring);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Datos: %s %lu", t->value->cstring, t->key);
      break;
		case KEY_T2:
      strcat(tiempo2, t->value->cstring);
			break;
		case KEY_T3:
      strcat(tiempo3, t->value->cstring);
			break;   
		case KEY_T4:
      tiempo4 = t->value->int32;
			break;  
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Datos: %s %s %s %d %d", tiempo1, tiempo2, tiempo3, tiempo4, devuelve_parada(tiempo4));


}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Acabo de recibir datos (pebble).");

	memset(&tiempo1[0], 0, sizeof(tiempo1));
  memset(&tiempo2[0], 0, sizeof(tiempo2));


  (void) context;	
	Tuple *t = dict_read_first(iter);
	if(t)	process_tuple(t);
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)	process_tuple(t);
	}
  
  
    // CODIGOS DE ERROR
    // 97 = Error 404. La web no existe. Posiblemente por que la parada seleccionada no existe.
    // 98 = Existe la línea y la parada pero no hay datos (posiblemente no circulen autobueses a esas horas).
    // 99 = No pasa esa linea por la parada seleccionada.
    if (strcmp(tiempo1,"99")==0)
      text_layer_set_text(mensaje_layer, "Servicio no disponible.");
    else if (strcmp(tiempo1,"98")==0)
      text_layer_set_text(mensaje_layer, "Parada sin autobuses disponibles.");
    else if (strcmp(tiempo1,"97")==0)
      text_layer_set_text(mensaje_layer, "La parada seleccionada no existe.");
    else
      {
        strcpy(texto,"Bicicletas: ");
        strcat(texto, tiempo1);
        strcat(texto, ". Candados libres: ");
        strcat(texto, tiempo2);
        text_layer_set_text(mensaje_layer, texto);
      }  
    text_layer_set_text(txtbici_layer, tiempo1);
    text_layer_set_text(txtcandado_layer, tiempo2);
    static char texto[80];
    
    snprintf(texto, sizeof(texto), "Cercana: %d", devuelve_parada(tiempo4));
  
    text_layer_set_text(textoparada_layer, texto);

    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, play_bitmap);
    cargando = 0;
    vibes_short_pulse();
    layer_set_hidden((Layer *)bici_layer, false);
    layer_set_hidden((Layer *)candado_layer, false);
    layer_set_hidden((Layer *)txtcandado_layer, false);
    layer_set_hidden((Layer *)txtbici_layer, false);
    layer_set_hidden((Layer *)mensaje_layer, true);

}

void send_int(int16_t parada, const char *linea)
{
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, KEY_T1, parada);
  dict_write_cstring(iter, KEY_T2, linea);
  dict_write_end(iter);
 	app_message_outbox_send();
}

void envia_peticion()
  {
      favorito=0;
      text_layer_set_text(mensaje_layer, "Cargando...");
      cargando = 1;
      //Borro la variable de tiempo 1 y 2 antes de volver a pedir datos.
      memset(&tiempo1[0], 0, sizeof(tiempo1));
      memset(&tiempo2[0], 0, sizeof(tiempo2));
      memset(&tiempo3[0], 0, sizeof(tiempo3));

      send_int(devuelve_parada(numero1),0);
}
  

void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  if (cargando==1) return;
  numero1==total_paradas-1 ? numero1=0 : numero1++;
  pinta_datos();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  if (cargando==1) return;
  numero1==0 ? numero1=total_paradas-1 : numero1--;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Numero: %d. Parada: %d", numero1, devuelve_parada(numero1));

  pinta_datos();
}

void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  if (cargando==1) return;
  envia_peticion();    
}


void select_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
  persist_write_int(FAV5_PKEY, persist_read_int(FAV4_PKEY));
  persist_write_int(FAV4_PKEY, persist_read_int(FAV3_PKEY));
  persist_write_int(FAV3_PKEY, persist_read_int(FAV2_PKEY));
  persist_write_int(FAV2_PKEY, persist_read_int(FAV1_PKEY));
  persist_write_int(FAV1_PKEY, numero1);

  layer_set_hidden((Layer *)bici_layer, true);
  layer_set_hidden((Layer *)candado_layer, true);
  layer_set_hidden((Layer *)txtcandado_layer, true);
  layer_set_hidden((Layer *)txtbici_layer, true);
  layer_set_hidden((Layer *)mensaje_layer, false);
  text_layer_set_text(mensaje_layer, "Parada agregada a favoritos.");

}


void click_config_provider(void *context) 
{
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 2000, select_long_click_handler, NULL);
}


static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_overflow_mode(layer, GTextOverflowModeWordWrap);
	text_layer_set_text_alignment(layer, alignment);
	return layer;
}


void window_load(Window *window)
{

  //Barra de opciones de la derecha
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
  //Asignación de recursos gráficos
  bici_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGEN_BICI);
  candado_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGEN_CANDADO);
  arriba_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICONO_ARRIBA);
  abajo_bitmap =  gbitmap_create_with_resource(RESOURCE_ID_ICONO_ABAJO);
  pulsar_bitmap =  gbitmap_create_with_resource(RESOURCE_ID_ICON_PULSAR);
  play_bitmap =  gbitmap_create_with_resource(RESOURCE_ID_ICONO_PLAY);
  buscar_bitmap =  gbitmap_create_with_resource(RESOURCE_ID_ICONO_BUSCAR);
  //Asignación de iconos a la barra de opciones
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, arriba_bitmap );
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, abajo_bitmap);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, buscar_bitmap);
  //Capas principales del programa
  
  bici_layer = bitmap_layer_create(GRect(10, 84, 54, 36));
  bitmap_layer_set_bitmap(bici_layer, bici_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bici_layer));
 
  candado_layer = bitmap_layer_create(GRect(75, 76, 40, 44));
  bitmap_layer_set_bitmap(candado_layer, candado_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(candado_layer));

  textoparada_layer = init_text_layer(GRect(0, 0, 120, 30), GColorBlack, GColorClear, FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentCenter);
	text_layer_set_text(textoparada_layer, "Parada");
  layer_add_child(window_get_root_layer(window), (Layer*) textoparada_layer);  
  
  mensaje_layer = init_text_layer(GRect(5, 70, 120, 80), GColorBlack, GColorClear, FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentLeft);
	text_layer_set_text(mensaje_layer, "");
  layer_add_child(window_get_root_layer(window), (Layer*) mensaje_layer);
 
  dig1_layer = init_text_layer(GRect(0, 25, 120, 50), GColorBlack, GColorClear, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), (Layer*) dig1_layer);

  txtbici_layer = init_text_layer(GRect(10, 118, 54, 30), GColorBlack, GColorClear, FONT_KEY_GOTHIC_28_BOLD, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), (Layer*) txtbici_layer);
  
  txtcandado_layer = init_text_layer(GRect(75, 118, 40, 30), GColorBlack, GColorClear, FONT_KEY_GOTHIC_28_BOLD, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), (Layer*) txtcandado_layer);

  pinta_datos();
  if (favorito==1) envia_peticion();
}

void window_unload(Window *window)
{
  gbitmap_destroy(arriba_bitmap);
  gbitmap_destroy(abajo_bitmap);
  gbitmap_destroy(pulsar_bitmap);
  gbitmap_destroy(play_bitmap);
  gbitmap_destroy(buscar_bitmap);
  gbitmap_destroy(bici_bitmap);
  gbitmap_destroy(candado_bitmap);
 
//Destroy BitmapLayers
  bitmap_layer_destroy(bici_layer);
  bitmap_layer_destroy(candado_layer);
  action_bar_layer_destroy(action_bar);

  text_layer_destroy(txtbici_layer);
  text_layer_destroy(txtcandado_layer);
  text_layer_destroy(textoparada_layer);
  
  text_layer_destroy(dig1_layer);
  text_layer_destroy(mensaje_layer);
   window_destroy(window);
  persist_write_int(PRINCIPAL_PKEY, numero1);

}

/* Initialize the main app elements */
void carga_paradas(int numero, int fav)
{
  numero1 = numero;
  if (numero1 > total_paradas) numero1 = 0;
	window = window_create();
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
  if (fav==1) favorito=1;
	app_message_register_inbox_received(in_received_handler);					 
	window_set_window_handlers(window, (WindowHandlers) handlers);
	window_stack_push(window, true);


}



