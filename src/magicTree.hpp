#pragma once
#include "Wizard.hpp"

class SpellManager; // Forward declaration

class MagicTree {
private:
    Wizard* root;
    SpellManager* spellManager;
    Wizard* findWizardById(Wizard* node, int id);
    void insertWizard(Wizard* parent, Wizard* newWizard);
    void clearTree(Wizard* node); // Destructor recursivo
    Wizard* findCurrentOwner(Wizard* node) const;
    Wizard* findBestSuccessor(Wizard* node) const;
    void resetAllOwners(Wizard* node) const;
    void simulateSuccessionLine(const Wizard* start, bool* deadMask, int maxWizards) const;
    void fillWizardArray(const Wizard* node, Wizard** arr, int& count, int max) const;

public:
    MagicTree();
    ~MagicTree();
    void buildFromCSV(const char* filePath);
    void loadSpellsFromCSV(const char* filePath);
    Wizard* getRoot() const;
    void printSuccessionLine() const; // Muestra la línea de sucesión (magos vivos)
    void printTrueSuccessionLine() const; // Muestra la línea de sucesión real según las reglas
    void reassignOwnerOnDeath(); // Reasigna el dueño del hechizo si el actual muere
    Wizard* findWizardByIdPublic(int id); // Búsqueda pública por ID
    void editWizardData(Wizard* wizard); // Editar datos permitidos de un mago
    void saveToCSV(const char* filePath) const; // Guarda todos los magos en un archivo CSV
    void showWizardSpells(int wizardId); // Mostrar hechizos de un mago específico
}; 