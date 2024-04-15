#include <string>
#include <vector>

class Function {
public:
    std::string file;
    std::string name;
    std::vector<std::string> protections;
};

class Parameter {
public:
    std::string file;
    std::string name;
    std::string functionName;
    std::vector<std::string> protections;
};

class Global {
public:
    std::string file;
    std::string name;
    std::vector<std::string> protections;
};

class Structure {
public:
    std::string file;
    std::string name;
    std::vector<std::string> protections;
};

class BlackList {
public:
    std::vector<std::string> files;
    std::vector<std::string> protections;
};




/*
Function : représente une fonction avec les protections associées.
Parameter : représente un paramètre de fonction avec les protections associées.
Global : représente une variable globale avec les protections associées.
Structure : représente une structure avec les protections associées.
BlackList : représente une liste noire de fichiers avec les protections associées.
*/