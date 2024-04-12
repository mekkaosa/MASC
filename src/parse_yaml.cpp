#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

class TypeName {
    std::string name;
    std::string file;
    int protection;
};

// Utilisez l'espace de noms requis
namespace fs = std::filesystem;
// Supposons que cette fonction compare deux fichiers YAML et imprime les différences
void differences_with(const std::string& file1Path, const std::string& file2Path) {
    YAML::Node file1 = YAML::LoadFile(file1Path);
    YAML::Node file2 = YAML::LoadFile(file2Path);

    // Cette fonction est très basique et doit être étendue pour une comparaison complète
    for (YAML::const_iterator it = file1.begin(); it != file1.end(); ++it) {
        std::string key = it->first.as<std::string>();
        if (file2[key]) {
            if (file1[key] != file2[key]) {
                std::cout << "Difference found in key: " << key << "\n";
                std::cout << "File 1: " << file1[key] << "\n";
                std::cout << "File 2: " << file2[key] << "\n";
            }
        } else {
            std::cout << "Key " << key << " not found in file 2\n";
        }
    }

    // Similaire pour les clés uniquement dans file2, etc.
}

//configuration_dict est un dictionnaire qui associe des clés de type TypeName à des valeurs de type TypeName
std::vector<TypeName> check_null(std::vector<TypeNamee, TypeName>& list_of_type_name) {
    std::vector<std::pair<TypeName, int>> null_protection_objects;
    std::vector<int> tab;
    int i = 0;
    // là on itere dans les type mane de list of type name et verifie si la protection est null et ajoute le typename et son indice au tableau null_protection
    for (auto& obj : list_of_type_name) {
        if (obj.protection == NULL) {
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
    // removes them if they have the same name and file as another TypeName object with a non-null protection field.
    return list_of_type_name;
}
// Ici la traduction du python en C++ message warning 
void print_warning(const std::string& msg) {
    std::cout << "ATTENTION : " << msg << std::endl;
}
// un check pour s'assurer de la coherence de declaration  d'objet .
void check_coherence(const std::vector<TypeName>& list_of_type_name) {

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
std::vector<TypeName> check_default_special(const std:: map<TypeName,Typename>&configuration_dict){
    std::vector<bool> defined;
    defined ={true,true};
    for (auto& obj: configuration_dict) {
        //Dans cet exemple, la fonction find du dictionnaire configuration_dict est utilisée pour rechercher la clé DEFAULT_RULES. Si la clé n'est pas trouvée, la fonction find retourne un itérateur pointant vers end(), qui est la fin du dictionnaire.
        if (configuration_dict.find(DEFAULT_RULES) == configuration_dict.end()){
            std::cout<<"No default_rules in this configuration"<<std::endl;
            defined[0]=false;
        }
        if(configuration_dict.find(SPECIAL_RULES) == configuration_dict.end()){
            std::cout<<"No special_rules in this configuration"<<std::endl;
            defined[1]=false;
        }
    
    }
    return defined ;
   
}
void print_error(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}
Yaml::Node check_is_none(Yaml::Node element,  const std::string &attr){
    if(!element ){
        if(attr==NULL){
            print_error("YAML: yaml attribute is empty: " + str(attr));
        }
    }else{
        return  element;
    }
}

int main(){
    //check_coherence( list_of_type_name);
    return 0;
}