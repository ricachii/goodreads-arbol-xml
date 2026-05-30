#include <iostream>
#include <fstream>
#include <string>
#include "XMLParser.h"
#include "GeneralTree.h"

static std::ofstream openFile(const std::string& nombre) {
    std::ofstream f(nombre);
    if (!f.is_open())
        std::cerr << "No se pudo crear: " << nombre << "\n";
    return f;
}

int main() {
    const std::string dirDatos = "../data";

    std::cout << "Cargando libros...\n";
    std::vector<Book> libros = XMLParser::loadDirectory(dirDatos);
    std::cout << "Cargados " << libros.size() << " libros.\n";

    GeneralTree arbol;
    for (const Book& b : libros)
        arbol.addBook(b);

    arbol.build();
    std::cout << "Arbol construido.\n";
    arbol.stats();

    // --- listar ---
    std::cout << "\n=== listar (preorder) -> listar.txt ===\n";
    {
        auto f = openFile("listar.txt");
        arbol.listar(f);
    }

    // --- borrar_ratings ---
    double umbral = 3.5;
    std::cout << "=== borrar_ratings(" << umbral << ") -> listar_post_borrado.txt ===\n";
    arbol.borrarRatings(umbral);
    {
        auto f = openFile("listar_post_borrado.txt");
        arbol.listar(f);
    }

    // --- precursores ---
    std::string idConsulta = "1";
    std::cout << "=== precursores(" << idConsulta << ") -> precursores.txt ===\n";
    {
        auto f = openFile("precursores.txt");
        arbol.precursores(idConsulta, f);
    }

    std::cout << "Listo. Archivos guardados en el directorio de ejecucion.\n";
    return 0;
}
