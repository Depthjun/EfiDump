#include "argtable3.h"
#include <chrono>
#include "log.h"
#include "dumper.h"


struct arg_lit *verb, *help, *about;
struct arg_int *processId, *bufferSize;
struct arg_str *moduleName, *outputFile;
struct arg_end *end;

int main(int argc, char *argv[]) 
{
    void *argtable[] = {
        help        = arg_litn("h", "help", 0, 1, "display this help and exit"),
        about       = arg_litn("a", "about", 0, 1, "display information and exit"),
        verb        = arg_litn("v", "verbose", 0, 1, "verbose output"),
        processId   = arg_intn("p", "pid", "<pid>", 1, 1, "ID of target process"),
        moduleName  = arg_strn("m", "module", "<module>", 1, 1, "name of module to dump"),
        outputFile  = arg_strn("o", "output", "<output>", 1, 1, "path for output file"),
        bufferSize  = arg_intn("s", "size", "<size>", 1, 1, "size of buffer"),
        end         = arg_end(20)
    };

    char progname[] = "efidump.exe";
    int nerrors;
    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Windows process memory dumper using EFI runtime driver.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        
        return 0;
    }

    if (about->count > 0)
    {
        printf("Made by Samuel Tulach\n");
        printf("(https://github.com/SamuelTulach)\n");
        printf("Binaries of this program are under WTFPL license\n");
        printf("(https://en.wikipedia.org/wiki/WTFPL)\n");
        
        return 0;
    }

    if (nerrors > 0)
    {
        arg_print_errors(stdout, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        return 1;
    }

    if (verb->count > 0) 
    {
        LogEnabled = true;
        log::Print("Verbose logging enabled\n");
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    dumper::DumpProcess(*processId->ival, *moduleName->sval, *outputFile->sval, *bufferSize->ival);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto durationS = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
    
    printf("Execution time: %I64us\n", durationS);
    
    return 0;
}