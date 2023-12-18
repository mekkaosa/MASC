#! /usr/bin/python3

import yaml 
import sys
import getopt
import os
from  object_yaml import *


# ############################################################################ #
# This variable are tools used globally in the program                         #
# ############################################################################ #

DEFAULT_CONFIGURATION =     "default_configuration"
CONFIGURATIONS =            "configurations"
SPECIAL_RULES =             "special_rules"
DEFAULT_RULES =             "default_rules"
OPTION =                    "option"
FILES =                     "files"
PROTECTION =                "protection"
PARAMETERS =                "parameters"

NEG =                       "no-"
TYPE =                      "type"
SIZE =                      "size"
MEMDUP =                    "memdup"
MEMDUP_PTR =                "memdup*"
IPCFG =                     "ipcfg"
IPDUP =                     "ipdup"
DUP =                       "dup"
CFG =                       "cfg"
ICALL =                     "icall"
NULL =                      "NULL"

FUNCTION_TYPE =             "function"
GLOBAL_TYPE =               "global"
STRUCTURE_TYPE =            "structure"
TYPEDEF_TYPE =              "typedef"
BLACKFILE_TYPE =            "blackFile"


choosen_configuration =     ""
default_configuration =     ""
dict_default_rules =        {}
enum_type_name =            [FUNCTION_TYPE, GLOBAL_TYPE, STRUCTURE_TYPE, TYPEDEF_TYPE]

class bcolors:
    HEADER =                '\033[95m'
    OKBLUE =                '\033[94m'
    OKCYAN =                '\033[96m'
    OKGREEN =               '\033[92m'
    WARNING =               '\033[93m'
    FAIL =                  '\033[91m'
    ENDC =                  '\033[0m'
    BOLD =                  '\033[1m'
    UNDERLINE =             '\033[4m'
    CWHITE =                '\33[97m'

# ############################################################################ #
# This list are used to check the validity of the yaml input                   #
# ############################################################################ #


blacklist_key_whitelist =       [FILES, PROTECTION]
configuration_key_withlist =    [OPTION, DEFAULT_RULES, SPECIAL_RULES]
function_key_whitelist =         [PROTECTION, PARAMETERS]
structure_key_whitelist =        [SIZE, TYPE, PROTECTION]
global_key_whitelist =           [SIZE, TYPE, PROTECTION]
protection_key_whitelist =       [CFG, IPCFG, DUP, IPDUP, ICALL, MEMDUP, 
                                 NEG+CFG, NEG+IPCFG, NEG+DUP, NEG+IPDUP, MEMDUP_PTR, 
                                 NEG+ICALL, NEG+MEMDUP, NEG+MEMDUP_PTR]
parameters_key_whitelist =       [ICALL, MEMDUP_PTR, IPCFG, IPDUP]
global_protection_whitelist =   [MEMDUP, NULL]
function_protection_whitelist = [NULL, CFG, IPCFG, DUP, IPDUP, ICALL, MEMDUP_PTR]


# ############################################################################ #
# This part of the code is related to the reading of the configuration file    #
# and the options pass as arguments                                            #
# ############################################################################ #


# Read YAML file 
def read_YAML(filePath):
    try:
        file = open(filePath, "r") 
    except:
        print_error(filePath + " do not exist")
    with file:
        configuration_dict = yaml.load(file, yaml.FullLoader)
    return check_is_none(configuration_dict, filePath)


# Choose configuration
def initialize(argv):
    nargs = len(argv)
    if (nargs > 8):
            sys.exit("too much arguments")
    configuration_file_in = "configuration.yml"
    opts, args = getopt.getopt(argv[1:],"i:o:c:",["file=", "out=", "conf="])
    # read yaml
    for opt, arg in opts:
        if opt in ("-i", "--in"):
            configuration_file_in = str(arg)
    configuration_dict = read_YAML(configuration_file_in)
    # create
    
    global default_configuration 
    global choosen_configuration
    default_configuration = str(configuration_dict[DEFAULT_CONFIGURATION])
    choosen_configuration = str(configuration_dict[DEFAULT_CONFIGURATION])

    configuration_file_out = str(configuration_file_in.split(".")[0]) + ".txt" 
    for opt, arg in opts:
        if opt in ("-o", "--out"):
            configuration_file_out = arg
        elif opt in ("-c", "--conf"):
            choosen_configuration = arg
            if choosen_configuration not in configuration_dict[CONFIGURATIONS].keys():
                sys.exit("The configuration passed as arguments does not exist")

    path_configuration_file_out = configuration_file_out.split("/")
    if len(path_configuration_file_out) > 1:
        pwd_configuration_file_out = "".join([str(path_configuration_file_out[i]) +"/" for i in range(len(path_configuration_file_out) -1)])
        if configuration_file_out[0] != "/":
            pwd_configuration_file_out = os.getcwd() + "/" + pwd_configuration_file_out
        try:
            os.makedirs(pwd_configuration_file_out, exist_ok=True)
        except:
            pass

    # configuration_file_out = "build/" + configuration_file_out
    with open(configuration_file_out, "w") as f:
        f.write("")
        
    return check_is_none(configuration_dict[CONFIGURATIONS][choosen_configuration], choosen_configuration), \
            configuration_file_out

# verifier
def check_configuration(configuration_dict):
    for key in configuration_dict.keys():
        if key not in configuration_key_withlist:
            return False
    return True


# ############################################################################ #
# This part of the code is related to the conversion of the dictionnary to     #
# standarized objects implemented in object_yaml.py                            #
# ############################################################################ #


# create object for option
def create_options_object(configuration_dict):
    options = ""
    try:
        options = configuration_dict[OPTION]
    except:
        print_remarks("No option in this configuration")
        return
    list_options_object = []
    check_is_none(configuration_dict[OPTION], OPTION)
    for item in options.items():
        check_is_none(item[1], item[0])
        list_options_object.append(Option(item[0], item[1]))
    return list_options_object


# create object for blacklist
def create_default_rules_object(configuration_dict):
    list_default_rules = []
    if DEFAULT_RULES not in list(configuration_dict.keys()):
        return []
    check_is_none(True if DEFAULT_RULES in configuration_dict.keys() else True, DEFAULT_RULES)
    default_rules = configuration_dict[DEFAULT_RULES]
    
    groups = [key for key in default_rules.keys()]
    if all(i in blacklist_key_whitelist for i in groups):
        check_is_none(default_rules[FILES], " files")
        files = default_rules[FILES]
        check_is_none(default_rules[PROTECTION], " protection")
        protection = default_rules[PROTECTION]
        for f in files:
            list_default_rules.append(BlackFile(f, protection, 0))
            dict_default_rules[f] = protection
    else:
        for group in groups:
            secswift_ACI_group = default_rules[group]
            try:
                secswift_ACI_group.keys()
            except:
                print_error("Issue with the default rules, each group should contain files and protection key")
            if all(i not in secswift_ACI_group.keys() for i in blacklist_key_whitelist):
                print_error( str(group) + " do not contains the required key: files, protection")
            check_is_none(secswift_ACI_group[FILES], str(group) + " files")
            files = secswift_ACI_group[FILES]
            check_is_none(secswift_ACI_group[PROTECTION], str(group) + " protection")
            protection = check_protection(secswift_ACI_group[PROTECTION], PROTECTION)
            for f in files:
                list_default_rules.append(BlackFile(f, protection, 0))
                dict_default_rules[f] = protection
    return list_default_rules

def check_protection(list, type):
    to_pop = []
    k = 0
    for i in list:
        if type == PROTECTION and i not in protection_key_whitelist:
            print_warning(str(i) + " is not an accepted protection in the default_rules")
            to_pop.append(k)
        k += 1
    for i in to_pop[::-1]:
        list.pop(i)
    if len(list) == 0:
        print_error(str(type) + " do not contain any accepted attribute")
    return list


# create object for special rules

def create_special_rules_object(configuration_dict, type_name):
    check_is_none(True if SPECIAL_RULES in configuration_dict.keys() else None, SPECIAL_RULES)
    special_rules = configuration_dict[SPECIAL_RULES]
    # create object Function 
    list_protection = []
    list_file_with_type_name = get_file_with_type_name(special_rules, type_name)
    for file_with_type_name in list_file_with_type_name:
        special_rules_file = special_rules[file_with_type_name]
        list_name_object = []
        for name in special_rules_file.keys():
            if name == type_name and type_name == FUNCTION_TYPE:
                list_name_object += create_function_object(special_rules_file, type_name ,file_with_type_name )
            if (name == type_name and type_name == GLOBAL_TYPE 
                and type(create_global_object(special_rules_file, type_name ,file_with_type_name )) != type(False)) :
                list_name_object.append(create_global_object(special_rules_file, type_name ,file_with_type_name ))
            if (name == type_name and type_name == STRUCTURE_TYPE 
                and type(create_structure_object(special_rules_file, type_name, file_with_type_name)) != type(False)):
                list_name_object.append(create_structure_object(special_rules_file, type_name, file_with_type_name))
            if (name == type_name and type_name == TYPEDEF_TYPE 
                and type(create_typedef_object(special_rules_file, type_name, file_with_type_name)) != type(False)):
                list_name_object.append(create_typedef_object(special_rules_file, type_name, file_with_type_name))
        list_protection += list_name_object        
    return list_protection


def create_function_object(special_rules_file , type_name, file_name):
    list_of_protection = []
    check_is_none(special_rules_file[type_name], str(file_name) + " " + str(type_name))
    for attribute in special_rules_file[type_name].items():
        check_is_none(attribute[1], attribute[0])
        element_names = attribute[0].split(".")
        if len(element_names) == 2:
            if all(i in parameters_key_whitelist for i in attribute[1]):
                list_of_protection.append(Function(file_name, element_names[0], parameters=[element_names[1], attribute[1]]))
            else:
                print_warning("Function: " + str(element_names[0]) + " located in " + str(file_name) +
                " has a non compatible parameter" + ": " + str(attribute[1]))
        else:
            if (type(attribute[1]) == type([]) and all( i in protection_key_whitelist for i in attribute[1])): 
                list_of_protection.append(Function(file_name, element_names[0], protection=attribute[1]))
            else:
                print_warning("Function: " + str(element_names[0]) + " located in " + str(file_name) +
                " has a non compatible protection: " + str(attribute[1]))
    return list_of_protection

def create_typedef_object(special_rules_file , typedef_name, type_name, file_name):
    check = 0
    check_is_none(special_rules_file[type_name][typedef_name], typedef_name)
    for attribute in special_rules_file[type_name][typedef_name].items():
        if attribute[0] == PROTECTION:
            check_is_none(attribute[1], attribute[0])
            if all( i in protection_key_whitelist for i in attribute[1]):
                protection = attribute[1]
                check += 1        
            else:
                print_warning("TypeDef: " + str(typedef_name) + " located in " + str(file_name) + 
                "' has a non compatible protection: " + str(attribute[1]))
                return False   
        if attribute[0] not in function_key_whitelist:
            print_warning("TypeDef: " + str(typedef_name) + " has a non compatible attribute: " 
            + str(attribute[0]))
    if check != 1:
        print_error("Typedef: " + str(typedef_name) + " located in " + str(file_name) 
        + " miss an attribute")
    return TypeDef(file_name, typedef_name, protection)

def create_global_object(special_rules_file, type_name, file_name):
    protection = []
    check_is_none(special_rules_file[type_name], type_name)
    for attribute in special_rules_file[type_name].items():
        if type(attribute[1]) == type("str"):
            attr = attribute[1].split(",") 
        else:
            attr = attribute[1]
        check_is_none(attr, attribute[0])
        if all( i in protection_key_whitelist for i in attr):
            protection = attr
        else:
            print_warning("Global: " + str(attribute[0]) + " located in " + str(file_name) + 
            "' has a non compatible protection: " + str(attribute[1]))
            return False
    return Global(file_name, attribute[0], protection)


def create_structure_object(special_rules_file, type_name, file_name):
    protection = []
    check_is_none(special_rules_file[type_name], type_name)
    for attribute in special_rules_file[type_name].items():
        if type(attribute[1]) == type("str"):
            attr = attribute[1].split(",") 
        else:
            attr = attribute[1]
        check_is_none(attr, attribute[0])
        if all( i in protection_key_whitelist for i in attr):
            protection = attr
        else:
            print_warning("Structure: " + str(attribute[0]) + " located in " + str(file_name) + 
            "' has a non compatible protection: " + str(attribute[1]))
            return False
    return Element(file_name, attribute[0], protection)


# ############################################################################ #
# Standardized object can reference to multiple files, types or protections    #
# This part is used to make each object reference to only one entity           #
# ############################################################################ #


def separate_multiple_option(option_list):
    if option_list is None:
        return []
    i = 0
    to_pop = []
    option_list_tmp = []
    for opt in option_list:
        if type(opt.args) == type([]) and len(opt.args) > 1:
            for arg in opt.args:
                option_list_tmp.append(Option(opt.name, arg))
            to_pop.append(i)
        i += 1 
    for pos in to_pop[::-1]:
        option_list.pop(pos)
    option_list += option_list_tmp
    return option_list


def separate_multiple_file_and_name(list_type_name_object, type_name):
    if type_name != BLACKFILE_TYPE:
        # split name
        i = 0
        to_pop = []
        list_type_name_object_tmp = []
        for obj in list_type_name_object:
            if "," in obj.name:
                list_of_type_name_name = obj.name.split(",")
                for name in list_of_type_name_name:
                    if type_name == FUNCTION_TYPE:
                        list_type_name_object_tmp.append(Function(obj.file, name.replace(" ", ""), 
                        obj.protection, obj.parameters))
                    if type_name == GLOBAL_TYPE:
                        list_type_name_object_tmp.append(Global(obj.file, name.replace(" ", ""), 
                        obj.protection))
                    if type_name == STRUCTURE_TYPE:
                        list_type_name_object_tmp.append(Element(obj.file, name.replace(" ", ""), 
                        obj.protection))
                    if type_name == TYPEDEF_TYPE:
                        list_type_name_object_tmp.append(TypeDef(obj.file, name.replace(" ", ""), 
                        obj.protection))
                to_pop.append(i)
            i += 1
        for pos in to_pop[::-1]:
            list_type_name_object.pop(pos)
        list_type_name_object += list_type_name_object_tmp
        del list_type_name_object_tmp
    # split file name
    j = 0
    to_pop = []
    list_type_name_object_tmp = []
    for obj in list_type_name_object:
        if "," in obj.file:
            list_of_file_name = obj.file.split(",")
            for file_name in list_of_file_name:
                if type_name == FUNCTION_TYPE:
                    list_type_name_object_tmp.append(Function(file_name.replace(" ", ""), 
                    obj.name, obj.protection, obj.parameters))
                if type_name == GLOBAL_TYPE:
                    list_type_name_object_tmp.append(Global(file_name.replace(" ", ""), obj.name, 
                    obj.protection))
                if type_name == STRUCTURE_TYPE:
                    list_type_name_object_tmp.append(Element(file_name.replace(" ", ""), obj.name, 
                    obj.protection))
                if type_name == TYPEDEF_TYPE:
                    list_type_name_object_tmp.append(TypeDef(file_name.replace(" ", ""), obj.name, 
                    obj.protection))
            to_pop.append(j)
        j += 1
    for pos in to_pop[::-1]:
        list_type_name_object.pop(pos)
    list_type_name_object += list_type_name_object_tmp
    return list_type_name_object


def separate_multiple_protection(list_type_name_object, type_name):
    # split protection
    k = 0
    to_pop = []
    list_type_name_object_tmp = []
    for obj in list_type_name_object:
        if obj.protection is not None and type(obj.protection) == type([]) and len(obj.protection) == 1:
            obj.protection = obj.protection[0]
        if obj.protection is not None and type(obj.protection) == type([]) and len(obj.protection) > 1:
            for protect in obj.protection:
                if type_name == FUNCTION_TYPE:
                    list_type_name_object_tmp.append(Function(obj.file, 
                    obj.name, str(protect), obj.parameters))
                if type_name == GLOBAL_TYPE:
                    list_type_name_object_tmp.append(Global(obj.file, obj.name, 
                    str(protect)))
                if type_name == STRUCTURE_TYPE:
                    list_type_name_object_tmp.append(Element(obj.file, obj.name, 
                    str(protect)))
                if type_name == BLACKFILE_TYPE:
                    list_type_name_object_tmp.append(BlackFile(obj.file, str(protect), obj.exception_number))
                if type_name == TYPEDEF_TYPE:
                    list_type_name_object_tmp.append(TypeDef(obj.file, obj.name, str(protect)))
            to_pop.append(k)
        k += 1
    for pos in to_pop[::-1]:
        list_type_name_object.pop(pos)
    list_type_name_object += list_type_name_object_tmp
    return list_type_name_object

def separate_multiple_parameters(list_type_name_object, type_name):
    # split parameters
    k = 0
    to_pop = []
    list_type_name_object_tmp = []
    for obj in list_type_name_object:
        if obj.parameters is not None and len(obj.parameters[1]) == 1:
            obj.parameters[1] = obj.parameters[1][0]
        if obj.parameters is not None and type(obj.parameters[1]) == type([]) and len(obj.parameters[1]) > 1:
            for param in obj.parameters[1]:
                if type_name == FUNCTION_TYPE:
                    list_type_name_object_tmp.append(Function(obj.file, 
                    obj.name, obj.protection, [obj.parameters[0], str(param)]))
                if type_name == PARAMETERS:
                    list_type_name_object_tmp.append(Parameters(obj.file, str(obj.parameters[0]), 
                    obj.name, str(param)))
            to_pop.append(k)
        k += 1
    for pos in to_pop[::-1]:
        list_type_name_object.pop(pos)
    list_type_name_object += list_type_name_object_tmp
    return list_type_name_object

# ############################################################################ #
# In the .yml file, exceptions are written the same way as affirmations. This  #
# part of the code aims to convert the exception to affirmation according to   #
# the protection specified in the default rules:                               #
# (file_name, function_name, no-protection1) ->                                #
# (file_name, function_name, protection2, protection3)                         #
# ############################################################################ #


def exception_to_affirmation(list_type_name):
    i = 0
    to_pop = []
    global dict_default_rules
    for obj in list_type_name:
        if obj.protection is not None:
            if obj.file in list(dict_default_rules.keys()):
                obj.protection = attribute_substraction(dict_default_rules[obj.file], obj.protection)
            tmp = obj.protection
            if len(tmp) == 0:
                obj.protection.append("NULL")
                # to_pop.append(i)
            if type(obj.protection) == type("str"):
                tmp = obj.protection.split(",")
            for elem in tmp:
                if elem[0:3] == NEG:
                    print_error("an exception is referencing to a non declared default file: " + str(obj.file))
        i += 1
    for pos in to_pop[::-1]:
        list_type_name.pop(pos)
    return list_type_name

def attribute_substraction(deft, specials):
    default = deft
    to_remove = []
    if type(specials) == type([]):
        specials += [attr for attr in default if (attr not in specials and NEG+attr not in specials)]
        for attr in default:
            no_attr = NEG + attr
            if no_attr in specials:
                to_remove.append((attr, no_attr))
        for i in to_remove:
            specials.remove(i[1]) 
    elif type(specials) == type("str"):
        for attr in default:
            no_attr = NEG + attr
            if no_attr == specials:
                to_remove.append((attr, no_attr))
        for i in to_remove:
            default.remove(i[0])
            specials = ""
        if specials != "" and specials[0:3] != NEG and specials not in default:
            specials.append(default)
    return list(specials)


# ############################################################################ #
# This part of the code is used to write the object into the output            #
# configuration file                                                           #
# ############################################################################ #


def write_configuration_file(list_type_name_object, type_name, configuration_file):
    if list_type_name_object is None:
        return
    with open(configuration_file, "a") as f:
        if type_name == FUNCTION_TYPE:
            for function in list_type_name_object:
                if function.parameters is not None:
                    f.write(str(function.to_parameters()))
                    f.write("\n")
                elif function.protection is not None :
                    f.write(str(function))
                    f.write("\n")
        elif type_name == GLOBAL_TYPE:
            for globale in list_type_name_object:
                f.write(str(globale))
                f.write("\n")
        elif type_name == OPTION:
            f.write("__begin_option__\n") # A flag to limit the option part
            for option in list_type_name_object:
                f.write(str(option))
                f.write("\n")
            f.write("__end_option__\n")
        elif type_name == STRUCTURE_TYPE:
            for element in list_type_name_object:
                f.write(str(element))
                f.write("\n")
        elif type_name == BLACKFILE_TYPE:
            f.write("__begin_blacklist__\n")
            for element in list_type_name_object:
                f.write(str(element))
                f.write("\n")
            f.write("__end_blacklist__\n")
        elif type_name == TYPEDEF_TYPE:
            for element in list_type_name_object:
                f.write(str(element))
                f.write("\n")

def write_option_file(list_type_name_object, configuration_file):
    global choosen_configuration
    global default_configuration
    
    if list_type_name_object is None:
        return  
    tmp = configuration_file.split(".")
    configuration_file = "".join([name + "." for name in tmp[:-1]]) + "option.acf" #+ str(tmp[-1])
    with open(configuration_file, "w") as f:
        f.write('configuration "' + str(choosen_configuration) + '" {\n\t')
        for option in list_type_name_object:
            if option.name == "compilation_options":
                for arg in option.args:
                    f.write(str(arg) + " ")
            else:
                f.write(str(option.name) + "=" + str(option.args) + " ")
        f.write("\n\t}")
        f.write('\nactive configuration "' + default_configuration + '"')


def write_in(sentence, configuration_file):
    with open(configuration_file, "a") as f:
        f.write(sentence)
        f.write("\n")


# ############################################################################ #
# This part of the code are tools function                                     #
# ############################################################################ #

def check_coherence(list_of_type_name):
    for obj in list_of_type_name:
        for obj_cmp in list_of_type_name:
            if obj.differences_with(obj_cmp) == [True, False, False]:
                print_warning("The object " + str(obj.name) + " has different declaration in the file "
                              + str(obj.file) + " and " + str(obj_cmp.file))



# Null is added instead of removing the object. For exemple, this is used when memdup is in the default rules
# We have a special rules that says "no-memdup". If the element is deleted it would seems as we did nothing.
# NULL is here to notify that there are in fact no protection applied
def check_attribute(list_of_type_name, type_name):
    to_pop_list = []
    to_pop = []
    k = 0
    list_obj = []
    for obj in list_of_type_name:
        list_obj.append(obj)
        if type_name == FUNCTION_TYPE:
            if type(obj.protection) == type([]) and len(obj.protection) > 1:
                for protec in obj.protection:
                    if protec not in function_protection_whitelist:
                        to_pop_list.append([k, MEMDUP])
            if type(obj.protection) == type([]) and len(obj.protection) == 1:
                if obj.protection[0] not in function_protection_whitelist and obj.protection[0] in protection_key_whitelist:
                    obj.protection = NULL
                if obj.protection[0] not in global_protection_whitelist:
                    to_pop.append(k)
            if type(obj.protection) == type("str"):
                if obj.protection not in function_protection_whitelist and obj.protection in protection_key_whitelist:
                    obj.protection = NULL
                if obj.protection not in function_protection_whitelist:
                    to_pop.append(k)
        if type_name == GLOBAL_TYPE or type_name == STRUCTURE_TYPE:
            if type(obj.protection) == type([]) and len(obj.protection) > 1:
                for protec in obj.protection:
                    if protec not in global_protection_whitelist:
                        to_pop_list.append([k, protec])
            if type(obj.protection) == type([]) and len(obj.protection) == 1:
                if obj.protection[0] not in global_protection_whitelist and obj.protection[0] in protection_key_whitelist:
                    obj.protection = [NULL]
                elif obj.protection[0] not in global_protection_whitelist:
                    to_pop.append(k)
            if type(obj.protection) == type("str"):
                if obj.protection not in global_protection_whitelist and obj.protection in protection_key_whitelist:
                    obj.protection = NULL
                elif obj.protection not in global_protection_whitelist:
                    to_pop.append(k)
        k+=1

    # debug(list_obj)
    # print(to_pop)
    # error of shared memory, the protection are shared by two different object
    # for el in to_pop_list[::-1]:
    #     list_of_type_name[el[0]].protection = list_of_type_name[el[0]].protection.remove(el[1])
    for el in to_pop[::-1]:
        list_of_type_name.pop(el)
    check_null(list_of_type_name)
    return list_of_type_name

def check_unicity(list_type_name):
    encountered_ls = []
    to_pop = []
    i = 0
    for item in list_type_name:
        if item in encountered_ls:
            to_pop.append(i)
            # print_warning("Item: " + str(item.name) + " located in " + str(item.file) + 
            # " is defined twice")
        encountered_ls.append(item)
        i += 1
    if len(to_pop) == 0:
        return list_type_name, True
    else:
        for pos in to_pop[::-1]:
            list_type_name.pop(pos)
        return list_type_name, False

def check_null(list_of_type_name):
    null_protection_objects = []
    to_pop = []
    i = 0
    for obj in list_of_type_name:
        if obj.protection == NULL:
            null_protection_objects.append((obj, i))
        i += 1
    for obj in list_of_type_name:
        for null_obj in null_protection_objects:
            # return tab [same name?, same file?, same protection?]
            if obj.differences_with(null_obj[0]) == [True, True, False]:
                to_pop.append(null_obj[1])
    # for i in range(len(list_of_type_name)):
    #     print(str(list_of_type_name[i]), i)
    # print(list(set(to_pop[::-1])))
    to_pop = list(set(to_pop))
    to_pop.sort()
    for pos in to_pop[::-1]:
        list_of_type_name.pop(pos)
    return list_of_type_name


def split_coma(list):
    to_pop = []
    k = 0
    for file in list:
        if len(file.split(",")) != 1:
            to_pop.append(k)
            for f in file.split(","):
                list.append(f)
        k+=1
    for pos in to_pop[::-1]:
        list.pop(pos)
    return(list)


def check_default_special(configuration_dict):
    defined = [True, True]
    if DEFAULT_RULES not in configuration_dict.keys():
        print_remarks("No default_rules in this configuration")
        defined[0] = False
    if SPECIAL_RULES not in configuration_dict.keys():
        print_remarks("No special_rules in this configuration")
        defined[1] = False
    return defined
        

def get_file_with_type_name(special_rules, name):
    file_with_name = []
    check_is_none(special_rules, name)
    for file in special_rules.keys():
        check_is_none(special_rules[file], file)
        for element in special_rules[file].keys():
            if name in element:
                file_with_name.append(file)
    return check_is_none(file_with_name, name)


def check_is_none(element, attr):
    if element is None:
        print_error("YAML: yaml attribute is empty: " + str(attr))
    else:
        return element


def debug(list, word = "list"):
    print("\n" + word + ":")
    for i in list:
        print(i)


def print_warning(comment):
    if type(comment) != type("str"):
        print("comment is not of type string")
        return
    print("parse_yaml: " + bcolors.WARNING + "warning " + bcolors.ENDC + bcolors.CWHITE + comment 
          + bcolors.ENDC )
    return


def print_error(comment):
    if type(comment) != type("str"):
        sys.exit("comment is not of type string")
    sys.exit("parse_yaml: " + bcolors.FAIL + "error: " + bcolors.CWHITE + comment 
             + bcolors.ENDC)


def print_remarks(comment):
    if type(comment) != type("str"):
        print("comment is not of type string")
        return
    print("parse_yaml: " + bcolors.OKCYAN + "remark: " + bcolors.CWHITE + comment 
          + bcolors.ENDC)


# ############################################################################ #
#                                   Main                                       #
# ############################################################################ #


if __name__=="__main__":

    # Read the yaml file and initialize input/output file with command line arguments 
    configuration_dict, configuration_file_name_out = initialize(sys.argv)
    
    #           option          #
    # create object
    list_options_object = create_options_object(configuration_dict)

    # separate object with multiple arguments
    list_options_object = separate_multiple_option(list_options_object)

    # write in configuration file
    write_configuration_file(list_options_object, OPTION, configuration_file_name_out)

    # write in an acf file
    write_option_file(list_options_object, configuration_file_name_out)

    # check if there are specials or default rules in the configuration file
    defined = check_default_special(configuration_dict)

    #        default_rules       #
    if defined[0]:

        # create object
        black_file = create_default_rules_object(configuration_dict)

        # separate object with multiple protection (name_file, name_function, protec1:protect2) -> 
        # (name_file, name_function, protec1), (name_file, name_function, protect2)
        black_file = separate_multiple_protection(black_file, BLACKFILE_TYPE)

        # write in the configuration file
        write_configuration_file(black_file, BLACKFILE_TYPE, configuration_file_name_out)

    #       sepcial_rules       #
    write_in("__begin_whitelist__", configuration_file_name_out)
    if defined[1]:
        for type_name in enum_type_name:

            # create object
            list_function_object = create_special_rules_object(configuration_dict, type_name)
            
            # separate object which defined multiple file or "type_name"
            list_function_object = separate_multiple_file_and_name(list_function_object, type_name)
            
            # transform the exception protection (no-cfg) into affirmative protection (dup, ipdup)
            # based on the default rules
            list_function_object = exception_to_affirmation(list_function_object)
            
            # check if protection are the same for each object in each files
            # check_coherence(list_function_object)
            
            # separate object with multiple protection/parameters (name_file, name_function, protec1:protect2) -> 
            # (name_file, name_function, protec1), (name_file, name_function, protect2)
            list_function_object = separate_multiple_protection(list_function_object, type_name)
            
            if type_name == FUNCTION_TYPE:
                list_function_object = separate_multiple_parameters(list_function_object, type_name)
            
            # check if each of the object has appropriate attribute (unfinished, must be done for global and structure too)
            list_function_object = check_attribute(list_function_object, type_name)

            # check if an object if the user defined an object multiple times
            list_function_object, bool = check_unicity(list_function_object)

            # write in the configuration file
            write_configuration_file(list_function_object, type_name, configuration_file_name_out)
    write_in("__end_whitelist__", configuration_file_name_out)
  
