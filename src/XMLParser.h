#pragma once
#include "Book.h"
#include <string>
#include <vector>

/*
 * Clase: XMLParser
 * ----------------
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
    /*
     * parseFile
     * ---------
     * Parsea un único archivo XML del dataset GoodReads y rellena el Book de salida.
     * Maneja campos numéricos vacíos (publicationYear, numPages, averageRating)
     * asignándoles 0 en lugar de lanzar una excepción.
     *
     * Parámetros:
     *   filepath — ruta absoluta o relativa al archivo .xml.
     *   out      — Book donde se escriben los datos extraídos.
     *
     * Retorna true si el archivo fue parseado correctamente, false en caso contrario.
     */
    static bool parseFile(const std::string& filepath, Book& out);

    /*
     * loadDirectory
     * -------------
     * Carga recursivamente todos los archivos .xml dentro de `directory`,
     * parseando cada uno con parseFile() y mostrando una barra de progreso.
     *
     * Parámetro:
     *   directory — ruta a la carpeta que contiene los archivos XML.
     *
     * Retorna un vector con todos los Books cargados exitosamente.
     */
    static std::vector<Book> loadDirectory(const std::string& directory);
};
