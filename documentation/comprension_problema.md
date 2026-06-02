# Comprensión del Problema — Entregable 2: XML

## ¿Qué es XML?

XML es un lenguaje de marcado que organiza información de forma jerárquica usando etiquetas del estilo `<tag>...</tag>`, similar a HTML. Esa jerarquía se puede representar directamente como un árbol.

Ejemplo: este XML se puede representar como un árbol donde cada tag es un nodo y los tags anidados son sus hijos.

```xml
<customers>
  <customer id="55000">
    <name>Charter Group</name>
    <address>
      <street>100 Main</street>
      <city>Framingham</city>
    </address>
  </customer>
  <customer id="75000">
    <name>Travel Co</name>
  </customer>
</customers>
```

Cada tag es un nodo. Los tags anidados son hijos de ese nodo.

```
                     customers
                     /          \
          customer(55000)      customer(75000)
            /        \                \
          name      address           name
           |        /     \             |
       Charter   street   city       Travel Co
       Group    100 Main  Framingham
```

---

## El Dataset

10.000 archivos XML, uno por libro, del sitio GoodReads. Cada archivo contiene:

| Campo | Descripción |
|---|---|
| `id` | Identificador único del libro |
| `title` | Título |
| `isbn` | ISBN |
| `publication_year` | Año de publicación |
| `language_code` | Idioma |
| `description` | Descripción |
| `average_rating` | Rating promedio (ej: 3.85) |
| `num_pages` | Número de páginas |
| `similar_books` | Lista de libros similares (cada uno con title, isbn, publication_year) |

---

## El Problema

Se pide construir un **árbol general** en C++ a partir de esos 10.000 archivos y soportar tres operaciones.

### ¿Por qué es un grafo y no directamente un árbol?

Las relaciones de similitud en GoodReads son **bidireccionales**: si el libro A tiene a B como similar, B también tiene a A como similar. Eso crea ciclos:

```
A → B → A → B → ...  (ciclo infinito)
```

Para convertir ese grafo en un árbol hay que romper los ciclos. La solución usada es un **DFS con conjunto de visitados**: cada nodo se agrega al árbol una sola vez, la primera vez que es alcanzado.

---

## Las Tres Funciones

### 1. `listar`
Recorre el árbol en **preorder** e imprime los IDs.

Preorder = visitar el nodo actual **antes** que sus hijos.

```
      A
     / \
    B   C
   / \
  D   E
```
Preorder: A → B → D → E → C

### 2. `borrar_ratings(r)`
Elimina del árbol todos los libros con `average_rating <= r`. Al eliminar un nodo se elimina también todo su subárbol (sus hijos quedan sin padre).

Se usa **post-order** (hijos antes que el padre) para liberar la memoria correctamente de abajo hacia arriba.

### 3. `precursores(id)`
Lista los libros que cumplen:
- Tienen al menos un libro similar.
- **Todos** sus libros similares tienen `publication_year` mayor al del libro.

Es decir: el libro fue publicado **antes** que todos sus similares.

> Ejemplo: libro publicado en 2000 cuyos similares fueron publicados en 2005, 2010 y 2015 → es precursor.

**Importante:** si un libro similar tiene `publication_year = 0` (campo vacío en el XML), se trata como anterior → el libro **no** se reporta.

---

## Estructura del Árbol

```
[raíz virtual "__root__"]
        |
   ┌────┴────┐
 Libro A   Libro D        ← libros que nadie reclama como hijo
   |
 Libro B                  ← aparece en similar_books de A
   |
 Libro C                  ← aparece en similar_books de B
```

Los libros que no son hijo de ningún otro quedan directamente bajo la raíz virtual.

---

## Ejemplo con datos reales del dataset

El libro con `id=1` es **Harry Potter and the Half-Blood Prince** (2006, rating 4.54).
Sus `similar_books` incluyen entre otros:

| id | título | año |
|---|---|---|
| 153795 | Squire (Protector of the Small, #3) | 2007 |
| 201341 | Mossflower (Redwall, #2) | 1998 |

Así quedaría representado en el árbol:

```
               [__root__]
                    |
          ┌─────────┴──────────┐
    id=1 (HP Half-Blood, 2006)  ...otros libros huérfanos...
       rating: 4.54
         /          \
  id=153795        id=201341
  Squire (2007)    Mossflower (1998)
  rating: 4.27
```

- `id=1` es hijo directo de la raíz porque ningún otro libro lo reclama.
- `id=153795` y `id=201341` son hijos de `id=1` porque aparecen en su `similar_books`.
- Si aplicamos `borrarRatings(4.3)`, se eliminaría `id=153795` (rating 4.27 ≤ 4.3).
- `id=1` **no** es precursor porque su similar `id=201341` (Mossflower, 1998) fue publicado **antes** que él (1998 < 2006).

---

## Cómo el DFS rompe los ciclos

Sin el conjunto `visited`, al recorrer los similares de Harry Potter llegaríamos a Mossflower, y desde Mossflower volveríamos a Harry Potter infinitamente:

```
Paso 1: proceso id=1  (HP)        → agrego id=153795 (Squire) como hijo
Paso 2: proceso id=153795 (Squire) → sus similares incluyen id=1 (HP)
         ¿agrego HP como hijo de Squire? → SIN visited: SÍ → ciclo
                                         → CON visited: NO, ya fue visitado
```

Con el conjunto `visited` el algoritmo queda:

```
visited = {}

1. Tomo id=1 (HP),      lo marco como visitado  → visited = {1}
   Sus similares: 153795, 201341
     - 153795 no visitado → lo agrego como hijo de HP, lo marco → visited = {1, 153795}
     - 201341 no visitado → lo agrego como hijo de HP, lo marco → visited = {1, 153795, 201341}

2. Tomo id=153795 (Squire), ya visitado → lo salteo ✓
3. Tomo id=201341 (Mossflower), ya visitado → lo salteo ✓

Resultado: árbol sin ciclos
```

---

## Ejemplo visual de borrar_ratings

Árbol antes de `borrarRatings(4.3)`:

```
         id=1  (HP, rating 4.54)
        /          \
id=153795         id=201341
Squire            Mossflower
rating 4.27       rating 4.10
    |
id=99999
otro libro
rating 3.80
```

Después de `borrarRatings(4.3)` — se eliminan todos los nodos con rating ≤ 4.3:

```
         id=1  (HP, rating 4.54)  ← sobrevive (4.54 > 4.3)
```

`id=153795` (4.27 ≤ 4.3) se elimina junto con su hijo `id=99999` (subárbol completo).
`id=201341` (4.10 ≤ 4.3) también se elimina.

---

## Ejemplo visual de precursores

Dado este subárbol con años de publicación:

```
  id=500  (año 1990)
  similares: id=501 (año 2000), id=502 (año 1995)
  → PRECURSOR ✓  (1990 < 2000 y 1990 < 1995)

  id=600  (año 2005)
  similares: id=601 (año 2010), id=602 (año 1999)
  → NO precursor ✗  (1999 < 2005)

  id=700  (año 2000)
  similares: id=701 (año 0 — sin año en XML)
  → NO precursor ✗  (año 0 se trata como anterior)
```

Solo `id=500` aparecería en `precursores.txt`.

---

## Flujo de ejecución

```
XMLParser::loadDirectory("data/")
    └── por cada .xml → XMLParser::parseFile() → Book
                                                    ↓
                                          GeneralTree::addBook()
                                          (guarda en nodeMap_)
                                                    ↓
                                          GeneralTree::build()
                                          (DFS → conecta padres e hijos)
                                                    ↓
                              ┌───────────────────────────────┐
                              ↓           ↓                   ↓
                          listar()  borrarRatings(r)   precursores()
                              ↓           ↓                   ↓
                        listar.txt  listar_post_         precursores
                                    borrado.txt              .txt
```

---

## Glosario

| Término | Definición |
|---|---|
| **Nodo** | Cada libro en el árbol |
| **Raíz** | Punto de entrada del árbol (`__root__` virtual) |
| **Hoja** | Nodo sin hijos |
| **Subárbol** | Un nodo y todos sus descendientes |
| **Preorder** | Recorrido que visita el nodo antes que sus hijos |
| **Post-order** | Recorrido que visita los hijos antes que el nodo |
| **DFS** | Depth First Search — recorre en profundidad antes de pasar al siguiente vecino |
| **Visited** | Conjunto que registra nodos ya procesados para evitar ciclos |
| **Raíz virtual** | Nodo ficticio `__root__` que agrupa todos los libros sin padre |