#include "MagicTree.hpp"
#include "csvManager.hpp"
#include <iostream>
#include <cstring>

MagicTree::MagicTree() : root(nullptr) {}

MagicTree::~MagicTree() {
    // Destructor recursivo para liberar todos los nodos
    clearTree(root);
}

// Función auxiliar recursiva para liberar memoria
void MagicTree::clearTree(Wizard* node) {
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

void MagicTree::buildFromCSV(const char* filePath) {
    int count = 0;
    Wizard** wizards = csvManager::loadWizards(filePath, &count);

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

// Busca el mago que actualmente es dueño del hechizo
Wizard* MagicTree::findCurrentOwner(Wizard* node) const {
    if (!node) return nullptr;
    if (node->isOwner) return node;
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

// Mostrar hechizos de un mago específico
void MagicTree::showWizardSpells(int wizardId) {
    Wizard* wizard = findWizardByIdPublic(wizardId);
    if (wizard) {
        wizard->showSpells();
    } else {
        std::cout << "Mago con ID " << wizardId << " no encontrado." << std::endl;
    }
}