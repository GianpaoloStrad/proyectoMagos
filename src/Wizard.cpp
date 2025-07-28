#include "Wizard.hpp"
#include "SpellManager.hpp"
#include <cstring> // Para strcpy
#include <iostream>

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