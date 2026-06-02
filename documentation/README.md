# Tarea 2 - Arbol General sobre XML de GoodReads

## Integrantes
- Nicolas Renato Ricciardi Acuna
- Enzo Gabriel Levancini Arriagada

---

## Descripcion

Proyecto de Estructura de Datos que construye un arbol general en C++17 a partir
de 10.000 archivos XML del dataset GoodReads. Cada nodo del arbol representa un libro;
sus hijos son los libros listados en su seccion `<similar_books>`.

Como las relaciones de similitud son bidireccionales (forman un grafo con ciclos),
se aplica un DFS con conjunto de visitados para convertir el grafo en un arbol
sin ciclos.

---

## Dependencias

- CMake >= 3.16
- Compilador C++17 (GCC 8+, Clang 7+, MSVC 2017+)
- PugiXML — ya incluido en `libs/pugixml/`

---

## Compilacion

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

El ejecutable queda en `build/tarea2.exe` (Windows) o `build/tarea2` (Linux/Mac).

---

## Ejecucion

1. Coloca los 10.000 archivos `.xml` del dataset dentro de `data/`
2. Desde la carpeta `build/`, ejecuta:

```bash
./tarea2
```

Los resultados se generan en `build/`:

| Archivo | Contenido |
|---|---|
| `listar.txt` | IDs de todos los libros en preorder |
| `listar_post_borrado.txt` | IDs tras eliminar libros con rating <= 3.5 |
| `precursores.txt` | IDs de libros cuyo año es menor al de todos sus similares |

Cada archivo comienza con la columna `id`.

---

## Tests

El proyecto incluye un programa de tests con datos reales del dataset:

```bash
cd build
./tests
```

Verifica las tres funciones requeridas con 4 libros reales (id=1, 153795, 201341, 15842230).

---

## Estructura del proyecto

```
Tarea2/
├── CMakeLists.txt
├── README.md
├── data/                  <- 10.000 archivos .xml del dataset
├── documentation/
│   ├── comprension_problema.md  <- explicacion del problema
│   └── documentacion.tex        <- documentacion LaTeX
├── libs/
│   └── pugixml/           <- pugixml.hpp, pugixml.cpp, pugiconfig.hpp
├── tests/
│   ├── data/              <- 4 XMLs reales para tests
│   └── test_main.cpp      <- programa de tests
└── src/
    ├── Book.h             <- structs Book y SimilarBook
    ├── TreeNode.h         <- nodo del arbol general
    ├── GeneralTree.h/cpp  <- arbol con listar, borrarRatings, precursores, stats
    ├── XMLParser.h/cpp    <- parseo de XML con PugiXML
    └── main.cpp           <- punto de entrada
```

---

## Funciones implementadas

### `listar(os)`
Recorre el arbol en preorder (nodo antes que sus hijos) e imprime cada ID.
Implementacion iterativa con pila explicita para evitar stack overflow.

### `borrarRatings(r)`
Elimina todos los nodos con `averageRating <= r` y su subrarbol completo.
Recorrido post-order: los hijos se evaluan antes que el padre.

### `precursores(id, os)`
Lista los libros donde todos sus `similar_books` tienen `publication_year`
estrictamente mayor al del libro. Libros similares con año 0 (campo vacio en el XML)
se tratan como anteriores y excluyen al nodo del resultado.

### `stats()`
Imprime estadisticas del arbol: nodos totales, altura maxima,
promedio de hijos por nodo y libro con mas conexiones.

---

## Decisiones de disenio

- `build()` usa DFS con conjunto `visited` para romper los ciclos del grafo bidireccional
- Los IDs se ordenan numericamente antes del DFS para que el arbol sea determinístico
- `addBook()` ignora IDs duplicados para evitar memory leak
- Los recorridos `listar` y `precursores` son iterativos (pila explicita)
