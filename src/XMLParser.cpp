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
    pugi::xml_parse_result resultado = doc.load_file(filepath.c_str());

    if (!resultado) {
        std::cerr << "Error al parsear " << filepath << ": " << resultado.description() << "\n";
        return false;
    }

    // 1. Obtener el nodo raíz <book> navegando dentro de la respuesta
    pugi::xml_node bookNode = doc.child("GoodreadsResponse").child("book");
    if (!bookNode) return false;

    // 2. Extraer campos de texto directos de forma segura
    out.id          = bookNode.child("id").text().as_string();
    out.title       = bookNode.child("title").text().as_string();
    out.isbn        = bookNode.child("isbn").text().as_string();
    out.language    = bookNode.child("language_code").text().as_string();
    out.description = bookNode.child("description").text().as_string();

    // Lambdas con try/catch para evitar crash en campos malformados
    auto toInt = [](const char* s) -> int {
        try { return (s && *s) ? std::stoi(s) : 0; }
        catch (...) { return 0; }
    };
    auto toDbl = [](const char* s) -> double {
        try { return (s && *s) ? std::stod(s) : 0.0; }
        catch (...) { return 0.0; }
    };

    // 3. Convertir campos numéricos
    out.publicationYear = toInt(bookNode.child("publication_year").child_value());
    out.averageRating   = toDbl(bookNode.child("average_rating").child_value());
    out.numPages        = toInt(bookNode.child("num_pages").child_value());

    // 4. Parsear similar_books
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
    std::vector<Book> libros;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Error: El directorio de datos '" << directory << "' no existe.\n";
        return libros;
    }

    std::vector<fs::path> paths;
    for (const auto& entrada : fs::recursive_directory_iterator(directory)) {
        std::string ext = entrada.path().extension().string();
        if (ext == ".xml" || ext == ".XML")
            paths.push_back(entrada.path());
    }

    int total = static_cast<int>(paths.size());
    for (int i = 0; i < total; ++i) {
        Book b;
        if (parseFile(paths[i].string(), b))
            libros.push_back(b);
        printProgressBar(i + 1, total);
    }
    std::cout << "\n";

    return libros;
}