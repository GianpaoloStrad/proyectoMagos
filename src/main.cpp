#include "MagicTree.hpp"
#include <iostream>

int main() {
    MagicTree tree;
    tree.buildFromCSV("../bin/wizards.csv");

    if (!tree.getRoot()) {
        std::cerr << "Error al cargar el CSV." << std::endl;
        return 1;
    }

    int option = 0;
    do {
        std::cout << "\n--- Menú ---" << std::endl;
        std::cout << "1. Mostrar mago raíz" << std::endl;
        std::cout << "2. Mostrar línea de sucesión (magos vivos)" << std::endl;
        std::cout << "3. Simular muerte del dueño y reasignar hechizo" << std::endl;
        std::cout << "4. Editar datos de un mago" << std::endl;
        std::cout << "5. Guardar cambios en CSV" << std::endl;
        std::cout << "6. Salir" << std::endl;
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
            case 5:
                tree.saveToCSV("../bin/wizards.csv");
                break;
            case 6:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while(option != 6);

    return 0;
}