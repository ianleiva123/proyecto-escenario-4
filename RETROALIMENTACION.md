# Retroalimentación de la Etapa 1

**EIF-201 Programación I. II Ciclo 2026. Universidad Nacional, Sede Regional Brunca.**

Ian Leiva Araya. Entrega individual.

Este documento revisa el estado del repositorio en la entrega de la Etapa 1. Cada punto indica el
archivo y la línea, qué ocurre y cómo corregirlo.

En el código vas a encontrar además comentarios `// [REVISION]` colocados justo donde está cada
hallazgo. Podés borrarlos a medida que vayas corrigiendo. Los números de línea de este documento
corresponden al código tal como lo entregaste; al agregar esos comentarios las líneas se corrieron
unas pocas posiciones, así que si el número no cae exacto, guiate por el comentario.

Todo lo que aparece acá debe quedar corregido para la Etapa 2. Lo que quede sin corregir se vuelve
a señalar en la siguiente entrega.

---

## Lo que quedó bien resuelto

**El motor de evolución está correcto.** `evolve()` en `Mundo.cpp:414-465` reserva `nextCells` en la
línea 421, lee de `cells` en la 434, escribe en `nextCells` en la 442 y la 449, e intercambia en la
462. La actualización es simultánea de verdad, que es el punto donde más proyectos se caen.

**El vecindario y los bordes están bien.** Los 8 vecinos en `Mundo.cpp:241-262`, con el centro
salteado, y borde cerrado con `isInside()` en las líneas 232-235. No hay aritmética modular sobre
índices en ninguna parte.

**La matriz es propia, sin STL.** `Cell** cells` en `Mundo.h:15`, reservada en `Mundo.cpp:178-183`.
No hay `vector`, `array` ni otra colección; solo `std::string` para el nombre de especie.

**El desempate es el correcto.** `getDominantSpecies()` en `Mundo.cpp:267-317`: el menor índice sale
de `dominantSpecies = 1` en la línea 301 y del `>` estricto en la 306, tal como pide la Configuración
4 asignada, que además está reproducida valor por valor.

**La memoria está impecable, y esto conviene decirlo con detalle porque es lo más difícil de lograr.**
No hay fugas ni punteros colgantes en 1459 líneas. `World` tiene constructor de copia
(`Mundo.cpp:40-88`), destructor (`:90-96`) y `operator=` con comprobación de autoasignación
(`:98-155`). `SpeciesRules` tiene los tres (`Reglas.cpp:36-57`, `59-62`, `64-92`). Todos los `new[]`
tienen su `delete[]`, incluidos los temporales de `Mundo.cpp:314` y `:601` y los arreglos del menú 9
en `Simulador_Vida.cpp:321` y `346`. Los accesos a la matriz pasan siempre por `isInside()`
(`Mundo.cpp:254`, `289`, `335`, `401`), y la conversión de especie a índice con `species - 1` va
precedida de una validación de rango en `Mundo.cpp:293`, `340`, `575` y `671`. No hay división por
cero.

**El código está en inglés**, los atributos son `private`, los métodos de consulta llevan `const`, y
el menú de 11 opciones (`Consola.cpp:24-47`) es completo y navegable.

**El `.gitignore` está bien.** Es la plantilla de Visual Studio y cubre todo lo que hace falta.
Verificado con `git ls-files`: el repositorio tiene los 9 fuentes, el `.gitignore`, el `README.md` y
los archivos de proyecto, sin ejecutables, objetos ni carpetas de compilación. Dejalo tal cual.

---

## Qué corregir, en orden de prioridad

### 1. El patrón predefinido no coincide con el que muestra el menú

Antes de pedir la posición, `Simulador_Vida.cpp:197-201` te muestra el patrón que va a colocar:

```
. W .
. . S
G G G
```

Lo que termina en el tablero es otra cosa:

```
. S .
. . W
G W S
```

La causa está en `Mundo.cpp:403`, que calcula la especie con `((i + j) % speciesCount) + 1` sobre las
coordenadas dentro del patrón 3x3. La celda (0,1) da `(0+1)%3+1 = 2`, o sea S, y el menú prometía W.
La (2,1) da 1, o sea W, y el menú prometía G. Cuatro de las cinco celdas salen con la especie
equivocada.

Para reproducirlo: configurá 10x10, opción 4, fila 1 columna 1, y mirá el resultado con la opción 5.

**Corrección:** guardá la especie dentro del patrón en vez de calcularla. Cambiá el
`int pattern[3][3]` de `Mundo.cpp:382-387` por los valores de especie directos,
`{0,1,0},{0,0,2},{3,3,3}`, y usá ese valor en la línea 403 en lugar de la fórmula.

### 2. Resembrar reinicia el contador de generación

`seedRandom()` (`Mundo.cpp:348-378`) y `seedPattern()` (`:380-412`) llaman `clearWorld()` en las
líneas 360 y 389. Vaciar el tablero antes de sembrar está bien, porque la densidad se define sobre el
total de celdas del mundo. El problema es otro: `clearWorld()` también hace `generation = 0` en
`Mundo.cpp:482`, y las dos siembras lo vuelven a poner en 0 por su cuenta en las líneas 377 y 411.
`seedManual()` (`:333`) no toca el contador.

Se ve así: sembrá, avanzá 30 generaciones con la opción 7, colocá el patrón con la opción 4 y pedí la
opción 8. El contador volvió a 0, y el número que imprime `showStatistics()` ya no corresponde a lo
simulado. Encima el comportamiento cambia según cómo sembrés: con la opción 2 el contador se mantiene,
con la 3 y la 4 se reinicia.

**Corrección:** dejá el reinicio en un solo lugar y hacé que las tres siembras se comporten igual.
Quitá `generation = 0;` de las líneas 377 y 411, que ya es redundante, y decidí si `clearWorld()`
reinicia o no. Si reinicia, imprimí un aviso al sembrar para que el usuario sepa que el conteo vuelve
a empezar.

### 3. El menú de reglas acepta valores que nunca se pueden cumplir

Una celda tiene entre 0 y 8 vecinos. Un valor negativo, o mayor que 8, se guarda igual y no se cumple
nunca, así que la regla queda muerta. `Simulador_Vida.cpp:306` y `:331` validan cuántos valores se
ingresan, pero ningún punto valida cuál. `World::setBirthRules` en `Mundo.cpp:650-667` y
`SpeciesRules::setSurvivalRules` en `Reglas.cpp:136-157` tampoco lo hacen.

Probalo así: opción 9, "Cantidad de valores de nacimiento: 1", "Valor de nacimiento: 12". El programa
responde "Reglas actualizadas correctamente." y desde ahí no nace ninguna celda. Nada le indica al
usuario por qué.

El tope de cantidad tampoco corresponde. Las líneas 306 y 331 rechazan más de 8 valores, pero los
conteos posibles son nueve, de 0 a 8, así que con ese tope no se puede listar el conjunto completo.

**Corrección:** validá cada valor al leerlo, en `Simulador_Vida.cpp:316` y `:341`, con
`valor >= 0 && valor <= 8`, y volvé a pedirlo si queda fuera. Subí el tope de cantidad de 8 a 9 en las
líneas 306 y 331.

### 4. El menú de reglas dice que actualizó cuando no actualizó

El `break` de `Simulador_Vida.cpp:331-335` sale del `for (species...)` completo, no solo de la
iteración actual.

Se dispara así: opción 9, nacimiento con 2 valores (3 y 6), y para E1 se ingresa "Cantidad de valores:
0". Aparece "Cantidad invalida.", se corta el bucle de especies, E2 y E3 nunca se preguntan y quedan
con las reglas anteriores. Acto seguido la línea 351 imprime "Reglas actualizadas correctamente.", que
contradice lo que pasó.

**Corrección:** cambiá ese `break` por `continue` para volver a pedir la especie, o mové el mensaje de
éxito adentro de un `if` que compruebe que las tres quedaron configuradas.

### 5. `setDefaultRules()` puede escribir sobre un puntero nulo

Está declarado `public` en `Mundo.h:55` y en `Mundo.cpp:208` escribe `speciesRules[0]` sin comprobar
nada. Sobre un `World` construido con el constructor por defecto de `Mundo.cpp:8-21`, `speciesRules`
vale `nullptr` y `speciesCount` vale 0, así que la llamada escribe en nulo y el programa cae. Hoy no
se dispara, porque `main` solo llega ahí a través de `configure()`. Aun así queda expuesto en la
interfaz pública de la clase.

**Corrección:** mové la declaración a la sección `private` de `Mundo.h`, junto a `clearCells()`.

### 6. La cantidad de especies no se puede elegir

`configure()` en `Mundo.cpp:157-190` pide filas y columnas. La cantidad de especies no:
`Mundo.cpp:159` rechaza todo valor distinto de 3 y `main` la pasa fija desde `Simulador_Vida.cpp:11`.
El enunciado incluye la cantidad de especies dentro de la configuración inicial.

**Corrección:** aceptá 3 o 4 en `configure()` y creá esa cantidad de `SpeciesRules`. Los tres lugares
que hoy recorren la constante ya usan `speciesCount`, así que el cambio es acotado.

### 7. El encabezado de columnas se desalinea a partir de la columna 10

Cada celda de la rejilla ocupa 2 caracteres (`". "`), pero `Mundo.cpp:503` imprime
`cout << j + 1 << " ";`, y de la columna 10 en adelante ese número ocupa 3. Desde ahí el encabezado
deja de indicar la columna real. Como el mundo llega hasta 50x50, alcanza a casi cualquier
configuración útil, y la siembra manual pide justamente coordenadas que se leen de ese encabezado.

Para verlo: configurá 10x15 y pedí la opción 5.

**Corrección:** incluí `<iomanip>` y escribí `cout << setw(2) << j + 1 << " ";`. Las filas ya están
alineadas en `Mundo.cpp:510-517`.

### 8. Los nombres de especie cambian entre el mapa y las estadísticas

La leyenda de `Mundo.cpp:546-548` dice `W = Worker`, `S = Scout` y `G = Guardian`. Las estadísticas de
la línea 596 usan `getName()`, que devuelve "Trabajador", "Explorador" y "Guardian" según las líneas
208-214. El usuario ve dos nombres distintos para la misma especie.

**Corrección:** usá `speciesRules[i].getName()` también en la leyenda, y dejá los tres nombres en
español.

### 9. Avanzar varias generaciones vuelca todas las rejillas seguidas

`Simulador_Vida.cpp:269-274` llama `display()` en cada vuelta sin pausa. Avanzá 50 generaciones en un
mundo 30x30 y salen 50 rejillas de corrido: el búfer de la consola se llena y las primeras ya no se
pueden leer. El requisito pide mostrar el mundo tras cada paso, y para eso tiene que quedar visible.

**Corrección:** limpiá la pantalla entre pasos, o pedí Enter cada cierto número de generaciones.

### 10. `getDominantSpecies()` reserva y libera memoria por cada celda vacía

`Mundo.cpp:269` hace `new int[speciesCount]` y la línea 314 lo libera. La función corre una vez por
cada celda vacía candidata y por cada generación, así que en un mundo 50x50 durante 100 generaciones
son cientos de miles de reservas.

**Corrección:** como `speciesCount` está acotado, usá un arreglo local en la pila:
`int speciesCountList[MAX_SPECIES] = { 0 };`. No hay que reservar ni liberar nada.

### 11. `readInteger` se cuelga si la entrada llega a su fin

`readInteger` en `Simulador_Vida.cpp:13-32` usa `cin.clear()` y `cin.ignore()` para recuperarse de una
entrada del tipo equivocado, y ahí funciona. Con fin de archivo el caso es distinto: `clear()` quita
el `failbit`, pero la lectura siguiente vuelve a fallar de inmediato y el bucle no termina.

Pasa al ejecutar `Simulador_Vida.exe < entradas.txt` cuando el archivo se agota, o al pulsar Ctrl+Z en
la consola. El programa imprime "Entrada invalida." sin parar y hay que matarlo.

**Corrección:** adentro del bucle, después de `cin.clear()`, agregá `if (cin.eof()) { return 0; }`, o
salí del programa de forma ordenada. Lo mismo en `readDouble` (`Simulador_Vida.cpp:34-52`).

### 12. Faltan las tildes en los mensajes al usuario

Los mensajes van sin acentos, por ejemplo `Consola.cpp:38` "Ver estadisticas".

**Corrección:** escribí los acentos y llamá `setlocale(LC_ALL, "spanish")` al inicio de `main`, o
configurá la página de códigos con `SetConsoleOutputCP(1252)`. Es una pasada de una sola vez.

---

## Lo que tiene que llevar el repositorio de la Etapa 2

**Un `README.md` con cuatro apartados.** Portada con nombre completo, carné, curso y profesor.
Instrucciones de compilación y uso: abrir `Simulador_Vida.slnx` en Visual Studio, compilar en Debug
x64, ejecutar. La configuración asignada, que ya está escrita en `Simulador_Vida.cpp:66-80` y se puede
copiar de ahí: Configuración 4 Enjambre, nacimiento con 3 o 6 vecinos, supervivencia con 2 o 3 vecinos
para las tres especies, desempate por menor índice. Y qué hace cada opción del menú.

**Un apartado "Declaración de uso de IA" dentro del README.** Indicá si usaste IA y en qué partes del
trabajo. Si no la usaste, escribilo igual. Lo que se evalúa es que el apartado esté presente, no lo
que diga.

**Al menos 10 commits repartidos en dos semanas.** Esta vez fueron 8 commits en 2 días, el 14 y el 15
de setiembre. El enunciado pide un mínimo de 10 commits significativos y que el historial refleje al
menos dos semanas de trabajo. Hacé commit desde el primer día y al menos uno por sesión, aunque el
avance sea parcial: una clase terminada, un método que ya funciona, una corrección.

Un detalle técnico del mismo tema: 7 de tus 8 commits aparecen como `Ian_H <Ian_H@DESKTOP-QT38I9P>`,
que es la identidad que Git usa cuando no está configurada. Corregilo con
`git config --global user.name "Ian Leiva Araya"` y
`git config --global user.email "tu.correo@est.una.ac.cr"`, así el historial queda a tu nombre.

**Mensajes de commit que digan qué cambió en el código.** El formato es un verbo y el cambio concreto,
por ejemplo "Agrega conteo de vecinos Moore con bordes cerrados" o "Corrige el reinicio del contador
de generacion".

**Carpetas `src/` y `build/`.** Mové los `.h` y `.cpp` a `src/` y apuntá la salida del compilador a
`build/`.

---

## Buenas prácticas

15 tipos de falta. Se cuentan tipos, no ocurrencias.

| Regla | Ocurrencias | Ejemplo | Solución |
|---|---|---|---|
| **BP-F1** Espacios alrededor de operadores | ~40 | `Mundo.cpp:432` `int currentSpecies =cells[i][j].getSpecies();`; `:368` `if (randomValue <density * 100)`; `Simulador_Vida.cpp:92` `int rows =readInteger(...)` | Poné un espacio a cada lado de los operadores binarios y después de cada coma. En Visual Studio, Ctrl+K seguido de Ctrl+D te corrige el archivo entero |
| **BP-A1, BP-C2** El archivo no se llama como la clase | 5 archivos | `Celda.h` contiene `class Cell`; `Mundo.h` contiene `class World`; `Reglas.h` contiene `class SpeciesRules`; `Consola.h` contiene `class ConsoleDisplay` | El contenido ya está en inglés, así que solo hay que tocar los nombres y los `#include`. Renombrá a `cell.h`, `world.h`, `species_rules.h` y `console_display.h` |
| **BP-A1** Mayúsculas del `#include` | 1 | `Reglas.cpp:1` incluye `"reglas.h"` cuando el archivo es `Reglas.h` | Escribí `#include "Reglas.h"`. Windows ignora mayúsculas y minúsculas, Linux y macOS no, así que hoy el proyecto solo compila en Windows |
| **BP-V3** Números mágicos | ~12 | `Mundo.cpp:159` `rows < 5 \|\| rows > 50 \|\| ... speciesCount != 3`; `:366` `rand() % 100`; `Simulador_Vida.cpp:30` `cin.ignore(10000, '\n')` | `const int MIN_SIZE = 5;`, `MAX_SIZE = 50;`, `SPECIES_COUNT = 3;`, `PERCENT_BASE = 100;` |
| **BP-V3, BP-D6** Enteros donde corresponde una enumeración | todo el proyecto | `Celda.h:4` `int species;`; `Mundo.cpp:523-538` decide el carácter a imprimir con una cadena de `if/else if` sobre 0, 1, 2 y 3 | `enum class Species { Empty, Worker, Scout, Guardian };` y usalo como tipo del atributo. Con eso se van de una sola vez la cadena de comparaciones y los números sueltos |
| **BP-V2** Colecciones sin sufijo | 7 | `Mundo.h:15` `Cell** cells`; `:17` `int* birthRules`; `Mundo.cpp:269` `int* speciesCounts` | `cellMatrix`, `birthRuleList`, `speciesCountList` |
| **BP-M6** El parámetro se llama igual que el atributo | 10 | `Celda.cpp:18-21` `void Cell::setSpecies(int species) { this->species = species; }`; `Mundo.cpp:23-38`; `Reglas.cpp:11-19` | Prefijá el parámetro: `setSpecies(int newSpecies) { species = newSpecies; }`. Así desaparecen los 10 `this->` |
| **BP-M5** Más de 2 parámetros | 3 | `Mundo.h:50-53` `configure(rows, columns, speciesCount)`; `:57-60` `seedManual(row, column, species)`; `:82-85` `setSpeciesSurvivalRules(species, rules, count)` | Agrupá los parámetros en clases chicas. Una con filas y columnas, y otra con el arreglo de valores y su cantidad, te cubren los tres casos |
| **PR-X4** Código duplicado | 9 | El bloque `if (!world.isConfigured()) { cout << "Primero configure el mundo." << endl; break; }` está repetido igual en `Simulador_Vida.cpp:116, 165, 189, 225, 239, 254, 280, 293, 358` | Extraé `bool isWorldReady(const World& aWorld)` que imprima el mensaje y devuelva `false`. Cada caso queda en una línea |
| **BP-F4, BP-D1** Más de 4 niveles de anidación | 2 funciones | `main` llega a 5: `while`, `switch`, bloque del `case`, `while`, `if` (`Simulador_Vida.cpp:82-161`). El `case 9` llega a los mismos 5 (`:291-353`) | Sacá cada caso a su propia función, por ejemplo `runManualSeeding(world)` y `runRuleConfiguration(world)`. `main` baja a unas 40 líneas |
| **BP-D3** `else` evitable | 5 | `Reglas.cpp:30-33`, `:53-56` y `:85-88` usan `else { survivalRules = nullptr; }`; `Simulador_Vida.cpp:177-182` y `:213-218` usan `else` donde el resto del archivo usa cláusula guarda | En `Reglas.cpp`, poné `survivalRules = nullptr;` antes del `if` y borrá el `else`. En `Simulador_Vida.cpp`, aplicá el patrón que ya usás en las otras opciones |
| **BP-A2** Cabecera sin guarda de inclusión | 1 | `Celda.h` no tiene `#pragma once` ni `#ifndef`. Es la única; las otras tres sí la tienen | Agregá `#pragma once` como primera línea |
| **BP-E2** Método interno expuesto | 1 | `Mundo.h:55` `setDefaultRules()` declarado `public` | Movelo a `private`. Es el punto 5 |
| **BP-N3** Constante fuera de convención | 1 | `Simulador_Vida.cpp:11` `const int speciesCount = 3;` | Escribí `const int SPECIES_COUNT = 3;`, y declarala dentro de `main` en vez de dejarla global |
| **BP-V5** Variables sin inicializar | 2 | `Simulador_Vida.cpp:15` `int value;`; `:36` `double value;` | `int value = 0;` y `double value = 0.0;` |
| **BP-A3** Estructura de carpetas | 1 | El código está en `Simulador_Vida/Simulador_Vida/`, sin `src/` ni `build/` | Mové los `.h` y `.cpp` a `src/` |

Una observación que no descuenta: `Reglas.h:4` y `Mundo.h:5` tienen `using namespace std;` dentro de
la cabecera, y eso se le impone a todo archivo que las incluya. Quitalo de los `.h` y escribí
`std::string` calificado.

---

## Antes de la Etapa 2

- [ ] Guardar la especie dentro del patrón en vez de calcularla (punto 1)
- [ ] Unificar el reinicio del contador de generación (punto 2)
- [ ] Validar los valores de las reglas entre 0 y 8, y subir el tope a 9 (punto 3)
- [ ] Cambiar el `break` por `continue` en el menú de reglas (punto 4)
- [ ] Mover `setDefaultRules()` a `private` (punto 5)
- [ ] Permitir elegir 3 o 4 especies (punto 6)
- [ ] Alinear el encabezado de columnas y unificar los nombres de especie (puntos 7 y 8)
- [ ] `README.md` con portada, instrucciones, reglas y declaración de uso de IA
- [ ] Configurar tu identidad de Git y empezar a commitear desde el primer día

El motor de evolución y la gestión de memoria están muy bien resueltos. Lo que hay que enderezar es
la validación de las entradas y el paquete de entrega.
