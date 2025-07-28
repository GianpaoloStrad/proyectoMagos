#include "Wizard.hpp"
#include "SpellManager.hpp"
#include <cstring> // Para copiar cadenas de texto
#include <iostream>

// Creamos un nuevo mago con todos sus datos
Wizard::Wizard(int id, const char* name, const char* lastName, char gender, int age, 
               int idFather, bool isDead, const char* typeMagic, bool isOwner) {
    this->id = id;
    strcpy(this->name, name); // Copiamos el nombre del mago
    strcpy(this->lastName, lastName);
    this->gender = gender;
    this->age = age;
    this->idFather = idFather;
    this->isDead = isDead;
    strcpy(this->typeMagic, typeMagic);
    this->isOwner = isOwner;
    this->left = nullptr;  // Inicialmente no tiene discípulos
    this->right = nullptr;
}