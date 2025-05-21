"""
The purpose of this file is to generate the X-Macros for the data logger
to properly show the scheme of the files.
"""

def printcerr (*args, **kwargs):
    print(*args, **kwargs, file=sys.stderr)


import hashlib
import os
import subprocess
import sys
import tempfile
import time
from typing import Dict, List, Set, Tuple
import clang.cindex

class CacheDB:
    @staticmethod
    def mkentries (files: List[str]):
        payload = set()

        for file in files:
            with open(file, "r") as _f:
                text = _f.read()
            payload.add(file + ":" + hashlib.sha256(text.encode()).hexdigest())
        
        return sorted(list(payload))
    
    @staticmethod
    def cache_file ():
        return os.path.join(os.path.dirname(__file__), "cache")
    @staticmethod
    def load (files: List[str]):
        try:
            with open(CacheDB.cache_file(), "r") as file:
                import json
                data = json.loads(file.read())
                if data["key"] == CacheDB.mkentries(files):
                    return data["content"]
                return None
        except Exception:
            return None

    @staticmethod
    def save (files: List[str], content):
        try:
            import json
            with open(CacheDB.cache_file(), "w") as file:
                file.write(json.dumps({
                    "key": CacheDB.mkentries( files ),
                    "content": content
                }))
        except Exception:
            pass

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

                    if len(token_spellings) == 0:
                        return
                    
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
    printcerr("   [+] Generate payload of", filename)

    def get_cache_index ():
        return 

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

    is_first = True

    def generate_value_check (name, value):
        nonlocal is_first
        prefix = "" if is_first else "else "
        is_first = False
        return f"\t{prefix}if (((int) value) == {value}) stream << \"{name},\";"
    def generate_define ():
        name = generate_dumper_function_definition(typename, qualname)

        if len(values) == 0:
            return f"{name} " + "{\n" + "\tstream << \"<EMPTY>,\";\n}"; 

        lines = [
            f"{name} " + "{",
        ] + list(map(lambda x : generate_value_check(x[0], x[1]), values)) + [
            "\telse stream << \"<UNKNOWN>\";",
            "}"
        ]
        return "\n".join(lines)

    header = f"{generate_dumper_function_definition(typename, qualname)};"
    define = generate_define()

    return header, define

BASE_HEADER = [
    f"#include <iostream>",
    f"#include <fstream>",
    f"#include <sstream>",
    f"#include <cstdint>",
    f"{generate_dumper_function_definition('int')};",
    f"{generate_dumper_function_definition('unsigned int')};",
    f"{generate_dumper_function_definition('unsigned')};",
    f"{generate_dumper_function_definition('float')};",
    f"{generate_dumper_function_definition('double')};",
    f"{generate_dumper_function_definition('uint8_t')};",
    f"{generate_dumper_function_definition('bool')};",
    f"{generate_dumper_function_definition('uint32_t')};"
]

DEFINE_SIMPLE_STREAM = "{ stream << value << \",\"; }"
BASE_DEFINE = [
    f"{generate_dumper_function_definition('int')}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition('unsigned int')}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition('unsigned')}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition('float')}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition('double')}" + DEFINE_SIMPLE_STREAM,
    f"{generate_dumper_function_definition('uint8_t')}" + "{ stream << ((int) value) << \",\"; }",
    f"{generate_dumper_function_definition('uint32_t')}" + "{ stream << ((long long) value) << \",\"; }",
    f"{generate_dumper_function_definition('bool')}" + " {\n\tif(value) { stream << \"true,\"; } else { stream << \"false,\"; } }"
]

def generate_xmacros (structs: Dict[str, List[Tuple[str, str]]], enums: Dict[str, Tuple[int, List[Tuple[str, int]]]]):
    headers = list(BASE_HEADER)
    defines = list(BASE_DEFINE)

    top_sort = []
    top_sort_set = set()

    def topological_sort (node: str):
        nonlocal top_sort, top_sort_set
        if node in top_sort_set: return
        if node not in structs: return
        top_sort_set.add(node)

        for name, type in structs[node]:
            if type.startswith("struct "):
                type = type[7:]
            topological_sort(type)

        top_sort.append(node)
    for key in structs:
        topological_sort(key)

    for key in enums:
        headers.append("enum " + key + " {")

        params, size, _key = enums[key]
        for name, value in params:
            headers.append(f"\t{name} = {value},")
        headers.append("};")
    for key in top_sort:
        headers.append("struct " + key + "{")

        for name, type in structs[key]:
            if type.startswith("struct "):
                type = type[7:]
            headers.append(f"\t{type} {name};")
        headers.append("};")

    for key in structs:
        header, define = generate_csv_dumper_struct(key, structs[key])

        headers.append(header)
        defines.append(define)
    for key in enums:
        header, define = generate_csv_dumper_enum(key, enums[key])

        headers.append(header)
        defines.append(define)

    return "\n".join(headers + defines)

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
	std::ifstream stream("%(file)s", std::ios_base::binary);

	std::cout << "%(scheme)s\\n";

	while (1) {
		stream.read(buffer, sizeof(%(typename)s));
		if (stream.eof() || stream.fail()) break ;

		std::ostringstream strm;
		dump_csv_%(typename)s(dump, strm);
		
		std::string res = strm.str();
		res[res.size() - 1] = '\\n';
		std::cout << res;
	}

	stream.close();
}
"""
def generate_main (struct: str, structs: Dict[str, List[Tuple[str, str]]], file: str):
    scheme = csv_header = ",".join(compile_csv_header( struct, structs ))

    return TEMPLATE.replace( "%(typename)s", struct ) \
        .replace( "%(scheme)s", scheme ) \
        .replace( "%(file)s", file )

def main (target: str, struct_name: str, file: str):
    printcerr(" [+] Finding all headers")
    headers = find_all_headers(target)

    cached = CacheDB.load(headers)

    all_structs: Dict[str, List[Tuple[str, str]]]             = {}
    all_enums  : Dict[str, Tuple[int, List[Tuple[str, int]]]] = {}

    if cached is None:
        for header in headers:
            structs, enums = find_dumpable_of_header(header)

            for key in structs.keys():
                all_structs[key] = structs[key]
            
            for key in enums.keys():
                all_enums[key] = enums[key]

        CacheDB.save( headers, [ all_structs, all_enums ] )
    else:
        printcerr(" [+] Cache hit on headers")
        all_structs, all_enums = cached
    
    filter_useful(struct_name, all_structs, all_enums)

    xmacros = generate_xmacros(all_structs, all_enums)
    main = generate_main( struct_name, all_structs, file )

    code = xmacros + "\n" + main

    printcerr(" [+] Setting up temporary directory...")
    with tempfile.TemporaryDirectory(prefix = os.getcwd() + os.path.sep) as dir:
        target = os.path.basename(dir)

        main_file = os.path.join(".", target, "main")
        code_file = os.path.join(".", target, "code.cpp")

        printcerr("   [+] Using directory", target)
        printcerr(" [+] Dumping macro code...")
        with open(code_file, "w") as file:
            file.write(code)
        with open("code.cpp", "w") as file:
            file.write(code)
        
        printcerr(" [+] Compiling dump2csv...")
        subprocess.run([ "g++", "-o", main_file, code_file ])
        
        printcerr(" [+] Running dump2csv")
        subprocess.run([ main_file ])
        
if __name__ == "__main__":
    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print("Usage: python dump2csv.py <header | folder> <struct> <dump>")
        print("  If dump isn't defined then dump will be log.txt")
        print("  The dump is written into the standard output")
        print()
        print("  For example, if you are in folder src/post_flight, you may want to do")
        print("    python3 dump2csv.py ../../ DataDump ~/log.txt > res.csv")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) >= 4 else "log.txt")
