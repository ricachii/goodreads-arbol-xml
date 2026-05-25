#include "XMLParser.h"
#include "../libs/pugixml/pugixml.hpp"
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
        std::cerr << "Error al parsear: " << filepath << "\n";
        return false;
    }

    // TODO:
    //   1. Obtener el nodo raíz <book>:
    //        pugi::xml_node bookNode = doc.child("book");
    //
    //   2. Extraer cada campo usando .child("tag").child_value():
    //        out.id    = bookNode.child("id").child_value();
    //        out.title = bookNode.child("title").child_value();
    //        ...
    //
    //   3. Convertir campos numéricos:
    //        std::string yearStr = bookNode.child("publication_year").child_value();
    //        out.publicationYear = yearStr.empty() ? 0 : std::stoi(yearStr);
    //        (Mismo patrón para averageRating con std::stod, y numPages con std::stoi)
    //
    //   4. Parsear similar_books:
    //        pugi::xml_node simNode = bookNode.child("similar_books");
    //        for (pugi::xml_node sb : simNode.children("book")) {
    //            SimilarBook s;
    //            s.title = sb.child("title").child_value();
    //            s.isbn  = sb.child("isbn").child_value();
    //            // ... parsear año
    //            out.similarBooks.push_back(s);
    //        }
    //
    //   5. Retornar true.

    return false; // placeholder — eliminar una vez implementado
}

// ---------------------------------------------------------------------------
// loadDirectory
// ---------------------------------------------------------------------------
std::vector<Book> XMLParser::loadDirectory(const std::string& directory) {
    std::vector<Book> libros;

    // TODO:
    //   Iterar sobre cada archivo en `directory` usando std::filesystem:
    //
    //   for (const auto& entrada : fs::directory_iterator(directory)) {
    //       if (entrada.path().extension() == ".xml") {
    //           Book b;
    //           if (XMLParser::parseFile(entrada.path().string(), b))
    //               libros.push_back(b);
    //       }
    //   }
    //
    //   Retornar libros.

    return libros;
}
