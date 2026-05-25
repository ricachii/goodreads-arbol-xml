#include <iostream>
#include <string>
#include "XMLParser.h"
#include "GeneralTree.h"

int main() {
    const std::string dirDatos = "../data";   // carpeta con los 10 000 archivos .xml

    std::cout << "Cargando libros...\n";
    std::vector<Book> libros = XMLParser::loadDirectory(dirDatos);
    std::cout << "Cargados " << libros.size() << " libros.\n";

    GeneralTree arbol;
    for (const Book& b : libros)
        arbol.addBook(b);

    arbol.build();
    std::cout << "Árbol construido.\n";

    // --- listar ---
    std::cout << "\n=== listar (preorder) ===\n";
    arbol.listar();

    // --- borrar_ratings ---
    double umbral = 3.5;
    std::cout << "\n=== borrar_ratings(" << umbral << ") ===\n";
    arbol.borrarRatings(umbral);
    std::cout << "Después de la eliminación:\n";
    arbol.listar();

    // --- precursores ---
    std::string idConsulta = "1";   // cambiar por un ID real para probar
    std::cout << "\n=== precursores(" << idConsulta << ") ===\n";
    arbol.precursores(idConsulta);

    return 0;
}
