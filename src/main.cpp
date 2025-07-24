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
        std::cout << "4. Salir" << std::endl;
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
            case 4:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while(option != 4);

    return 0;
}