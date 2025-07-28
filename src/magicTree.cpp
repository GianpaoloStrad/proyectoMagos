#include "MagicTree.hpp"
#include "csvManager.hpp"
#include "SpellManager.hpp"
#include <iostream>
#include <cstring>

MagicTree::MagicTree() : root(nullptr), spellManager(nullptr) {}

MagicTree::~MagicTree() {
    // Destructor recursivo para liberar todos los nodos
    clearTree(root);
    if (spellManager) {
        delete spellManager;
    }
}

// Función auxiliar recursiva para liberar memoria
void MagicTree::clearTree(Wizard* node) {
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

bool MagicTree::buildFromCSV(const char* filePath) {
    int count = 0;
    Wizard** wizards = csvManager::loadWizards(filePath, &count);
    
    if (!wizards || count == 0) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        if (wizards[i]->idFather == -1) {
            root = wizards[i];
        } else {
            Wizard* parent = findWizardById(root, wizards[i]->idFather);
            if (parent) insertWizard(parent, wizards[i]);
        }
    }

    // No liberar la memoria aquí, ya que los nodos están en el árbol
    // Solo liberar el array de punteros, no los objetos Wizard
    delete[] wizards;
    
    // Verificar y reasignar automáticamente si el dueño actual está muerto
    checkAndReassignOwner();
    
    return true;
}

void MagicTree::loadSpellsFromCSV(const char* filePath) {
    if (!spellManager) {
        spellManager = new SpellManager();
    }
    spellManager->loadSpellsFromCSV(filePath);
}

void MagicTree::checkAndReassignOwner() {
    // Buscar el dueño actual (solo vivos)
    Wizard* currentOwner = findCurrentOwner(root);
    
    if (currentOwner) {
        return; // Hay un dueño vivo, no hacer nada
    }
    
    // Si no hay dueño vivo, buscar cualquier dueño (vivo o muerto)
    Wizard* anyOwner = nullptr;
    for (int i = 1; i <= 20; ++i) { // Buscar en un rango razonable de IDs
        Wizard* wizard = findWizardByIdPublic(i);
        if (wizard && wizard->isOwner) {
            anyOwner = wizard;
            break;
        }
    }
    
    if (anyOwner && anyOwner->isDead) {
        // Si hay un dueño muerto, reasignar
        std::cout << "Detectado dueño muerto: " << anyOwner->name << " " << anyOwner->lastName << std::endl;
        std::cout << "Reasignando hechizo automáticamente..." << std::endl;
        
        // Resetear todos los dueños
        resetAllOwners(root);
        
        // Buscar el mejor sucesor en todo el árbol
        Wizard* successor = findBestSuccessorFromAll();
        
        if (successor) {
            successor->isOwner = true;
            std::cout << "Nuevo dueño automático: " << successor->name << " " << successor->lastName << " (ID: " << successor->id << ")\n";
        } else {
            std::cout << "No se encontró sucesor válido. El hechizo queda sin dueño." << std::endl;
        }
    } else if (!anyOwner) {
        // No hay dueño en el sistema, asignar uno automáticamente
        std::cout << "No hay dueño en el sistema. Asignando nuevo dueño automáticamente..." << std::endl;
        
        // Buscar el mejor sucesor en todo el árbol
        Wizard* successor = findBestSuccessorFromAll();
        
        if (successor) {
            successor->isOwner = true;
            std::cout << "Nuevo dueño asignado: " << successor->name << " " << successor->lastName << " (ID: " << successor->id << ")\n";
        } else {
            std::cout << "No se encontró sucesor válido. El hechizo queda sin dueño." << std::endl;
        }
        
        // Guardar automáticamente los cambios
        saveToCSV("bin/wizards.csv");
        std::cout << "Cambios guardados automáticamente." << std::endl;
    }
}

Wizard* MagicTree::findBestSuccessorFromAll() const {
    // Recolectar todos los magos vivos del árbol
    const int MAX_CANDIDATES = 100;
    Wizard* candidates[MAX_CANDIDATES];
    int count = 0;
    fillWizardArray(root, candidates, count, MAX_CANDIDATES);
    
    // Filtrar solo magos vivos
    Wizard* aliveCandidates[MAX_CANDIDATES];
    int aliveCount = 0;
    for (int i = 0; i < count; ++i) {
        if (!candidates[i]->isDead) {
            aliveCandidates[aliveCount++] = candidates[i];
        }
    }
    
    if (aliveCount == 0) return nullptr;
    
    // Aplicar reglas de prioridad
    // 1. Magia elemental o unique
    Wizard* priority1[MAX_CANDIDATES]; int p1 = 0;
    for (int i = 0; i < aliveCount; ++i) {
        if (strcmp(aliveCandidates[i]->typeMagic, "elemental") == 0 || strcmp(aliveCandidates[i]->typeMagic, "unique") == 0)
            priority1[p1++] = aliveCandidates[i];
    }
    if (p1 > 0) {
        // Bubble sort manual para ordenar por edad descendente
        for (int i = 0; i < p1 - 1; ++i) {
            for (int j = 0; j < p1 - i - 1; ++j) {
                if (priority1[j]->age < priority1[j+1]->age ||
                    (priority1[j]->age == priority1[j+1]->age && priority1[j]->id > priority1[j+1]->id)) {
                    Wizard* tmp = priority1[j];
                    priority1[j] = priority1[j+1];
                    priority1[j+1] = tmp;
                }
            }
        }
        // Si el mejor tiene >70, buscar uno más joven
        if (priority1[0]->age > 70) {
            for (int i = 0; i < p1; ++i) if (priority1[i]->age <= 70) return priority1[i];
        }
        return priority1[0];
    }
    
    // 2. Magia mixed
    Wizard* priority2[MAX_CANDIDATES]; int p2 = 0;
    for (int i = 0; i < aliveCount; ++i) {
        if (strcmp(aliveCandidates[i]->typeMagic, "mixed") == 0)
            priority2[p2++] = aliveCandidates[i];
    }
    if (p2 > 0) {
        // Bubble sort manual para ordenar por edad descendente
        for (int i = 0; i < p2 - 1; ++i) {
            for (int j = 0; j < p2 - i - 1; ++j) {
                if (priority2[j]->age < priority2[j+1]->age ||
                    (priority2[j]->age == priority2[j+1]->age && priority2[j]->id > priority2[j+1]->id)) {
                    Wizard* tmp = priority2[j];
                    priority2[j] = priority2[j+1];
                    priority2[j+1] = tmp;
                }
            }
        }
        if (priority2[0]->age > 70) {
            for (int i = 0; i < p2; ++i) if (priority2[i]->age <= 70) return priority2[i];
        }
        return priority2[0];
    }
    
    // 3. Primer hombre vivo
    Wizard* males[MAX_CANDIDATES]; int m = 0;
    for (int i = 0; i < aliveCount; ++i) {
        if (aliveCandidates[i]->gender == 'H') males[m++] = aliveCandidates[i];
    }
    if (m > 0) {
        // Bubble sort manual para ordenar por edad descendente
        for (int i = 0; i < m - 1; ++i) {
            for (int j = 0; j < m - i - 1; ++j) {
                if (males[j]->age < males[j+1]->age ||
                    (males[j]->age == males[j+1]->age && males[j]->id > males[j+1]->id)) {
                    Wizard* tmp = males[j];
                    males[j] = males[j+1];
                    males[j+1] = tmp;
                }
            }
        }
        if (males[0]->age > 70) {
            for (int i = 0; i < m; ++i) if (males[i]->age <= 70) return males[i];
        }
        return males[0];
    }
    
    // 4. Mujer más joven (regla especial)
    Wizard* females[MAX_CANDIDATES]; int f = 0;
    for (int i = 0; i < aliveCount; ++i) {
        if (aliveCandidates[i]->gender == 'M') females[f++] = aliveCandidates[i];
    }
    if (f > 0) {
        // Ordenar por edad ascendente (más joven primero)
        for (int i = 0; i < f - 1; ++i) {
            for (int j = 0; j < f - i - 1; ++j) {
                if (females[j]->age > females[j+1]->age ||
                    (females[j]->age == females[j+1]->age && females[j]->id > females[j+1]->id)) {
                    Wizard* tmp = females[j];
                    females[j] = females[j+1];
                    females[j+1] = tmp;
                }
            }
        }
        return females[0]; // Retornar la mujer más joven
    }
    
    return nullptr;
}

Wizard* MagicTree::findWizardById(Wizard* node, int id) {
    if (!node) return nullptr;
    if (node->id == id) return node;
    
    Wizard* leftSearch = findWizardById(node->left, id);
    if (leftSearch) return leftSearch;
    
    return findWizardById(node->right, id);
}
void MagicTree::insertWizard(Wizard* parent, Wizard* newWizard) {
    if (!parent->left) {
        parent->left = newWizard;
    } else if (!parent->right) {
        parent->right = newWizard;
    } else {
        // Ambos hijos existen, no se inserta para evitar sobrescribir
        // Puedes imprimir un mensaje de advertencia si lo deseas
        // printf("Advertencia: El mago con id %d ya tiene dos discípulos.\n", parent->id);
    }
}
Wizard* MagicTree::getRoot() const {
    return root;
}

// Busca el mago que actualmente es dueño del hechizo (solo vivos)
Wizard* MagicTree::findCurrentOwner(Wizard* node) const {
    if (!node) return nullptr;
    if (node->isOwner && !node->isDead) return node;
    Wizard* left = findCurrentOwner(node->left);
    if (left) return left;
    return findCurrentOwner(node->right);
}

// Resetea el campo isOwner de todos los magos
void MagicTree::resetAllOwners(Wizard* node) const {
    if (!node) return;
    node->isOwner = false;
    resetAllOwners(node->left);
    resetAllOwners(node->right);
}

// Recolecta todos los descendientes vivos de un nodo (DFS) en un arreglo dinámico manual
void collectDescendants(const Wizard* node, Wizard** arr, int& count, int max, bool onlyAlive = true) {
    if (!node || count >= max) return;
    if ((onlyAlive && !node->isDead) || !onlyAlive) {
        arr[count++] = const_cast<Wizard*>(node);
    }
    collectDescendants(node->left, arr, count, max, onlyAlive);
    collectDescendants(node->right, arr, count, max, onlyAlive);
}

// Bubble sort manual para ordenar por edad descendente y luego por menor ID
void sortCandidates(Wizard** arr, int count) {
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

// Aplica las reglas de prioridad para elegir al mejor sucesor
Wizard* MagicTree::findBestSuccessor(Wizard* node) const {
    if (!node) return nullptr;
    const int MAX_CANDIDATES = 100; // Ajustar según máximo esperado
    Wizard* candidates[MAX_CANDIDATES];
    int count = 0;
    collectDescendants(node->left, candidates, count, MAX_CANDIDATES, true);
    collectDescendants(node->right, candidates, count, MAX_CANDIDATES, true);
    if (count == 0) return nullptr;

    // 1. Magia elemental o unique
    Wizard* priority1[MAX_CANDIDATES]; int p1 = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(candidates[i]->typeMagic, "elemental") == 0 || strcmp(candidates[i]->typeMagic, "unique") == 0)
            priority1[p1++] = candidates[i];
    }
    if (p1 > 0) {
        sortCandidates(priority1, p1);
        // Si el mejor tiene >70, buscar uno más joven
        if (priority1[0]->age > 70) {
            for (int i = 0; i < p1; ++i) if (priority1[i]->age <= 70) return priority1[i];
        }
        return priority1[0];
    }
    // 2. Magia mixed
    Wizard* priority2[MAX_CANDIDATES]; int p2 = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(candidates[i]->typeMagic, "mixed") == 0)
            priority2[p2++] = candidates[i];
    }
    if (p2 > 0) {
        sortCandidates(priority2, p2);
        if (priority2[0]->age > 70) {
            for (int i = 0; i < p2; ++i) if (priority2[i]->age <= 70) return priority2[i];
        }
        return priority2[0];
    }
    // 3. Primer hombre vivo
    Wizard* males[MAX_CANDIDATES]; int m = 0;
    for (int i = 0; i < count; ++i) {
        if (candidates[i]->gender == 'H') males[m++] = candidates[i];
    }
    if (m > 0) {
        sortCandidates(males, m);
        if (males[0]->age > 70) {
            for (int i = 0; i < m; ++i) if (males[i]->age <= 70) return males[i];
        }
        return males[0];
    }
    // 4. Primera mujer viva
    Wizard* females[MAX_CANDIDATES]; int f = 0;
    for (int i = 0; i < count; ++i) {
        if (candidates[i]->gender == 'M') females[f++] = candidates[i];
    }
    if (f > 0) {
        sortCandidates(females, f);
        if (females[0]->age > 70) {
            for (int i = 0; i < f; ++i) if (females[i]->age <= 70) return females[i];
        }
        return females[0];
    }
    return nullptr;
}

// Lógica principal de sucesión
void MagicTree::reassignOwnerOnDeath() {
    Wizard* currentOwner = findCurrentOwner(root);
    if (!currentOwner) {
        std::cout << "No hay dueño actual del hechizo." << std::endl;
        return;
    }
    currentOwner->isDead = true;
    resetAllOwners(root);
    Wizard* successor = findBestSuccessor(currentOwner);
    if (successor) {
        successor->isOwner = true;
        std::cout << "Nuevo dueño del hechizo: " << successor->name << " " << successor->lastName << " (ID: " << successor->id << ")\n";
    } else {
        std::cout << "No hay sucesor válido. El hechizo queda sin dueño." << std::endl;
    }
    
    // Guardar automáticamente los cambios
    saveToCSV("bin/wizards.csv");
    std::cout << "Cambios guardados automáticamente." << std::endl;
}

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

// Llena un arreglo con todos los magos del árbol (orden no relevante para la sucesión)
void MagicTree::fillWizardArray(const Wizard* node, Wizard** arr, int& count, int max) const {
    if (!node || count >= max) return;
    arr[count++] = const_cast<Wizard*>(node);
    fillWizardArray(node->left, arr, count, max);
    fillWizardArray(node->right, arr, count, max);
}

// Simula la línea de sucesión real, imprimiendo el orden correcto
void MagicTree::simulateSuccessionLine(const Wizard* start, bool* deadMask, int maxWizards) const {
    const Wizard* current = start;
    int printed = 0;
    while (true) {
        // Buscar el siguiente sucesor según las reglas, ignorando los marcados como muertos en deadMask
        // Creamos una versión modificada de findBestSuccessor que respeta deadMask
        // ---
        // Recolectar candidatos vivos (no marcados como muertos)
        Wizard* candidates[100];
        int count = 0;
        // Solo descendientes del actual
        if (current->left) fillWizardArray(current->left, candidates, count, 100);
        if (current->right) fillWizardArray(current->right, candidates, count, 100);
        // Filtrar solo vivos y no marcados como muertos
        int filtered = 0;
        Wizard* filteredCandidates[100];
        for (int i = 0; i < count; ++i) {
            if (!candidates[i]->isDead && !deadMask[candidates[i]->id])
                filteredCandidates[filtered++] = candidates[i];
        }
        if (filtered == 0) break;
        // Aplicar reglas de prioridad
        // 1. Magia elemental o unique
        Wizard* priority1[100]; int p1 = 0;
        for (int i = 0; i < filtered; ++i) {
            if (strcmp(filteredCandidates[i]->typeMagic, "elemental") == 0 || strcmp(filteredCandidates[i]->typeMagic, "unique") == 0)
                priority1[p1++] = filteredCandidates[i];
        }
        if (p1 > 0) {
            sortCandidates(priority1, p1);
            // Si el mejor tiene >70, buscar uno más joven
            if (priority1[0]->age > 70) {
                for (int i = 0; i < p1; ++i) if (priority1[i]->age <= 70) {
                    current = priority1[i];
                    break;
                }
                if (current != priority1[0]) current = priority1[0];
            } else {
                current = priority1[0];
            }
        } else {
            // 2. Magia mixed
            Wizard* priority2[100]; int p2 = 0;
            for (int i = 0; i < filtered; ++i) {
                if (strcmp(filteredCandidates[i]->typeMagic, "mixed") == 0)
                    priority2[p2++] = filteredCandidates[i];
            }
            if (p2 > 0) {
                sortCandidates(priority2, p2);
                if (priority2[0]->age > 70) {
                    for (int i = 0; i < p2; ++i) if (priority2[i]->age <= 70) {
                        current = priority2[i];
                        break;
                    }
                    if (current != priority2[0]) current = priority2[0];
                } else {
                    current = priority2[0];
                }
            } else {
                // 3. Primer hombre vivo
                Wizard* males[100]; int m = 0;
                for (int i = 0; i < filtered; ++i) {
                    if (filteredCandidates[i]->gender == 'H') males[m++] = filteredCandidates[i];
                }
                if (m > 0) {
                    sortCandidates(males, m);
                    if (males[0]->age > 70) {
                        for (int i = 0; i < m; ++i) if (males[i]->age <= 70) {
                            current = males[i];
                            break;
                        }
                        if (current != males[0]) current = males[0];
                    } else {
                        current = males[0];
                    }
                } else {
                    // 4. Primera mujer viva
                    Wizard* females[100]; int f = 0;
                    for (int i = 0; i < filtered; ++i) {
                        if (filteredCandidates[i]->gender == 'M') females[f++] = filteredCandidates[i];
                    }
                    if (f > 0) {
                        sortCandidates(females, f);
                        if (females[0]->age > 70) {
                            for (int i = 0; i < f; ++i) if (females[i]->age <= 70) {
                                current = females[i];
                                break;
                            }
                            if (current != females[0]) current = females[0];
                        } else {
                            current = females[0];
                        }
                    } else {
                        break; // No hay más sucesores
                    }
                }
            }
        }
        // Imprimir el sucesor
        std::cout << ++printed << ". " << current->name << " " << current->lastName << " (ID: " << current->id << ")\n";
        // Marcarlo como "muerto" virtualmente
        deadMask[current->id] = true;
    }
}

void MagicTree::printTrueSuccessionLine() const {
    // Obtener todos los magos para saber el máximo ID
    Wizard* all[100]; int count = 0;
    fillWizardArray(root, all, count, 100);
    // Buscar dueño actual
    Wizard* owner = nullptr;
    for (int i = 0; i < count; ++i) {
        if (all[i]->isOwner) { owner = all[i]; break; }
    }
    if (!owner) {
        std::cout << "No hay dueño actual del hechizo.\n";
        return;
    }
    std::cout << "\n--- Línea de sucesión real (según reglas) ---\n";
    bool deadMask[100] = {0};
    deadMask[owner->id] = true; // El dueño actual no está en la línea de sucesión
    simulateSuccessionLine(owner, deadMask, 100);
}

// Búsqueda pública por ID
Wizard* MagicTree::findWizardByIdPublic(int id) {
    return findWizardById(root, id);
}

// Editar datos permitidos de un mago
void MagicTree::editWizardData(Wizard* wizard) {
    if (!wizard) {
        std::cout << "Mago no encontrado." << std::endl;
        return;
    }
    std::cout << "\nEditando mago: " << wizard->name << " " << wizard->lastName << " (ID: " << wizard->id << ")\n";
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
    
    // Guardar automáticamente los cambios
    saveToCSV("bin/wizards.csv");
    std::cout << "Cambios guardados automáticamente." << std::endl;
}

// Función auxiliar para guardar un mago y sus descendientes en el archivo CSV
void saveWizardToCSV(const Wizard* node, FILE* file) {
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
    saveWizardToCSV(node->left, file);
    saveWizardToCSV(node->right, file);
}

void MagicTree::saveToCSV(const char* filePath) const {
    FILE* file = fopen(filePath, "w");
    if (!file) {
        std::cout << "No se pudo abrir el archivo para guardar." << std::endl;
        return;
    }
    // Escribir cabecera
    fprintf(file, "id,name,last_name,gender,age,id_father,is_dead,type_magic,is_owner\n");
    saveWizardToCSV(root, file);
    fclose(file);
    std::cout << "\nÁrbol guardado exitosamente en el archivo CSV.\n";
}

// Mostrar datos completos de un mago específico
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
        
        // Mostrar información de discípulos
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
        
        // Mostrar hechizos
        std::cout << "\n--- Hechizos Conocidos ---" << std::endl;
        if (spellManager) {
            spellManager->printSpellsForWizard(wizard->id);
        } else {
            std::cout << "- No hay información de hechizos disponible" << std::endl;
        }
        
        // Mostrar hechizos legendarios si es dueño
        if (wizard->isOwner) {
            std::cout << "\n*** HECHIZOS LEGENDARIOS (DUEÑO DEL HECHIZO) ***" << std::endl;
            std::cout << "- Hechizo de Visión de Sucesión" << std::endl;
            std::cout << "- Control Total de la Escuela" << std::endl;
            std::cout << "- Acceso a Todos los Conocimientos" << std::endl;
        }
        
        std::cout << "\n";
    } else {
        std::cout << "Mago con ID " << wizardId << " no encontrado." << std::endl;
    }
}

Wizard* MagicTree::getCurrentOwner() const {
    return findCurrentOwner(root);
}