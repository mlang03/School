#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include "vcutil.h"

/* extend.c

Extension functions for python-to-c calling

Created: mar 15 2014
Author: Michael Lang 0733368
Contact: mlang03@uoguelph.ca

*/

static VcFile * openFile;
static int n = 0;


static PyObject *Vcf_readFile(PyObject *self, PyObject *args){
    const char *filename;
    PyObject *rtn;
    //If args is not of a "s" object, so in this case string, from Python, return NULL
    if (!PyArg_ParseTuple(args, "s", &filename)){
        return NULL;
    }
    VcStatus status;
    FILE * inFile;
    inFile = fopen(filename,"r");
    openFile = malloc(sizeof(VcFile));
    status = readVcFile(inFile,openFile);
    fclose(inFile);
    rtn = Py_BuildValue("i",status.code);
    n = 0;
    getUnfolded(NULL,NULL);
    //If everything goes well, return SYS as a python long HINT: Will be 0 if good
    return rtn;//PyLong_FromLong(sts);
}

static PyObject *Vcf_writeFile(PyObject *self, PyObject *args){
    const char *filename;
    PyObject *rtn;
    PyObject *cards;
    //If args is not of a "s" object, so in this case string, from Python, return NULL
    if (!PyArg_ParseTuple(args, "sO", &filename,&cards)){
        return NULL;
    }
    
      if (PyList_Size(cards) == 0)
          return Py_BuildValue("No");
    VcStatus status;
    
    VcFile toSave;
    toSave.ncards = 0;
    Vcard * temp;
    PyObject * tempCard;
    PyObject * tuple;
    toSave.cardp = malloc(sizeof(Vcard*));
    VcProp prop;
    VcProp * tempProp;
    int j = 0;
    int i = 0;
    int maxCard = PyList_Size(cards);
    while(i < maxCard){
        tempCard = (PyObject *) PyList_GetItem(cards,i);
        toSave.cardp = realloc(toSave.cardp,sizeof(Vcard*)*(i+1));
        temp = malloc(sizeof(Vcard) + sizeof(VcProp)*1);
        temp->nprops = 0;
        toSave.ncards++;
        int maxProp = PyList_Size(tempCard);
        while(j < maxProp){
            tuple = (PyObject *) PyList_GetItem(tempCard,j);
            temp = realloc(temp,sizeof(Vcard) + sizeof(VcProp)*(j+1));
            PyArg_ParseTuple(tuple, "izzz",&prop.name,&prop.partype,&prop.parval,&prop.value);
            copyProp(&(temp->prop[j]),&prop);
            temp->nprops++;
            j++;
        }
        toSave.cardp[i] = temp;
        i++;
        j = 0;
    }
    FILE * outFile;
    outFile = fopen(filename,"w");
    status = writeVcFile(outFile,&toSave);
    //if (status.code != OK) return Py_BuildValue("");
    //fprintf(outFile,"%d %s %s %s",prop.name,prop.partype,prop.parval,prop.value);
    fclose(outFile);
    //rtn = Py_BuildValue("i",status.code);

    //If everything goes well, return SYS as a python long HINT: Will be 0 if good
    return Py_BuildValue("OK");
}

static PyObject *Vcf_getCard(PyObject *self, PyObject *args){
    PyObject *card;
    PyObject *temp;
    //If args is not of a "s" object, so in this case string, from Python, return NULL
    if (!PyArg_ParseTuple(args, "O", &card)){
        return NULL;
    }
    if (n == openFile->ncards){
        getUnfolded(NULL,NULL);
        free(openFile->cardp);
        openFile->ncards = 0;
        free(openFile);
        return PyLong_FromLong(1);
    }
    else{
        Vcard * vcard = openFile->cardp[n];
        int p = vcard->nprops;
        int i,j;
        for (i = 0; i < p; i++){
            temp = Py_BuildValue("i s s s",vcard->prop[i].name, vcard->prop[i].partype, vcard->prop[i].parval, vcard->prop[i].value);
            PyList_Append(card,temp);
        }
    }
    n++;
    
    
    //If everything goes well, return SYS as a python long HINT: Will be 0 if good
    return PyLong_FromLong(0);
}

//Module Method Definition
static PyMethodDef vcfMethods[] = {
    //this is our list of functions we can perform in this module
    {"readFile",  Vcf_readFile, METH_VARARGS,"Reads a Vcard File."},
    {"getCard",  Vcf_getCard, METH_VARARGS,"Gets a Vcard."}, 
    {"writeFile",  Vcf_writeFile, METH_VARARGS,"Writes a Vcard File."}, 
    {NULL, NULL, 0, NULL}    
    /* Sentinel */
};

//Module Header Definition
static struct PyModuleDef VCFmodule = {
   PyModuleDef_HEAD_INIT,
   "Vcf",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   vcfMethods /* uses the Module Method Definition of "SpamMethods" declared above */
};


PyMODINIT_FUNC //Needed

PyInit_Vcf(void){
    //creates the module with "spammodule" as the module header 
    (void) PyModule_Create(&VCFmodule);
}
