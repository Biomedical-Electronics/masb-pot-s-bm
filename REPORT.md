# Proyecto MASB-POT-S

introducción:

- en que consisteix un potenciostat
- quins son els objectius
- en que consisteixen les mesures electroquimiques y per a que serveixen.

diagrama de fluxe y explicació de l'aplicació final

els resultats obtinguts

conclusions del projecte i de l'assignatura.

### crono

### Voltametría cíclica

definiriem volta ciclica

En primer lugar, definimos y fijamos los valores de las siguientes variables:

- `tomarPunto_cv = TRUE`
- Hemos definido la resistencia `RTIA` de 10.000 Ω
- Fijamos la tensión de la celda `Vcell` a `eBegin` que obtenemos de la `cvConfiguration`.
- Fijamos `vObjetivo` a `eVertex1`
- Obtenemos el `eStep` y la `scanRate` de  la `cvConfiguration` y definimos el periodo entre muestras, dividiendo ambos parámetros.
- Definimos y inicializamos la variable `cycles` a 0.

Cerramos relé y, en caso de que el número de ciclos no haya superado al que está marcado en la `cvConfiguration` y que el _sampling period_ (periodo entre muestras) haya transcurrido, empezaremos las mediciones de la `Vcell` y la `Icell` y enviaremos los valores obtenidos al _host_.

(que torni a explicar el tomarPunto)

