#include <stdio.h>

#include "utils.h"
#include "parse.h"

int main(int argc, char* argv[])
{
    if(argc < 5 || argc > 9)
        usageExit(argv[0], "-d <int> -c <int> [-b <path> -t <path>]");

    int dataSig = -1;
    int cmdSig = -1;
    char* binPath = NULL;
    char* txtPath = NULL;

    parseArgs(argc, argv, &dataSig, &cmdSig, &binPath, &txtPath);

    //printf("DATA: %d\nCMD: %d\nBIN: %s\nTXT: %s\n", dataSig, cmdSig, binPath, txtPath);

    return 0;
}