#include "MagicTree.hpp"
#include "CSVManager.hpp"

MagicTree::MagicTree() : root(nullptr) {}

MagicTree::~MagicTree() {
    // Implementar destructor recursivo (DFS)
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
        // Priorizar izquierda (o implementar reglas del proyecto)
        parent->left = newWizard;
    }
}
Wizard* MagicTree::getRoot() const {
    return root;
}