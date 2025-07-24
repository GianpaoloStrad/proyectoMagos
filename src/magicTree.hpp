#pragma once
#include "Wizard.hpp"

class MagicTree {
private:
    Wizard* root;
    Wizard* findWizardById(Wizard* node, int id);
    void insertWizard(Wizard* parent, Wizard* newWizard);
    void clearTree(Wizard* node); // Destructor recursivo

public:
    MagicTree();
    ~MagicTree();
    void buildFromCSV(const char* filePath);
    Wizard* getRoot() const;
};