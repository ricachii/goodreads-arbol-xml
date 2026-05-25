#include "GeneralTree.h"
#include <iostream>

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------

GeneralTree::GeneralTree() {
    // Crear la raíz virtual con un Book vacío.
    // Su id puede ser algo como "__root__" para no confundirla con un libro real.
    root_ = new TreeNode(Book{});
    root_->book.id = "__root__";
}

GeneralTree::~GeneralTree() {
    deleteSubtree(root_);
}

// ---------------------------------------------------------------------------
// addBook
// ---------------------------------------------------------------------------
void GeneralTree::addBook(const Book& book) {
    // TODO:
    //   1. Crear un nuevo TreeNode para `book`.
    //   2. Insertarlo en nodeMap_ con clave = book.id.
    //
    // NO conectar hijos todavía — esperar a build().
}

// ---------------------------------------------------------------------------
// build
// ---------------------------------------------------------------------------
void GeneralTree::build() {
    // TODO:
    //   Paso 1 — marcar qué IDs aparecen como libro similar en al menos un nodo.
    //     Usar un unordered_set<string> de IDs "reclamados".
    //     Para cada nodo en nodeMap_, iterar sus book.similarBooks e insertar
    //     cada id similar en el conjunto.
    //
    //   Paso 2 — conectar hijos.
    //     Para cada nodo N en nodeMap_:
    //       Para cada SimilarBook S en N.book.similarBooks:
    //         Si nodeMap_ contiene S (comparar por id o isbn — elegir uno y ser consistente):
    //           N.children.push_back(nodeMap_[S.id])
    //
    //   Paso 3 — colgar huérfanos en la raíz virtual.
    //     Para cada nodo N en nodeMap_:
    //       Si N.book.id NO está en el conjunto "reclamados":
    //         root_->children.push_back(N)
    //
    // NOTA: los similar_books en el XML usan IDs. Asegúrate de comparar
    //       siempre el mismo campo (id vs isbn) de forma consistente.
}

// ---------------------------------------------------------------------------
// listar  — recorrido preorder
// ---------------------------------------------------------------------------
void GeneralTree::listar() const {
    // Omitir la raíz virtual; comenzar desde sus hijos.
    for (const TreeNode* hijo : root_->children)
        listarHelper(hijo);
}

void GeneralTree::listarHelper(const TreeNode* node) const {
    // TODO:
    //   1. Imprimir node->book.id  (visitar antes de los hijos = preorder).
    //   2. Llamar listarHelper recursivamente en cada hijo.
}

// ---------------------------------------------------------------------------
// borrarRatings
// ---------------------------------------------------------------------------
void GeneralTree::borrarRatings(double r) {
    // TODO:
    //   Llamar borrarRatingsHelper en cada hijo de root_.
    //   Guardar los hijos que sobreviven (rating > r) en una nueva lista y
    //   reemplazar root_->children con esa lista.
    //
    //   Ver borrarRatingsHelper para la lógica recursiva.
}

bool GeneralTree::borrarRatingsHelper(TreeNode* node, double r) {
    // TODO:
    //   Esta función retorna true si `node` debe ser ELIMINADO.
    //
    //   Algoritmo (post-order para procesar hijos antes que el padre):
    //     1. Llamar borrarRatingsHelper recursivamente en cada hijo.
    //        Construir una nueva lista de hijos conservando solo los que retornaron false.
    //        Para los hijos que retornaron true, llamar deleteSubtree sobre ellos.
    //     2. Reemplazar node->children con los sobrevivientes.
    //     3. Retornar (node->book.averageRating <= r).
    //        Si es true, el llamador eliminará este nodo y su subárbol restante.
    return false; // placeholder
}

// ---------------------------------------------------------------------------
// precursores
// ---------------------------------------------------------------------------
void GeneralTree::precursores(const std::string& /*id*/) const {
    // NOTA: El parámetro `id` podría usarse para filtrar desde un subárbol —
    //       releer el enunciado y ajustar según corresponda.
    //
    // TODO:
    //   1. Recolectar resultados con precursoresHelper.
    //   2. Imprimir cada ID del vector resultado.
    std::vector<std::string> resultado;
    precursoresHelper(root_, resultado);
    for (const auto& bid : resultado)
        std::cout << bid << "\n";
}

void GeneralTree::precursoresHelper(const TreeNode* node, std::vector<std::string>& resultado) const {
    // TODO:
    //   Ignorar la raíz virtual (id == "__root__").
    //
    //   Para cada nodo real:
    //     Condición: node->book.similarBooks no está vacío Y
    //                cada SimilarBook tiene publicationYear > node->book.publicationYear.
    //     Si la condición se cumple, agregar node->book.id a resultado.
    //
    //   Recurrir en los hijos.
    for (const TreeNode* hijo : node->children)
        precursoresHelper(hijo, resultado);
}

// ---------------------------------------------------------------------------
// deleteSubtree
// ---------------------------------------------------------------------------
void GeneralTree::deleteSubtree(TreeNode* node) {
    if (!node) return;
    for (TreeNode* hijo : node->children)
        deleteSubtree(hijo);
    delete node;
}
