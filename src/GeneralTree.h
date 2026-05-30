#pragma once
#include "TreeNode.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

/*
 * Clase: GeneralTree
 * ------------------
 * Árbol general de libros construido a partir del dataset GoodReads.
 *
 * Cada nodo del árbol representa un libro. Los hijos de un nodo son los libros
 * que aparecen listados en su sección <similar_books> dentro del XML.
 *
 * Dado que las relaciones de similitud en GoodReads son bidireccionales
 * (si A tiene a B como similar, B también tiene a A), el dataset forma un grafo
 * con ciclos. Para convertirlo en un árbol sin ciclos se aplica un recorrido
 * DFS durante la construcción: cada nodo se incorpora al árbol una sola vez,
 * la primera vez que es alcanzado desde algún vecino.
 *
 * Los libros que ningún otro libro reclama como hijo quedan como hijos directos
 * de una raíz virtual (nodo con Book vacío), formando así un bosque de expansión
 * con un único punto de entrada.
 *
 * Uso típico:
 *   GeneralTree arbol;
 *   for (const Book& b : libros) arbol.addBook(b);
 *   arbol.build();
 *   arbol.listar();
 */
class GeneralTree {
public:
    GeneralTree();
    ~GeneralTree();

    /*
     * addBook
     * -------
     * Registra un libro en el mapa interno del árbol.
     * Debe llamarse una vez por cada libro cargado, antes de invocar build().
     *
     * Parámetro:
     *   book — libro a insertar (se crea un TreeNode con copia del Book).
     */
    void addBook(const Book& book);

    /*
     * build
     * -----
     * Construye la estructura del árbol conectando los nodos según las relaciones
     * de similitud. Debe llamarse una única vez, después de todos los addBook().
     *
     * Estrategia:
     *   1. DFS iterativo sobre el mapa de nodos para asignar hijos sin crear ciclos.
     *   2. Los nodos que ningún otro reclamó como hijo pasan a ser hijos de la raíz virtual.
     */
    void build();

    /*
     * listar
     * ------
     * Recorre el árbol en preorder (nodo actual antes que sus hijos) e imprime
     * el ID de cada libro en el flujo de salida indicado.
     *
     * El recorrido es iterativo (pila explícita) para evitar desbordamiento de
     * pila en árboles con muchos niveles.
     *
     * Parámetro:
     *   os — flujo de salida (por defecto std::cout; se puede pasar un std::ofstream
     *        para escribir directamente en un archivo).
     *
     * Complejidad: O(n), donde n es el número de nodos en el árbol.
     */
    void listar(std::ostream& os = std::cout) const;

    /*
     * borrarRatings
     * -------------
     * Elimina del árbol todos los libros cuyo rating promedio es menor o igual a r.
     * Cuando se elimina un nodo, también se eliminan todos sus descendientes
     * (el subárbol completo queda fuera del árbol).
     *
     * El recorrido es post-order recursivo: se evalúan los hijos antes que el padre,
     * lo que garantiza que la memoria de los subárboles eliminados se libere
     * correctamente de abajo hacia arriba.
     *
     * Parámetro:
     *   r — umbral de rating (se eliminan los nodos con averageRating <= r).
     *
     * Complejidad: O(n), donde n es el número de nodos en el árbol.
     */
    void borrarRatings(double r);

    /*
     * precursores
     * -----------
     * Recorre todo el árbol e imprime el ID de cada libro que cumpla la siguiente
     * condición: todos sus libros similares fueron publicados en un año
     * estrictamente posterior al año de publicación del libro.
     *
     * Un libro sin similares no se reporta (la condición requiere al menos uno).
     * La comparación usa únicamente la información almacenada en el campo
     * similarBooks de cada nodo, sin realizar búsquedas adicionales en el árbol.
     *
     * Nota: libros similares sin año registrado (publicationYear == 0) se tratan
     * como anteriores al libro, por lo que su presencia excluye al nodo del resultado.
     *
     * Parámetros:
     *   id — reservado para uso futuro (actualmente el recorrido es global).
     *   os — flujo de salida (por defecto std::cout).
     *
     * Complejidad: O(n * m), donde n es el número de nodos y m el promedio
     *              de libros similares por nodo.
     */
    void precursores(const std::string& id, std::ostream& os = std::cout) const;

    /*
     * stats
     * -----
     * Imprime estadísticas generales del árbol:
     *   - Número total de nodos
     *   - Altura máxima (profundidad del nodo más lejano de la raíz)
     *   - Promedio de hijos por nodo
     *   - ID del libro con más hijos directos en el árbol
     *
     * Parámetro:
     *   os — flujo de salida (por defecto std::cout).
     *
     * Complejidad: O(n).
     */
    void stats(std::ostream& os = std::cout) const;

private:
    TreeNode* root_;
    std::unordered_map<std::string, TreeNode*> nodeMap_;

    bool borrarRatingsHelper(TreeNode* node, double r);
    void deleteSubtree(TreeNode* node);
};
