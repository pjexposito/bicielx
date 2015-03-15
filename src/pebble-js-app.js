var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 10000, 
  timeout: 10000
};


var posiciones = [
[38.272036,-0.694305],
[38.268457,-0.696731],
[38.270117,-0.685963],
[38.260487,-0.698565],
[38.264171,-0.695433],
[38.265409,-0.699424],
[38.273904,-0.673121],
[38.270115,-0.713323],
[38.259831,-0.701275],
[38.256839,-0.691995],
[38.261942,-0.708752],
[38.268924,-0.706933],
[38.274553,-0.708405],
[38.262993,-0.688257],
[38.26573,-0.71128],
[38.27609,-0.71529],
[38.27662,-0.67801],
[38.27832,-0.67195],
[38.26674,-0.69328],
[38.25835,-0.70852],
[38.25923,-0.71090],
[38.25399,-0.70548],
[38.25734,-0.70461],
[38.27553,-0.66915],
[38.26718,-0.70259],
[38.27425,-0.69111],
[38.27921,-0.68589],
[38.26253,-0.70313],
[38.27550,-0.70319],
[38.27052,-0.67020],
[38.26186,-0.68264],
[38.27272,-0.68108],
[38.26175,-0.71614],
[38.25961,-0.68586],
[38.25515,-0.71483]
];
    
var dict, lat_global, long_global, distancia, global_menor;

function distance(lat1, lon1, lat2, lon2) {
	var radlat1 = Math.PI * lat1/180;
	var radlat2 = Math.PI * lat2/180;
	var theta = lon1-lon2;
	var radtheta = Math.PI * theta/180;
	var dist = Math.sin(radlat1) * Math.sin(radlat2) + Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta);
	dist = Math.acos(dist);
	dist = dist * 180/Math.PI;
	dist = dist * 60 * 1.1515;
	dist = dist * 1.609344;
	return dist;
}                                                                           

function HTTPGET(url) {
	var req = new XMLHttpRequest();
  req.open("GET", url, false);
	req.send(null);
  
   //console.log(url + " es " + req.status);

  if (req.status == "404") 
    {
    //console.log("ME HA DADO ERROR 404");
    return 1;
  
    }
  else
    {
    //console.log("LA URL EXISTE. NO HAY ERROR");
    return req.responseText;
    }
}

function BuscaParadas(parada,linea) {
    //console.log("URL: http://www.auesa.es/paradas_qr/"+parada+".php?vari="+linea);

    //var response = HTTPGET("https://clientes.domoblue.es/onroll_data/infoOnroll.php?key=AQUI VA LA CLAVE API");
    var response = HTTPGET("https://dl.dropboxusercontent.com/u/11937/bicis.xml");

    var datos = [];                  // Good 
    var i;
    //console.log(response);
    // CODIGOS DE ERROR
    // 97 = Error 404. La web no existe. Posiblemente por que la parada seleccionada no existe.
    // 98 = Existe la línea y la parada pero no hay datos (posiblemente no circulen autobueses a esas horas.
    // 99 = No pasa esa linea por la parada seleccionada.
    if (response.match(/(errores)/) !== null)
      {
        console.log("Error");
        for (i = 1; i < 7; i++) { 
          datos[datos.length] = 99;
        }
      }
    else
    {
    console.log("Tengo datos: "  + response);
    var re = new RegExp('parada codigo="'+parada+'" nombre="(.*?)" estado="(.*?)" bicicletas="(.*?)" candadosLibres="(.*?)" nombreMapa=".*? lat="(.*?)" lng="(.*?)"'); 
    console.log("Mando: " + re);

    if (response.match(re) !== null)
    {
      for (i = 1; i < 7; i++) { 
        datos[datos.length] = response.match(re)[i];
      }
      console.log("Buscando: "  + datos[0] + " " + datos[1] + " " + datos[2] + " " + datos[3] + " " + datos[4] + " " + datos[5]);
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
    distancia = distance(lat_global, long_global, datos[4], datos[5]);
  console.log('lat= ' + lat_global + ' lon= ' + long_global);
  var menor=99999;
  var distancia_anterior = 99999;
      for (var x = 0; x < posiciones.length; x++) { 
          distancia = distance(lat_global, long_global, posiciones[x][0], posiciones[x][1]);
          if (distancia_anterior>distancia) 
          {
            menor = x;
            distancia_anterior = distancia;
          }
        console.log('Para ' + x + ' es  ' + distancia + ". Anterior es :" + distancia_anterior + ". Menor vale: " + menor);
      }
    global_menor = menor;
      console.log("El menor es la posición: " + menor);
    console.log("La distancia es: " + distancia);
    }
    else
    {
      console.log("La parada no existe");
        for (i = 1; i < 7; i++) { 
          datos[datos.length] = 98;
        }
    }  
    }
  if (distancia < 0) distancia = 0;

	//Este es el diccionario con los datos que voy a mandar al reloj
  dict = {"KEY_T1" : datos[2], "KEY_T2": datos[3], "KEY_T3": distancia.toFixed(2), "KEY_T4": global_menor};

	//Mando los datos de dirección al reloj
  console.log("Voy a mandar datos: "+datos[2] +", "+datos[3]+", "+ distancia.toFixed(2));

	Pebble.sendAppMessage(dict);
}


function locationSuccess(pos) {
  lat_global = pos.coords.latitude;
  long_global = pos.coords.longitude;
    console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);

}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

Pebble.addEventListener("ready",
  function(e) {
	//Se activa el reloj por primera vez. Se piden los datos por primera vez
  //console.log("Evento Ready");
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);

  }
);



Pebble.addEventListener("appmessage",
  function(e) {
    //Se refresca la información
    //console.log("Mensaje recibido:" + e.payload.KEY_T1 + " " + e.payload.KEY_T2);

  var parada=e.payload.KEY_T1;
  var linea=e.payload.KEY_T2;
  BuscaParadas(parada, linea);
  }
);
