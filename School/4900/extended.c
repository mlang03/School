#include <Python.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_ALG 21233664

/*
 * 22113121110011 1103
Found 22113122110001 1104
Found 22113122110011 1105
Found 23111211110011 1106
Found 23111212110011 1107
Found 23111221110011 1108
Found 23111222110001 1109
Found 23111222110011 1110
Found 23111311110011 1111
Found 23111312110011 1112
Found 23111321110011 1113
Found 23111322110001 xxxx
*/

char sizes[] = {4,4,4,4,4,4,3,3,4,4,3,3,2,2};
int size3[] = {0,0,1,2,3,4};
int used[] = {0,0,0,0,0,0};
int num = 0;
void genAlgorithms(char * string, int t, int n);
int genTestConfigs(char * string, int t, int n, int numZero,char * algo);
PyObject *pName, *pModule,*pFunc, *pArgs, *pString1,*pString2,*pValue;

int main(int argc, char * argv[]){
    //Initialize python
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
    
    //import module
    pName = PyUnicode_FromString("sorter");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    
    
    if (pModule != NULL){
        pFunc = PyObject_GetAttrString(pModule,"simulate");
    }
    else{
        printf("Error importing python\n");
    }
    
    int i;
    int n = 3;
    char * string = calloc(sizeof(char),15);
    genAlgorithms(string, 0, 12);
    printf("%d\n",num);
    free(string);
    Py_Finalize();
    return 0;
}

int testStub(char * algo, char * config){
    //num = num + 1;
    //if (num%100000 == 0)
    //    printf("%s %s %d\n",algo,config,num);
    
    pArgs = PyTuple_New(2);
    pString1 = Py_BuildValue("s",algo);
    pString2 = Py_BuildValue("s",config);
    PyTuple_SetItem(pArgs,0,pString1);
    PyTuple_SetItem(pArgs,1,pString2);
    pValue = PyObject_CallObject(pFunc,pArgs);
    int ret;
    PyArg_Parse(pValue,"i",&ret);
    Py_DECREF(pArgs);
    Py_DECREF(pValue);
    if (ret == 1){
        return 1;
    }
    else{
        return 0;
    }
    //Py_DECREF(pString1);
    //Py_DECREF(pString2);

}

int genTestConfigs(char * string, int t, int n, int numZero,char * algo){
    int i;
    if (t > n + 1){
        if (testStub(algo,string) == 0) return 0;
    }
    else{
        for (i = 0; i < n+1; i++){
            if (i == 0){
                if (numZero == 0){
                    string[t] = '0';
                    if (genTestConfigs(string,t+1,n,numZero+1,algo) == 0)
                        return 0;
                }
                else if (numZero == 1){
                     if (t - (strchr(string,'0')-string) > 2 ){
                            continue;
                     }
                     else{
                        string[t] = '0';
                        if (genTestConfigs(string,t+1,n,numZero+1,algo) == 0)
                            return 0;
                     }
                }
                else continue;
            }
            else{
                if (used[i-1] == 1) continue;
                used[i-1] = 1;
                string[t] = i + '0';
                if (genTestConfigs(string,t+1,n,numZero,algo) == 0)
                    return 0;
                used[i-1] = 0;
            }
        }
    }
    return 1;
}

void genAlgorithms(char * string, int t, int n){
    int i;
    if ( t > n + 1){
        //printf("%s\n",string);
        //num = num + 1;
        char * string2 = calloc(sizeof(char),10);
        if (genTestConfigs(string2,0,4,0,string) == 1){
            printf("Found %s %d\n",string,num);
            num = num+1;
        }
        free(string2);
        for ( i = 0; i < 5; i++) used[i] = 0;
    }
    else{
        for (i = 0; i < sizes[t]; i++){
            string[t] = i + '0';
            genAlgorithms(string,t+1,n);
        }
    }
}