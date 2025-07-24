#include "Wizard.hpp"
#include <cstring> // Para strcpy

// --- Implementación del Constructor ---
Wizard::Wizard(int id, const char* name, const char* lastName, char gender, int age, 
               int idFather, bool isDead, const char* typeMagic, bool isOwner) {
    this->id = id;
    strcpy(this->name, name); // Copiar el nombre
    strcpy(this->lastName, lastName);
    this->gender = gender;
    this->age = age;
    this->idFather = idFather;
    this->isDead = isDead;
    strcpy(this->typeMagic, typeMagic);
    this->isOwner = isOwner;
    this->left = nullptr;
    this->right = nullptr;
}

// --- Método Adicional (Ejemplo) ---
bool Wizard::canBeOwner() const {
    // Regla: Solo magos vivos con magia "elemental" o "unique" pueden ser dueños
    return !isDead && (strcmp(typeMagic, "elemental") == 0 || strcmp(typeMagic, "unique") == 0);
}