#pragma once
#include <cstring>

#define MAX_SPELL_NAME_LENGTH 50
#define MAX_SPELLS_PER_WIZARD 20

struct Spell {
    char name[MAX_SPELL_NAME_LENGTH];
    int wizardId;
};

class SpellManager {
private:
    Spell spells[MAX_SPELLS_PER_WIZARD * 20]; // 20 magos máximo
    int spellCount;

public:
    SpellManager();
    void loadSpellsFromCsv(const char* filePath);
    void getSpellsByWizardId(int wizardId, Spell* result, int& count) const;
    void printSpellsForWizard(int wizardId) const;
}; 