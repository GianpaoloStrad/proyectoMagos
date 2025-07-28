#include "MagicTree.hpp"
#include "csvManager.hpp"
#include "SpellManager.hpp"
#include <iostream>
#include <cstring>

MagicTree::MagicTree() : root(nullptr), spellManager(nullptr) {}

MagicTree::~MagicTree() {
    // Aqui estamos liberando la memoria de los magos 
    clearTree(root);
    if (spellManager) {
        delete spellManager;
    }
}

// para recorrer el árbol liberando cada mago de la memoria
void MagicTree::clearTree(Wizard* node) {
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

bool MagicTree::buildFromCsv(const char* filePath) {
    int count = 0;
    Wizard** wizards = CsvManager::loadWizards(filePath, &count);
    
    if (!wizards || count == 0) {
        return false;
    }

    // Construimos el árbol conectando cada mago con su maestro
    for (int i = 0; i < count; ++i) {
        if (wizards[i]->idFather == -1) {
            root = wizards[i]; // Este es el mago raíz (sin maestro)
        } else {
            Wizard* parent = findWizardById(root, wizards[i]->idFather);
            if (parent) insertWizard(parent, wizards[i]);
        }
    }

    // Liberamos el array temporal, pero mantenemos los magos en el árbol
    delete[] wizards;
    
    // Verificamos si necesitamos asignar un dueño al hechizo
    checkAndReassignOwner();
    
    return true;
}

void MagicTree::loadSpellsFromCsv(const char* filePath) {
    if (!spellManager) {
        spellManager = new SpellManager();
    }
    spellManager->loadSpellsFromCsv(filePath);
}

void MagicTree::checkAndReassignOwner() {
    // Buscamos si hay un dueño vivo del hechizo
    Wizard* currentOwner = findCurrentOwner(root);
    
    if (currentOwner) {
        return; // Ya hay un dueño vivo, todo bien :D
    }
    
    // Si no hay dueño vivo, buscamos cualquier dueño (vivo o muerto)
    Wizard* anyOwner = nullptr;
    for (int i = 1; i <= 20; ++i) { // Revisamos los primeros 20 magos
        Wizard* wizard = findWizardByIdPublic(i);
        if (wizard && wizard->isOwner) {
            anyOwner = wizard;
            break;
        }
    }
    
    if (anyOwner && anyOwner->isDead) {
        // Hay un dueño muerto, necesitamos reasignar el hechizo
        std::cout << "No hay dueño en el sistema. Asignando nuevo dueño automáticamente..." << std::endl;
        
        // Quitamos el estado de dueño de todos los magos
        resetAllOwners(root);
        
        // Buscamos el mejor candidato para ser el nuevo dueño
        Wizard* successor = findBestSuccessorFromAll();
        
        if (successor) {
            successor->isOwner = true;
            std::cout << "Nuevo dueño asignado: " << successor->name << " " << successor->lastName << " (ID: " << successor->id << ")\n";
        } else {
            std::cout << "No se encontró sucesor válido. El hechizo queda sin dueño." << std::endl;
        }
    }
}

// Función común para ordenar candidatos por edad (mayor primero) y luego por ID menor
void sortCandidatesByAge(Wizard** arr, int count) {
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - i - 1; ++j) {
            if (arr[j]->age < arr[j+1]->age ||
                (arr[j]->age == arr[j+1]->age && arr[j]->id > arr[j+1]->id)) {
                Wizard* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

// Función común para ordenar candidatos por edad (más joven primero) y luego por ID menor
void sortCandidatesByAgeYoungest(Wizard** arr, int count) {
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - i - 1; ++j) {
            if (arr[j]->age > arr[j+1]->age ||
                (arr[j]->age == arr[j+1]->age && arr[j]->id > arr[j+1]->id)) {
                Wizard* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

// Función común para aplicar las reglas de prioridad
Wizard* applyPriorityRules(Wizard** candidates, int count) {
    if (count == 0) return nullptr;
    
    const int MAX_CANDIDATES = 100;
    
    // Primera prioridad: Magos con magia elemental o única
    Wizard* priority1[MAX_CANDIDATES]; int p1 = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(candidates[i]->typeMagic, "elemental") == 0 || strcmp(candidates[i]->typeMagic, "unique") == 0)
            priority1[p1++] = candidates[i];
    }
    if (p1 > 0) {
        sortCandidatesByAge(priority1, p1);
        // Si el mejor candidato tiene más de 70 años, buscamos uno más joven
        if (priority1[0]->age > 70) {
            for (int i = 0; i < p1; ++i) if (priority1[i]->age <= 70) return priority1[i];
        }
        return priority1[0];
    }
    
    // Segunda prioridad: Magos con magia mixta
    Wizard* priority2[MAX_CANDIDATES]; int p2 = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(candidates[i]->typeMagic, "mixed") == 0)
            priority2[p2++] = candidates[i];
    }
    if (p2 > 0) {
        sortCandidatesByAge(priority2, p2);
        if (priority2[0]->age > 70) {
            for (int i = 0; i < p2; ++i) if (priority2[i]->age <= 70) return priority2[i];
        }
        return priority2[0];
    }
    
    // Tercera prioridad: Hombres vivos
    Wizard* males[MAX_CANDIDATES]; int m = 0;
    for (int i = 0; i < count; ++i) {
        if (candidates[i]->gender == 'H') males[m++] = candidates[i];
    }
    if (m > 0) {
        sortCandidatesByAge(males, m);
        if (males[0]->age > 70) {
            for (int i = 0; i < m; ++i) if (males[i]->age <= 70) return males[i];
        }
        return males[0];
    }
    
    // Cuarta prioridad: Mujeres (regla especial preferimos las más jóvenes)
    Wizard* females[MAX_CANDIDATES]; int f = 0;
    for (int i = 0; i < count; ++i) {
        if (candidates[i]->gender == 'M') females[f++] = candidates[i];
    }
    if (f > 0) {
        sortCandidatesByAgeYoungest(females, f);
        return females[0]; // Retornamos la mujer más joven
    }
    
    return nullptr;
}

Wizard* MagicTree::findBestSuccessorFromAll() const {
    // Recolectamos todos los magos vivos del árbol
    const int MAX_CANDIDATES = 100;
    Wizard* candidates[MAX_CANDIDATES];
    int count = 0;
    fillWizardArray(root, candidates, count, MAX_CANDIDATES);
    
    // Filtramos solo los magos que están vivos
    Wizard* aliveCandidates[MAX_CANDIDATES];
    int aliveCount = 0;
    for (int i = 0; i < count; ++i) {
        if (!candidates[i]->isDead) {
            aliveCandidates[aliveCount++] = candidates[i];
        }
    }
    
    return applyPriorityRules(aliveCandidates, aliveCount);
}

Wizard* MagicTree::findWizardById(Wizard* node, int id) {
    if (!node) return nullptr;
    if (node->id == id) return node;
    
    // Buscamos en el discípulo izquierdo
    Wizard* leftSearch = findWizardById(node->left, id);
    if (leftSearch) return leftSearch;
    
    // Buscamos en el discípulo derecho
    return findWizardById(node->right, id);
}

void MagicTree::insertWizard(Wizard* parent, Wizard* newWizard) {
    if (!parent->left) {
        parent->left = newWizard; // Asignamos como discípulo izquierdo
    } else if (!parent->right) {
        parent->right = newWizard; // Asignamos como discípulo derecho
    } else {
        // El maestro ya tiene dos discípulos, no podemos agregar más
    }
}

Wizard* MagicTree::getRoot() const {
    return root;
}

// Buscamos al mago que actualmente posee el hechizo legendario (solo vivos)
Wizard* MagicTree::findCurrentOwner(Wizard* node) const {
    if (!node) return nullptr;
    if (node->isOwner && !node->isDead) return node;
    
    // Buscamos en el discípulo izquierdo
    Wizard* left = findCurrentOwner(node->left);
    if (left) return left;
    
    // Buscamos en el discípulo derecho
    return findCurrentOwner(node->right);
}

// Quitamos el estado de dueño de todos los magos
void MagicTree::resetAllOwners(Wizard* node) const {
    if (!node) return;
    node->isOwner = false;
    resetAllOwners(node->left);
    resetAllOwners(node->right);
}

// Recolectamos todos los descendientes vivos de un mago
void collectDescendants(const Wizard* node, Wizard** arr, int& count, int max, bool onlyAlive = true) {
    if (!node || count >= max) return;
    if ((onlyAlive && !node->isDead) || !onlyAlive) {
        arr[count++] = const_cast<Wizard*>(node);
    }
    collectDescendants(node->left, arr, count, max, onlyAlive);
    collectDescendants(node->right, arr, count, max, onlyAlive);
}

// Aplicamos las reglas de sucesión para elegir al mejor candidato
Wizard* MagicTree::findBestSuccessor(Wizard* node) const {
    if (!node) return nullptr;
    const int MAX_CANDIDATES = 100;
    Wizard* candidates[MAX_CANDIDATES];
    int count = 0;
    collectDescendants(node->left, candidates, count, MAX_CANDIDATES, true);
    collectDescendants(node->right, candidates, count, MAX_CANDIDATES, true);
    
    return applyPriorityRules(candidates, count);
}

// Simulamos la muerte del dueño actual y reasignamos el hechizo
void MagicTree::reassignOwnerOnDeath() {
    Wizard* currentOwner = findCurrentOwner(root);
    if (!currentOwner) {
        std::cout << "No hay dueño actual del hechizo." << std::endl;
        return;
    }
    
    // Marcamos al dueño actual como muerto
    currentOwner->isDead = true;
    
    // Quitamos el estado de dueño de todos los magos
    resetAllOwners(root);
    
    // Buscamos al mejor sucesor según las reglas
    Wizard* successor = findBestSuccessor(currentOwner);
    if (successor) {
        successor->isOwner = true;
        std::cout << "Nuevo dueño del hechizo: " << successor->name << " " << successor->lastName << " (ID: " << successor->id << ")" << std::endl;
    } else {
        std::cout << "No hay sucesor válido. El hechizo queda sin dueño." << std::endl;
    }
}

// Función auxiliar para mostrar la línea de sucesión
void printSuccessionLineHelper(const Wizard* node) {
    if (!node) return;
    if (!node->isDead) {
        std::cout << node->name << " " << node->lastName << " (ID: " << node->id << ")\n";
    }
    printSuccessionLineHelper(node->left);
    printSuccessionLineHelper(node->right);
}

void MagicTree::printSuccessionLine() const {
    std::cout << "\n--- Línea de sucesión (magos vivos) ---\n";
    printSuccessionLineHelper(root);
}

// Llenamos un arreglo con todos los magos del árbol
void MagicTree::fillWizardArray(const Wizard* node, Wizard** arr, int& count, int max) const {
    if (!node || count >= max) return;
    arr[count++] = const_cast<Wizard*>(node);
    fillWizardArray(node->left, arr, count, max);
    fillWizardArray(node->right, arr, count, max);
}

// Búsqueda pública por ID para que otros módulos puedan encontrar magos
Wizard* MagicTree::findWizardByIdPublic(int id) {
    return findWizardById(root, id);
}

// Permitimos editar los datos de un mago (excepto ID y padre)
void MagicTree::editWizardData(Wizard* wizard) {
    if (!wizard) {
        std::cout << "Mago no encontrado." << std::endl;
        return;
    }
    std::cout << "\nEditando mago: " << wizard->name << " " << wizard->lastName << std::endl;
    std::cout << "(Dejar vacío y presionar Enter para no modificar un campo)\n";
    char buffer[100];
    
    // Nombre
    std::cout << "Nombre actual: " << wizard->name << "\nNuevo nombre: ";
    std::cin.ignore();
    std::cin.getline(buffer, 100);
    if (strlen(buffer) > 0) strncpy(wizard->name, buffer, MAX_NAME_LENGTH);
    
    // Apellido
    std::cout << "Apellido actual: " << wizard->lastName << "\nNuevo apellido: ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) > 0) strncpy(wizard->lastName, buffer, MAX_NAME_LENGTH);
    
    // Género
    std::cout << "Género actual (H/M): " << wizard->gender << "\nNuevo género (H/M): ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) == 1 && (buffer[0] == 'H' || buffer[0] == 'M')) wizard->gender = buffer[0];
    
    // Edad
    std::cout << "Edad actual: " << wizard->age << "\nNueva edad: ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) > 0) {
        int newAge = atoi(buffer);
        if (newAge > 0) wizard->age = newAge;
    }
    
    // Estado de vida/muerte
    std::cout << "¿Está muerto? (0 = No, 1 = Sí) Actual: " << wizard->isDead << "\nNuevo valor: ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) == 1 && (buffer[0] == '0' || buffer[0] == '1')) wizard->isDead = (buffer[0] == '1');
    
    // Tipo de magia
    std::cout << "Tipo de magia actual: " << wizard->typeMagic << "\nNuevo tipo de magia (elemental, unique, mixed, no_magic): ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) > 0) strncpy(wizard->typeMagic, buffer, MAX_MAGIC_TYPE_LENGTH);
    
    // Dueño del hechizo
    std::cout << "¿Es dueño del hechizo? (0 = No, 1 = Sí) Actual: " << wizard->isOwner << "\nNuevo valor: ";
    std::cin.getline(buffer, 100);
    if (strlen(buffer) == 1 && (buffer[0] == '0' || buffer[0] == '1')) wizard->isOwner = (buffer[0] == '1');
    
    std::cout << "\nDatos actualizados.\n";
}

// Guardamos un mago y todos sus descendientes en el archivo CSV
void saveWizardToCsv(const Wizard* node, FILE* file) {
    if (!node) return;
    fprintf(file, "%d,%s,%s,%c,%d,%d,%d,%s,%d\n",
        node->id,
        node->name,
        node->lastName,
        node->gender,
        node->age,
        node->idFather,
        node->isDead ? 1 : 0,
        node->typeMagic,
        node->isOwner ? 1 : 0
    );
    saveWizardToCsv(node->left, file);
    saveWizardToCsv(node->right, file);
}

void MagicTree::saveToCsv(const char* filePath) const {
    if (!root) {
        std::cout << "Error: No se puede guardar el árbol (árbol vacío)." << std::endl;
        return;
    }
    FILE* file = fopen(filePath, "w");
    if (!file) {
        std::cout << "No se pudo abrir el archivo para guardar: " << filePath << std::endl;
        return;
    }
    
    // Escribimos la cabecera del archivo CSV
    fprintf(file, "id,name,last_name,gender,age,id_father,is_dead,type_magic,is_owner\n");
    saveWizardToCsv(root, file);
    fclose(file);
    std::cout << "Árbol guardado exitosamente en el archivo CSV." << std::endl;
}

// Mostramos todos los datos completos de un mago específico
void MagicTree::showWizardCompleteData(int wizardId) {
    Wizard* wizard = findWizardByIdPublic(wizardId);
    if (wizard) {
        std::cout << "\n=== DATOS COMPLETOS DEL MAGO ===" << std::endl;
        std::cout << "ID: " << wizard->id << std::endl;
        std::cout << "Nombre: " << wizard->name << " " << wizard->lastName << std::endl;
        std::cout << "Género: " << (wizard->gender == 'H' ? "Hombre" : "Mujer") << std::endl;
        std::cout << "Edad: " << wizard->age << " años" << std::endl;
        std::cout << "ID del Padre/Maestro: " << (wizard->idFather == -1 ? "Ninguno (Mago Raíz)" : std::to_string(wizard->idFather)) << std::endl;
        std::cout << "Estado: " << (wizard->isDead ? "Muerto" : "Vivo") << std::endl;
        std::cout << "Tipo de Magia: " << wizard->typeMagic << std::endl;
        std::cout << "Es Dueño del Hechizo: " << (wizard->isOwner ? "Sí" : "No") << std::endl;
        
        // Mostramos información de los discípulos
        std::cout << "\n--- Discípulos ---" << std::endl;
        if (wizard->left) {
            std::cout << "Discípulo Izquierdo: " << wizard->left->name << " " << wizard->left->lastName << " (ID: " << wizard->left->id << ")" << std::endl;
        } else {
            std::cout << "Discípulo Izquierdo: Ninguno" << std::endl;
        }
        if (wizard->right) {
            std::cout << "Discípulo Derecho: " << wizard->right->name << " " << wizard->right->lastName << " (ID: " << wizard->right->id << ")" << std::endl;
        } else {
            std::cout << "Discípulo Derecho: Ninguno" << std::endl;
        }
        
        // Mostramos los hechizos que conoce
        std::cout << "\n--- Hechizos Conocidos ---" << std::endl;
        if (spellManager) {
            spellManager->printSpellsForWizard(wizard->id);
        } else {
            std::cout << "- No hay información de hechizos disponible" << std::endl;
        }
        
        // Si es dueño del hechizo, mostramos los hechizos legendarios
        if (wizard->isOwner) {
            std::cout << "\n*** HECHIZOS LEGENDARIOS (DUEÑO DEL HECHIZO) ***" << std::endl;
            std::cout << "- Hechizo de Visión de Sucesión" << std::endl;
            std::cout << "- Control Total de la Escuela" << std::endl;
            std::cout << "- Acceso a Todos los Conocimientos" << std::endl;
        }
    } else {
        std::cout << "Mago no encontrado." << std::endl;
    }
}

Wizard* MagicTree::getCurrentOwner() const {
    return findCurrentOwner(root);
} 