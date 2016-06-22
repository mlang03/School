#!/usr/bin/python3

""" xvcf.py

GUI for vcards reading and editing Vcards

Created: mar 10 2014
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


def openFile():
    global cardList
    global filen
    global fileIndex
    global revert
    global com
    if not com:
        if not askyesno("Continue?","Discard uncommitted changes?"):
            return
    openFile = filedialog.askopenfilename()
    filen = openFile
    if not openFile:
        writeLogBox("No file selected.\n")
        return
    status = Vcf.readFile(openFile)
    if (status != 0):
        showerror("Error.","No cards could be found in that file.");
        writeLogBox("Failed to read file, error: ")
        writeLogBox(status)
        writeLogBox("\n")
        writeLogBox(openFile)
        writeLogBox("\n")
        return
    writeLogBox("Succesfully opened file.\n")
    writeLogBox(openFile)
    writeLogBox("\n")
    menubar.entryconfig(1,state=NORMAL)
    mlb.delete(0,END)
    ml2.delete(0,END)
    tk.title(openFile)
    cardList = []
    card = []
    p = 0
    i = 0
    while (p == 0):
        p = Vcf.getCard(card)
        if (p == 0):
            cardList.append(card)
            i = i + 1
        card = []
    revert = list(cardList)
    fileIndex = 0
    writeFVP(cardList)
    writeCVP(cardList[0])
    com = 1
    
def appendFile():
    global fileIndex
    global cardList
    global revert
    global com
    if not com:
        if not askyesno("Continue?","Discard uncommitted changes?"):
            return
    openFile = filedialog.askopenfilename()
    if not openFile:
        writeLogBox("No file selected.\n")
        return
    status = Vcf.readFile(openFile)
    if (status != 0):
        showerror("Error.","No cards could be found in that file.");
        writeLogBox("Failed to read file, error: ")
        writeLogBox(status)
        writeLogBox("\n")
        writeLogBox(openFile)
        writeLogBox("\n")
        return
    writeLogBox("Succesfully opened file to append.\n")
    writeLogBox(openFile)
    writeLogBox("\n")
    mlb.delete(0,END)
    ml2.delete(0,END)
    card = []
    p = 0
    i = 0
    while (p == 0):
        p = Vcf.getCard(card)
        if (p == 0):
            cardList.append(card)
            i = i + 1
        card = []
    revert = list(cardList)
    fileIndex = 0
    writeFVP(cardList)
    writeCVP(cardList[0])
    com = 1
    
def writeFVP(cardList):
    i = len(cardList)
    for k in range(i):
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
        for m in range(len(cardList[k])):
            if (cardList[k][m][0] == 4):
                FN = cardList[k][m][3]
                NFN = NFN + 1
            if (cardList[k][m][0] == 3):
                NN = NN + 1
            if (cardList[k][m][0] == 10):
                TELC = TELC + 1
            if (cardList[k][m][0] == 6):
                NPHOTO = NPHOTO + 1
            if (cardList[k][m][0] == 17):
                NURL = NURL + 1
            if (cardList[k][m][0] == 12):
                NGEO = NGEO + 1
            if (cardList[k][m][0] == 8):
                REGION = cardList[k][m][3]
                COUNTRY = cardList[k][m][3]
                ADRC = ADRC + 1
            if (cardList[k][m][0] == 16):
                UID = cardList[k][m][3]
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
        mlb.insert(END, (k+1,FN,REGION,COUNTRY,ADRC,TELC,FLAGS))
        
def writeCVP(card):
    for i in range(len(card)):
        if not card[i][1]:
            ptype = ""
        else:
            ptype = card[i][1]
            
        if not card[i][2]:
            pval = ""
        else:
            pval = card[i][2]
            
        ml2.insert(END,(i+1,card[i][0],ptype,pval,card[i][3]))
        
def saveFile():
    global cardList
    global com
    if not com:
        if not askyesno("Continue?","Discard uncommitted changes?"):
            return
    out = Vcf.writeFile(filen,cardList)
    if not out:
       writeLogBox("Can not save empty file.\n")
    else:
        writeLogBox("Saved file.\n")
        writeLogBox(filen)
        writeLogBox("\n")
        
def saveFileAs():
    def getsavename():
        saven = entry.get()
        if not saven:
            writeLogBox("Please enter a file name.\n")
            return
        if os.path.isfile(saven):
            if not askokcancel("Overwrite?","Are you sure you want to overwrite the selected file?"):
                return
        global cardList
        global filen
        global com
        out = Vcf.writeFile(saven,cardList)
        if not out:
            writeLogBox("Can not save empty file.\n")
        else:
            writeLogBox("Saved file.\n")
            writeLogBox(saven)
            writeLogBox("\n")
        tk.title(saven)
        filen = saven
        savewin.destroy()
        
    def closesavewin():
        savewin.destroy()
    global com
    if not com:
        if not askyesno("Continue?","Discard uncommitted changes?"):
            return
    savewin = Toplevel()
    savewin.title("Save as..")
    Label(savewin, text='Save as..').pack()
    entry = Entry(savewin)
    Button(savewin,text="Save",command=getsavename).pack()
    entry.pack()
    Button(savewin,text="Cancel",command=closesavewin).pack()
    
def appendCard(index,propp):
    global cardList
    global com
    cardList[index].append(propp)
    ml2.delete(0,END)
    writeCVP(cardList[index])
    com = 0
    
def select():
    global fileIndex
    global com
    if not cardList:
        return
    if not com:
        if not askyesno("Continue?","Discard uncommitted changes?"):
            return
    index = (int)(mlb.curselection()[0])
    ml2.delete(0,END)
    writeCVP(cardList[index])
    fileIndex = index
    
def addProp():
    global cardList
    if not cardList:
        return
    def exitaddProp():
        addwin.destroy()
    def enterAddProp():
        z = partype.get()
        propp = (int)(choicebox.curselection()[0]) +5, partype.get(), parval.get(), value.get()
        appendCard(0,propp)
        exitaddProp()
    if not cardList:
        writeLogBox("You must open a file first.\n")
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
    choicebox.pack()
    Label(addwin, text='Parameter Type:').pack()
    partype = Entry(addwin)
    partype.pack()
    Label(addwin, text='Parameter Value:').pack()
    parval = Entry(addwin)
    parval.pack()
    Label(addwin, text='Property Value:').pack()
    value = Entry(addwin)
    value.pack()
    
    Button(addwin,text="Enter.",command=enterAddProp).pack()
    Button(addwin,text="Cancel.",command=exitaddProp).pack()
    

def writeLogBox(msg):
    log.config(state=NORMAL)
    log.insert(END,msg)
    log.config(state=DISABLED)
    
def clearLogBox():
    log.config(state=NORMAL)
    log.delete(1.0,END)
    log.config(state=DISABLED)
    
def showAbout():
    showinfo("About xvcf","Created by Mike \'OTL\' Lang.\n Compatible with Vcard 3.0.")
    
def quitProg():
    if askyesno("Quit?", "Are you sure you want to quit?"):
        killServers()
        tk.quit()
        
def deleteProp():
    global fileIndex
    global cardList
    global com
    if not cardList:
        return
    index = (int)(ml2.curselection()[0])
    if (index > len(cardList[fileIndex])):
        return
    if (len(cardList[fileIndex]) == 1):
        writeLogBox("Can not delete last property.\n")
        return
    cardList[fileIndex].pop(index)
    ml2.delete(0,END)
    writeCVP(cardList[fileIndex])
    com = 0
    
def editProp():
    global cardList
    global fileIndex
    if not cardList:
        return
    def exiteditProp():
        editwin.destroy()
    def enterEditProp():
        ppty = (int) (pname.get()), partype.get(), parval.get(), value.get()
        if not ppty[0]:
           writeLogBox("Missing property name.\n")
           return
        if not ppty[3]:
           writeLogBox("Missing property value.\n")
           return
        cardList[fileIndex][index] = ppty
        ml2.delete(0,END)
        writeCVP(cardList[fileIndex])
        exiteditProp()
    index = (int)(ml2.curselection()[0])
    if (index > len(cardList[fileIndex])):
        return
    editwin = Toplevel()
    Label(editwin, text='Parameter Name:').pack()
    pname = Entry(editwin)
    pname.pack()
    pname.insert(0,cardList[fileIndex][index][0])
    Label(editwin, text='Parameter Type:').pack()
    partype = Entry(editwin)
    partype.pack()
    if cardList[fileIndex][index][1]:
        partype.insert(0,cardList[fileIndex][index][1])
    Label(editwin, text='Parameter Value:').pack()
    parval = Entry(editwin)
    parval.pack()
    if cardList[fileIndex][index][2]:
        parval.insert(0,cardList[fileIndex][index][2])
    Label(editwin, text='Property Value:').pack()
    value = Entry(editwin)
    value.pack()
    value.insert(0,cardList[fileIndex][index][3])
    
    Button(editwin,text="Enter.",command=enterEditProp).pack()
    Button(editwin,text="Cancel.",command=exiteditProp).pack()
    com = 0
    
def moveProp(direction):
    global cardList
    global fileIndex
    global com
    if not cardList:
        return
    dex = ml2.curselection()
    if dex:
        index = (int)(ml2.curselection()[0])
    else:
        return
    if (index > len(cardList[fileIndex])):
        return
    if (direction == "UP"):
        if (index > 0):
            propp2 = cardList[fileIndex].pop(index)
            cardList[fileIndex].insert(index-1,propp2)
            ml2.delete(0,END)
            writeCVP(cardList[fileIndex])
        else:
            return
    if (direction == "DOWN"):
        if (index < len(cardList[fileIndex]) - 1):
            propp2 = cardList[fileIndex].pop(index)
            cardList[fileIndex].insert(index+1,propp2)
            ml2.delete(0,END)
            writeCVP(cardList[fileIndex])
        else:
            return
    com = 0
            
            
def revertChanges():
    global com
    global cardList
    if not cardList:
        return
    if not revert:
        cardList = []
        return
    com = 1
    if revert[fileIndex]:
        cardList[fileIndex] = list(revert[fileIndex])
    else:
        cardList[fileIndex] = []
    ml2.delete(0,END)
    writeCVP(cardList[fileIndex])
    
def commitChanges():
    global com
    global cardList
    if not cardList:
        return
    com = 1
    revert[fileIndex] = list(cardList[fileIndex])
    ml2.delete(0,END)
    writeCVP(cardList[fileIndex])
    
def loadMap():
    global cardList
    global points
    global fileIndex
    global mapp
    if not cardList:
        return
    photo = "http://www.uoguelph.ca/~gardnerw/head.gif" # photo url
    address = ";;50 Stone Road East\, Reynolds 105;\\nGuelph;Ontario;N1G2W1" # address
    for propt in cardList[fileIndex]:
        if (propt[0] == 12):
            coord = propt[3].split(',')
            x = (float)(coord[0])
            y = (float)(coord[1])
            cdd = []
            cdd.append(x)
            cdd.append(y)
            points.append(cdd)
            mapp.addPoint(cdd,photo,address)
    
    mapp.addOverlay(0,1,1)
    mapp.serve("public_html/index.html")
    launchBrowser("http://localhost:43368/")
    
    
def browseSelection():
    global cardList
    global fileIndex
    if not cardList:
        return
    for propt in cardList[fileIndex]:
        if (propt[0] == 17):
            url = propt[3]
            launchBrowser(url)
            
def addaCard():
    global cardList
    global fileIndex
    newcard = []
    Nprop = (3,'','',"John Doe")
    FNprop = (4,'','',"Doe;John")
    newcard.append(Nprop)
    newcard.append(FNprop)
    cardList.append(newcard)
    mlb.delete(0,END)
    ml2.delete(0,END)
    writeFVP(cardList)
    writeCVP(cardList[len(cardList)-1])
    fileIndex = len(cardList) - 1
    com = 0
    
def delaCard():
    global cardList
    if not cardList:
        return
    if (len(cardList) == 1):
        writeLogBox("Can't delete last card.")
        return
    index = (int)(mlb.curselection()[0])
    if not askokcancel("Delete?","Are you sure you want to delete card %d" % (index+1)):
        return
    cardList.pop(index)
    mlb.delete(0,END)
    ml2.delete(0,END)
    writeFVP(cardList)
    writeCVP(cardList[0])
    fileindex = 0
    
def colourInfo():
    showinfo("Color","Green: Card is canon.\nRed: Card needs fixing.\nYellow: default.")
    
tk = Tk()
tk.title("xvcf")
cardList = []
points = []
revert = []
fileIndex = 0
filen = ""
com = 1
mapp = GMapData("Map","Mapping",[45.530318,-80.223241],15)
portnum = 43368
startWebServer(portnum)

#Creating the FVP
FVP = Frame(tk)
Label(FVP, text='File View').pack()

#Creating the drop down menus
menubar = Menu(tk)

#Creating file menu
filemenu = Menu(menubar, tearoff=0)
filemenu.add_command(label="Open..",command=openFile)
filemenu.add_command(label="Append..",command=appendFile)
filemenu.add_command(label="Save",command=saveFile)
filemenu.add_command(label="Save as...",command=saveFileAs)
filemenu.add_separator()
filemenu.add_command(label="Exit",command=quitProg)

#Creating organization menu
orgmenu = Menu(menubar,tearoff=0)
orgmenu.add_command(label="Sort")
orgmenu.add_command(label="Canonicalize")
orgmenu.add_command(label="Select..")
orgmenu.add_command(label="Undo")

#create help menu
helpmenu = Menu(menubar,tearoff=0)
helpmenu.add_command(label="Card flags and colours..",command=colourInfo)
helpmenu.add_separator()
helpmenu.add_command(label="About xvcf",command=showAbout)

menubar.add_cascade(label="File",menu=filemenu)
menubar.add_cascade(label="Organize",menu=orgmenu)
menubar.add_cascade(label="Help",menu=helpmenu)

#Creating the MLB for the FVP
mlb = MultiListbox.MultiListbox(FVP, (('Card', 5),('Name',5), ('Region', 5), ('Country', 5), ('Address',5),('Telephone',5),('Flags',5)))
mlb.pack(expand=YES,fill=BOTH)

#The 5 FVP buttons
FVPButtons = Frame(FVP)
mapSelect = Button(FVPButtons,text='Map Selected',command=loadMap)
mapSelect.grid(row=0,column=0)
resetMap = Button(FVPButtons,text='Reset Map')
resetMap.grid(row=0,column=1)
browseSelect = Button(FVPButtons,text='Browse Selected',command=browseSelection)
browseSelect.grid(row=0,column=2)
deleteSelect = Button(FVPButtons,text='Delete Card',command=delaCard)
deleteSelect.grid(row=0,column=3)
addCard = Button(FVPButtons,text='Add Card',command=addaCard)
addCard.grid(row=0,column=4)
selectCard = Button(FVPButtons,text='Select',command=select)
selectCard.grid(row=0,column=5)
FVPButtons.pack(expand=YES,fill=BOTH)

#Packing the final FVP
FVP.pack(expand=YES,fill=BOTH)

#Creating the CVP
CVP = Frame(tk)
Label(CVP, text='Card View').pack()
ml2 = MultiListbox.MultiListbox(CVP, ( ('Property',5),('Name',5),('Type',5), ('Value',5), ('Property',5)))
ml2.pack(expand=YES,fill=BOTH)

#Creating the CVP buttons
CVPButtons = Frame(CVP)
up = Button(CVPButtons,text='Up',width=10,command=lambda:moveProp(direction="UP"))
up.grid(row=0,column=0)
down = Button(CVPButtons,text='Down',width=10,command=lambda:moveProp(direction="DOWN"))
down.grid(row=1,column=0)
addProp = Button(CVPButtons,text='Add Property',width=10,command=addProp)
addProp.grid(row=0,column=1)
delProp = Button(CVPButtons,text='Delete Property',width=10,command=deleteProp)
delProp.grid(row=1,column=1)
commit = Button(CVPButtons,text='Commit',width=10,command=commitChanges)
commit.grid(row=0,column=2)
revert = Button(CVPButtons,text='Revert',width=10,command=revertChanges)
revert.grid(row=1,column=2)
edit = Button(CVPButtons,text='Edit',width=10,command=editProp)
edit.grid(row=0,column=3)
CVPButtons.pack(expand=YES,fill=BOTH)

#Packing the final CVP
CVP.pack(expand=YES,fill=BOTH)


#Creating the log screen
Label(tk,text="Log").pack()
Button(tk,text="Clear",command=clearLogBox).pack()
log = tkst.ScrolledText(tk,state=DISABLED)
log.pack(expand=YES,fill=BOTH)


tk.config(menu=menubar)
tk.mainloop()