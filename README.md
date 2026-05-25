# Tarea 2 — Árbol General sobre XML de GoodReads

## Integrantes
- Nicolas Renato Ricciardi Acuna
- Enzo Gabriel Levancini Arriagada
---

## Dependencias

### PugiXML (ya incluido en `libs/pugixml/`)
Descarga los tres archivos desde https://pugixml.org/docs/quickstart.html:
- `pugixml.hpp`
- `pugixml.cpp`
- `pugiconfig.hpp`

Colócalos en `libs/pugixml/`.

---

## Compilación

### Con CMake (recomendado)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
El ejecutable queda en `build/tarea2.exe` (Windows) o `build/tarea2` (Linux/Mac).

### Con g++ directo
```bash
g++ -std=c++17 -O2 \
    src/main.cpp src/XMLParser.cpp src/GeneralTree.cpp \
    libs/pugixml/pugixml.cpp \
    -I src -I libs/pugixml \
    -o tarea2
```

---

## Datos
Coloca los 10 000 archivos `.xml` del dataset GoodReads dentro de `data/`.

---

## Estructura del proyecto
```
Tarea2/
├── CMakeLists.txt
├── README.md
├── data/              ← 10 000 archivos .xml aquí
├── libs/
│   └── pugixml/       ← pugixml.hpp, pugixml.cpp, pugiconfig.hpp
└── src/
    ├── Book.h          ← struct Book y SimilarBook (NO modificar)
    ├── TreeNode.h      ← struct TreeNode (NO modificar)
    ├── GeneralTree.h   ← declaración del árbol
    ├── GeneralTree.cpp ← IMPLEMENTAR aquí
    ├── XMLParser.h     ← declaración del parser
    ├── XMLParser.cpp   ← IMPLEMENTAR aquí
    └── main.cpp        ← punto de entrada
```

---

## Qué implementar (por archivo)

### `XMLParser.cpp`
- [ ] `parseFile()` — extraer los campos del XML con PugiXML
- [ ] `loadDirectory()` — iterar todos los `.xml` de una carpeta

### `GeneralTree.cpp`
- [ ] `addBook()` — crear TreeNode e insertar en el mapa interno
- [ ] `build()` — conectar nodos padre-hijo usando `similar_books`, colgar huérfanos en la raíz virtual
- [ ] `listarHelper()` — recorrido preorder recursivo
- [ ] `borrarRatings()` + `borrarRatingsHelper()` — eliminar nodos con rating ≤ r (post-order)
- [ ] `precursoresHelper()` — detectar libros cuyo año < todos sus similares

---

## Lógica de construcción del árbol

```
Paso 1: Cargar los 10 000 libros → map<id, TreeNode*>

Paso 2: Para cada nodo N:
          Para cada similar_book S en N.book.similarBooks:
            Si map contiene S.id → N.children.push_back(map[S.id])

Paso 3: IDs que NO aparecen como similar de nadie = hijos directos de la raíz virtual
```

---

## Algoritmo borrar_ratings (post-order)

```
borrarRatingsHelper(nodo, r):
    nueva_lista = []
    para cada hijo en nodo.children:
        debe_borrar = borrarRatingsHelper(hijo, r)
        si debe_borrar:
            deleteSubtree(hijo)
        sino:
            nueva_lista.append(hijo)
    nodo.children = nueva_lista
    retornar (nodo.averageRating <= r)
```

---

## Algoritmo precursores

```
Para cada nodo N en el árbol:
    si N.similarBooks no está vacío Y
       todos los S en N.similarBooks tienen S.publicationYear > N.publicationYear:
        reportar N.id
```
