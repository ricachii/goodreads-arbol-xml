#pragma once
#include "Book.h"
#include <vector>

/*
 * Nodo del árbol general.
 * Cada nodo almacena un Book y una lista de punteros a sus hijos.
 * Los hijos = libros listados en la sección <similar_books> de este libro.
 */
struct TreeNode {
    Book                    book;
    std::vector<TreeNode*>  children;

    explicit TreeNode(const Book& b) : book(b) {}

    // TODO: agregar un destructor si quieres que el nodo elimine
    //       recursivamente su subárbol al ser liberado.
};
