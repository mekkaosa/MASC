#include <yaml-cpp/yaml.h> 
//#include<llvm/Support/YAMLTraits.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>


#include "parse_yaml.h"
////////////////////////////////////////////////////////////////////////////////
// This part of the code is related to the reading of the configuration file  //
// and the options pass as arguments                                          //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// This part of the code is related to the conversion of the dictionnary to   //
// standarized objects implemented in object_yaml.py                          //
////////////////////////////////////////////////////////////////////////////////

// Déclarations des fonctions
std::vector<Option> createOptionsObject(std::map<std::string, std::string> options);
std::vector<BlackFile> createDefaultRulesObject(std::map<std::string, std::vector<std::string>> default_rules);
std::vector<std::string> get_file_with_type_name(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name);
std::vector<std::string> create_special_rules_object(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules, std::string type_name);
std::vector<Function> createFunctionObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<TypeDef> createTypeDefObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<Global> createGlobalObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);
std::vector<Structure> createStructureObject(std::map<std::string, std::map<std::string, std::vector<std::string>>> special_rules_file, std::string file_name);

// Définition des fonctions
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
                list_protection.push_back(typedef_.name);
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
std::vector<TypeDef> createTypeDefs(const std::map<std::string, std::map<std::string, std::vector<std::string>>> &special_rules_file, const std::string &file_name) {
    std::vector<TypeDef> list_typedefs;
    auto file_rules = special_rules_file.find(file_name);
    if (file_rules != special_rules_file.end()) {
        auto globals = file_rules->second.find("Global");
        if (globals != file_rules->second.end()) {
            for (const auto &global : globals->second) {
                list_typedefs.push_back(TypeDef(file_name, global, {}));
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

////////////////////////////////////////////////////////////////////////////////
// Standardized object can reference to multiple files, types or protections  //
// This part is used to make each object reference to only one entity         //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// In the .yml file, exceptions are written the same way as affirmations. This//
// part of the code aims to convert the exception to affirmation according to //
// the protection specified in the default rules:                             //
// (file_name, function_name, no-protection1) ->                              //
// (file_name, function_name, protection2, protection3)                       //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// This part of the code is used to write the object into the output          //
// configuration file                                                         //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// This part of the code are tools function                                   //
////////////////////////////////////////////////////////////////////////////////
const std::string DEFAULT_RULES = "DefaultRules";
const std::string SPECIAL_RULES = "SpecialRules";

std::string nodeToString(const YAML::Node& node) {
    std::stringstream ss;
    ss << node;
    return ss.str();
}
void print_warning(const std::string& msg) {
    std::cout << "ATTENTION : " << msg << std::endl;
}

void differences_with(const std::string& file1Path, const std::string& file2Path) {
    try {
        YAML::Node file1 = YAML::LoadFile(file1Path);
        YAML::Node file2 = YAML::LoadFile(file2Path);

        for (auto it = file1.begin(); it != file1.end(); ++it) {
            std::string key = it->first.as<std::string>();
            if (file2[key] && nodeToString(file1[key]) != nodeToString(file2[key]))  {
                std::cout << "Difference found in key: " << key << "\n";
                std::cout << "File 1: " << file1[key] << "\n";
                std::cout << "File 2: " << file2[key] << "\n";
            } else if (!file2[key]) {
                std::cout << "Key " << key << " not found in file 2\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing files: " << e.what() << std::endl;
    }
}

//configuration_dict est un dictionnaire qui associe des clés de type TypeDef à des valeurs de type TypeDef
std::vector<TypeDef> check_null(std::vector<TypeDef>& list_of_type_name)  {
    std::vector<std::pair<TypeDef, int>> null_protection_objects;
    std::vector<int> tab;
    int i = 0;
    // là on itere dans les type mane de list of type name et verifie si la protection est null et ajoute le TypeDef et son indice au tableau null_protection
    for (auto& obj : list_of_type_name) {
       if (obj.protection.empty()) {
            null_protection_objects.push_back(std::make_pair(obj, i));
        }
        i++;
    }
    // une autre iteration mais cette fois pour verifier sil s'agit du meme fichier , mm nom mais dune protection different ; on ajoute l'indice au deuxieme tableau : tab
    for (auto& obj : list_of_type_name) {
        for (auto& null_obj : null_protection_objects) {
            std::vector<bool> differences = obj.differences_with(null_obj.first);
            if (differences[0] && differences[1] &&!differences[2]) {
                tab.push_back(null_obj.second);
            }
        }
    }
    // trier tab 
    std::sort(tab.begin(), tab.end());
    //supprimer du list of type name les elements type name dindice dans tab 
    for (int i = tab.size() - 1; i >= 0; i--) {
        list_of_type_name.erase(list_of_type_name.begin() + tab[i]);
    }
    // removes them if they have the same name and file as another TypeDef object with a non-null protection field.
    return list_of_type_name;
} 

// un check pour s'assurer de la coherence de declaration  d'objet .
void check_coherence(const std::vector<TypeDef>& list_of_type_name) {

    for (const auto& obj : list_of_type_name) {
        for (const auto& obj_cmp : list_of_type_name) {
            auto differences = obj.differences_with(obj_cmp);
            if (differences == std::vector<bool>{true, false, false}) {
                print_warning("The object " + obj.name + " has different declaration in the file "
                              + obj.file + " and " + obj_cmp.file);
            }
        }
    }
}
// Ici la traduction du python en C++ message warning 
// retour dun booleen 
 std::vector<bool> check_default_special(const std::map<TypeDef, TypeDef>& configuration_dict) {
    std::vector<bool> defined = {true, true};
    for (auto& obj: configuration_dict) {
        //Dans cet exemple, la fonction find du dictionnaire configuration_dict est utilisée pour rechercher la clé DEFAULT_RULES. Si la clé n'est pas trouvée, la fonction find retourne un itérateur pointant vers end(), qui est la fin du dictionnaire.
        if (configuration_dict.find(TypeDef(DEFAULT_RULES)) == configuration_dict.end()){
            std::cout<<"No default_rules in this configuration"<<std::endl;
            defined[0]=false;
        }
        if(configuration_dict.find(TypeDef(SPECIAL_RULES)) == configuration_dict.end()){
            std::cout<<"No special_rules in this configuration"<<std::endl;
            defined[1]=false;
        }
    
    }
    return defined ;
   
}

void print_error(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}
/*
Yaml::Node check_is_none(Yaml::Node element,  const std::string &attr){
    if(!element ){
        if(attr==NULL){
            print_error("YAML: yaml attribute is empty: " + str(attr));
        }
    }else{
        return  element;
    }
}
std::vector<TypeDef> check_protection(std::vector<TypeDef> & list ,  const std:: string  & type){
    // liste de retour 
    std::vector<TypeDef> lreturn ;
    int k =0;
    //une référence à un élément de la liste list
    for (auto& i : list ){
        // verfie si l'element de la liste a une protection 
        // il cherchera ce attribut dans la liste des protection_key_whitelist
        if(type=="PROTECTION" && i!= "NULL" && i!= "NO-PROTECTION" && std::find(protection_key_whitelist.begin(), protection_key_whitelist.end(), i) == protection_key_whitelist.end()) {
            print_warning( " not an accepted protection in the default_rules")
            lreturn.push_back(k);
        }
        k++;
    }
    for (int i : lreturn) {
        list.erase(list.begin() + i);
    }
    // le message d'erreur ds le cas ou tout les attribut de type protection sont ds le whitelist 
    if(list.size()==0){
        print_error(" do not contain any accepted attribute")
    }
    return list ; 

}
std::vector<std:: string > split_coma(std::vector<std:: string > list){
    // le tableaux pour stocker les indices des ","
    std::vector < std:: string > to_pop ;
    int k=0;
    for (auto& file : list) {
        if (file.find(',')!= std::string::npos) {
            // lindices de ','
            to_pop.push_back(k);
            for (auto& f : file.split(',')) {
                list.push_back(f);
            }
        }
        k++;
    }
    //enlever les ',' de la list stocker ds to_pop
    for (int i : to_pop) {
        list.erase(list.begin() + i);
    }

    return list;

}
 */

////////////////////////////////////////////////////////////////////////////////
//                                   Main                                     //
////////////////////////////////////////////////////////////////////////////////




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