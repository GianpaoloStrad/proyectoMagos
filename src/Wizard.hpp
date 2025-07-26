#pragma once
#define MAX_NAME_LENGTH 50
#define MAX_MAGIC_TYPE_LENGTH 20

class Wizard {
public:
    int id;
    char name[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
    char gender; // 'H' o 'M'
    int age;
    int idFather;
    bool isDead;
    char typeMagic[MAX_MAGIC_TYPE_LENGTH];
    bool isOwner;

    Wizard* left;
    Wizard* right;

    Wizard(int id, const char* name, const char* lastName, char gender, int age, 
           int idFather, bool isDead, const char* typeMagic, bool isOwner);
    bool canBeOwner() const;
    void showSpells() const; // Mostrar lista de hechizos del mago
};