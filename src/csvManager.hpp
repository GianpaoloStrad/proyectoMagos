#pragma once
#include "Wizard.hpp"
class csvManager {
public:
    static Wizard** loadWizards(const char* filePath, int* count); // Devuelve array dinámico
};