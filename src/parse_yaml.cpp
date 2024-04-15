// #include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>


#include "parse_yaml.h"


// Déclarations des fonctions
std::vector<Option> createOptionsObject(std::map<std::string, std::string> options);
std::vector<BlackFile> createDefaultRulesObject(std::map<std::string, std::vector<std::string>> default_rules);
std::vector<std::string> get_file_with_type_name(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name);
std::vector<std::string> create_special_rules_object(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name);
std::vector<Function> createFunctionObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<TypeDef> createTypeDefObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<Global> createGlobalObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<Structure> createStructureObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);

// Définitions des fonctions

std::vector<Option> createOptionsObject(std::map<std::string, std::string> options) {
    std::vector<Option> list_options;
    if (!options.empty()) {
        for (const auto& option : options) {
            list_options.push_back(Option{option.first, option.second});
        }
    } else {
        std::cout << "Aucune option dans cette configuration" << std::endl;
    }
    return list_options;
}

std::vector<BlackFile> createDefaultRulesObject(std::map<std::string, std::vector<std::string>> default_rules) {
    std::vector<BlackFile> list_default_rules;
    if (default_rules.find("files") != default_rules.end() && default_rules.find("protection") != default_rules.end()) {
        std::vector<std::string> files = default_rules["files"];
        std::vector<std::string> protections = default_rules["protection"];
        for (const auto& file : files) {
            for (const auto& protection : protections) {
                list_default_rules.push_back(BlackFile{file, protection, 0});
            }
        }
    } else {
        std::cout << "Les règles par défaut ne contiennent pas les clés requises : files, protection" << std::endl;
    }
    return list_default_rules;
}

std::vector<std::string> get_file_with_type_name(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name) {
    std::vector<std::string> file_names;
    for (const auto& file_rule : special_rules) {
        if (file_rule.second.find(type_name) != file_rule.second.end()) {
            file_names.push_back(file_rule.first);
        }
    }
    return file_names;
}

std::vector<std::string> create_special_rules_object(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name) {
    std::vector<std::string> list_protection;
    std::vector<std::string> files_with_type = get_file_with_type_name(special_rules, type_name);
    for (const auto& file : files_with_type) {
        if (type_name == "Function") {
            std::vector<Function> functions = createFunctionObject(special_rules, file);
            for (const auto& func : functions) {
                list_protection.push_back(func.functionName);
            }
        } else if (type_name == "Global") {
            std::vector<Global> globals = createGlobalObject(special_rules, file);
            for (const auto& global : globals) {
                list_protection.push_back(global.globalName);
            }
        } else if (type_name == "Structure") {
            std::vector<Structure> structures = createStructureObject(special_rules, file);
            for (const auto& structure : structures) {
                list_protection.push_back(structure.structureName);
            }
        } else if (type_name == "TypeDef") {
            std::vector<TypeDef> typedefs = createTypeDefObject(special_rules, file);
            for (const auto& typedef_ : typedefs) {
                list_protection.push_back(typedef_.typedefName);
            }
        }
    }
    return list_protection;
}

std::vector<Function> createFunctionObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name) {
    std::vector<Function> list_functions;
    for (const auto& rule : special_rules_file[file_name]) {
        if (rule.first == "Function") {
            for (const auto& func : rule.second) {
                list_functions.push_back(Function{file_name, func, {}});
            }
        }
    }
    return list_functions;
}

std::vector<TypeDef> createTypeDefObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name) {
    std::vector<TypeDef> list_typedefs;
    for (const auto& rule : special_rules_file[file_name]) {
        if (rule.first == "TypeDef") {
            for (const auto& typedef_ : rule.second) {
                list_typedefs.push_back(TypeDef{file_name, typedef_, {}});
            }
        }
    }
    return list_typedefs;
}

std::vector<Global> createGlobalObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name) {
    std::vector<Global> list_globals;
    for (const auto& rule : special_rules_file[file_name]) {
        if (rule.first == "Global") {
            for (const auto& global : rule.second) {
                list_globals.push_back(Global{file_name, global, {}});
            }
        }
    }
    return list_globals;
}

std::vector<Structure> createStructureObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name) {
    std::vector<Structure> list_structures;
    for (const auto& rule : special_rules_file[file_name]) {
        if (rule.first == "Structure") {
            for (const auto& structure : rule.second) {
                list_structures.push_back(Structure{file_name, structure, {}});
            }
        }
    }
    return list_structures;
}




int main() {
    // Test de createOptionsObject
    std::map<std::string, std::string> options = {{"option1", "value1"}, {"option2", "value2"}};
    std::vector<Option> optionsObjects = createOptionsObject(options);
    assert(optionsObjects.size() == 2); // Vérifie si deux objets ont été créés

    // Test de createDefaultRulesObject
    std::map<std::string, std::vector<std::string>> defaultRules = {{"files", {"file1", "file2"}}, {"protection", {"read", "write"}}};
    std::vector<BlackFile> defaultRulesObjects = createDefaultRulesObject(defaultRules);
    assert(defaultRulesObjects.size() == 4); // Vérifie si quatre objets ont été créés (2 fichiers x 2 protections)

    // Test de get_file_with_type_name et create_special_rules_object
    std::map<std::string, std::map<std::string, std::vector<std::string>>> specialRules = {
        {"file1", {{"Function", {"func1", "func2"}}, {"TypeDef", {"type1"}}}},
        {"file2", {{"Global", {"global1", "global2"}}, {"Structure", {"struct1", "struct2"}}}}
    };

    // Test de get_file_with_type_name
    std::vector<std::string> filesWithFunction = get_file_with_type_name(specialRules, "Function");
    assert(filesWithFunction.size() == 1); // Vérifie si un fichier contenant des fonctions a été trouvé

    std::vector<std::string> filesWithGlobal = get_file_with_type_name(specialRules, "Global");
    assert(filesWithGlobal.size() == 1); // Vérifie si un fichier contenant des variables globales a été trouvé

    // Test de create_special_rules_object
    std::vector<std::string> functionObjects = create_special_rules_object(specialRules, "Function");
    assert(functionObjects.size() == 2); // Vérifie si deux fonctions ont été créées

    std::vector<std::string> globalObjects = create_special_rules_object(specialRules, "Global");
    assert(globalObjects.size() == 2); // Vérifie si deux variables globales ont été créées

    std::vector<std::string> structureObjects = create_special_rules_object(specialRules, "Structure");
    assert(structureObjects.size() == 2); // Vérifie si deux structures ont été créées

    std::vector<std::string> typeDefObjects = create_special_rules_object(specialRules, "TypeDef");
    assert(typeDefObjects.size() == 1); // Vérifie si un typedef a été créé

    std::cout << "Tous les tests ont réussi !" << std::endl;

    return 0;
}