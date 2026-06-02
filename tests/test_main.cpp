#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include "../src/XMLParser.h"
#include "../src/GeneralTree.h"

// ---------------------------------------------------------------------------
// Mini framework de testing
// ---------------------------------------------------------------------------
static int passed = 0;
static int failed = 0;

static void check(const std::string& nombre, bool condicion) {
    if (condicion) {
        std::cout << "  [OK]  " << nombre << "\n";
        passed++;
    } else {
        std::cout << "  [FAIL] " << nombre << "\n";
        failed++;
    }
}

static GeneralTree buildTestTree() {
    std::vector<Book> libros = XMLParser::loadDirectory("../tests/data");
    GeneralTree arbol;
    for (const Book& b : libros)
        arbol.addBook(b);
    arbol.build();
    return arbol;
}

static std::vector<std::string> listarAVector(GeneralTree& arbol) {
    std::ostringstream oss;
    arbol.listar(oss);
    std::istringstream iss(oss.str());
    std::vector<std::string> ids;
    std::string linea;
    while (std::getline(iss, linea))
        if (!linea.empty()) ids.push_back(linea);
    return ids;
}

static std::vector<std::string> precursoresAVector(GeneralTree& arbol) {
    std::ostringstream oss;
    arbol.precursores("", oss);
    std::istringstream iss(oss.str());
    std::vector<std::string> ids;
    std::string linea;
    while (std::getline(iss, linea))
        if (!linea.empty()) ids.push_back(linea);
    return ids;
}

// ---------------------------------------------------------------------------
// Dataset de prueba — libros reales del dataset GoodReads
// ---------------------------------------------------------------------------
// id=1       Harry Potter and the Half-Blood Prince (2006, rating 4.54)
//              similares en dataset: 153795, 201341
// id=153795  Squire - Protector of the Small #3   (2007, rating 4.27)
//              hijo de 1
// id=201341  Mossflower - Redwall #2              (1998, rating 4.09)
//              hijo de 1
// id=15842230 The Forgotten - John Puller #2      (year 0, rating 4.07)
//              huerfano (similares no estan en dataset)
//              PRECURSOR: todos sus similares tienen year 2012-2013 > 0
//
// Arbol esperado (DFS orden numerico):
//   __root__
//     +-- 1 (HP HBP, 2006, 4.54)
//     |     +-- 153795 (Squire, 2007, 4.27)
//     |     +-- 201341 (Mossflower, 1998, 4.09)
//     +-- 15842230 (The Forgotten, year 0, 4.07)
//
// Preorder: 1, 153795, 201341, 15842230
// borrarRatings(4.1): elimina 201341 (4.09) y 15842230 (4.07) -> quedan 1, 153795
// precursores: solo 15842230 (year 0, todos similares tienen year > 0)
// ---------------------------------------------------------------------------

void test_carga() {
    std::cout << "\n--- Test: carga de libros ---\n";
    std::vector<Book> libros = XMLParser::loadDirectory("../tests/data");
    check("Se cargan 4 libros", libros.size() == 4);

    bool tiene1 = false, tieneSquire = false, tieneMoss = false, tieneForgotten = false;
    for (const auto& b : libros) {
        if (b.id == "1")        tiene1        = true;
        if (b.id == "153795")   tieneSquire   = true;
        if (b.id == "201341")   tieneMoss     = true;
        if (b.id == "15842230") tieneForgotten = true;
    }
    check("Existe HP Half-Blood Prince (id=1)", tiene1);
    check("Existe Squire (id=153795)", tieneSquire);
    check("Existe Mossflower (id=201341)", tieneMoss);
    check("Existe The Forgotten (id=15842230)", tieneForgotten);

    for (const auto& b : libros) {
        if (b.id == "1") {
            check("HP HBP tiene similares", !b.similarBooks.empty());
            check("HP HBP rating = 4.54", b.averageRating == 4.54);
            check("HP HBP anio = 2006", b.publicationYear == 2006);
        }
        if (b.id == "201341") {
            check("Mossflower anio = 1998", b.publicationYear == 1998);
            check("Mossflower rating = 4.09", b.averageRating == 4.09);
        }
    }
}

void test_listar() {
    std::cout << "\n--- Test: listar (preorder) ---\n";
    GeneralTree arbol = buildTestTree();
    auto ids = listarAVector(arbol);

    check("listar retorna 4 IDs", ids.size() == 4);
    check("Primer ID es 1 (raiz numericamente menor)", ids[0] == "1");
    check("1 aparece antes que 153795 (hijo de 1)",
          std::find(ids.begin(), ids.end(), "1") < std::find(ids.begin(), ids.end(), "153795"));
    check("1 aparece antes que 201341 (hijo de 1)",
          std::find(ids.begin(), ids.end(), "1") < std::find(ids.begin(), ids.end(), "201341"));
    check("153795 aparece antes que 201341 (orden similar_books de 1)",
          std::find(ids.begin(), ids.end(), "153795") < std::find(ids.begin(), ids.end(), "201341"));
    check("15842230 esta en el resultado",
          std::find(ids.begin(), ids.end(), "15842230") != ids.end());
}

void test_borrar_ratings() {
    std::cout << "\n--- Test: borrarRatings(4.1) ---\n";
    GeneralTree arbol = buildTestTree();
    arbol.borrarRatings(4.1);
    auto ids = listarAVector(arbol);

    check("Quedan 2 libros tras borrar rating <= 4.1", ids.size() == 2);
    check("1 sobrevive (rating 4.54 > 4.1)",
          std::find(ids.begin(), ids.end(), "1") != ids.end());
    check("153795 sobrevive (rating 4.27 > 4.1)",
          std::find(ids.begin(), ids.end(), "153795") != ids.end());
    check("201341 eliminado (rating 4.09 <= 4.1)",
          std::find(ids.begin(), ids.end(), "201341") == ids.end());
    check("15842230 eliminado (rating 4.07 <= 4.1)",
          std::find(ids.begin(), ids.end(), "15842230") == ids.end());
}

void test_precursores() {
    std::cout << "\n--- Test: precursores ---\n";
    GeneralTree arbol = buildTestTree();
    auto ids = precursoresAVector(arbol);

    check("Hay exactamente 1 precursor", ids.size() == 1);
    check("15842230 es precursor (year 0, todos similares tienen year 2012-2013 > 0)",
          std::find(ids.begin(), ids.end(), "15842230") != ids.end());
    check("1 NO es precursor (similar Mossflower 1998 < 2006)",
          std::find(ids.begin(), ids.end(), "1") == ids.end());
    check("153795 NO es precursor (tiene similar con year 0)",
          std::find(ids.begin(), ids.end(), "153795") == ids.end());
    check("201341 NO es precursor (tiene similar con year 0)",
          std::find(ids.begin(), ids.end(), "201341") == ids.end());
}

// ---------------------------------------------------------------------------
int main() {
    std::cout << "========================================\n";
    std::cout << "  Tests - Arbol General GoodReads\n";
    std::cout << "========================================\n";

    test_carga();
    test_listar();
    test_borrar_ratings();
    test_precursores();

    std::cout << "\n========================================\n";
    std::cout << "  Resultado: " << passed << " OK  |  " << failed << " FAIL\n";
    std::cout << "========================================\n";

    return failed > 0 ? 1 : 0;
}
