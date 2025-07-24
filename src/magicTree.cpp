#include "MagicTree.hpp"
#include "CSVManager.hpp"

MagicTree::MagicTree() : root(nullptr) {}

MagicTree::~MagicTree() {
    // Destructor recursivo para liberar todos los nodos
    clearTree(root);
}

// Función auxiliar recursiva para liberar memoria
void MagicTree::clearTree(Wizard* node) {
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

void MagicTree::buildFromCSV(const char* filePath) {
    int count = 0;
    Wizard** wizards = csvManager::loadWizards(filePath, &count);

    for (int i = 0; i < count; ++i) {
        if (wizards[i]->idFather == -1) {
            root = wizards[i];
        } else {
            Wizard* parent = findWizardById(root, wizards[i]->idFather);
            if (parent) insertWizard(parent, wizards[i]);
        }
    }

    csvManager::freeWizardArray(wizards, count);
}
Wizard* MagicTree::findWizardById(Wizard* node, int id) {
    if (!node) return nullptr;
    if (node->id == id) return node;
    
    Wizard* leftSearch = findWizardById(node->left, id);
    if (leftSearch) return leftSearch;
    
    return findWizardById(node->right, id);
}
void MagicTree::insertWizard(Wizard* parent, Wizard* newWizard) {
    if (!parent->left) {
        parent->left = newWizard;
    } else if (!parent->right) {
        parent->right = newWizard;
    } else {
        // Ambos hijos existen, no se inserta para evitar sobrescribir
        // Puedes imprimir un mensaje de advertencia si lo deseas
        // printf("Advertencia: El mago con id %d ya tiene dos discípulos.\n", parent->id);
    }
}
Wizard* MagicTree::getRoot() const {
    return root;
}

void printSuccessionLineHelper(const Wizard* node) {
    if (!node) return;
    if (!node->isDead) {
        std::cout << node->name << " " << node->lastName << " (ID: " << node->id << ")\n";
    }
    printSuccessionLineHelper(node->left);
    printSuccessionLineHelper(node->right);
}

void MagicTree::printSuccessionLine() const {
    std::cout << "\n--- Línea de sucesión (magos vivos) ---\n";
    printSuccessionLineHelper(root);
}