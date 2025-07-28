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
    void fillWizardArray(const Wizard* node, Wizard** arr, int& count, int max) const;

public:
    MagicTree();
    ~MagicTree();
    bool buildFromCsv(const char* filePath);
    void loadSpellsFromCsv(const char* filePath);
    Wizard* getRoot() const;
    void printSuccessionLine() const; // Muestra la línea de sucesión (magos vivos)
    void reassignOwnerOnDeath(); // Reasigna el dueño del hechizo si el actual muere
    Wizard* findWizardByIdPublic(int id); // Búsqueda pública por ID
    void editWizardData(Wizard* wizard); // Editar datos permitidos de un mago
    void saveToCsv(const char* filePath) const; // Guarda todos los magos en un archivo CSV
    void showWizardCompleteData(int wizardId); // Mostrar datos completos de un mago específico
    void checkAndReassignOwner(); // Verificar y reasignar automáticamente si el dueño está muerto
    Wizard* getCurrentOwner() const; // Obtener el dueño actual vivo del hechizo
    Wizard* findBestSuccessorFromAll() const; // Buscar el mejor sucesor en todo el árbol
};