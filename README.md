# proyecto-escenario-4

Ian Leiva Araya

605070314

Programacion 1

Ruben Mora Vargas

Mecánica propia: mutación de especies

Agregamos una mutación que permite que una celda viva cambie de especie durante la evolución. Esto puede modificar la distribución de la población y producir resultados distintos aunque se parta del mismo tablero.

# Enjambre / Laboratorio — Simulador de Vida Artificial

Simulador de autómatas celulares multiespecie desarrollado en **C++17** con interfaz gráfica en **SFML 2.5.1**. Permite gestionar múltiples mundos independientes de forma simultánea, navegar libremente por su línea de tiempo, configurar reglas de nacimiento/supervivencia por especie y guardar o cargar sesiones completas en archivos de texto plano.

---

## Características Principales

### 1. Gestión del Laboratorio (Multimundo)
El sistema administra una colección enlazada de mundos independientes (hasta **32 mundos simultáneos** por laboratorio)[cite: 1]:
* **Crear mundo (`Nuevo` / tecla `N`):** Abre un formulario modal donde se define el nombre del mundo (máx. 24 bytes), sus dimensiones (filas y columnas dentro de los rangos permitidos en `config.ini`) y la cantidad de especies activas (3 o 4: *Trabajador*, *Explorador*, *Guardián* y *Reina*)[cite: 1]. Inicializa el mundo con las reglas predeterminadas del archivo de configuración[cite: 1].
* **Duplicar mundo (`Duplicar`):** Crea una copia profunda e independiente del mundo activo, conservando tanto su estado actual como todo su historial retenido en la línea de tiempo[cite: 1]. Se le asigna automáticamente el sufijo `" copia"`[cite: 1].
* **Eliminar mundo (`Eliminar`):** Remueve el mundo activo de la memoria previa confirmación en ventana modal, reasignando automáticamente el foco al mundo siguiente o al primero disponible[cite: 1].
* **Alternar entre mundos (`< Mundo` / `Mundo >` / tecla `Tab`):** Permite desplazarse cíclicamente entre todos los mundos abiertos en el laboratorio sin perder el progreso ni el historial de ninguno de ellos[cite: 1].
* **Persistencia y reinicio (`Nuevo laboratorio`, `Guardar`, `Cargar`):** Permite limpiar el espacio de trabajo actual o serializar/recuperar todos los mundos y sus historiales en archivos `.txt` (por defecto `laboratorio.txt`)[cite: 1].

### 2. Línea de Tiempo (Historial Recuperable)
Cada mundo cuenta con su propia lista doblemente enlazada de estados (`Timeline`)[cite: 1] que funciona como un control de versiones temporal[cite: 1]:
* **Avance y retroceso (`< Atrás` / `Adelante >` / Flechas `Izquierda` y `Derecha`):** Permite retroceder generación por generación para inspeccionar estados pasados y volver a avanzar sobre ellos[cite: 1].
* **Bifurcación automática:** Si se retrocede en el historial y se realiza cualquier edición manual (pintar/borrar celdas, cambiar reglas, alterar vecindario, bordes o mutación), los estados futuros almacenados a partir de ese punto se descartan automáticamente para mantener la coherencia causal[cite: 1].
* **Gestión de memoria (ventana deslizante):** El historial retiene hasta un límite configurable de generaciones (`history_limit = 300` por defecto en `config.ini`, máximo `1000`)[cite: 1]. Al superar este límite durante la evolución, el estado más antiguo se libera automáticamente de memoria y se contabiliza en el indicador de *estados antiguos liberados*[cite: 1].

### 3. Especies y Reglas Configurables
* **4 Especies diferenciadas:**
  * `W` — **Trabajador** (Cuadrado verde azulado)[cite: 1]
  * `S` — **Explorador** (Círculo azul)[cite: 1]
  * `G` — **Guardián** (Triángulo amarillo)[cite: 1]
  * `Q` — **Reina** (Rombo violeta)[cite: 1]
* **Parámetros dinámicos:** Configuración individual de reglas de nacimiento y supervivencia (0 a 8 vecinos), tipo de vecindario (**Moore** de 8 vecinos o **Von Neumann** de 4 vecinos), condiciones de borde (**cerrado** o **toroidal**), probabilidad de mutación y densidad de siembra aleatoria[cite: 1].

---

## Instrucciones de Compilación

### Requisitos Previos
* **Sistema Operativo:** Windows (x64 o Win32).
* **Compilador / IDE:** Visual Studio 2022 o superior con soporte para **C++17** (`stdcpp17`) y el conjunto de herramientas MSVC (`v143` / `v145`).
* **Biblioteca gráfica:** **SFML 2.5.1** (headers y librerías `.lib` / `.dll` correspondientes a la arquitectura seleccionada).

### Pasos para Compilar en Visual Studio
1. Abrir el archivo `Simulador_Vida.sln` en Visual Studio.
2. Seleccionar la configuración deseada en la barra superior (por ejemplo, `Debug` o `Release` en plataforma `x64`).
3. Verificar las rutas de **SFML 2.5.1** en *Propiedades del proyecto (`Simulador_Vida`) → C/C++ → General → Directorios de inclusión adicionales* y *Vinculador → General → Directorios de bibliotecas adicionales* (por defecto apuntan a `SFML-2.5.1\include` y `SFML-2.5.1\lib`).
4. Asegurarse de que las DLLs de SFML (`sfml-graphics-2.dll`, `sfml-window-2.dll`, `sfml-system-2.dll` o sus variantes `-d-2.dll` en Debug) estén disponibles junto al ejecutable o en el `PATH` del sistema.
5. Compilar la solución (`Ctrl + Shift + B`). El evento post-compilación copiará automáticamente `config.ini` y la carpeta `assets/` al directorio de salida (`build\x64\Debug\`).
6. *(Opcional)* Para ejecutar las pruebas unitarias del modelo, establecer `PruebasModelo` como proyecto de inicio y ejecutarlo.

---

## Guía de Uso y Controles

### Controles de Mouse
| Acción | Función |
| :--- | :--- |
| **Clic izquierdo en el tablero** | Pinta una celda con la especie seleccionada (o coloca el patrón 3×3 si el modo patrón está activo). |
| **Clic izquierdo + arrastrar** | Dibuja de forma continua sobre múltiples celdas. |
| **Clic derecho en el tablero (o arrastrar)** | Borra celdas (las deja vacías). |
| **Clic izquierdo en botones de reglas (`0`–`8`)** | Activa o desactiva condiciones de nacimiento o supervivencia para la especie activa. |
| **Clic izquierdo en botones superiores/inferiores** | Gestiona mundos, archivos, velocidad, densidad, mutación y reproducción. |

### Atajos de Teclado
| Tecla / Combinación | Función |
| :--- | :--- |
| `Espacio` | Inicia o pausa la simulación automática. |
| `Flecha Izquierda` | Retrocede un estado en la línea de tiempo (`< Atrás`). |
| `Flecha Derecha` | Avanza una generación en la línea de tiempo (`Adelante >`). |
| `1`, `2`, `3`, `4` | Selecciona la especie activa (`1`: Trabajador, `2`: Explorador, `3`: Guardián, `4`: Reina). |
| `Tab` | Alterna al siguiente mundo abierto en el laboratorio (o cambia de campo en el formulario de creación). |
| `N` | Abre la ventana modal para crear un mundo nuevo. |
| `Ctrl + S` | Abre la ventana modal para guardar el laboratorio en `.txt`. |
| `Ctrl + O` | Abre la ventana modal para cargar un laboratorio desde `.txt`. |
| `Ctrl + A` | Marca el texto del campo activo en ventanas modales para reemplazarlo al escribir. |
| `Enter` | Confirma la acción en la ventana modal activa. |
| `Esc` | Cancela la ventana modal activa, detiene la simulación o desactiva el modo patrón. |

---

## Créditos de Recursos

* **Tipografía:** **DejaVu Sans** (`assets/DejaVuSans.ttf`), desarrollada por el proyecto *DejaVu Fonts* (autor original: Štěpán Roh) basada en *Bitstream Vera* (Copyright © 2003 Bitstream, Inc.). Distribuida bajo la licencia **Bitstream Vera Fonts License** (con modificaciones de DejaVu en dominio público). El texto completo de la licencia se encuentra disponible en [`assets/FONT_LICENSE.txt`](assets/FONT_LICENSE.txt).
* **Biblioteca Gráfica:** **SFML 2.5.1** (*Simple and Fast Multimedia Library*), utilizada bajo licencia zlib/png.

---

## Declaración de Uso de Inteligencia Artificial

En cumplimiento con las normas del proyecto, el uso de herramientas de Inteligencia Artificial se limitó exclusivamente al **apartado estético y visual** de la interfaz gráfica, sin intervenir en la lógica del modelo, estructuras de datos ni algoritmos de simulación:

* **Herramienta utilizada:** *ChatGPT*
* **Elementos generados / asistidos:**
  * Selección de la paleta de colores de la interfaz en `src/sfml_view.cpp` (tonos de fondo oscuro, paneles, controles y color de acento) y asignación cromática de las cuatro especies en `src/species.cpp`.
  * Definición de coordenadas, márgenes, espaciado y dimensiones de botones y paneles en `src/layout.h`.
  * Representación geométrica de cada especie en la cuadrícula (cuadrado, círculo, triángulo y rombo).



## Estructura del Proyecto

```text
Simulador_Vida/
├── assets/
│   ├── DejaVuSans.ttf          # Tipografía utilizada en la interfaz
│   └── FONT_LICENSE.txt        # Licencia Bitstream Vera / DejaVu
├── examples/
│   ├── ejemplo_laboratorio.txt # Laboratorio de ejemplo precargado
│   └── ejemplo_vacio.txt       # Plantilla de laboratorio vacío
├── src/
│   ├── Simulador_Vida.cpp      # Punto de entrada de la aplicación
│   ├── configuration.h/.cpp    # Lectura y validación de config.ini
│   ├── file_manager.h/.cpp     # Serialización y carga de laboratorios (.txt)
│   ├── laboratory.h/.cpp       # Lista enlazada de mundos y estado global
│   ├── layout.h                # Coordenadas y dimensiones de la interfaz
│   ├── settings.h              # Constantes y estructuras base
│   ├── sfml_view.h/.cpp        # Renderizado y eventos en SFML 2.5.1
│   ├── species.h/.cpp          # Jerarquía de especies y reglas B/S
│   ├── timeline.h/.cpp         # Lista doblemente enlazada de historial
│   ├── view.h                  # Interfaz abstracta de vista
│   └── world.h/.cpp            # Matriz de celdas y lógica de evolución
├── tests/
│   └── model_tests.cpp         # Pruebas unitarias del modelo
├── config.ini                  # Parámetros iniciales y límites
├── Simulador_Vida.sln          # Solución de Visual Studio
├── Simulador_Vida.vcxproj      # Proyecto principal
└── PruebasModelo.vcxproj       # Proyecto de pruebas unitarias


