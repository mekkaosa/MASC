#include <string>
#include <vector>

// Déclarations des classes
class Option {
public:
    std::string name;
    std::string value;
    Option(std::string name, std::string value) : name(name), value(value) {}
};

class BlackFile {
public:
    std::string name;
    std::string protection;
    int value;
    BlackFile(std::string name, std::string protection, int value) : name(name), protection(protection), value(value) {}
};

class Function {
public:
    std::string fileName;
    std::string functionName;
    std::vector<std::string> parameters;
    Function(std::string fileName, std::string functionName, std::vector<std::string> parameters) : fileName(fileName), functionName(functionName), parameters(parameters) {}
};

class TypeDef {
public:
    std::string fileName;
    std::string typedefName;
    std::vector<std::string> protection;
    TypeDef(std::string fileName, std::string typedefName, std::vector<std::string> protection) : fileName(fileName), typedefName(typedefName), protection(protection) {}
    
    std::string getTypeDefName() const {
        return typedefName;
    }
};

class Global {
public:
    std::string fileName;
    std::string globalName;
    std::vector<std::string> protection;
    Global(std::string fileName, std::string globalName, std::vector<std::string> protection) : fileName(fileName), globalName(globalName), protection(protection) {}
};

class Structure {
public:
    std::string fileName;
    std::string structureName;
    std::vector<std::string> protection;
    Structure(std::string fileName, std::string structureName, std::vector<std::string> protection) : fileName(fileName), structureName(structureName), protection(protection) {}
};




/*
Function : représente une fonction avec les protections associées.
Parameter : représente un paramètre de fonction avec les protections associées.
Global : représente une variable globale avec les protections associées.
Structure : représente une structure avec les protections associées.
BlackList : représente une liste noire de fichiers avec les protections associées.
*/