#include "SpellManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring> // For strcspn

SpellManager::SpellManager() : spellCount(0) {}

void SpellManager::loadSpellsFromCsv(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (!file) {
        std::cout << "No se pudo abrir el archivo de hechizos: " << filePath << std::endl;
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); // Saltamos la primera línea (cabecera)

    spellCount = 0;
    while (fgets(line, sizeof(line), file) && spellCount < MAX_SPELLS_PER_WIZARD * 20) {
        // Limpiamos el final de la línea (saltos de línea)
        line[strcspn(line, "\r\n")] = 0;

        int wizardId;
        char spellName[MAX_SPELL_NAME_LENGTH];

        // Leemos el ID del mago y el nombre del hechizo
        if (sscanf(line, "%d,%[^,]", &wizardId, spellName) == 2) {
            // Limpiamos espacios en blanco innecesarios
            char* namePtr = spellName;

            // Quitamos espacios al inicio del nombre
            while (*namePtr == ' ') namePtr++;

            // Quitamos espacios al final del nombre
            char* end = namePtr + strlen(namePtr) - 1;
            while (end > namePtr && *end == ' ') *end-- = 0;

            // Guardamos el hechizo en nuestra lista
            spells[spellCount].wizardId = wizardId;
            strcpy(spells[spellCount].name, namePtr);
            spellCount++;
        }
    }

    fclose(file);
}

void SpellManager::getSpellsByWizardId(int wizardId, Spell* result, int& count) const {
    count = 0;
    // Buscamos todos los hechizos que pertenecen a este mago
    for (int i = 0; i < spellCount; ++i) {
        if (spells[i].wizardId == wizardId) {
            result[count] = spells[i];
            count++;
        }
    }
}

void SpellManager::printSpellsForWizard(int wizardId) const {
    Spell wizardSpells[MAX_SPELLS_PER_WIZARD];
    int count = 0;
    getSpellsByWizardId(wizardId, wizardSpells, count);

    if (count == 0) {
        std::cout << "- No hay hechizos registrados para este mago\n";
        return;
    }

    // Mostramos cada hechizo que conoce el mago
    for (int i = 0; i < count; ++i) {
        std::cout << "- " << wizardSpells[i].name << "\n";
    }
} 