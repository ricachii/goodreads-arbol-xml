#pragma once
#include "TreeNode.h"
#include <string>
#include <unordered_map>
#include <vector>

/*
 * Árbol general de libros.
 *
 * Estrategia de construcción (puedes ajustar el enfoque):
 *   1. Cargar cada Book desde los 10 000 XML en un mapa plano: id -> TreeNode*
 *   2. Para cada nodo, buscar sus similarBooks en el mapa y agregarlos como hijos.
 *   3. Un nodo sin padre pasa a ser hijo directo de la raíz virtual.
 *      (Un nodo no tiene padre si su id nunca aparece en los similar_books de otro libro.)
 *
 * La raíz virtual tiene un Book vacío y sirve solo como punto de entrada al árbol.
 */
class GeneralTree {
public:
    GeneralTree();
    ~GeneralTree();

    // Inserta un libro en el mapa interno (llamar mientras se cargan los XML).
    void addBook(const Book& book);

    // Una vez cargados todos los libros, conecta los enlaces padre-hijo y coloca
    // los huérfanos bajo la raíz virtual. Llamar una sola vez después de todos los addBook().
    void build();

    // -------------------------------------------------------------------------
    // Funciones requeridas
    // -------------------------------------------------------------------------

    // Lista los IDs de todos los libros en preorder (raíz primero, luego hijos de izquierda a derecha).
    void listar() const;

    // Elimina del árbol todos los nodos cuyo averageRating <= r.
    // También elimina todos los descendientes del nodo eliminado.
    // Pista: usar recorrido post-order para evaluar hijos antes que el padre.
    void borrarRatings(double r);

    // Lista los IDs de libros donde TODOS los libros similares tienen publicationYear
    // estrictamente mayor que el publicationYear del libro.
    // Usa solo el campo similarBooks ya almacenado en cada Book —
    // no se requiere búsqueda cruzada en el árbol.
    void precursores(const std::string& id) const;

private:
    TreeNode* root_;   // raíz virtual (Book vacío)

    // Búsqueda rápida por ID de libro — necesaria para conectar similar_books como hijos.
    std::unordered_map<std::string, TreeNode*> nodeMap_;

    // -------------------------------------------------------------------------
    // Métodos auxiliares privados — implementar estos para mantener limpios los métodos públicos
    // -------------------------------------------------------------------------

    // Recorrido preorder recursivo comenzando en `node`.
    void listarHelper(const TreeNode* node) const;

    // Auxiliar recursivo: elimina hijos cuyo rating <= r,
    // retorna true si este nodo mismo debe ser eliminado.
    bool borrarRatingsHelper(TreeNode* node, double r);

    // Auxiliar recursivo: visita cada nodo y verifica la condición de precursores.
    void precursoresHelper(const TreeNode* node, std::vector<std::string>& result) const;

    // Libera el subárbol con raíz en `node`.
    void deleteSubtree(TreeNode* node);
};
