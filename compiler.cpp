#include "./json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

void define_enum(json& j, std::ofstream& file) {
    file << "typedef enum " << j["name"].get<std::string>() << "{\n";
    for (auto& el: j["member"].items()) {
        auto& k = el.key();
        auto& v = el.value();
        file << "    " << k << " = " << v << ",\n";
    }
    file << "} "<< j["name"].get<std::string>() << ";\n\n";
}

std::string get_primitive_type(json& j, bool ignore_repeated) {
    std::string type = j["type"].get<std::string>();
    std::string property = j["property"].get<std::string>();
    if (!ignore_repeated && property == "repeated") {
        std::string element_primitive_type = get_primitive_type(j, true);
        return "Array<" + element_primitive_type + ", " + type + ">";
    }

    if(type == "INT_32") return "int32_t";
    else if(type == "INT_64") return "int64_t";
    else if(type == "UINT_32") return "uint32_t";
    else if(type == "UINT_64") return "uint64_t";
    else if(type == "SINT_32") return "int32_t";
    else if(type == "SINT_64") return "int64_t";
    else if(type == "BOOL") return "bool";
    else if(type == "FIXED_64") return "uint64_t";
    else if(type == "SFIXED_64") return "int64_t";
    else if(type == "DOUBLE") return "double";
    else if(type == "FIXED_32") return "uint32_t";
    else if(type == "SFIXED_32") return "int32_t";
    else if(type == "FLOAT") return "float";
    else return type;
};

std::string get_origin_type(json& j) {
    std::string type = j["type"].get<std::string>();
    std::string property = j["property"].get<std::string>();

    if(property == "repeated") return "ARRAY";
    
    if(type == "INT_32") return "INT_32";
    else if(type == "INT_64") return "INT_64";
    else if(type == "UINT_32") return "UINT_32";
    else if(type == "UINT_64") return "UINT_64";
    else if(type == "SINT_32") return "SINT_32";
    else if(type == "SINT_64") return "SINT_64";
    else if(type == "BOOL") return "BOOL";
    else if(type == "FIXED_64") return "FIXED_64";
    else if(type == "SFIXED_64") return "SFIXED_64";
    else if(type == "DOUBLE") return "DOUBLE";
    else if(type == "FIXED_32") return "FIXED_32";
    else if(type == "SFIXED_32") return "SFIXED_32";
    else if(type == "FLOAT") return "FLOAT";
    else return "MESSAGE";
};

void define_class(json& j, std::ofstream& file) {
    file << "class " << j["name"].get<std::string>() << "{\n"
         << "public:\n";
    file << "    " << j["name"].get<std::string>() << "() {}\n";
    // define member;
    for (auto& el : j["member"].items()) {
        auto& v = el.value();
        auto& k = el.key();
        file << "    " << get_primitive_type(v, false) << " " << k << ";\n";
    }
    // define do_pack_size;
    file << "    size_t do_pack_size() const {\n";
    file << "        size_t payload_size = 0;\n";
    for (auto& el : j["member"].items()) {
        auto& v = el.value();
        auto& k = el.key();
        file << "        payload_size += pack_size<INT_32>(generate_key<INT_32>(" << v["field_number"] << "));\n";
        file << "        payload_size += pack_size<" << get_origin_type(v) << ">(" << k << ");\n";
    }
    file << "        return payload_size;\n";
    file << "    };\n\n";

    // define do_pack;
    file << "    size_t do_pack(uint8_t* buf) const {\n";
    file << "        size_t payload_size = 0;\n";
    for (auto& el : j["member"].items()) {
        auto& v = el.value();
        auto& k = el.key();
        file << "        payload_size += pack_key<INT_32>(buf + payload_size, " << v["field_number"] << ");\n";
        file << "        payload_size += pack<" << get_origin_type(v) << ">(buf + payload_size, " << k << ");\n";
        
    }
    file << "        return payload_size;\n";
    file << "    };\n\n";

    // define do_unpack;
    file << "    size_t do_unpack(uint8_t* buf, size_t& payload_size){\n";
    file << "        uint32_t field_number = 0;\n";
    file << "        size_t offset = 0;\n";
    for (auto& el : j["member"].items()) {
        auto& v = el.value();
        auto& k = el.key();
        file << "        offset += unpack_key<INT_32>(buf + offset, field_number);\n";
        file << "        offset += unpack<" << get_origin_type(v) << ">(buf + offset, " << k << ");\n";
        
    }
    file << "        return payload_size;\n";
    file << "    };\n\n";

    // enclose class definition
    file << "};\n\n";
}

void define_include(std::ofstream& file) {
    file << "#include <pack.hpp>\n\n";
    file << "using namespace navi_trans;\n\n";
}

void generate(json& j, char* output_dir) {
    std::ofstream myfile (std::string(output_dir) + "/message_" + j["name"].get<std::string>() + ".hpp");
    define_include(myfile);
    for (auto& el : j.items()) {
        if (el.key() == "message") {
            define_class(el.value(), myfile);
        }else if (el.key() == "enum") {
            define_enum(el.value(), myfile);
        }
    }
    define_class(j, myfile);
    myfile.close();
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "usage: compiler [proto_file] [output_directory]";
        exit(0);
    }

    char* filename = argv[1];
    std::ifstream myfile (filename);
    if (myfile.is_open())
    {
        json j;
        myfile >> j;
        generate(j, argv[2]);
        myfile.close();
    } else {
        std::cout << "can not open file " + std::string(filename) << "\n";
    }
}