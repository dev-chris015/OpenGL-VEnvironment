# OpenGL & Assimp 3D Model Loader (C++ Workspace)

Este repositorio contiene un entorno de desarrollo educativo y experimental para **OpenGL Moderno (OpenGL 3.3+ Core Profile)** en C++. Está diseñado como un espacio de trabajo incremental para el aprendizaje de computación gráfica, cubriendo desde la renderización de primitivas básicas hasta la carga de modelos 3D complejos con iluminación avanzada.

---

## 🛠️ Stack Tecnológico

El proyecto está construido utilizando las siguientes herramientas y librerías:

* **Lenguaje:** C++ (Estándar C++17)
* **API Gráfica:** OpenGL Moderno (OpenGL 3.3+ Core Profile)
* **Gestión de Ventanas y Contexto:** GLFW3
* **Carga de Extensiones:** GLEW
* **Cálculos Matemáticos:** GLM (OpenGL Mathematics)
* **Importación de Modelos 3D:** Assimp (Open Asset Import Library)
* **Carga y Procesamiento de Texturas:**
  * **SOIL2** (Simple OpenGL Image Library 2) para formatos tradicionales.
  * **stb_image** para decodificación directa desde memoria (imágenes comprimidas PNG/JPG integradas en archivos GLB/GLTF).
* **Sistema de Construcción:** CMake (versión mínima 3.10)
* **Gestor de Paquetes (Windows):** `vcpkg` (para resolver dependencias como GLFW, GLM, GLEW y Assimp)
* **Framework Legacy (opcional):** FreeGLUT (incluido localmente en `freeglut-MSVC` para compatibilidad)

---

## 🌿 Ramas del Repositorio (Branches)

El repositorio se organiza en diferentes ramas que reflejan la evolución del aprendizaje y la implementación de los distintos laboratorios:

* **`main`**: Rama base que establece la estructura inicial del proyecto y la integración de dependencias a través de `vcpkg` (como GLEW).
* **`Lab2`**: Prácticas iniciales sobre la creación del contexto y renderizado básico.
* **`Lab3`**: Reestructuración y modularización del archivo `main.cpp` estructurado.
* **`Lab4` y `Lab5`**: Implementación de proyecciones, transformaciones espaciales en 3D y renderizado de formas geométricas.
* **`Lab6`**: Finalización de la base del entorno tridimensional y preparativos para el sistema de iluminación.
* **`Lab7`**: Implementación del sistema de sombreado e iluminación básica (luces direccionales, puntuales y combinaciones).
* **`Lab8` (Rama activa actual)**: Integración del cargador de modelos Assimp con soporte completo para texturas embebidas (GLB/GLTF), corrección de coordenadas UV y shaders con modelo de iluminación optimizado (Key Light, Fill Light, reflejos especulares).
* **`Expo`**: Código y recursos optimizados específicamente para demostración y exposición.

---

## 🚀 Características Clave

### 1. Cargador de Modelos 3D con Assimp
- **Formatos Soportados**: Carga de modelos 3D complejos en formato `.glb`, `.gltf` (e imports genéricos de Assimp). Por defecto, el proyecto carga un modelo de computadora portátil (`laptop.glb`) desde la carpeta de assets.
- **Optimización de Texturas**: Prevención de duplicación al mantener un registro de texturas ya cargadas (`textures_loaded`).
- **Corrección UV**: Se omite intencionalmente la bandera de inversión de UV (`aiProcess_FlipUVs`) para archivos GLB/GLTF, ya que estos formatos siguen la convención estándar de OpenGL de forma nativa. Esto evita la distorsión y el renderizado incorrecto de texturas.

### 2. Soporte de Texturas Embebidas (GLB/GLTF)
El cargador en [Model.h](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/Model.h) maneja texturas integradas dentro del archivo del modelo sin depender de archivos de imagen externos:
- **Imágenes Comprimidas (PNG/JPG)**: Decodificación directa desde memoria usando la librería `stb_image` (`stbi_load_from_memory`), forzando el formato RGBA para evitar incompatibilidades de canales.
- **Datos de Píxeles Crudos (ARGB)**: Conversión y reordenamiento manual de estructuras `aiTexel` de Assimp (ARGB8888) a formato RGBA compatible con OpenGL.

### 3. Sombras e Iluminación Avanzada (Shaders)
El sombreador (`model_loading.frag`) cuenta con un modelo de iluminación personalizado y realista:
- **Luz Direccional Principal (Key Light)**: Simula una fuente de luz desde el cuadrante superior-frontal-derecho para resaltar la forma volumétrica del modelo.
- **Luz de Relleno Suave (Fill Light)**: Ubicada de forma opuesta a la luz principal (`vec3(-1.0, 0.5, -1.0)`) para suavizar las sombras y añadir realismo.
- **Luz Ambiental Mejorada**: Un multiplicador base de `0.45` para asegurar que el modelo se aprecie claramente en todas las caras.
- **Reflejos Especulares (Specular Highlights)**: 
  - Ajuste del exponente de brillo (shininess) a `16.0` para que el brillo se disperse de forma más suave (ideal para materiales como plástico y metal satinado).
  - Intensidad de brillo configurada en `0.7` para reflejos definidos y atractivos.
- **Fallback a Color Base**: Si el modelo carece de texturas de difusión, el shader realiza un fallback automático utilizando el color base del material (`materialColor`) definido en el archivo 3D.

---

## 🛠️ Requisitos Previos

### Windows
1. **Visual Studio 2022 Community** (o superior) con la carga de trabajo **"Desarrollo de escritorio con C++"** instalada.
2. **Visual Studio Code** (opcional, pero recomendado).
3. **vcpkg** (Gestor de paquetes C++ de Microsoft) integrado para resolver las dependencias:
   - `glfw3`, `glm`, `glew`, `assimp`.

> [!NOTE]
> El repositorio incluye carpetas de dependencias locales como `freeglut-MSVC/` para compatibilidad heredada, y el directorio local de `SOIL2` en `dependencies/soil2/`.

### macOS / Linux
Instala las dependencias principales a través del gestor de paquetes de tu sistema:
```bash
# macOS (Homebrew)
brew install glfw glm glew assimp

# Linux (Debian/Ubuntu)
sudo apt install libglfw3-dev libglm-dev libglew-dev libassimp-dev
```

---

## ⚙️ ¿Cómo compilar y correr?

### Forma 1: Ejecución Rápida en Windows (Doble clic)
1. Haz doble clic en el script **`compilar_y_correr.bat`** en la raíz del proyecto.
2. El script detectará automáticamente el target `Carga_Modelos`, compilará el código en modo `Release` usando CMake y Visual Studio, validará la existencia del ejecutable y lanzará la aplicación dentro de la carpeta correspondiente para que las rutas relativas de los shaders se carguen correctamente.

### Forma 2: VS Code con Ctrl+Shift+B
1. Abre la carpeta del proyecto en VS Code.
2. Abre la terminal e inicializa la compilación con CMake **solo la primera vez**:
   ```powershell
   cmake -B build_win -S .
   ```
3. A partir de ese momento, presiona **`Ctrl + Shift + B`** para compilar y ejecutar de forma automatizada.

### Forma 4: macOS / Linux (Terminal)
Compila usando el Makefile proporcionado en la raíz:
```bash
make
./app
```

---

## 📁 Estructura del Proyecto

El espacio de trabajo se organiza de la siguiente manera:

| Ruta | Descripción |
|---|---|
| [src/Carga_Modelos/main.cpp](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/main.cpp) | Punto de entrada del cargador. Configura la ventana GLFW, inicializa GLEW, gestiona las entradas de teclado/mouse (cámara FPS) y corre el bucle de renderizado. |
| [src/Carga_Modelos/Model.h](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/Model.h) | Clase que procesa nodos y mallas usando Assimp y gestiona la carga de texturas locales y embebidas. |
| [src/Carga_Modelos/Mesh.h](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/Mesh.h) | Representa una malla individual con sus respectivos VBO, VAO y EBO para envío de datos a la GPU. |
| [src/Carga_Modelos/Camera.h](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/Camera.h) | Sistema de cámara estilo FPS (vuelo libre) controlable con ratón y teclado (teclas W, A, S, D). |
| [src/Carga_Modelos/res/shaders/](file:///c:/c++/OpenGL-VEnvironment/src/Carga_Modelos/res/shaders/) | Contiene `model_loading.vs` (Vertex Shader) y `model_loading.frag` (Fragment Shader) con el modelo de iluminación mejorado. |
| `assets/models/` | Contiene los recursos 3D cargados (como `laptop.glb`). |
| `dependencies/` | Código fuente de librerías embebidas en el proyecto como `SOIL2`. |
| `CMakeLists.txt` | Script de configuración de CMake que define las dependencias del proyecto y crea el ejecutable `Carga_Modelos`. |

---

## 🎮 Controles de la Aplicación
- **Ratón**: Mueve la cámara para mirar alrededor.
- **Teclas W / A / S / D**: Desplazan la cámara hacia adelante, izquierda, atrás y derecha respectivamente.
- **Escape**: Cierra la ventana del visualizador y finaliza la aplicación.
