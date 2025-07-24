#include "csvManager.hpp"
#include "Wizard.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Wizard** csvManager::loadWizards(const char* filePath, int* count) {
    FILE* file = fopen(filePath, "r");
    if (!file) return nullptr;

    char line[256];
    fgets(line, sizeof(line), file); // Saltar cabecera

    // Contar líneas para reservar memoria
    int lines = 0;
    while (fgets(line, sizeof(line), file)) lines++;

    Wizard** wizards = new Wizard*[lines];
    rewind(file);
    fgets(line, sizeof(line), file); // Saltar cabecera nuevamente

    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        int id, age, idFather, isDead, isOwner;
        char name[MAX_NAME_LENGTH], lastName[MAX_NAME_LENGTH], gender, typeMagic[MAX_MAGIC_TYPE_LENGTH];

        sscanf(line, "%d,%[^,],%[^,],%c,%d,%d,%d,%[^,],%d",
               &id, name, lastName, &gender, &age, &idFather, 
               &isDead, typeMagic, &isOwner);

        wizards[index++] = new Wizard(id, name, lastName, gender, age, 
                                     idFather, isDead, typeMagic, isOwner);
    }

    fclose(file);
    *count = index;
    return wizards;
}

void csvManager::freeWizardArray(Wizard** wizards, int count) {
    for (int i = 0; i < count; ++i) {
        delete wizards[i];
    }
    delete[] wizards;
}