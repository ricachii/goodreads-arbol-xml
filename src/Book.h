#pragma once
#include <string>
#include <vector>

// Representa un libro similar referenciado dentro del XML de un libro
struct SimilarBook {
    std::string title;
    std::string isbn;
    int         publicationYear;
};

// Contiene todos los datos extraídos de un archivo XML individual
struct Book {
    std::string             id;
    std::string             title;
    std::string             isbn;
    int                     publicationYear;
    std::string             language;
    std::string             description;
    double                  averageRating;
    int                     numPages;
    std::vector<SimilarBook> similarBooks;
};
