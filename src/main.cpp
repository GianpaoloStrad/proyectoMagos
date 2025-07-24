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
        std::cout << "2. Salir" << std::endl;
        std::cout << "Seleccione una opción: ";
        std::cin >> option;
        std::cin.ignore(); // Limpiar buffer

        switch(option) {
            case 1:
                std::cout << "Mago raíz: " << tree.getRoot()->name << std::endl;
                break;
            case 2:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while(option != 2);

    return 0;
}