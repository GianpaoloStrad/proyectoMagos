#include "SpellManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

SpellManager::SpellManager() : spellCount(0) {}

void SpellManager::loadSpellsFromCSV(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (!file) {
        std::cout << "No se pudo abrir el archivo de hechizos: " << filePath << std::endl;
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); // Saltar cabecera

    spellCount = 0;
    while (fgets(line, sizeof(line), file) && spellCount < MAX_SPELLS_PER_WIZARD * 20) {
        // Limpiar el final de la línea
        line[strcspn(line, "\r\n")] = 0;
        
        int wizardId;
        char spellName[MAX_SPELL_NAME_LENGTH];
        
        if (sscanf(line, "%d,%[^,]", &wizardId, spellName) == 2) {
            // Limpiar espacios en blanco
            char* namePtr = spellName;
            
            // Eliminar espacios al inicio
            while (*namePtr == ' ') namePtr++;
            
            // Eliminar espacios al final
            char* end = namePtr + strlen(namePtr) - 1;
            while (end > namePtr && *end == ' ') *end-- = 0;

            spells[spellCount].wizardId = wizardId;
            strcpy(spells[spellCount].name, namePtr);
            spellCount++;
        }
    }

    fclose(file);
}

void SpellManager::getSpellsByWizardId(int wizardId, Spell* result, int& count) const {
    count = 0;
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
    
    for (int i = 0; i < count; ++i) {
        std::cout << "- " << wizardSpells[i].name << "\n";
    }
} 