#include "MagicTree.hpp"
#include <iostream>

int main() {
    MagicTree tree;
    // Siempre busca los archivos en el mismo directorio donde está el ejecutable
    const char* wizardsPath = "wizards.csv";
    const char* spellsPath = "spells.csv";
    tree.buildFromCsv(wizardsPath);
    tree.loadSpellsFromCsv(spellsPath);

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
        std::cout << "6. Salir" << std::endl;
        std::cout << "Seleccione una opción: ";
        std::cin >> option;
        std::cin.ignore(); // Limpiar buffer

        switch(option) {
            case 1: {
                // Mostrar el dueño vivo actual del hechizo
                Wizard* owner = tree.getCurrentOwner();
                if (owner) {
                    std::cout << "Dueño actual del hechizo: " << owner->name << " " << owner->lastName << " (ID: " << owner->id << ")" << std::endl;
                } else {
                    std::cout << "No hay dueño vivo del hechizo. Reasignando automáticamente..." << std::endl;
                    tree.checkAndReassignOwner();
                    owner = tree.getCurrentOwner();
                    if (owner) {
                        std::cout << "Nuevo dueño: " << owner->name << " " << owner->lastName << " (ID: " << owner->id << ")" << std::endl;
                    } else {
                        std::cout << "No se pudo asignar un nuevo dueño." << std::endl;
                    }
                }
                break;
            }
            case 2:
                tree.printSuccessionLine();
                break;
            case 3:
                tree.reassignOwnerOnDeath();
                std::cout << "Guardando cambios tras muerte del dueño..." << std::endl;
                tree.saveToCsv("wizards.csv");
                break;
            case 4: {
                int id;
                std::cout << "Ingrese el ID del mago a editar: ";
                std::cin >> id;
                tree.editWizardData(tree.findWizardByIdPublic(id));
                std::cout << "Guardando cambios tras edición..." << std::endl;
                tree.saveToCsv("wizards.csv");
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
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while(option != 6);

    return 0;
}