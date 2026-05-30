#include "GeneralTree.h"
#include <iostream>
#include <stack>
#include <unordered_set>

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------

GeneralTree::GeneralTree() {
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
    if (nodeMap_.count(book.id)) {
        std::cerr << "Advertencia: ID duplicado ignorado -> " << book.id << "\n";
        return;
    }
    nodeMap_[book.id] = new TreeNode(book);
}

// ---------------------------------------------------------------------------
// build
// ---------------------------------------------------------------------------
// PROBLEMA DEL DATASET: los similar_books de GoodReads son bidireccionales:
// si A tiene a B como similar, B también tiene a A. Eso crea ciclos (A→B→A→…).
// Solución: construir un bosque de expansión con DFS — cada nodo se visita
// una sola vez, por lo que nunca se puede formar un ciclo.
// ---------------------------------------------------------------------------
void GeneralTree::build() {
    std::unordered_set<std::string> visited;

    for (auto& [startId, startNode] : nodeMap_) {
        if (visited.count(startId)) continue;

        visited.insert(startId);
        std::stack<TreeNode*> pila;
        pila.push(startNode);

        while (!pila.empty()) {
            TreeNode* curr = pila.top(); pila.pop();
            for (const SimilarBook& sb : curr->book.similarBooks) {
                auto it = nodeMap_.find(sb.id);
                if (it != nodeMap_.end() && !visited.count(sb.id)) {
                    visited.insert(sb.id);
                    curr->children.push_back(it->second);
                    pila.push(it->second);
                }
            }
        }
    }

    // Nodos que nadie reclamó como hijo → van directo bajo la raíz virtual
    std::unordered_set<std::string> claimed;
    for (auto& [id, node] : nodeMap_)
        for (const TreeNode* hijo : node->children)
            claimed.insert(hijo->book.id);

    for (auto& [id, node] : nodeMap_)
        if (!claimed.count(id))
            root_->children.push_back(node);
}

// ---------------------------------------------------------------------------
// listar — preorder ITERATIVO
// Se insertan los hijos en orden inverso en la pila para que al hacer pop
// se procesen de izquierda a derecha, respetando el orden preorder.
// El uso de pila explícita evita stack overflow en árboles muy profundos.
// ---------------------------------------------------------------------------
void GeneralTree::listar(std::ostream& os) const {
    std::stack<const TreeNode*> pila;
    for (auto it = root_->children.rbegin(); it != root_->children.rend(); ++it)
        pila.push(*it);

    while (!pila.empty()) {
        const TreeNode* node = pila.top(); pila.pop();
        os << node->book.id << "\n";
        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
            pila.push(*it);
    }
}

// ---------------------------------------------------------------------------
// borrarRatings — post-order RECURSIVO
// Se procesan los hijos antes que el padre (post-order) para poder liberar
// la memoria de los subárboles eliminados de abajo hacia arriba sin perder
// referencias. El árbol ya no tiene ciclos tras build(), por lo que la
// recursión termina siempre.
// ---------------------------------------------------------------------------
void GeneralTree::borrarRatings(double r) {
    std::vector<TreeNode*> sobrevivientes;
    for (TreeNode* hijo : root_->children) {
        if (!borrarRatingsHelper(hijo, r))
            sobrevivientes.push_back(hijo);
        else
            deleteSubtree(hijo);
    }
    root_->children = sobrevivientes;
}

bool GeneralTree::borrarRatingsHelper(TreeNode* node, double r) {
    std::vector<TreeNode*> sobrevivientes;
    for (TreeNode* hijo : node->children) {
        if (!borrarRatingsHelper(hijo, r))
            sobrevivientes.push_back(hijo);
        else
            deleteSubtree(hijo);
    }
    node->children = sobrevivientes;
    return node->book.averageRating <= r;
}

// ---------------------------------------------------------------------------
// precursores — recorrido ITERATIVO
// Para cada nodo se verifica que su lista de similarBooks sea no vacía y que
// todos los años de publicación de esos libros similares sean estrictamente
// mayores al año del nodo. Si un libro similar tiene año 0 (no registrado en
// el XML) se lo trata como anterior, excluyendo al nodo del resultado.
// ---------------------------------------------------------------------------
void GeneralTree::precursores(const std::string& /*id*/, std::ostream& os) const {
    std::stack<const TreeNode*> pila;
    for (auto it = root_->children.rbegin(); it != root_->children.rend(); ++it)
        pila.push(*it);

    while (!pila.empty()) {
        const TreeNode* node = pila.top(); pila.pop();

        const auto& sibs = node->book.similarBooks;
        if (!sibs.empty()) {
            bool todos_posteriores = true;
            for (const SimilarBook& sb : sibs) {
                // año 0 = no registrado en el XML; se trata como anterior → excluye el nodo
                if (sb.publicationYear == 0 || sb.publicationYear <= node->book.publicationYear) {
                    todos_posteriores = false;
                    break;
                }
            }
            if (todos_posteriores)
                os << node->book.id << "\n";
        }

        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
            pila.push(*it);
    }
}

// ---------------------------------------------------------------------------
// stats — recorrido BFS para calcular altura, promedio de hijos y nodo más conectado
// ---------------------------------------------------------------------------
void GeneralTree::stats(std::ostream& os) const {
    if (root_->children.empty()) {
        os << "El árbol está vacío.\n";
        return;
    }

    int alturaMax      = 0;
    int totalNodos     = 0;
    int totalHijos     = 0;
    int maxHijos       = 0;
    std::string idMasConectado;

    // BFS con pares (nodo, profundidad)
    std::stack<std::pair<const TreeNode*, int>> pila;
    for (auto it = root_->children.rbegin(); it != root_->children.rend(); ++it)
        pila.push({*it, 1});

    while (!pila.empty()) {
        auto [node, prof] = pila.top(); pila.pop();

        totalNodos++;
        int nhijos = static_cast<int>(node->children.size());
        totalHijos += nhijos;

        if (prof > alturaMax) alturaMax = prof;

        if (nhijos > maxHijos) {
            maxHijos       = nhijos;
            idMasConectado = node->book.id;
        }

        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
            pila.push({*it, prof + 1});
    }

    double promHijos = (totalNodos > 0) ? static_cast<double>(totalHijos) / totalNodos : 0.0;

    os << "=== Estadisticas del arbol ===\n"
       << "  Nodos totales       : " << totalNodos     << "\n"
       << "  Altura maxima       : " << alturaMax      << "\n"
       << "  Promedio de hijos   : " << promHijos      << "\n"
       << "  Libro mas conectado : " << idMasConectado << " (" << maxHijos << " hijos)\n";
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
