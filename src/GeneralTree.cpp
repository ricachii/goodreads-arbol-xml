#include "GeneralTree.h"
#include <iostream>
#include <stack>
#include <unordered_set>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------
GeneralTree::GeneralTree() {
    // Crear la raíz virtual con un Book vacío.
    // Su id es "__root__" para no confundirla con un libro real.
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
    if (nodeMap_.count(book.id)) return; // evitar memory leak por ID duplicado
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
    // Ordenar IDs numéricamente para que el DFS sea determinístico
    // (unordered_map no garantiza orden de iteración).
    std::vector<std::string> ids;
    ids.reserve(nodeMap_.size());
    for (auto& [id, _] : nodeMap_) ids.push_back(id);
    std::sort(ids.begin(), ids.end(), [](const std::string& a, const std::string& b) {
        try { return std::stoll(a) < std::stoll(b); }
        catch (...) { return a < b; }
    });

    std::unordered_set<std::string> visited;

    for (const std::string& startId : ids) {
        if (visited.count(startId)) continue;

        visited.insert(startId);
        std::stack<TreeNode*> pila;
        pila.push(nodeMap_[startId]);

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

    for (const std::string& id : ids)
        if (!claimed.count(id))
            root_->children.push_back(nodeMap_[id]);
}

// ---------------------------------------------------------------------------
// listar — Recorrido Preorder General
// ---------------------------------------------------------------------------
void GeneralTree::listar(std::ostream& os) const {
    // Recorrido preorder iterativo con pila explícita para evitar stack overflow.
    // Los hijos se insertan en orden inverso para procesarlos de izquierda a derecha.
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
// borrarRatings — Eliminación por umbral (Post-order)
// ---------------------------------------------------------------------------
void GeneralTree::borrarRatings(double r) {
    std::vector<TreeNode*> sobrevivientes;
    for (TreeNode* hijo : root_->children) {
        if (!borrarRatingsHelper(hijo, r)) {
            sobrevivientes.push_back(hijo);
        }
    }
    root_->children = sobrevivientes;
}

bool GeneralTree::borrarRatingsHelper(TreeNode* node, double r) {
    if (!node) return false;

    // 1. Procesar primero los hijos (Post-order) para limpiar desde abajo hacia arriba
    std::vector<TreeNode*> sobrevivientes;
    for (TreeNode* hijo : node->children) {
        if (!borrarRatingsHelper(hijo, r)) {
            sobrevivientes.push_back(hijo);
        }
    }
    node->children = sobrevivientes;

    // 2. Si el nodo actual no supera el umbral, se elimina con todo su subárbol
    if (node->book.averageRating <= r) {
        for (TreeNode* h : node->children) {
            deleteSubtree(h);
        }
        delete node;
        return true; // Le avisa al padre que este hijo fue eliminado
    }
    return false;
}

// ---------------------------------------------------------------------------
// precursores — Filtrado por año de publicación de similares
// ---------------------------------------------------------------------------
void GeneralTree::precursores(const std::string& /*id*/, std::ostream& os) const {
    // Recorrido iterativo para evitar stack overflow en árboles profundos.
    std::stack<const TreeNode*> pila;
    for (auto it = root_->children.rbegin(); it != root_->children.rend(); ++it)
        pila.push(*it);

    while (!pila.empty()) {
        const TreeNode* node = pila.top(); pila.pop();

        const auto& sibs = node->book.similarBooks;
        if (!sibs.empty()) {
            bool todosPosteriores = true;
            for (const SimilarBook& sim : sibs) {
                // año 0 = no registrado en el XML; se trata como anterior → excluye el nodo
                if (sim.publicationYear == 0 || sim.publicationYear <= node->book.publicationYear) {
                    todosPosteriores = false;
                    break;
                }
            }
            if (todosPosteriores) os << node->book.id << "\n";
        }

        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
            pila.push(*it);
    }
}


// ---------------------------------------------------------------------------
// stats — estadísticas generales del árbol en un único recorrido
// ---------------------------------------------------------------------------
void GeneralTree::stats(std::ostream& os) const {
    if (root_->children.empty()) {
        os << "El árbol está vacío.\n";
        return;
    }

    int alturaMax = 0, totalNodos = 0, totalHijos = 0, maxHijos = 0;
    std::string idMasConectado;

    std::stack<std::pair<const TreeNode*, int>> pila;
    for (auto it = root_->children.rbegin(); it != root_->children.rend(); ++it)
        pila.push({*it, 1});

    while (!pila.empty()) {
        auto [node, prof] = pila.top(); pila.pop();
        totalNodos++;
        int nhijos = static_cast<int>(node->children.size());
        totalHijos += nhijos;
        if (prof > alturaMax) alturaMax = prof;
        if (nhijos > maxHijos) { maxHijos = nhijos; idMasConectado = node->book.id; }
        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
            pila.push({*it, prof + 1});
    }

    double prom = totalNodos > 0 ? static_cast<double>(totalHijos) / totalNodos : 0.0;
    os << "=== Estadisticas del arbol ===\n"
       << "  Nodos totales       : " << totalNodos     << "\n"
       << "  Altura maxima       : " << alturaMax      << "\n"
       << "  Promedio de hijos   : " << prom           << "\n"
       << "  Libro mas conectado : " << idMasConectado << " (" << maxHijos << " hijos)\n";
}

// ---------------------------------------------------------------------------
// deleteSubtree — Liberación recursiva de memoria
// ---------------------------------------------------------------------------
void GeneralTree::deleteSubtree(TreeNode* node) {
    if (!node) return;
    
    // Primero liberamos la memoria de todos los hijos recursivamente
    for (TreeNode* hijo : node->children) {
        deleteSubtree(hijo);
    }
    
    // Finalmente eliminamos el nodo actual
    delete node;
}