# PlotClock

![PlotClock](http://thingiverse-production-new.s3.amazonaws.com/renders/a7/ad/24/29/c3/plotclock_preview_featured.jpg)

[PlotClock](http://www.thingiverse.com/thing:248009) es un dispositivo controlado por Arduino y que dibuja cada minuto la hora con un rotulador(borrando la dibujada anteriormente)

Utiliza un mecanismo sencillo y 3 servomotores para hacer los movimientos del rotulador.

En este [vídeo](https://www.youtube.com/embed/iOLFP90DneY) podemos verlo en funcionamiento

<iframe width="560" height="315" src="https://www.youtube.com/embed/iOLFP90DneY" frameborder="0" allowfullscreen></iframe>

## Construcción

Todo el diseño de [PlotClock](http://www.thingiverse.com/thing:248009)  está liberado con licencia de [Atribución Creative Commons](http://creativecommons.org/licenses/by/3.0/)

Para su construcción se requieren una serie de piezas que pueden ser cortadas por laser a partir de [este diseño](http://www.thingiverse.com/download:1697382) o impresas con una impresora 3D a partir de los [archivos STLs](http://www.thingiverse.com/thing:250204/zip)

![Partes](http://thingiverse-production-new.s3.amazonaws.com/renders/dc/e6/bb/ff/31/151018_plotclock_preview_featured.jpg)

El montaje de las piezas es sencillo

![montaje1](http://thingiverse-production-new.s3.amazonaws.com/renders/e3/ea/de/15/61/lifting_preview_featured.jpg)

Utilizando tornillos (de 10mm de largo) y tuercas de métrica M3.

Su naturaleza abierta ha permitido que se realicen [muchas variantes](http://www.thingiverse.com/thing:248009/#remixes) y mejoras.

## Componentes

* Piezas impresas o cortadas con láser
* Tornillos y tuercas M3
* 3 Servos de 9g
* Placa Arduino
* Algunos cables
* Baterías o alimentador eléctrico

## Programación

Para su programación basta con descargar el [código del autor](http://www.thingiverse.com/download:1697346) e instalarlo en nuestra placa arduino

![montaje](https://i1.wp.com/www.geekmomprojects.com/wp-content/uploads/2014/12/plotclock_electronics-e1419920478217.jpg)

Para calibrarlo debemos medir lo más exactamente posible que los servos se desplazan 90º alrededor del eje vertical

Una vez calibrado comentamos la línea

    #define CALIBRATION      // enable calibration mode

dejándola así


    // #define CALIBRATION      // enable calibration mode

Podemos mejorar la precisión conectando un reloj de tiempo real a Arduino (RTC) y activando la opción en el código


    #define REALTIMECLOCK    // enable real time clock

## Referencias para el montaje

### [Building PlotClock](http://www.geekmomprojects.com/building-plotclock/)

### [STLs](http://www.thingiverse.com/thing:250204/#files)

### [Montaje detallado ](http://linksprite.com/wiki/index.php5?title=Plotclock_DIY_Robot_with_Arduino_Uno_Drawing_Robot)

### [Extensión del área de dibujo](http://techiemalc.blogspot.com.es/2015/11/simulation-of-plot-clock-for.html)


### [PlotClock for Dummies](https://www.instructables.com/id/Plot-Clock-for-dummies/)

### [Más instrucciones para PlotClock](http://www.thingiverse.com/thing:931498)

### [Mejora de PlotClock](http://www.thingiverse.com/thing:346784)

### [Diseño más robusto](http://www.thingiverse.com/thing:371248) ([una pequeña modificación](http://www.thingiverse.com/thing:940737))

### [Sensor pir para que sólo dibuje si hay alguien](https://www.thingiverse.com/thing:386449)

![Diseño más robusto](http://www.thingiverse.com/thing:371248)

### [Vídeo explicación](https://youtu.be/2DZSj8b2RGw)


![licencia](./images/Licencia_CC.png)
