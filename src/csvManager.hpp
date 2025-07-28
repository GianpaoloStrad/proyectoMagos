#pragma once
#include "Wizard.hpp"
class CsvManager {
public:
    static Wizard** loadWizards(const char* filePath, int* count); // Devuelve array dinámico
};