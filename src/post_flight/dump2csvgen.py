"""
The purpose of this file is to generate the X-Macros for the data logger
to properly show the scheme of the files.
"""


import os
import sys
from typing import Dict, List, Tuple
import clang.cindex
def get_qualified_enum_name(enum_node):
    parent = enum_node.semantic_parent
    if parent.kind == clang.cindex.CursorKind.STRUCT_DECL or parent.kind == clang.cindex.CursorKind.CLASS_DECL:
        return f"{parent.spelling}::{enum_node.spelling}"
    return enum_node.spelling

def traverse_ast(ast_node, structs, enums):
    """
    Recursively traverse the AST and find struct definitions.
    """
    if ast_node.kind == clang.cindex.CursorKind.STRUCT_DECL and ast_node.is_definition():
        struct_name = ast_node.spelling
        if struct_name:
            fields = []
            for child in ast_node.get_children():
                if child.kind == clang.cindex.CursorKind.FIELD_DECL:
                    token_spellings = []
                    for token in child.get_tokens():
                        token_spellings.append(token.spelling)
                    
                    field_type = " ".join(token_spellings[:-1])
                    field_name = token_spellings[-1]
                    fields.append((field_name, field_type))
            structs[struct_name] = fields
    elif ast_node.kind == clang.cindex.CursorKind.ENUM_DECL:
        enum_name = ast_node.spelling
        values = []
        for child in ast_node.get_children():
            if child.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
                values.append((child.spelling, child.enum_value))
        if enum_name:
            enums[enum_name] = (values, ast_node.enum_type.get_size(), get_qualified_enum_name(ast_node))
    
    for child in ast_node.get_children():
        traverse_ast(child, structs, enums)

def find_all_headers (target: str, result = [], maxdepth = 10):
    if maxdepth <= 0:
        return result
    if not os.path.exists(target):
        raise FileExistsError( f"Path {target} does not exist" )
    if os.path.isfile(target):
        _name, ext = os.path.splitext( target )
        
        if ext == ".h":
            result.append(target)
        
        return result
    
    if not os.path.isdir(target):
        return result
    
    for name in os.listdir(target):
        find_all_headers( os.path.join(target, name), result, maxdepth - 1 )
    
    return result
def find_dumpable_of_header (filename: str):
    index = clang.cindex.Index.create()
    tu = index.parse(filename, args=['-std=c++17', '-x', 'c++-header'])
    
    structs = {}
    enums = {}
    traverse_ast(tu.cursor, structs, enums)

    return structs, enums

def generate_dumper_name (name: str):
    if " " in name:
        name = "_".join(name.split(" "))
    if name.startswith("struct_"):
        name = name[7:]
    return f"dump_csv_{name}"
def generate_dumper_function_definition (name: str, qualname: str = None):
    if qualname is None: qualname = name
    return f"void {generate_dumper_name(name)} ({qualname} &value, std::ostringstream &stream)"
def generate_dumper_call (field_name: str, target: str):
    return f"\t{generate_dumper_name(target)}(value.{field_name}, stream);"

def filter_useful (struct: str, structs: Dict[str, List[Tuple[str, str]]], enums: Dict[str, Tuple[int, List[Tuple[str, int]]]]):
    useful = []
    used = set()

    used.add(struct)
    useful.append(struct)

    offset = 0
    while offset < len(useful):
        if useful[offset] in structs:
            for _field, nxt in structs[useful[offset]]:
                if nxt.startswith("struct "):
                    nxt = nxt[7:]
                if nxt in used: continue
                useful.append(nxt)
                used.add(nxt)
        offset += 1
    
    useless = []
    for key in structs:
        if key not in used:
            useless.append(key)
    
    for key in useless:
        del structs[key]
    
    useless = []
    for key in enums:
        if key not in used:
            useless.append(key)
    
    for key in useless:
        del enums[key]

def generate_csv_dumper_struct (typename: str, struct: List[Tuple[str, str]]):
    def generate_define ():
        name = generate_dumper_function_definition(typename)
        lines = [
            f"{name} " + "{",
        ] + list(map(lambda x : generate_dumper_call(x[0], x[1]), struct)) + [ "}" ]
        return "\n".join(lines)
    header = f"{generate_dumper_function_definition(typename)};"
    define = generate_define()
    return header, define
def generate_csv_dumper_enum (typename: str, enum: Tuple[int, List[Tuple[str, int]]]):
    values, typesize, qualname = enum

    def generate_value_check (name, value):
        return f"\tif (((int) value) == {value}) stream << \"{name},\";"
    def generate_define ():
        name = generate_dumper_function_definition(typename, qualname)
        lines = [
            f"{name} " + "{",
        ] + list(map(lambda x : generate_value_check(x[0], x[1]), values)) + [ "}" ]
        return "\n".join(lines)

    header = f"{generate_dumper_function_definition(typename, qualname)};"
    define = generate_define()

    return header, define

BASE_HEADER = [
    f"#include <iostream>",
    f"#include <fstream>",
    f"#include <sstream>",
    f"#include \"dump2csv.h\"",
    f"{generate_dumper_function_definition("int")};",
    f"{generate_dumper_function_definition("unsigned int")};",
    f"{generate_dumper_function_definition("unsigned")};",
    f"{generate_dumper_function_definition("float")};",
    f"{generate_dumper_function_definition("double")};",
    f"{generate_dumper_function_definition("uint8_t")};",
    f"{generate_dumper_function_definition("bool")};"
]

DEFINE_SIMPLE_STREAM = "{ stream << value << \",\"; }"
BASE_DEFINE = [
    f"{generate_dumper_function_definition("int")}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition("unsigned int")}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition("unsigned")}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition("float")}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition("double")}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition("uint8_t")}" + "{ stream << ((int) value) << \",\"; }",
    f"{generate_dumper_function_definition("bool")}" + " {\n\tif(value) { stream << \"true,\"; } else { stream << \"false,\"; } }"
]

def generate_xmacros (structs: Dict[str, List[Tuple[str, str]]], enums: Dict[str, Tuple[int, List[Tuple[str, int]]]]):
    headers = list(BASE_HEADER)
    defines = list(BASE_DEFINE)

    for key in structs:
        header, define = generate_csv_dumper_struct(key, structs[key])

        headers.append(header)
        defines.append(define)
    for key in enums:
        header, define = generate_csv_dumper_enum(key, enums[key])

        headers.append(header)
        defines.append(define)

    print("\n".join(headers))
    print("\n".join(defines))

def compile_csv_header (struct: str, structs: Dict[str, List[Tuple[str, str]]], codename: str = "", result = []):
    def accumulate (field: str):
        if codename == "":
            return field
        return codename + "." + field
    
    if struct not in structs:
        result.append(codename)
        return result
    
    for fname, ftype in structs[struct]:
        compile_csv_header(ftype, structs, accumulate(fname), result)
    return result

TEMPLATE = """
int main (void) {
    %(typename)s dump;
	char* buffer = (char*) (&dump);
	std::ifstream stream("log.txt");

	std::cout << "%(scheme)s\\n";

	while (1) {
		stream.read(buffer, sizeof(%(typename)s));
		if (stream.eof()) break ;

		std::ostringstream strm;
		dump_csv_%(typename)s(dump, strm);
		
		std::string res = strm.str();
		res[res.size() - 1] = '\\n';
		std::cout << res;
	}

	stream.close();
}
"""
def generate_main (struct: str, structs: Dict[str, List[Tuple[str, str]]]):
    scheme = csv_header = ",".join(compile_csv_header( struct, structs ))

    return TEMPLATE.replace( "%(typename)s", struct ) \
        .replace( "%(scheme)s", scheme )

def main (target: str, struct_name: str):
    headers = find_all_headers(target)

    all_structs: Dict[str, List[Tuple[str, str]]]             = {}
    all_enums  : Dict[str, Tuple[int, List[Tuple[str, int]]]] = {}
    for header in headers:
        structs, enums = find_dumpable_of_header(header)

        for key in structs.keys():
            all_structs[key] = structs[key]
        
        for key in enums.keys():
            all_enums[key] = enums[key]
    
    filter_useful(struct_name, all_structs, all_enums)

    generate_xmacros(all_structs, all_enums)

    print(generate_main( struct_name, structs ))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_macros_clang.py <header | folder> <struct>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])
