#pragma once
#include "Wizard.hpp"

class MagicTree {
private:
    Wizard* root;
    Wizard* findWizardById(Wizard* node, int id);
    void insertWizard(Wizard* parent, Wizard* newWizard);
    void clearTree(Wizard* node); // Destructor recursivo
    void reassignOwnerOnDeath(); // Reasigna el dueño del hechizo si el actual muere
private:
    Wizard* findCurrentOwner(Wizard* node) const;
    Wizard* findBestSuccessor(Wizard* node) const;
    void resetAllOwners(Wizard* node) const;

public:
    MagicTree();
    ~MagicTree();
    void buildFromCSV(const char* filePath);
    Wizard* getRoot() const;
    void printSuccessionLine() const; // Muestra la línea de sucesión (magos vivos)
};