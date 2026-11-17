# Proyecto, Etapa 2 (EIF-201): evaluación

**Estudiante:** Ian Leiva Araya (entrega individual)
**Commit evaluado:** `dcbd8d0` ("Update README.md", 13 de octubre, 23:37)

## NOTA FINAL: 77,7

Esta etapa no tiene defensa, así que la nota es la suma ponderada de los rubros.

| Rubro | Peso | Porcentaje | Puntos |
|---|:---:|:---:|:---:|
| Funcionalidad del simulador (R1–R12) | 50% | 88% | 44,0 |
| Claridad del código y organización | 20% | 64% | 12,8 |
| Calidad visual y temática | 10% | 74% | 7,4 |
| Manejo de errores y estabilidad | 10% | 85% | 8,5 |
| Configuración del proyecto y Git | 10% | 50% | 5,0 |
| **Total** | 100% | | **77,7** |

El proyecto compila en Debug x64. Para hacerlo tuve que cambiar la ruta de SFML del `.vcxproj`, que apunta a tu computadora (ver más abajo).

## Lo que quedó bien

- **La arquitectura de la etapa está muy bien pensada.** El laboratorio es una lista enlazada propia (`laboratory.h`) y cada mundo guarda su historial en una lista doblemente enlazada propia (`timeline.h`), con los nodos privados dentro de cada clase. No usaste ninguna colección de la STL.
- **La memoria está impecable.** `World` y `Timeline` tienen constructor de copia, `operator=` y destructor. Duplicar un mundo copia todo su historial en profundidad (`laboratory.cpp:21-29`). Al modificar el pasado, `discardFuture()` libera de verdad los nodos del futuro (`timeline.cpp:32-37`). Al eliminar un mundo se libera todo su historial. No encontré fugas ni dobles liberaciones.
- **La línea de tiempo funciona como pide el enunciado.** Avanzar reutiliza el futuro guardado si existe, retroceder es inmediato, y cualquier cambio de celda o de regla pasa por `changed()`, que descarta el futuro y avisa con "Futuro descartado.".
- **La lógica no sabe nada de SFML.** `world.h`, `species.h`, `timeline.h` y `laboratory.h` no incluyen SFML, y `main` usa la vista a través de la clase abstracta `View`.
- **Las especies se distinguen sin depender del color:** cuadrado, círculo, triángulo y rombo, más la letra cuando la celda es grande.
- **La mutación está bien hecha y bien documentada.** Es observable, se activa y se ajusta desde la interfaz, y el README explica la regla exacta.
- **La interfaz es clara.** Todo se hace con el mouse, el formulario de creación está dentro de la ventana y valida los datos, eliminar pide confirmación, y la línea de mensajes confirma cada acción.
- **Corregiste los cinco errores funcionales de la Etapa 1**, además de los nombres de archivo, `src/` y `build/`, el `#pragma once`, los `this->` y el `main` gigante.

## Lo que conviene mejorar

En orden de prioridad. En tu copia del código dejé comentarios `// FEEDBACK:` en los puntos principales.

1. **El historial de Git sigue concentrado en dos días** (pendiente de la Etapa 1). Los 11 commits de la etapa son del 12 y el 13 de octubre, y el enunciado pide al menos dos semanas de trabajo. Además, 10 de ellos salen como `Ian_H <Ian_H@DESKTOP-QT38I9P>`, porque Git no está configurado en tu computadora. Corregilo con `git config --global user.name "Ian Leiva Araya"` y `git config --global user.email ian.leiva.araya@est.una.ac.cr`, y hacé commit en cada sesión de trabajo, con mensajes que digan qué cambió en el código ("finalizacion." o "Update README.md" no lo dicen).

2. **El README está incompleto.** Le faltan la explicación del laboratorio (crear, duplicar, eliminar y alternar mundos), la línea de tiempo, los créditos de recursos y las instrucciones de compilación y uso (los controles de mouse y teclado). La declaración de IA dice "tuvo uso de IA en el diseño", pero tiene que indicar **qué herramienta** usaste y **qué elementos** generó. Recordá que la IA solo se permite en lo estético. Para los créditos, citá la fuente DejaVu Sans y su licencia, que ya tenés en `assets/FONT_LICENSE.txt`.

3. **El proyecto no compila ni corre desde un clon limpio.** `Simulador_Vida.vcxproj:129` y `134` apuntan a `C:\Users\Ian_H\source\repos\...`. Como SFML ya está en el repositorio, usá `$(ProjectDir)..\SFML-2.5.1\include` y `$(ProjectDir)..\SFML-2.5.1\lib`. Además, el `.gitignore` excluye la carpeta `bin/` de SFML, donde están las DLL, así que el `.exe` no arranca. Agregá la excepción `!Simulador_Vida/SFML-2.5.1/bin/*.dll` y un paso posterior a la compilación que las copie a `build\`. Solo Debug x64 tiene SFML configurado; configurá también Release.

4. **Hay coordenadas duplicadas entre `draw()` y `click()`.** Cada botón se escribe dos veces, por ejemplo `830,477,200` en `sfml_view.cpp:129` y en `sfml_view.cpp:230`. Hoy coinciden, pero si movés uno y te olvidás del otro el clic deja de funcionar. Definí cada rectángulo una sola vez (constantes o un arreglo de botones) y usalo en los dos lados o piensa en POO.

5. **Cambiar la velocidad o la densidad borra el futuro y pausa** (`sfml_view.cpp:237` y `239`). Ninguno de los dos modifica el estado del mundo, así que no deberían descartar generaciones. Además `speed` y `density` viven dentro de `World` (`world.h:14`), y al retroceder vuelve la velocidad vieja. Pasalos a `SfmlView` o a `Laboratory` y cambialos sin llamar `changed()`.

6. **Se escriben varias sentencias por línea y hay `if` sin llaves en casi todo el código**, por ejemplo `world.cpp:23`, `world.cpp:44-51` y `sfml_view.cpp:185-186`. Usá una sentencia por línea y llaves siempre. Cuesta un poco más de espacio, pero el código se lee y se depura mucho mejor.

7. **`SfmlView` hace demasiado.** Dibuja, atiende los eventos, valida el formulario y decide cuándo se descarta el futuro, todo en 321 líneas. `draw()` tiene 67 líneas y `click()` 47. Separá el control (eventos y acciones sobre el laboratorio) del dibujo, o al menos partí esas funciones por zona de la pantalla.

8. **Hay código muerto.** `getIdAt()`, `getWorldAt()` y `selectId()` (`laboratory.cpp:50-70`) no se usan, y `laboratoryReference` repite lo que `run()` ya recibe por parámetro. Quitalos, o aprovechalos para mostrar dos mundos en paralelo.

9. **"< Atrás" muestra "Estado anterior restaurado." aunque no haya pasado** (`sfml_view.cpp:241`). Usá el `bool` que devuelve `backward()` para elegir el mensaje.

10. **El historial crece sin límite.** Cada generación guarda una copia completa del mundo, y reproduciendo a 20 gen/s en 50×50 son unos 13 MB por minuto. Poné un tope de nodos o avisale al usuario.

11. **Los comentarios están en español** (`world.cpp:44`, `105`, `134`, `135` y `144`). Pasalos a inglés, o mejor borralos si no son necesarios.

## Pendientes de la Etapa 1 que siguen sin corregir

- **Números mágicos:** `4` en `species[4]`, `counts[4]` y en los `for (i < 4)`; `5`, `50` y `24` en `world.cpp:18`; toda la geometría de la vista. Declará `MAX_SPECIES`, `MIN_SIZE`, `MAX_SIZE` y `MAX_NAME_LENGTH`.
- **La especie sigue siendo un `int` sin `enum`:** `getShape()` devuelve 0..3, y la vista decide con `shape == 1 ? 24 : shape == 2 ? 3 : 4` (`sfml_view.cpp:65`). Un `enum class Shape` lo deja legible.
- **Espaciado de operadores:** por ejemplo `Timeline* timeline=laboratory.getActive();` (`sfml_view.cpp:185`). Ctrl+K, Ctrl+D lo arregla.
- **Variables sin inicializar:** `int rows, columns, count;` (`sfml_view.cpp:177`) e `int counts[4], total;` (`world.cpp:128`).
- **Listas de parámetros largas:** `World(name, rows, columns, speciesCount)` y `Laboratory::create(...)`. Habías creado `WorldSize` y lo quitaste en la reestructuración.
- **Nombres de punteros:** `int** cells` e `int** next`.
- **Tildes en los textos de la interfaz:** "Generacion", "Mutacion", "Patron", "Atras". La fuente las soporta.
- **README, declaración de IA, dos semanas de commits y mensajes de commit**, como se detalla arriba.
- Recordatorio sin descuento: `using namespace std;` sigue en `world.h:5`.

## Cierre

Es un avance muy sólido en lo que más cuesta de esta etapa: las listas propias, la línea de tiempo y la memoria están bien resueltas. Lo que más baja la nota es el orden del código y el paquete de entrega (Git y README), y eso se puede corregir sin tocar la lógica.

Todo lo señalado en esta evaluación debe corregirse para la Etapa 3. Lo que quede pendiente se penaliza de nuevo.

Prof. Rubén Mora Vargas
Programación I (EIF-201)
Escuela de Informática, Universidad Nacional
