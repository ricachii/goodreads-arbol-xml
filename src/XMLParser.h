#pragma once
#include "Book.h"
#include <string>
#include <vector>

/*
 * Carga datos de Book desde archivos XML de GoodReads usando PugiXML.
 *
 * Estructura típica del XML (campos que nos interesan):
 *
 *   <book>
 *     <id>...</id>
 *     <title>...</title>
 *     <isbn>...</isbn>
 *     <publication_year>...</publication_year>
 *     <language_code>...</language_code>
 *     <description>...</description>
 *     <average_rating>...</average_rating>
 *     <num_pages>...</num_pages>
 *     <similar_books>
 *       <book>
 *         <id>...</id>
 *         <title>...</title>
 *         <isbn>...</isbn>
 *         <publication_year>...</publication_year>
 *       </book>
 *       ...
 *     </similar_books>
 *   </book>
 */
class XMLParser {
public:
    // Parsea un único archivo XML y retorna el Book que representa.
    // Retorna false si el archivo no pudo ser parseado.
    static bool parseFile(const std::string& filepath, Book& out);

    // Carga todos los archivos .xml dentro de `directory` y retorna un vector de Books.
    static std::vector<Book> loadDirectory(const std::string& directory);
};
