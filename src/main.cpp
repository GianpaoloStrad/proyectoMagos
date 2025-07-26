#include "MagicTree.hpp"
#include <iostream>

int main() {
    MagicTree tree;
    
    // Intentar cargar desde diferentes ubicaciones
    const char* wizardsPath = "wizards.csv";
    const char* spellsPath = "spells.csv";
    
    // Si no encuentra en el directorio actual, intentar en bin/
    if (!tree.buildFromCSV(wizardsPath)) {
        wizardsPath = "bin/wizards.csv";
        spellsPath = "bin/spells.csv";
        tree.buildFromCSV(wizardsPath);
        tree.loadSpellsFromCSV(spellsPath);
    } else {
        tree.loadSpellsFromCSV(spellsPath);
    }

    if (!tree.getRoot()) {
        std::cerr << "Error al cargar el CSV." << std::endl;
        return 1;
    }

    int option = 0;
    do {
        std::cout << "\n--- Menú ---" << std::endl;
        std::cout << "1. Mostrar mago raíz" << std::endl;
        std::cout << "2. Mostrar línea de sucesión (magos vivos, preorden)" << std::endl;
        std::cout << "3. Simular muerte del dueño y reasignar hechizo" << std::endl;
        std::cout << "4. Editar datos de un mago" << std::endl;
        std::cout << "5. Mostrar datos completos de un mago" << std::endl;
        std::cout << "6. Guardar cambios en CSV" << std::endl;
        std::cout << "7. Salir" << std::endl;
        std::cout << "Seleccione una opción: ";
        std::cin >> option;
        std::cin.ignore(); // Limpiar buffer

        switch(option) {
            case 1:
                std::cout << "Mago raíz: " << tree.getRoot()->name << std::endl;
                break;
            case 2:
                tree.printSuccessionLine();
                break;
            case 3:
                tree.reassignOwnerOnDeath();
                break;
            case 4: {
                int id;
                std::cout << "Ingrese el ID del mago a editar: ";
                std::cin >> id;
                tree.editWizardData(tree.findWizardByIdPublic(id));
                break;
            }
            case 5: {
                int id;
                std::cout << "Ingrese el ID del mago para ver sus datos completos: ";
                std::cin >> id;
                tree.showWizardCompleteData(id);
                break;
            }
            case 6:
                // Guardar en la misma ubicación donde se cargó
                tree.saveToCSV(wizardsPath);
                break;
            case 7:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while(option != 7);

    return 0;
}