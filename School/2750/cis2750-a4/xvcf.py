#!/usr/bin/python3

""" xvcf.py

GUI for vcards reading and editing Vcards, equipped with SQL database storage

Created (A3): mar 10 2014
Edited (A4): april 4 2014
Author: Michael Lang 0733368
Contact: mlang03@uoguelph.ca

"""

from tkinter import *
from tkinter import filedialog
from tkinter.messagebox import *
from GMapData import *
import os.path
import tkinter.scrolledtext as tkst
import MultiListbox
import Vcf
import copy
import mysql.connector
from mysql.connector import errorcode
import sys

def vCardPropNametoString(name):
    if name == 3:
        return "Name"
    if name == 4:
        return "Full Name"
    if name == 5:
        return "Nickname"
    if name == 6:
        return "Photo"
    if name == 7:
        return "Birthday"
    if name == 8:
        return "Address"
    if name == 9:
        return "Label"
    if name == 10:
        return "Telephone"
    if name == 11:
        return "Email"
    if name == 12:
        return "Geo"
    if name == 13:
        return "Title"
    if name == 14:
        return "Organization"
    if name == 15:
        return "Note"
    if name == 16:
        return "UID"
    if name == 17:
        return "URL"
    if name == 18:
        return "Other"
        
def vCardPropNametoStringSHORT(name):
    if name == 3:
        return "NAME"
    if name == 4:
        return "FULL N"
    if name == 5:
        return "NICK N"
    if name == 6:
        return "PHOTO"
    if name == 7:
        return "BDAY"
    if name == 8:
        return "ADR"
    if name == 9:
        return "LABEL"
    if name == 10:
        return "TEL"
    if name == 11:
        return "EMAIL"
    if name == 12:
        return "GEO"
    if name == 13:
        return "TITLE"
    if name == 14:
        return "ORG"
    if name == 15:
        return "NOTE"
    if name == 16:
        return "UID"
    if name == 17:
        return "URL"
    if name == 18:
        return "OTHER"
        
def nameToNum(name):
    if name == "NAME":
        return 3
    if name == "FULL N":
        return 4
    if name == "NICK N":
        return 5
    if name == "PHOTO":
        return 6
    if name == "BDAY":
        return 7
    if name == "ADR":
        return 8
    if name == "LABEL":
        return 9
    if name == "TEL":
        return 10
    if name == "EMAIL":
        return 11
    if name == "GEO":
        return 12
    if name == "TITLE":
        return 13
    if name == "ORG":
        return 14
    if name == "NOTE":
        return 15
    if name == "UID":
        return 16
    if name == "URL":
        return 17
    if name == "OTHER":
        return 18
    

class xvcfGui():
    'Gui for Vcards'
    def __init__(self,cnx):
        self.mainArea = None
        self.fvp_mlb = None
        self.cvp_mlb = None
        self.cnx = cnx
        self.log = None
        self.cardList = []
        self.mapPoints = []
        self.revert = []
        self.fileIndex = 0
        self.filename = None
        self.upToDate = 1 #Means the current version is up do date i.e nothing changes when u revert
        self.queryOpen = 0
        self.conflict = 0
        self.conflicts = []
        self.filemenu = None
        self.datamenu = None
        pass
    
    def quitProg(self):
        if not self.upToDate:
            if askyesno("Quit?", "You have uncommitted changes. Are you sure you want to quit?"):
                self.mainArea.quit()
        else:
            if askyesno("Quit?", "Are you sure you want to quit?"):
                self.mainArea.quit()
            #killServers()
    
    def openFile(self,append="FALSE"):
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        if self.filename == "xvcf" and append != "FALSE":
            return
        openFile = filedialog.askopenfilename()
        if not openFile:
            self.writeLogBox("No file selected.\n")
            return
        self.filename = openFile
        status = Vcf.readFile(openFile)
        if (status != 0):
            showerror("Error.","No cards could be found in that file.");
            self.writeLogBox("Failed to read file, error: ")
            self.writeLogBox(str(status) + '\n')
            self.writeLogBox(openFile + '\n')
            return
        else:
            if append == "FALSE":
                self.writeLogBox("Succesfully opened file.\n")
            else:
                self.writeLogBox("Succesfully appended file.\n")
            self.writeLogBox(openFile + '\n')
        self.filemenu.entryconfig(2,state=NORMAL)
        self.datamenu.entryconfig(0,state=NORMAL)
        self.datamenu.entryconfig(1,state=NORMAL)
        self.upToDate = 1
        self.fvp_mlb.delete(0,END) #clearfvp
        self.cvp_mlb.delete(0,END) #clearcvp
        self.mainArea.title(openFile)
        if append == "FALSE":
            self.cardList = []
        card = []
        p = 0
        i = 0
        while (p == 0):
            p = Vcf.getCard(card)
            if (p == 0):
                self.cardList.append(card)
                i = i + 1
            card = []
        self.revert = copy.deepcopy(self.cardList)
        self.fileIndex = 0
        self.writeFVP()
        self.writeCVP(self.cardList[0])
        self.fvp_mlb.selection_set(0)
        self.cvp_mlb.selection_set(0)
        
    def saveFile(self,saveAs = "FALSE"):
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        if not self.cardList:
            self.writeLogBox("Can not save empty file.\n")
            return
        self.filemenu.entryconfig(2,state=NORMAL)
        if saveAs == "TRUE":
            oldfilename = self.filename
            self.filename = filedialog.asksaveasfilename()
            if not self.filename:
                self.filename = oldfilename
                self.writeLogBox("No file selected.\n")
                return
            self.mainArea.title(self.filename)
        out = Vcf.writeFile(self.filename,self.cardList)
        #print (out)
        self.writeLogBox("Saved file.\n")
        self.writeLogBox(self.filename + "\n")
    
    def writeFVP(self):
        for k in range(len(self.cardList)):
            FN = ''
            REGION = ''
            COUNTRY = ''
            FLAGS = ''
            UID = ''
            NFN = 0
            NN = 0
            ADRC = 0
            TELC = 0
            NURL = 0
            NPHOTO = 0
            NGEO = 0
            for m in range(len(self.cardList[k])):
                if (self.cardList[k][m][0] == 4):
                    FN = self.cardList[k][m][3]
                    NFN = NFN + 1
                if (self.cardList[k][m][0] == 3):
                    NN = NN + 1
                if (self.cardList[k][m][0] == 10):
                    TELC = TELC + 1
                if (self.cardList[k][m][0] == 6):
                    NPHOTO = NPHOTO + 1
                if (self.cardList[k][m][0] == 17):
                    NURL = NURL + 1
                if (self.cardList[k][m][0] == 12):
                    NGEO = NGEO + 1
                if (self.cardList[k][m][0] == 8):
                    REGION = self.cardList[k][m][3]
                    COUNTRY = self.cardList[k][m][3]
                    ADRC = ADRC + 1
                if (self.cardList[k][m][0] == 16):
                    UID = self.cardList[k][m][3]
            if (UID != '' and UID[0] == '@' and UID[1] != '*' and UID[2] != '*' and UID[3] != '*' and UID[4] != '*' and UID[5] == '@'):
                FLAGS += 'C'
            else:
                FLAGS += '-'
            if (NFN > 1 or NN > 1):
                FLAGS += 'M'
            else:
                FLAGS += '-'
            if (NURL > 0):
                FLAGS += 'U'
            else:
                FLAGS += '-'
            if (NPHOTO > 0):
                FLAGS += 'P'
            else:
                FLAGS += '-'
            if (NGEO > 0):
                FLAGS += 'G'
            else:
                FLAGS += '-'
            self.fvp_mlb.insert(END, (k+1,FN,REGION,COUNTRY,ADRC,TELC,FLAGS))
            if k == self.fileIndex:
                if 'M' in FLAGS:
                    self.fvp_mlb.lists[6].config(bg='red')
                elif 'C' in FLAGS:
                    self.fvp_mlb.lists[6].config(bg='green')
                else:
                    self.fvp_mlb.lists[6].config(bg='yellow')
            
    def addCard(self):
        if self.filename == "xvcf":
            return
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        card = []
        Nprop = (3,None,None,"Doe;John")
        FNprop = (4,None,None,"John Doe")
        card.append(Nprop)
        card.append(FNprop)
        self.datamenu.entryconfig(0,state=NORMAL)
        self.datamenu.entryconfig(1,state=NORMAL)
        self.cardList.append(card)
        self.revert.append(card)
        self.fvp_mlb.delete(0,END)
        self.cvp_mlb.delete(0,END)
        self.writeFVP()
        self.writeCVP(self.cardList[len(self.cardList)-1])
        self.fileIndex = len(self.cardList) - 1
        self.fvp_mlb.selection_set(self.fileIndex)
        self.cvp_mlb.selection_set(0)
        self.upToDate = 1
    
    def deleteCard(self):
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        if not self.cardList:
            return
        if (len(self.cardList) == 1):
            self.writeLogBox("There must be at least 1 card to display.\n")
            return
        if self.fvp_mlb.curselection() == ():
            return
        index = (int)(self.fvp_mlb.curselection()[0])
        if not askokcancel("Delete?","Are you sure you want to delete card %d?" % (index+1)):
            return
        deleted = self.cardList.pop(index)
        self.fvp_mlb.delete(0,END)
        self.cvp_mlb.delete(0,END)
        self.writeFVP()
        self.writeCVP(self.cardList[index-1])
        self.fileIndex -= 1
        self.fvp_mlb.selection_set(self.fileIndex)
        self.cvp_mlb.selection_set(0)
        self.upToDate = 1
            
            
    def writeCVP(self,card):
        for i in range(len(card)):
            if not card[i][1]:
                ptype = ""
            else:
                ptype = card[i][1]
            if not card[i][2]:
                pval = ""
            else:
                pval = card[i][2]
            if not card[i][3]:
                val = ""
            else:
                val = card[i][3]
            self.cvp_mlb.insert(END,(i+1,vCardPropNametoString(card[i][0]),ptype,pval,val))
            
    def moveProp(self,direction):
        if not self.cardList:
            return
        if self.cvp_mlb.curselection() == ():
            return
        index = (int)(self.cvp_mlb.curselection()[0])
        if (direction == "UP"):
            if (index > 0):
                prop = self.cardList[self.fileIndex].pop(index)
                self.cardList[self.fileIndex].insert(index-1,prop)
                self.cvp_mlb.delete(0,END)
                self.writeCVP(self.cardList[self.fileIndex])
                self.cvp_mlb.selection_set(index-1)
                self.upToDate = 0
            else:
                return
        if (direction == "DOWN"):
            if (index < len(self.cardList[self.fileIndex]) - 1):
                prop = self.cardList[self.fileIndex].pop(index)
                self.cardList[self.fileIndex].insert(index+1,prop)
                self.cvp_mlb.delete(0,END)
                self.writeCVP(self.cardList[self.fileIndex])
                self.cvp_mlb.selection_set(index+1)
                self.upToDate = 0
            else:
                return
       
            
            
    def select(self,y):
        if not self.cardList:
            return
        index = self.fvp_mlb.lists[0].nearest(y)
        if index == self.fileIndex:
            return
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        self.fileIndex = index
        self.fvp_mlb.delete(0,END)
        self.writeFVP()
        self.fvp_mlb._select(y)
        self.cvp_mlb.delete(0,END)
        self.writeCVP(self.cardList[index])
        self.cvp_mlb.selection_set(0)
        self.upToDate = 1
        return 'break'
    
    def writeLogBox(self,msg):
        self.log.config(state=NORMAL)
        self.log.insert(END,msg)
        self.log.config(state=DISABLED)
        
    def clearLogBox(self):
        self.log.config(state=NORMAL)
        self.log.delete(1.0,END)
        self.log.config(state=DISABLED)
        
    def revertChanges(self):
        if not self.cardList:
            return
        if not self.revert:
            self.cardList = []
            return
        self.upToDate = 1
        if self.revert[self.fileIndex]:
            self.cardList[self.fileIndex] = copy.deepcopy(self.revert[self.fileIndex])
        else:
            self.cardList[self.fileIndex] = []
        y1 = (int)(self.fvp_mlb.curselection()[0])
        y2 = (int)(self.cvp_mlb.curselection()[0])
        self.cvp_mlb.delete(0,END)
        self.fvp_mlb.delete(0,END)
        self.writeCVP(self.cardList[self.fileIndex])
        self.writeFVP()
        self.fvp_mlb.selection_set(y1)
        if (y2 >= len(self.cardList[self.fileIndex])):
            self.cvp_mlb.selection_set(len(self.cardList[self.fileIndex]) - 1)
        else:
            self.cvp_mlb.selection_set(y2)
    
    def commitChanges(self):
        if not self.cardList:
            return
        self.upToDate = 1
        self.revert[self.fileIndex] = copy.deepcopy(self.cardList[self.fileIndex])
        y1 = self.fvp_mlb.curselection()[0];
        y2 = self.cvp_mlb.curselection()[0];
        self.cvp_mlb.delete(0,END)
        self.fvp_mlb.delete(0,END)
        self.writeCVP(self.cardList[self.fileIndex])
        self.writeFVP()
        self.fvp_mlb.selection_set(y1)
        self.cvp_mlb.selection_set(y2)
        
    def editField(self,box,y):
        
        def enterEdit():
            prop = []
            for i in range(4):
                if i != box:
                    prop.append(self.cardList[self.fileIndex][index][i])
                else:
                    prop.append(editBox.get())
            propTuple = tuple(prop)
            self.cardList[self.fileIndex][index] = propTuple
            self.upToDate = 0
            y1 = self.fvp_mlb.curselection()[0]
            y2 = self.cvp_mlb.curselection()[0]
            self.cvp_mlb.delete(0,END)
            self.fvp_mlb.delete(0,END)
            self.writeCVP(self.cardList[self.fileIndex])
            self.writeFVP()
            self.fvp_mlb.selection_set(y1)
            self.cvp_mlb.selection_set(y2)
            editwin.destroy()
        
        if not self.cardList:
            return
        self.cvp_mlb._select(y)
        index = self.cvp_mlb.lists[box].nearest(y)
        if self.cvp_mlb.lists[1].get(index) == "Other":
            if box == 1 or box == 2:
                return
        editwin = Toplevel()
        editwin.title("Edit..")
        entryType = list(("Parameter Type:","Parameter Value:","Value:"))
        Label(editwin, text=entryType[box-1]).pack()
        editBox = Entry(editwin)
        editBox.pack()
        if self.cardList[self.fileIndex][index][box]:
            editBox.insert(0,self.cardList[self.fileIndex][index][box])
        Button(editwin,text="Enter.",command=enterEdit).pack()
        Button(editwin,text="Cancel.",command=editwin.destroy).pack()
        
        
    def addProp(self):
        
        def enterAddProp():
            propName = int(choicebox.curselection()[0])
            if propName > 10:
                propName += 1
            prop = (propName+5, None,None,None)
            self.cardList[self.fileIndex].append(prop)
            self.cvp_mlb.delete(0,END)
            self.writeCVP(self.cardList[self.fileIndex])
            self.fvp_mlb.delete(0,END)
            self.writeFVP()
            self.fvp_mlb.selection_set(self.fileIndex)
            self.cvp_mlb.selection_set(len(self.cardList[self.fileIndex])-1)
            addwin.destroy()
            self.upToDate = 0
        if not self.cardList:
            return
        addwin = Toplevel()
        addwin.title("Add Property..")
        choicebox = Listbox(addwin)
        choicebox.insert(1,"Nickname")
        choicebox.insert(2,"Photo")
        choicebox.insert(3,"Birthday")
        choicebox.insert(4,"Address")
        choicebox.insert(5,"Label")
        choicebox.insert(6,"Telephone")
        choicebox.insert(7,"Email")
        choicebox.insert(8,"Geo..")
        choicebox.insert(9,"Title")
        choicebox.insert(10,"Organization")
        choicebox.insert(11,"Note")
        choicebox.insert(12,"URL")
        choicebox.insert(13,"Other")
        choicebox.pack()
        choicebox.selection_set(0)
        Button(addwin,text="Enter.",command=enterAddProp).pack()
        Button(addwin,text="Cancel.",command=addwin.destroy).pack()

        
    def deleteProp(self):
        if not self.cardList:
            return
        if self.cvp_mlb.curselection() == ():
            return
        index = (int)(self.cvp_mlb.curselection()[0])
        if (self.cardList[self.fileIndex][index][0] == 3):
            numName = 0
            for prop in self.cardList[self.fileIndex]:
                if prop[0] == 3:
                    numName += 1
            if numName == 1:
                self.writeLogBox("Can not delete last name property.\n")
                return
        if (self.cardList[self.fileIndex][index][0] == 4):
            numFName = 0
            for prop in self.cardList[self.fileIndex]:
                if prop[0] == 4:
                    numFName += 1
            if numFName == 1:
                self.writeLogBox("Can not delete last full name property.\n")
                return 
        self.cardList[self.fileIndex].pop(index)
        self.cvp_mlb.delete(0,END)
        self.writeCVP(self.cardList[self.fileIndex])
        if index > 1:
            self.cvp_mlb.selection_set(index-1)
        else:
            self.cvp_mlb.selection_set(0)
        self.upToDate = 0
    
    def webServer(): #temp name
        mapp = GMapData("Map","Mapping",[45.530318,-80.223241],15)
        portnum = 43368
        startWebServer(portnum)
        
    def OnMouseDown(self,other):
        print("clickity click")
        
    def colourInfo(self):
        showinfo("Color","Green: Card is canon.\nRed: Card needs fixing.\nYellow: default.")
        
    def showAbout(self):
        showinfo("About xvcf","Created by Mike \'OTL\' Lang.\n Compatible with Vcard 3.0.")
        
    def openDB(self,APPEND="FALSE"):
        cursor = self.cnx.cursor()
        query = "SELECT name,name_id FROM NAME;"
        cursor.execute(query)
        nameids = []
        for line in cursor:
            nameids.append(line)
        if not nameids:
            self.writeLogBox("Database empty.\n")
            return
        clist = []
        if APPEND != "FALSE":
            clist = self.cardList
        for nameid in nameids:
            query = ("SELECT pname,partype,parval,value FROM PROPERTY "
            "WHERE name_id = '{}';".format(nameid[1]))
            cursor.execute(query)
            card = []
            card.append((3,None,None,nameid[0]))
            for line in cursor:
                partype = line[1]
                parval = line[2]
                value = line[3]
                if partype == "":
                    partype = None
                if parval == "":
                    parval = None
                if value == "":
                    value = None
                prop = (nameToNum(line[0]), partype, parval, value)
                card.append(prop)
            clist.append(card)
        if APPEND == "FALSE":
            self.cardList = clist
        self.fvp_mlb.delete(0,END) #clearfvp
        self.cvp_mlb.delete(0,END) #clearcvp
        self.revert = copy.deepcopy(self.cardList)
        self.fileIndex = 0
        self.upToDate = 1
        self.writeFVP()
        self.writeCVP(self.cardList[0])
        self.fvp_mlb.selection_set(0)
        self.cvp_mlb.selection_set(0)
        self.mainArea.title("Database")
        self.filename = "Database"
        self.writeLogBox("Database opened.\n")
        self.filemenu.entryconfig(2,state=DISABLED)
        self.datamenu.entryconfig(0,state=NORMAL)
        self.datamenu.entryconfig(1,state=NORMAL)
        
    def query(self):
        def clear():
            qtext.config(state=NORMAL)
            qtext.delete(1.0,END)
            qtext.config(state=DISABLED)
            
        def runQuery():
            querymsg = str(qbox.get())
            cursor = self.cnx.cursor()
            try:
                cursor.execute(querymsg)
            except mysql.connector.Error as err:
                qtext.config(state=NORMAL)
                qtext.insert(END,err.msg +  "\n")
                qtext.insert(END,"-----------------------------------------\n")
                qtext.config(state=DISABLED)
            else:
                qtext.config(state=NORMAL)
                for line in cursor:
                    qtext.insert(END,str(line)+"\n")
                qtext.insert(END,"-----------------------------------------\n")
                qtext.config(state=DISABLED)
                self.cnx.commit()
        def exitQuery():
            self.queryOpen = 0
            qwin.destroy()
            
        def showHelp():
            showinfo("Table Info","NAME: name_id, name\nPROPERTY: name_id, pname, pinst, partype, parval, value")
        
        def country():
            def enterCountry():
                countryName = centry.get()
                wildName = "%;%;%;%;%;%;{}".format(countryName)
                cursor = self.cnx.cursor()
                query = "SELECT COUNT(DISTINCT name_id) FROM PROPERTY WHERE (pname = 'ADR') AND (value LIKE '{}');".format(wildName)
                cursor.execute(query)
                num = 0
                for line in cursor:
                    num = line[0]
                qtext.config(state=NORMAL)
                qtext.insert(END,"There are {} cards with addresses from {}.\n".format(num,countryName))
                qtext.insert(END,"-----------------------------------------\n")
                qtext.config(state=DISABLED)
                countrywin.destroy()
            countrywin = Toplevel()
            countrywin.title("Enter country..")
            centry = Entry(countrywin)
            centry.grid(row=0,column=0)
            cButton = Button(countrywin,text="Enter",command=enterCountry)
            cButton.grid(row=0,column=1)
            
        def names():
            def enterNames():
                searchName = nentry.get()
                #wildName = "%;%;%;%;%;%;{}".format(countryName)
                cursor = self.cnx.cursor()
                query = "SELECT DISTINCT name_id FROM PROPERTY WHERE (pname = 'FULL N') AND (value LIKE '{}');".format(searchName)
                cursor.execute(query)
                nameids = []
                for line in cursor:
                    nameids.append(line[0])
                for nameid in nameids:
                    realName = ""
                    query = "SELECT DISTINCT value FROM PROPERTY WHERE (pname = 'FULL N') AND (name_id = {});".format(nameid)
                    cursor.execute(query)
                    for line in cursor:
                        realName = line[0]
                    qtext.config(state=NORMAL)
                    qtext.insert(END,"Properties belonging to: {}.\n".format(realName))
                    query = "SELECT pname,partype,parval,value FROM PROPERTY WHERE (name_id = {});".format(nameid)
                    cursor.execute(query)
                    for line in cursor:
                        qtext.insert(END,"{}\n".format(str(line)))
                    qtext.insert(END,"\n")
                qtext.insert(END,"-----------------------------------------\n")
                qtext.config(state=DISABLED)
                namewin.destroy()
            namewin = Toplevel()
            namewin.title("Enter name..")
            nentry = Entry(namewin)
            nentry.grid(row=0,column=0)
            nButton = Button(namewin,text="Enter",command=enterNames)
            nButton.grid(row=0,column=1)
        
        def showURLS():
            cursor = self.cnx.cursor()
            query = "SELECT DISTINCT name_id FROM PROPERTY WHERE (pname = 'URL');"
            nameList = []
            cursor.execute(query)
            for line in cursor:
                nameList.append(line[0])
            for nameid in nameList:
                query = "SELECT name FROM NAME WHERE (name_id = {});".format(nameid)
                realName = ""
                cursor.execute(query)
                for line in cursor:
                    realName = line[0]
                qtext.config(state=NORMAL)
                qtext.insert(END,"{}'s URLS:\n".format(realName))
                query = "SELECT value FROM PROPERTY WHERE (pname = 'URL') and (name_id = {});".format(nameid)
                cursor.execute(query)
                for line in cursor:
                    qtext.insert(END,"\t{}\n".format(line[0]))
            qtext.insert(END,"-----------------------------------------\n")
            qtext.config(state=DISABLED)
            
        def showAVG():
            cursor = self.cnx.cursor()
            query = "SELECT name_id FROM NAME"
            nameids = []
            totalNames = 0
            Sum = 0
            cursor.execute(query)
            for nameid in cursor:
                totalNames += 1
                nameids.append(nameid[0])
            for nameid in nameids:
                query = "SELECT COUNT(name_id) FROM PROPERTY WHERE (name_id = {});".format(nameid)
                cursor.execute(query)
                for line in cursor:
                    Sum += line[0]
            qtext.config(state=NORMAL)
            qtext.insert(END,"Average number of properties for each card: {}.\n".format(Sum/totalNames))
            qtext.insert(END,"-----------------------------------------\n")
            qtext.config(state=DISABLED)        
            
        if self.queryOpen:
            return
        self.queryOpen = 1
        qwin = Toplevel()
        qwin.protocol('WM_DELETE_WINDOW', exitQuery)
        qwin.title("Enter Query..")
        qbox = Entry(qwin)
        qbox.pack(fill=BOTH)
        qbox.insert(0,"SELECT ")
        qsubmit = Button(qwin,text="Submit",command=runQuery)
        qsubmit.pack()
        Button(qwin,text="Help",command=showHelp).pack()
        Label(qwin, text='Canned Transactions:').pack()
        cannedframe = Frame(qwin)
        Button(cannedframe,text="Show all 'Name'",command=names).grid(row=0,column=0)
        Button(cannedframe,text="Show all 'Country'",command=country).grid(row=0,column=1)
        Button(cannedframe,text="Show all URL's",command=showURLS).grid(row=0,column=2)
        Button(cannedframe,text="Show AVG # Props",command=showAVG).grid(row=0,column=3)
        cannedframe.pack()
        Label(qwin, text='Log:').pack()
        qtext = tkst.ScrolledText(qwin,state=DISABLED)
        qtext.pack(expand=YES,fill=BOTH)
        Button(qwin,text="Clear",command=clear).pack()
        
    def storeAll(self):
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        if not self.cardList:
            return
        self.upToDate = 1
        for i in range(len(self.cardList)):
            self.storeSelected(index = i)
            
            
    def confliction(self,index,name="next"):
        def writeConflictProps(name):
            cursor = self.cnx.cursor()
            query = "SELECT name_id from NAME WHERE name = '{}'".format(name)
            cursor.execute(query)
            nameid = 0
            for line in cursor:
                nameid = line[0]
            query = "SELECT * FROM PROPERTY WHERE name_id = {}".format(nameid)
            cursor.execute(query)
            ctext.config(state=NORMAL)
            for line in cursor:
                ctext.insert(END,str(line)+"\n")
            ctext.config(state=DISABLED)
            
        def exitCon():
            conwin.destroy()
            self.conflicts.pop(0)
            if self.conflicts:
                self.confliction(-1,"next")
                
        def replaceCard(name,index):
            cursor = self.cnx.cursor()
            query = "SELECT name_id from NAME WHERE name = '{}'".format(name)
            cursor.execute(query)
            nameid = 0
            for line in cursor:
                nameid = line[0]
            query = "DELETE FROM PROPERTY WHERE name_id = {}".format(nameid)
            cursor.execute(query)
            for line in cursor:
                print (line)
            exitCon()
            self.storeSelected(index,force="TRUE")
           
        def mergeCards(name,index):
            cursor = self.cnx.cursor()
            query = "SELECT name_id FROM NAME WHERE (name) = '{}'".format(name)
            cursor.execute(query)
            nameid = ""
            for line in cursor:
                nameid = line[0]
            #print (nameid)
            checked = []
            adds = 0
            for prop in self.cardList[index]:
                propType = vCardPropNametoStringSHORT(prop[0])
                if propType != "NAME":
                    #checked.append(prop[0])
                    query = "SELECT pname,partype,parval,value,pinst FROM PROPERTY WHERE (pname) = '{}' AND (name_id) = {};".format(propType,nameid)
                    cursor.execute(query)
                    maxInst = 1
                    equal = 0
                    partypee = prop[1]
                    parvall = prop[2]
                    valuee = prop[3]
                    for line in cursor:
                        #print((nameToNum(line[0]),line[1],line[2],line[3]))
                        partype = line[1]
                        parval = line[2]
                        value = line[3]
                        if line[1] == "":
                            partype = None
                        if line[2] == "":
                            parval = None
                        if line[3] == "":
                            value = None
                        if line[4] > maxInst:
                            maxInst = line[4]
                        if prop == (nameToNum(line[0]),partype,parval,value):
                            equal = 1
                    if not equal:
                        if parvall == None:
                            parvall = ""
                        if partypee == None:
                            partypee = ""
                        if valuee == None:
                            valuee = ""
                        query = ("INSERT INTO PROPERTY (name_id,pname,pinst,partype,parval,value)"
                        "VALUES ('{}','{}',{},'{}','{}','{}');".format(nameid,vCardPropNametoStringSHORT(prop[0]),maxInst+1,partypee,parvall,valuee))
                        cursor.execute(query)
                        adds += 1
                        for line in cursor:
                            print(line)
            exitCon()
            self.cnx.commit()
            self.writeLogBox("Merged {} with the database ({} properties added).\n".format(name,adds))
        def cancelConflicts():
            conwin.destroy()
            self.conflicts = []
                
        if name != "next":
            self.conflicts.append((name,index))    
            if len(self.conflicts) > 1:
                return
        name = self.conflicts[0][0]
        index = self.conflicts[0][1]
        numActions = len(self.conflicts)
        conwin = Toplevel()
        conwin.title("Choose Action..")
        conwin.grab_set()
        conwin.protocol('WM_DELETE_WINDOW', exitCon)
        Label(conwin, text='{} already exists in the database.'.format(name)).pack()
        Label(conwin, text='Properties in DB:',relief=RAISED).pack()
        ctext = tkst.ScrolledText(conwin,state=DISABLED)
        ctext.pack(expand=YES,fill=BOTH)
        Label(conwin, text='Options:',relief=RAISED).pack()
        conwin2 = Frame(conwin)
        Button(conwin2,text="Don't Store This Card",command=exitCon).grid(row=0,column=0)
        Button(conwin2,text="Replace",command=lambda: replaceCard(name,index)).grid(row=0,column=1)
        Button(conwin2,text="Merge",command=lambda:mergeCards(name,index)).grid(row=0,column=2)
        Button(conwin2,text="Cancel All",command=cancelConflicts).grid(row=0,column=3)
        conwin2.pack()
        writeConflictProps(name)
        
    def storeSelected(self,index=-1,force="FALSE"):
        if not self.upToDate:
            if not askyesno("Continue?","Discard uncommitted changes?"):
                return
            else:
                self.revertChanges()
        if self.conflict:
            return 2
        if not self.cardList:
            return
        if index == -1:
            index = self.fileIndex
        card = self.cardList[index]
        name = ""
        cursor = self.cnx.cursor()
        for prop in card:
            if prop[0] == 3:
                name = prop[3]
        if force == "FALSE":
            query = "SELECT name FROM NAME;"
            cursor.execute(query)
            exists = 0
            for line in cursor:
                if (line[0].rstrip(';') == name.rstrip(';')):
                    exists = 1
            if not exists:
                query = ("INSERT INTO NAME (name) VALUES ('{}');".format(name))
                cursor.execute(query)
            else:
                self.confliction(index,name)
                return
        query = "SELECT name_id FROM NAME WHERE name = '{}';".format(name)
        cursor.execute(query)
        name_id = ""
        for lines in cursor:
            name_id = lines[0]
        instances = [0 for i in range(19)]
        for prop in card:
            if prop[0] != 3:
                instances[prop[0]] += 1
                pinst = instances[prop[0]]
                pname = vCardPropNametoStringSHORT(prop[0])
                partype = ""
                parval = ""
                if prop[1]:
                   partype = prop[1]
                if prop[2]:
                   parval = prop[2]
                value = prop[3]
                query = ("INSERT INTO PROPERTY (name_id,pname,pinst,partype,parval,value)"
                "VALUES ('{}','{}',{},'{}','{}','{}');".format(name_id,pname,pinst,partype,parval,value))
                cursor.execute(query)
                for line in cursor:
                    print(line)
        self.cnx.commit()
        self.writeLogBox("{} stored in DB.\n".format(name))
        self.upToDate = 1

    def buildGui(self):
        self.mainArea = Tk()
        self.mainArea.title("xvcf")
        self.filename = "xvcf"
        
        #Creating the drop down menus
        menubar = Menu(self.mainArea)

        #Creating file menu
        self.filemenu = Menu(menubar, tearoff=0)
        self.filemenu.add_command(label="Open..",command=self.openFile)
        self.filemenu.add_command(label="Append..",command=lambda s=self:s.openFile(append="TRUE"))
        self.filemenu.add_command(label="Save",command=self.saveFile,state=DISABLED)
        self.filemenu.add_command(label="Save as...",command=lambda:self.saveFile(saveAs="TRUE"))
        self.filemenu.add_separator()
        self.filemenu.add_command(label="Exit",command=self.quitProg)

        #Creating organization menu
        orgmenu = Menu(menubar,tearoff=0)
        orgmenu.add_command(label="Sort")
        orgmenu.add_command(label="Canonicalize")
        orgmenu.add_command(label="Select..")
        orgmenu.add_command(label="Undo")
        
        self.datamenu = Menu(menubar,tearoff=0)
        self.datamenu.add_command(label="Store All",command=self.storeAll,state=DISABLED)
        self.datamenu.add_command(label="Store Selected",command=self.storeSelected,state=DISABLED)
        self.datamenu.add_command(label="Open From Database..",command=self.openDB)
        self.datamenu.add_command(label="Append From Database..",command=lambda:self.openDB(APPEND="TRUE"))
        self.datamenu.add_command(label="Query..",command=self.query)

        #create help menu
        helpmenu = Menu(menubar,tearoff=0)
        helpmenu.add_command(label="Card flags and colours..",command=self.colourInfo)
        helpmenu.add_separator()
        helpmenu.add_command(label="About xvcf",command=self.showAbout)

        menubar.add_cascade(label="File",menu=self.filemenu)
        menubar.add_cascade(label="Organize",menu=orgmenu)
        menubar.add_cascade(label="Database",menu=self.datamenu)
        menubar.add_cascade(label="Help",menu=helpmenu)
        
        #Creating the FVP
        FVP = Frame(self.mainArea)
        Label(FVP, text='File View').pack()

        #Creating the MLB for the FVP
        self.fvp_mlb = MultiListbox.MultiListbox(FVP, (('Card', 5),('Name',5), ('Region', 5), ('Country', 5), ('Address',5),('Telephone',5),('Flags',5)))
        self.fvp_mlb.pack(expand=YES,fill=BOTH)
        
        

        #The 5 FVP buttons
        FVPButtons = Frame(FVP)
        mapSelectButton = Button(FVPButtons,text='Map Selected')#,command=loadMap)
        mapSelectButton.grid(row=0,column=0)
        resetMapButton = Button(FVPButtons,text='Reset Map')
        resetMapButton.grid(row=0,column=1)
        browseSelectButton = Button(FVPButtons,text='Browse Selected')#,command=browseSelection)
        browseSelectButton.grid(row=0,column=2)
        deleteSelectButton = Button(FVPButtons,text='Delete Card',command=self.deleteCard)
        deleteSelectButton.grid(row=0,column=3)
        addCardButton = Button(FVPButtons,text='Add Card',command=self.addCard)
        addCardButton.grid(row=0,column=4)
        FVPButtons.pack(expand=YES,fill=BOTH)

        #Packing the final FVP
        self.fvp_mlb.lists[0].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[1].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[2].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[3].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[4].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[5].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        self.fvp_mlb.lists[6].bind("<Button-1>",lambda e,s=self:s.select(e.y))
        #lambda e,s=self:s.select(e.y))
        FVP.pack(expand=YES,fill=BOTH)
        
        #Creating the CVP
        CVP = Frame(self.mainArea)
        Label(CVP, text='Card View').pack()
        self.cvp_mlb = MultiListbox.MultiListbox(CVP, ( ('Property',5),('Name',5),('Type',5), ('Value',5), ('Property',5)))
        self.cvp_mlb.pack(expand=YES,fill=BOTH)
        
        self.cvp_mlb.lists[2].bind("<Double-Button-1>",lambda e,s=self:s.editField(1,e.y))
        self.cvp_mlb.lists[3].bind("<Double-Button-1>",lambda e,s=self:s.editField(2,e.y))
        self.cvp_mlb.lists[4].bind("<Double-Button-1>",lambda e,s=self:s.editField(3,e.y))
        #Creating the CVP buttons
        CVPButtons = Frame(CVP)
        upButton = Button(CVPButtons,text='Up',width=10,command=lambda:self.moveProp(direction="UP"))
        upButton.grid(row=0,column=0)
        downButton = Button(CVPButtons,text='Down',width=10,command=lambda:self.moveProp(direction="DOWN"))
        downButton.grid(row=1,column=0)
        addPropButton = Button(CVPButtons,text='Add Property',width=10,command=self.addProp)
        addPropButton.grid(row=0,column=1)
        delPropButton = Button(CVPButtons,text='Delete Property',width=10,command=self.deleteProp)
        delPropButton.grid(row=1,column=1)
        commitButton = Button(CVPButtons,text='Commit',width=10,command=self.commitChanges)
        commitButton.grid(row=0,column=2)
        revertButton = Button(CVPButtons,text='Revert',width=10,command=self.revertChanges)
        revertButton.grid(row=1,column=2)
        CVPButtons.pack(expand=YES,fill=BOTH)

        #Packing the final CVP
        CVP.pack(expand=YES,fill=BOTH)


        #Creating the log screen
        Label(self.mainArea,text="Log").pack()
        Button(self.mainArea,text="Clear",command=self.clearLogBox).pack()
        self.log = tkst.ScrolledText(self.mainArea,state=DISABLED)
        self.log.pack(expand=YES,fill=BOTH)
        self.mainArea.config(menu=menubar)
        self.mainArea.mainloop()
        
        
def initTables(cnx,userID,pwordID,hostnameID):
    cursor = cnx.cursor()
    
    query = ("CREATE TABLE NAME("
    "name_id INT AUTO_INCREMENT PRIMARY KEY,"
    "name VARCHAR(60) NOT NULL"
    ");")
    
    try:
        cursor.execute(query)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
            pass
        else:
            print(err.msg)     
            
    query = ("CREATE TABLE PROPERTY("
    "name_id INT NOT NULL,"
    "pname CHAR(8) NOT NULL,"
    "pinst SMALLINT NOT NULL,"
    "partype TINYTEXT,"
    "parval TINYTEXT,"
    "value TEXT,"
    "PRIMARY KEY(name_id,pname,pinst),"
    "FOREIGN KEY(name_id) REFERENCES NAME(name_id) ON DELETE CASCADE"
    ");")
    
    try:
        cursor.execute(query)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
            pass
        else:
            print(err.msg)
        
if __name__ == "__main__":
    if  len(sys.argv) < 3:
        print("Username and password required. Exiting.")
        sys.exit()
    user = str(sys.argv[1])
    pword = str(sys.argv[2])
    hostname = "dursley.socs.uoguelph.ca"
    if (len(sys.argv) > 3):
        hostname = str(sys.argv[3])
    try:
        cnx = mysql.connector.connect(user=user, password=pword,
        host=hostname, database=user)
    except mysql.connector.Error as err:
        print("Connection failed.")
        print(err.msg)
    else:
        initTables(cnx,user,pword,hostname)
        gui = xvcfGui(cnx)
        gui.buildGui()