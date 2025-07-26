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
        // Limpiar el final de la línea
        line[strcspn(line, "\r\n")] = 0;
        
        int id, age, idFather, isDead, isOwner;
        char name[MAX_NAME_LENGTH], lastName[MAX_NAME_LENGTH], gender, typeMagic[MAX_MAGIC_TYPE_LENGTH];

        // Usar un formato más robusto para el parsing
        if (sscanf(line, "%d,%[^,],%[^,],%c,%d,%d,%d,%[^,],%d",
                   &id, name, lastName, &gender, &age, &idFather, 
                   &isDead, typeMagic, &isOwner) == 9) {
            
            // Limpiar espacios en blanco
            char* namePtr = name;
            char* lastNamePtr = lastName;
            char* typeMagicPtr = typeMagic;
            
            // Eliminar espacios al inicio
            while (*namePtr == ' ') namePtr++;
            while (*lastNamePtr == ' ') lastNamePtr++;
            while (*typeMagicPtr == ' ') typeMagicPtr++;
            
            // Eliminar espacios al final
            char* end = namePtr + strlen(namePtr) - 1;
            while (end > namePtr && *end == ' ') *end-- = 0;
            
            end = lastNamePtr + strlen(lastNamePtr) - 1;
            while (end > lastNamePtr && *end == ' ') *end-- = 0;
            
            end = typeMagicPtr + strlen(typeMagicPtr) - 1;
            while (end > typeMagicPtr && *end == ' ') *end-- = 0;

            wizards[index++] = new Wizard(id, namePtr, lastNamePtr, gender, age, 
                                         idFather, isDead, typeMagicPtr, isOwner);
        }
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