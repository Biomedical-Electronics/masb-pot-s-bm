# Proyecto MASB-POT-S

### Introducción

Este proyecto ha consistido en la configuración y programación de un microcontrolador para poder realizar medidas electroquímicas, como la cronoamperometría y la voltametría cíclica de manera automática mediante un potenciostato. Se ha desarrollado con el objetivo de alcanzar y asentar todos los conocimiento vistos y trabajados en la asignatura de Microcontroladores para Aplicaciones y Sistemas Biomédicos. 

Las autoras de este trabajo son Bruna Martín Català y Maria Mor Basart.





#### Potenciostato

Este proyecto está basado en la programación de un potenciostato formado por un front-end y un back-end basado en la Evaluation Board (EVB) NUCLEO F401RE de STMicroelectronics. 

Un potenciostato es un dispositivo electrónico compuesto por tres electrodos, generalmente, y que se usa para medidas y experimento electroquímicos. 

El sistema consiste de tal manera que el electrodo de trabajo debe mantenerse a un potencial constante con respecto al electrodo de referencia mediante el ajuste de la corriente en el electrodo auxiliar [1]. Así pues, el potenciostato debe polarizar la celda electroquímica a una tensión Vcell y leer la corriente Icell que circula por esta. Para establecer la tensión Vcell, disponemos del DAC MCP4725, cuya dirección I2C es 1100000. Este modelo puede generar una tensión de salida de 0 a 4 V, pero se le ha añadido una etapa para convertir dicha señal a una bipolar de - 4 a 4 V para poder polarizar tanto a tensiones positivas como negativas [2]. 

El potenciostato usado para este proyecto puede verse representado en el siguiente esquema (ver _Figura 1_). 

<p align="center">
<a href="Docs/assets/imgs/potenciometro.jpeg">
<img src="Docs/assets/imgs/potenciometro.jpeg" alt="Esquema del potenciometro usado" width=500 />
</a>
</p>



**_Figura 1._** Esquema del potenciostato usado para este proyecto.

### Voltametría cíclica

En la siguiente figura, podemos observar el flujo realizado que corresponde a la operativa del microcontrolador a la hora de realizar una voltametría cíclica. 











**_Figura X._** Diagrama de flujo seguido para programar el microcontrolador cuando se realiza una voltametría cíclica.

A continuación se explicará con detalle las partes que no estan detalladas en la figura. En primer lugar, definimos y fijamos los valores de las siguientes variables:

- `tomarPunto_cv = TRUE`
- Hemos definido la resistencia `RTIA` de 10.000 Ω
- Fijamos la tensión de la celda `Vcell` a `eBegin` que obtenemos de la `cvConfiguration`.
- Fijamos `vObjetivo` a `eVertex1`
- Obtenemos el `eStep` y la `scanRate` de  la `cvConfiguration` y definimos el periodo entre muestras, dividiendo ambos parámetros.
- Definimos y inicializamos la variable `cycles` a 0.

Cerramos relé y, en caso de que el número de ciclos no haya superado al que está marcado en la `cvConfiguration` y que el _sampling period_ (periodo entre muestras) haya transcurrido, empezaremos las mediciones de la `Vcell` y la `Icell` y enviaremos los valores obtenidos al _host_.

Para empezar estas mediciones, hemos utilitzado la boolean `tomarPunto_CV` en un condicional if, de modo que si esta variable es `TRUE`, empezaremos estas mediciones. Justo después de el condicional cambiaremos el valor de la variable de `TRUE` a `FALSE`, que para hacer la próxima medición volverá a ser `TRUE` usando una de la funciones definidas al final del código de la cronoamperometría.  

Lo que hacemos a partir de ahora es un barrido triangular. Empezaremos aplicando una tensión `eBegin` a la celda e aplicaremos incrementos o decrementos de  `eStep` a esta tensión hasta llegar a la tensión de `eVertex1` y, una vez lleguemos a este, realizaremos lo mismo hasta que lleguemos al valor de `eVertex2`. Una vez lleguemos a este, haremos otro barrido hasta llegar a  `eBegin`. Todo este proceso corresponde a un ciclo, que deberemos repetir tantas veces como lo indique el `cycles` de la `cvConfiguration` .

### Conclusiones

El potenciostato es un instrumento muy usado en experimentos electroquímicos y es clave en el mundo de la ingeniería biomédica para medir diferentes concentraciones de sustancias relevantes que permiten determinar el nivel de salud de un individuo. Este proyecto nos permite tener un primer contacto con la programación de un microcontrolador para este instrumento y, a la vez, nos permite aplicar los conocimientos obtenidos a lo largo de todas las prácticas realizadas en esta asignatura.

Considerando que el resto de prácticas trataban los diferentes aspectos de programar un microcontrolador de modo individual y esta práctica lo hace integrando una gran mayoría de ellas, este proyecto ha supuesto varias dificultades y complicaciones. Sobre todo al crear diferentes archivos con códigos para cada una de las funciones y hacerlos funcionar conjuntamente y también en el momento de compartir y modificar los archivos compartidos con la compañera usando github. 

A nivel de asignatura, 

