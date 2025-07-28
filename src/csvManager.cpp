#include "csvManager.hpp"
#include "Wizard.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Wizard** CsvManager::loadWizards(const char* filePath, int* count) {
    FILE* file = fopen(filePath, "r");
    if (!file) return nullptr;

    char line[256];
    fgets(line, sizeof(line), file);

    // Para contar cuantos magos hay en el archivo
    int lines = 0;
    while (fgets(line, sizeof(line), file)) lines++;

    Wizard** wizards = new Wizard*[lines];
    rewind(file);
    fgets(line, sizeof(line), file); 

    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        // Limpiamos el final de la línea (saltos de línea)
        line[strcspn(line, "\r\n")] = 0;

        int id, age, idFather, isDead, isOwner;
        char name[MAX_NAME_LENGTH], lastName[MAX_NAME_LENGTH], gender, typeMagic[MAX_MAGIC_TYPE_LENGTH];

        // para leer cada campo del archivo CSV
        if (sscanf(line, "%d,%[^,],%[^,],%c,%d,%d,%d,%[^,],%d",
                   &id, name, lastName, &gender, &age, &idFather, 
                   &isDead, typeMagic, &isOwner) == 9) {
            
            // para limpiar espacios en blanco innecesarios
            char* namePtr = name;
            char* lastNamePtr = lastName;
            char* typeMagicPtr = typeMagic;
            
            // para quitar espacios al inicio de cada campo
            while (*namePtr == ' ') namePtr++;
            while (*lastNamePtr == ' ') lastNamePtr++;
            while (*typeMagicPtr == ' ') typeMagicPtr++;
            
            // para quitar espacios al final de cada campo
            char* end = namePtr + strlen(namePtr) - 1;
            while (end > namePtr && *end == ' ') *end-- = 0;

            end = lastNamePtr + strlen(lastNamePtr) - 1;
            while (end > lastNamePtr && *end == ' ') *end-- = 0;

            end = typeMagicPtr + strlen(typeMagicPtr) - 1;
            while (end > typeMagicPtr && *end == ' ') *end-- = 0;

            // para crear el mago con los datos limpios
            wizards[index++] = new Wizard(id, namePtr, lastNamePtr, gender, age, 
                                         idFather, isDead, typeMagicPtr, isOwner);
        }
    }

    fclose(file);
    *count = index;
    return wizards;
}