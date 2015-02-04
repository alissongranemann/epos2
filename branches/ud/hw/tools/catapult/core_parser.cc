
#include<fstream>
#include<iostream>
using namespace std;


const char core_tokens[11] = {'v','o','i','d',' ','c','o','r','e','(',')'};
unsigned char core_state = 0;
bool core_sm(char c){
    if(core_tokens[core_state] == c){
        ++core_state;
    }
    else{
        core_state = 0;
    }
    if(core_state == 11){
        core_state = 0;
        return true;
    }
    else return false;
}


int scope_cnt = 0;
bool scope_start = false;
bool scope_check(char ch){
    if(ch == '{') {++scope_cnt; scope_start = true;}
    else if(ch == '}') {--scope_cnt;}

    bool aux = (scope_cnt == 0) && scope_start;
    scope_start = aux ? false : scope_start;

    return aux;
}

int main(int argc, char *argv[]){

    char *cxx_file_name = argv[1];
    char *h_file_name = argv[2];
    char *cc_file_name = argv[3];
    char *include_name = argv[4];
    char *namespace_name = argv[5];
    char *class_name = argv[6];


    fstream f_cxx(cxx_file_name, fstream::in);
    fstream f_h(h_file_name, fstream::out);
    fstream f_cc(cc_file_name, fstream::out);

    char ch;
    bool on_core = false;
    bool out_core = false;
    while (f_cxx >> noskipws >> ch) {

        if(on_core){
            f_cc << ch;

            if(scope_check(ch)){
                f_cc << "\n}\n\n";
                out_core = true;
                on_core = false;
            }
        }
        else if(out_core){
            f_h << ch;
        }
        else {
            f_h << ch;
            if(core_sm(ch)){
                on_core = true;

                f_cc << "#include \"" << include_name << "\"\n\n";

                f_cc << "namespace "<< namespace_name <<" {\n\n";

                f_cc << "void ";
                f_cc << class_name;
                f_cc << "::core()";

                f_h << ";\n";
            }
        }
    }


};
