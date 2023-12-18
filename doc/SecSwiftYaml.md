# SecSwift Configuration File

## Purpose

This configuration file aims at facilitating the addition of SecSwift into a program/ an application. However, this is not the only method. You can also add attibutes to `function`, `variables`, `fields` directly into the code.

## Configuration File Grammar

In this part, we will explain how to write the yaml file in a correct way.

### Base

The top level of the file needs 2 `keys`: `configurations` and `default_configuration`.

The `configurations` part will contain all of your different `configuration names`. For exemple you can have `configuration names` named **"debug", "performance" or "security"**.

The `default_configuration` allows you to define the configuration you want to be applied if no arguments are written in the command line:

Here is a representation of what it would look like:
```yaml
---
configurations:
    debug: ...
    performance: ...
    security: ...
default_configuration: performance
...
```

### Configurations

A `configuration` take 3 different arguments: `general_rules`, `special_rules`, `option`. Only these 3 names will be accepted. These names are keys to their respective region.

Here is a representation of what it would look like for the "debug" `configuration name`:
```yaml
---
configurations:
    debug: 
        option: ...
        general_rules: ...
        special_rules: ...    

```

### General Rules

General rules are the easiest thing. It takes only two arguments: `files` and `protection`. The `files` key will contain the list of all the files you want to protect. For exemple you can have **"myFirstFile.c", "mySecondFile.c", "myThirdFile.c"**. The protection key will contain all the protections you want to apply to those files. There are 6 different types of protection:
- **memdup:**
- **dup:**
- **cfg:**
- **ipcfg:**
- **ipdup:**
- **icall:**

Here is a representation of what it would look like:
```yaml
---
configurations:
    debug:
        general_rules:
            files:
            - myFirstFile.c
            - mySecondFile.c
            - myThirdFile.c
            protection:
            - cfg
            - ipcfg
            - memdup

```

You can also define groups within the general rules. It allows you to be more precise in your protection description:

```yaml
---
configurations:
    debug:
        general_rules:
            group1:
                files:
                - myFirstFile.c
                - mySecondFile.c
                - myThirdFile.c
                protection:
                - cfg
                - ipcfg
                - memdup
            group2:
                files:
                - myForthFile.c
                protection:
                - dup
                - ipdup

```

### Special Rules

Special rules allows you to have precision, but it is more time-consuming to write. `special_rules` section take `file names` as keys. These `file names` can be: **"file1.c", "file2.c", "file3.c"**. It is to precise in which file you want to apply the rules. You can also write "file2.c, file3.c" if you want to apply the same rules to both files (the comma "," is important). 

```yaml
---
configurations:
    debug:
        special_rules:
            file1.c: ...
            file2.c, file3.c: ...

```

Once you have those files you can define 3 different types:
- **function**: to define functions
- **global**: to define global variables
- **structure**: to define fields of structures

Their implementation look like each other. For the three types you will have to add the `name` of your element. This `name` will be the key in which you will describe your rules. For exemple you can have `global` named **"myGlobal1" , "myGlobal2"** and a `function` named **"myFunc1", "myFunc2"**. This 'name' can also be regrouped. For exemple you can have a structure named **"myField1, myField2"**.

```yaml
---
configurations:
    debug:
        special_rules:
            file1.c: 
                function:
                    myFunc1: ...
                    myFunc2: ...
                global:
                    myGlobal1: ...
                    myGlobal2: ...
            file2.c, file3.c: 
                structure:
                    myField1, myField2: 

```

Then you will have to define the `protection` in each of the element. 

For the `function` type you can define a protection for a parameter, you should write `functionName`.`parameterName`. Then you can list all the protections

Here is a representation of what it would look like for **"myFunc1", "myGlobal1", "myField1"**

```yaml
---
configurations:
    debug:
        special_rules:
            file1.c: 
                function:
                    sortFunction:   
                        - ipcfg
                    myFunc1:
                        - ipcfg
                        - dup
                    myFunc1.sortFunction: 
                        - ipcfg
                global:
                    myGlob1:
                        - memdup
                structure:
                    myField1:
                        - memdup

```

#### Exception

In the special rules you can also make exception to the `general_rules`. It means that you can add or remove a protection. It has exactly the same structure as the example above. To remove a protection you just need to add **"no-"** before the name. For exemple **"no-memdup"** Here is an exemple:

```yaml
---
configurations:
    debug:
        general_rules:
            files:
            - file1.c
            - file2.c
            protection:
            - ipcfg
            - memdup
        special_rules:
            file1.c: 
                function:
                    myFunc1:
                        - dup
                global:
                    myGlob2:
                        - no-memdup
                    
```
In this exemple the **myFunc1** will have this protections: **dup, ipcfg**. The global variable **myGlob2** will have no protections

### Options

// not implemented yet

## LLVM Frontend

This file should ultimatly be parsed by the llvm frontend and not by the python Script. The python scipt is converting the yaml into object, then process them (special rules, general rules), convert them print it. However the LLVM compiler do not have to convert them to unique object. The object generated should have this structures:

**Special Rules :**

```cpp
class Function {
    string file;
    string name;
    vector<string> protections;
}
```

```cpp
class Parameter {
    string file;
    string name;
    string functionName;
    vector<string> protections;
}
```

```cpp
class Global {
    string file;
    string name;
    vector<string> protections;
}
```

```cpp
class Structure {
    string file;
    string name;
    vector<string> protections;
}
```

**General rules :**

```cpp
class BlackList {
    vector<string> files;
    vector<string> protections;
}
```