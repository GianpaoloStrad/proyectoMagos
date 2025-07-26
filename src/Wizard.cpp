#include "Wizard.hpp"
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

// --- Método Adicional (Ejemplo) ---
bool Wizard::canBeOwner() const {
    // Regla: Solo magos vivos con magia "elemental" o "unique" pueden ser dueños
    return !isDead && (strcmp(typeMagic, "elemental") == 0 || strcmp(typeMagic, "unique") == 0);
}

// --- Mostrar lista de hechizos del mago ---
void Wizard::showSpells() const {
    std::cout << "\n=== Hechizos de " << name << " " << lastName << " ===\n";
    std::cout << "Tipo de magia: " << typeMagic << "\n";
    std::cout << "Estado: " << (isDead ? "Muerto" : "Vivo") << "\n";
    std::cout << "Edad: " << age << " años\n";
    std::cout << "Género: " << (gender == 'H' ? "Hombre" : "Mujer") << "\n";
    
    // Mostrar hechizos según el tipo de magia
    std::cout << "\nHechizos conocidos:\n";
    if (strcmp(typeMagic, "elemental") == 0) {
        std::cout << "- Bola de Fuego\n";
        std::cout << "- Tormenta de Hielo\n";
        std::cout << "- Rayo Eléctrico\n";
        std::cout << "- Terremoto\n";
        std::cout << "- Tornado\n";
    } else if (strcmp(typeMagic, "unique") == 0) {
        std::cout << "- Hechizo de Invisibilidad\n";
        std::cout << "- Transformación Animal\n";
        std::cout << "- Telepatía\n";
        std::cout << "- Control Mental\n";
        std::cout << "- Portal Dimensional\n";
    } else if (strcmp(typeMagic, "mixed") == 0) {
        std::cout << "- Hechizo Combinado Elemental\n";
        std::cout << "- Transformación Múltiple\n";
        std::cout << "- Escudo Protector\n";
        std::cout << "- Curación Avanzada\n";
        std::cout << "- Ilusión Realista\n";
    } else if (strcmp(typeMagic, "no_magic") == 0) {
        std::cout << "- No posee hechizos mágicos\n";
        std::cout << "- Habilidades físicas mejoradas\n";
    }
    
    if (isOwner) {
        std::cout << "\n*** DUEÑO DEL HECHIZO LEGENDARIO ***\n";
        std::cout << "- Hechizo de Visión de Sucesión\n";
        std::cout << "- Control Total de la Escuela\n";
        std::cout << "- Acceso a Todos los Conocimientos\n";
    }
    
    std::cout << "\n";
}