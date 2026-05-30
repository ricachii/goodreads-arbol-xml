#include "XMLParser.h"
#include "pugixml.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// parseFile
// ---------------------------------------------------------------------------
bool XMLParser::parseFile(const std::string& filepath, Book& out) {
    pugi::xml_document doc;
    if (!doc.load_file(filepath.c_str())) {
        std::cerr << "Error al parsear: " << filepath << "\n";
        return false;
    }

    // El XML tiene la estructura <GoodreadsResponse><book>...</book></GoodreadsResponse>
    pugi::xml_node bookNode = doc.child("GoodreadsResponse").child("book");
    if (bookNode.empty()) bookNode = doc.child("book");  // fallback por si acaso
    if (bookNode.empty()) {
        std::cerr << "No se encontró <book> en: " << filepath << "\n";
        return false;
    }

    // Lambdas para convertir strings vacíos sin lanzar excepción
    auto toInt = [](const char* s) -> int {
        try { return (s && *s) ? std::stoi(s) : 0; }
        catch (...) { return 0; }
    };
    auto toDbl = [](const char* s) -> double {
        try { return (s && *s) ? std::stod(s) : 0.0; }
        catch (...) { return 0.0; }
    };

    out.id              = bookNode.child("id").child_value();
    out.title           = bookNode.child("title").child_value();
    out.isbn            = bookNode.child("isbn").child_value();
    out.language        = bookNode.child("language_code").child_value();
    out.description     = bookNode.child("description").child_value();
    out.publicationYear = toInt(bookNode.child("publication_year").child_value());
    out.averageRating   = toDbl(bookNode.child("average_rating").child_value());
    out.numPages        = toInt(bookNode.child("num_pages").child_value());

    pugi::xml_node simNode = bookNode.child("similar_books");
    for (pugi::xml_node sb : simNode.children("book")) {
        SimilarBook s;
        s.id              = sb.child("id").child_value();
        s.title           = sb.child("title").child_value();
        s.isbn            = sb.child("isbn").child_value();
        s.publicationYear = toInt(sb.child("publication_year").child_value());
        out.similarBooks.push_back(s);
    }

    return true;
}

// ---------------------------------------------------------------------------
// loadDirectory
// ---------------------------------------------------------------------------
static void printProgressBar(int actual, int total) {
    const int ancho = 40;
    int relleno = (total > 0) ? (actual * ancho / total) : 0;
    std::cout << "\r[";
    for (int i = 0; i < ancho; ++i)
        std::cout << (i < relleno ? '=' : ' ');
    std::cout << "] " << actual << "/" << total << std::flush;
}

std::vector<Book> XMLParser::loadDirectory(const std::string& directory) {
    // Paso 1: recolectar todos los paths XML
    std::vector<fs::path> paths;
    for (const auto& entrada : fs::recursive_directory_iterator(directory))
        if (entrada.path().extension() == ".xml")
            paths.push_back(entrada.path());

    // Paso 2: parsear con barra de progreso
    std::vector<Book> libros;
    int total = static_cast<int>(paths.size());
    for (int i = 0; i < total; ++i) {
        Book b;
        if (XMLParser::parseFile(paths[i].string(), b))
            libros.push_back(b);
        printProgressBar(i + 1, total);
    }
    std::cout << "\n";

    return libros;
}
