# proyecto-escenario-4

Ian Leiva Araya

605070314

Programacion 1

Ruben Mora Vargas

Mecánica propia: mutación de especies

Agregamos una mutación que permite que una celda viva cambie de especie durante la evolución. Esto puede modificar la distribución de la población y producir resultados distintos aunque se parta del mismo tablero.

Regla de funcionamiento

En cada generación, primero se aplican las reglas normales de supervivencia. Si una celda sobrevive y la mutación está activada, se genera un número con `rand() % 100`. Si ese número es menor que el porcentaje configurado, la celda cambia a otra de las especies disponibles, sin conservar su especie original.

La mutación no afecta a las celdas que mueren ni a las que nacen durante ese paso. La celda que cambia de especie utilizará las reglas de su nueva especie en la siguiente generación.


Este proyecto tuvo uso de IA en el diseño.
