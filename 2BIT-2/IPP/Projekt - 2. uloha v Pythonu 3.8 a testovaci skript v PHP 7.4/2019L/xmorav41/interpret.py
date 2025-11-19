#!/usr/bin/env python3

#   IPP Projet - 2. skript
#   interpret.py
#   xmorav41

import sys
import os
import re
import argparse
import xml.etree.ElementTree as ET


# návratové kódy
class ExitCodes:
    errorParam = 10
    errorOpeningFile = 11
    errorWriteToFile = 12
    errorWrongHeader = 21
    errorWrongOpcode = 22
    errorSynLex = 23
    errorBadFormat = 31
    errorUnexpectedStruct = 32
    errorSemantic = 52
    RTEWrongType = 53
    RTEVarNotExist = 54
    RTEFrameNotExist = 55
    RTEValueNotExist = 56
    RTEWrongValue = 57
    RTEStringMisuse = 58
    errorInternal = 99

# list podporovaných intrukcií
opcodes = ['MOVE',
           'CREATEFRAME',
           'PUSHFRAME',
           'POPFRAME',
           'DEFVAR',
           'CALL',
           'RETURN',
           'PUSHS',
           'POPS',
           'ADD',
           'SUB',
           'MUL',
           'IDIV',
           'LT',
           'GT',
           'EQ',
           'AND',
           'OR',
           'NOT',
           'INT2CHAR',
           'STRI2INT',
           'READ',
           'WRITE',
           'CONCAT',
           'STRLEN',
           'GETCHAR',
           'SETCHAR',
           'TYPE',
           'LABEL',
           'JUMP',
           'JUMPIFEQ',
           'JUMPIFNEQ',
           'EXIT',
           'DPRINT',
           'BREAK']

# typy hodnôt v IPPcode20
varTypes = ['string', 'bool', 'int', 'nil']

# :^)
# Špeciálna hodnota pre narábanie s nil@nil
customNil = '#(X}k8ge3X'

# trieda rámcov
class FRAMES:
    globalFrame = {}
    localFrame = None
    temporaryFrame = None
    frameStack = []
    currentFrame = None

    # Zistí hodnotu premennej/konštanty
    # param arg - intšrukcia
    # return - hodnota
    @classmethod
    def getVal(cls, arg):
        if cls.isItVar(arg):
            cls.checkIfExists(arg.text)
            if cls.currentFrame[arg.text[3:]] is None:
                sys.stderr.write(f"ERROR-> VAR HAS NO VALUE  {arg.text}")
                sys.exit(ExitCodes.RTEValueNotExist)
            else:
                return cls.currentFrame[arg.text[3:]]
        else:
            if arg.attrib['type'] == 'int':
                try:
                    if not arg.text:
                        sys.exit(53)
                    return int(arg.text)
                except ValueError:
                    sys.exit(32)
            elif arg.attrib['type'] == 'nil' and arg.text == 'nil':
                return customNil
            elif arg.attrib['type'] == 'bool':
                if arg.text.lower() == 'true':
                    return bool('xd')
                elif arg.text.lower() == 'false':
                    return bool()
                else:
                    sys.exit(32)
            elif arg.attrib['type'] == 'string':
                if arg.text is None:
                    return ''
                return cls.escapeS(arg.text)
            else:
                sys.exit(32)

    # Zistí typ premennej/konštanty
    # param arg - intšrukcia
    # return - typ
    @classmethod
    def getType(cls, arg):
        if arg.attrib['type'] == 'var':
            tmpVal = cls.getVal(arg)
            if isinstance(tmpVal, bool):
                return 'bool'
            elif isinstance(tmpVal, int):
                return 'int'
            elif tmpVal == customNil:
                return 'nil'
            elif isinstance(tmpVal, str):
                return 'string'
        elif arg.attrib['type'] in varTypes:
            return arg.attrib['type']
        else:
            sys.stderr.write(f"ERROR-> WRONG TYPE  {arg.attrib['type']}")
            sys.exit(ExitCodes.errorSynLex)

    # Vloží premennú na rámec
    # param - názov premennej
    @classmethod
    def addVar(cls, name):
        if cls.currentFrame is None:
            sys.stderr.write(f"ERROR-> FRAME NOT INITIALIZED: {name[:2]}")
            sys.exit(ExitCodes.RTEFrameNotExist)
        if name[3:] in cls.currentFrame:
            sys.stderr.write(f"ERROR-> REDEFINING VARIABLE {name}")
            sys.exit(ExitCodes.errorSemantic)
        cls.currentFrame[name[3:]] = None
        pass

    # PUSHFRAME
    @classmethod
    def pushframe(cls):
        if cls.temporaryFrame is None:
            sys.stderr.write(f"ERROR-> TEMPORARY FRAME NOT INITIALIZED [PUSHFRAME]")
            sys.exit(55)
        cls.frameStack.append(cls.temporaryFrame)
        cls.localFrame = cls.frameStack[len(cls.frameStack) - 1]
        cls.temporaryFrame = None
        pass

    # POPFRAME
    @classmethod
    def popframe(cls):
        if not cls.frameStack:
            sys.stderr.write(f"ERROR-> LOCAL FRAME NOT INITIALIZED [POPFRAME]")
            sys.exit(55)
        else:
            cls.temporaryFrame = cls.localFrame
            cls.frameStack.pop(len(cls.frameStack) - 1)
            if not cls.frameStack:
                cls.localFrame = None
            else:
                cls.localFrame = cls.frameStack[len(cls.frameStack) - 1]

    # Zistí rámec premennej
    # param - premenná
    @classmethod
    def frameType(cls, var):
        if var[:3].upper() == 'GF@':
            cls.currentFrame = cls.globalFrame
        elif var[:3].upper() == 'LF@':
            if cls.localFrame is None:
                sys.stderr.write(f"ERROR-> LOCAL FRAME NOT INITIALIZED")
                sys.exit(55)
            else:
                cls.currentFrame = cls.localFrame
        elif var[:3].upper() == 'TF@':
            if cls.temporaryFrame is None:
                sys.stderr.write(f"ERROR-> TEMPORARY FRAME NOT INITIALIZED")
                sys.exit(55)
            else:
                cls.currentFrame = cls.temporaryFrame
        else:
            sys.stderr.write(f"ERROR-> WRONG FRAME in {var}")
            sys.exit(ExitCodes.errorUnexpectedStruct)

    # Zistí či premenná existuje
    # param - prememnná
    @classmethod
    def checkIfExists(cls, var):
        cls.frameType(var)
        if var[3:] not in cls.currentFrame:
            sys.stderr.write(f"ERROR-> VAR DOES NOT EXIST {var}")
            sys.exit(ExitCodes.RTEVarNotExist)
        else:
            return True

    # True ak je to premenná, false ak konštanta
    @classmethod
    def isItVar(cls, arg):
        #
        # uwu = arg.attrib['type']
        if arg.attrib['type'] == 'var':
            return True
        elif arg.attrib['type'] in varTypes:
            return False
        else:
            sys.stderr.write(f"ERROR-> WRONG TYPE {arg.attrib['type']}")
            sys.exit(ExitCodes.errorSynLex)

    #  Only for vars and constants
    @classmethod
    def setVal(cls, arg1, arg2):
        # var is var
        cls.checkIfExists(arg1.text)
        if cls.isItVar(arg2):  #  
            cls.checkIfExists(arg2.text)
            value = cls.getVal(arg2)
            cls.frameType(arg1.text)
            cls.currentFrame[arg1.text] = value
        # var is constant
        else:
            cls.frameType(arg1.text)
            cls.currentFrame[arg1.text[3:]] = cls.getVal(arg2)
            """if arg2.attrib['type'] == 'int':
                try:
                    cls.currentFrame[arg1.text[3:]] = int(arg2.text)
                except ValueError:
                    sys.exit(57)
            elif arg2.attrib['type'] == 'bool':
                if arg2.text == 'true':
                    cls.currentFrame[arg1.text[3:]] = bool(arg2.text)  #  Pozor! python to prevedie na velke True/False
                elif arg2.text == 'false':
                    cls.currentFrame[arg1.text[3:]] = bool()  #  Pozor! python to prevedie na velke True/False
                else:
                    sys.exit(ExitCodes.errorSynLex)
            elif arg2.attrib['type'] == 'string':
                cls.currentFrame[arg1.text[3:]] = cls.escapeS(str(arg2.text))
            elif arg2.attrib['type'] == 'nil':
                cls.currentFrame[arg1.text[3:]] = customNil
            pass"""

    # Vloží hodnotu do premennej
    # param arg1 - inštrukcia
    # param val - hodnota
    @classmethod
    def setVal2(cls, arg1, val):
        cls.checkIfExists(arg1.text)
        cls.currentFrame[arg1.text[3:]] = val
        pass

    # Vráti escaped string
    @classmethod
    def escapeS(cls, val):
        regex_esc = re.compile(r"\\(\d{1,3})")
        hex_esc = re.compile(r"\\x([0-9a-f]{2})")
        val = hex_esc.sub(cls.replaceHex, val)
        val = regex_esc.sub(cls.replace, val)
        return val

    # Pomocná metóda - \XXX
    @classmethod
    def replace(cls, match):
        return chr(int(match.group(1)))

    # Pomocná metóda - \xXX (hex)
    @classmethod
    def replaceHex(cls, match):
        return chr(int(match.group(1), 16))

    # CREATEFRAME
    @classmethod
    def createFrame(cls):
        cls.temporaryFrame = {}


class VARS:
    fullname = None

    # Nastaví novú premennú na rámec
    def setName(self, varName):
        self.fullname = varName
        FRAMES.frameType(self.fullname)
        FRAMES.addVar(self.fullname)


class MAIN:

    # Zoradí intrukcie, nájde návestia, vykoná inštrukcie
    @classmethod
    def order(cls, rootM):
        #   zoradenie všetkých inštrukcií podľa orderu ##
        instructionsRaw = rootM.findall("./")
        keyList = []
        for elem in instructionsRaw:
            cls.checkArg(elem, len(elem))
            key = elem.attrib['order']
            try:
                if int(key) < 1:
                    sys.exit(32)
            except ValueError:
                sys.exit(32)
            INST.instructionList.append((int(key), elem))
            keyList.append(int(key))
        # kontrola unikatnosti orderov
        if len(keyList) != len(set(keyList)):
            sys.exit(32)
        INST.instructionList.sort()
        # labels
        i = 0
        for instruct in INST.instructionList:
            instruct[1].attrib['opcode'] = instruct[1].attrib['opcode'].upper()
            if instruct[1].attrib['opcode'] == 'LABEL':
                labArg = instruct[1].find('arg1')
                if labArg.text in LABELS.labels:
                    sys.exit(52)
                LABELS.labels[labArg.text] = i
            i += 1
            pass

        while INST.currentPosition < len(INST.instructionList):
            instruct = INST.instructionList[INST.currentPosition]
            instruct[1].attrib['opcode'] = instruct[1].attrib['opcode'].upper()
            if instruct[1].attrib['opcode'] not in opcodes:
                sys.stderr.write(f"ERROR-> order {instruct[0]}, {instruct[1].attrib['opcode']}")
                sys.exit(32)
            cls.opcode = instruct[1]
            instruction = INST(instruct[1])
            instruction.process()
            INST.currentPosition += 1
        pass

    # xml kontrola
    @classmethod
    def checkArg(cls, arg, count):
        zeroArg = ['CREATEFRAME','PUSHFRAME','POPFRAME','RETURN','BREAK']
        oneArg = ['DEFVAR','CALL','PUSHS','POPS','WRITE','LABEL','JUMP','EXIT','DPRINT']
        twoArg = ['MOVE','NOT','INT2CHAR','READ','STRLEN','TYPE']
        threeArg = ['ADD','SUB','MUL','IDIV','LT','GT','EQ','AND','OR', 'STRI2INT','CONCAT','GETCHAR','SETCHAR','JUMPIFEQ','JUMPIFNEQ']

        if not (0 <= count < 4):
            sys.exit(32)
        if arg.tag != 'instruction':
            sys.exit(32)
        if 'order' not in arg.attrib or 'opcode' not in arg.attrib:
            sys.exit(32)

        if count == 0 and arg.attrib['opcode'] in zeroArg:
            pass
        elif count == 1 and arg.attrib['opcode'] in oneArg:
            if arg.find('arg1') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg1').attrib:
                sys.exit(32)
        elif count == 2 and arg.attrib['opcode'] in twoArg:
            if arg.find('arg1') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg1').attrib:
                sys.exit(32)
            if arg.find('arg2') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg2').attrib:
                sys.exit(32)
        elif count == 3 and arg.attrib['opcode'] in threeArg:
            if arg.find('arg1') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg1').attrib:
                sys.exit(32)
            if arg.find('arg2') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg2').attrib:
                sys.exit(32)
            if arg.find('arg3') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg3').attrib:
                sys.exit(32)
        else:
            sys.exit(32)
        pass


class INST:
    instructionList = []
    currentPosition = 0
    opcode = None
    instruction = None
    callPositionStack = []

    def __init__(self, inst):
        self.instruction = inst

    # Identifikuje intrukciu a vykoná ju
    def process(self):
        if self.instruction.attrib['opcode'] == 'DEFVAR':
            self.DEFVAR()
            pass
        elif self.instruction.attrib['opcode'] == 'MOVE':
            self.MOVE()
            pass
        elif self.instruction.attrib['opcode'] == 'ADD':
            self.ADD()
            pass
        elif self.instruction.attrib['opcode'] == 'MUL':
            self.MUL()
            pass
        elif self.instruction.attrib['opcode'] == 'SUB':
            self.SUB()
            pass
        elif self.instruction.attrib['opcode'] == 'IDIV':
            self.IDIV()
            pass
        elif self.instruction.attrib['opcode'] == 'WRITE':
            self.WRITE()
            pass
        elif self.instruction.attrib['opcode'] == 'LT':
            self.LT()
            pass
        elif self.instruction.attrib['opcode'] == 'GT':
            self.GT()
            pass
        elif self.instruction.attrib['opcode'] == 'EQ':
            self.EQ()
            pass
        elif self.instruction.attrib['opcode'] == 'AND':
            self.AND()
            pass
        elif self.instruction.attrib['opcode'] == 'OR':
            self.OR()
            pass
        elif self.instruction.attrib['opcode'] == 'NOT':
            self.NOT()
            pass
        elif self.instruction.attrib['opcode'] == 'INT2CHAR':
            self.INT2CHAR()
            pass
        elif self.instruction.attrib['opcode'] == 'STRI2INT':
            self.STRI2INT()
            pass
        elif self.instruction.attrib['opcode'] == 'CONCAT':
            self.CONCAT()
            pass
        elif self.instruction.attrib['opcode'] == 'STRLEN':
            self.STRLEN()
            pass
        elif self.instruction.attrib['opcode'] == 'GETCHAR':
            self.GETCHAR()
            pass
        elif self.instruction.attrib['opcode'] == 'SETCHAR':
            self.SETCHAR()
            pass
        elif self.instruction.attrib['opcode'] == 'TYPE':
            self.TYPE()
            pass
        elif self.instruction.attrib['opcode'] == 'JUMP':
            self.JUMP()
            pass
        elif self.instruction.attrib['opcode'] == 'LABEL':
            pass
        elif self.instruction.attrib['opcode'] == 'JUMPIFEQ':
            self.JUMPIFEQ()
            pass
        elif self.instruction.attrib['opcode'] == 'JUMPIFNEQ':
            self.JUMPIFNEQ()
            pass
        elif self.instruction.attrib['opcode'] == 'EXIT':
            self.EXIT()
            pass
        elif self.instruction.attrib['opcode'] == 'PUSHS':
            self.PUSHS()
            pass
        elif self.instruction.attrib['opcode'] == 'POPS':
            self.POPS()
            pass
        elif self.instruction.attrib['opcode'] == 'CREATEFRAME':
            self.CREATEFRAME()
            pass
        elif self.instruction.attrib['opcode'] == 'PUSHFRAME':
            self.PUSHFRAME()
            pass
        elif self.instruction.attrib['opcode'] == 'POPFRAME':
            self.POPFRAME()
            pass
        elif self.instruction.attrib['opcode'] == 'CALL':
            self.CALL()
            pass
        elif self.instruction.attrib['opcode'] == 'RETURN':
            self.RETURN()
            pass
        elif self.instruction.attrib['opcode'] == 'READ':
            self.READ()
            pass
        elif self.instruction.attrib['opcode'] == 'DPRINT':
            self.DPRINT()
            pass
        elif self.instruction.attrib['opcode'] == 'BREAK':
            self.BREAK()
            pass

    def DEFVAR(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        currVar = VARS()
        currVar.setName(self.instruction.find('arg1').text)

    def MOVE(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        FRAMES.checkIfExists(self.instruction.find('arg1').text)  #  check var, check arg123

        FRAMES.setVal(self.instruction.find('arg1'), self.instruction.find('arg2'))

    def ADD(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if (FRAMES.getType(arg2) == 'int') and (FRAMES.getType(arg3) == 'int'):
            suma = int(FRAMES.getVal(arg2)) + int(FRAMES.getVal(arg3))
            FRAMES.setVal2(arg1, suma)
        else:
            sys.exit(ExitCodes.RTEWrongType)

    def MUL(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if (FRAMES.getType(arg2) == 'int') and (FRAMES.getType(arg3) == 'int'):
            suma = int(FRAMES.getVal(arg2)) * int(FRAMES.getVal(arg3))
            FRAMES.setVal2(arg1, suma)
        else:
            sys.exit(ExitCodes.RTEWrongType)

    def SUB(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if (FRAMES.getType(arg2) == 'int') and (FRAMES.getType(arg3) == 'int'):
            suma = int(FRAMES.getVal(arg2)) - int(FRAMES.getVal(arg3))
            FRAMES.setVal2(arg1, suma)
        else:
            sys.exit(ExitCodes.RTEWrongType)

    def IDIV(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if (FRAMES.getType(arg2) == 'int') and (FRAMES.getType(arg3) == 'int'):
            if int(FRAMES.getVal(arg3)) == 0:
                sys.stderr.write(f"ERROR-> DIVISION BY ZERO {self.instruction.attrib['order']}, {self.instruction.attrib['opcode']}")
                sys.exit(ExitCodes.RTEWrongValue)
            suma = int(FRAMES.getVal(arg2)) // int(FRAMES.getVal(arg3))
            FRAMES.setVal2(arg1, suma)
        else:
            sys.exit(ExitCodes.RTEWrongType)

    def WRITE(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        val = FRAMES.getVal(self.instruction.find('arg1'))
        if isinstance(val, bool):
            if val:
                val = 'true'
            else:
                val = 'false'
        elif val == customNil:
            print('', end='')
            return
        if isinstance(val, str):
            regex_esc = re.compile(r"\\(\d{1,3})")
            hex_esc = re.compile(r"\\x([0-9a-f]{2})")
            val = hex_esc.sub(self.replaceHex, val)
            val = regex_esc.sub(self.replace, val)
        print(val, end='')

    def replace(self, match):
        return chr(int(match.group(1)))

    def replaceHex(self, match):
        return chr(int(match.group(1), 16))

    def LT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            sys.stderr.write(f"ERROR-> WRONG TYPES LT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) == FRAMES.getType(arg3):
            if FRAMES.getVal(arg2) < FRAMES.getVal(arg3):
                FRAMES.setVal2(arg1, bool('xd'))
            else:
                FRAMES.setVal2(arg1, bool())
        else:
            sys.stderr.write(f"ERROR-> WRONG TYPES LT")
            sys.exit(ExitCodes.RTEWrongType)

    def GT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            sys.stderr.write(f"ERROR-> WRONG TYPES GT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) == FRAMES.getType(arg3):
            if FRAMES.getVal(arg2) > FRAMES.getVal(arg3):
                FRAMES.setVal2(arg1, bool('xd'))
            else:
                FRAMES.setVal2(arg1, bool())
        else:
            sys.stderr.write(f"ERROR-> WRONG TYPES GT")
            sys.exit(ExitCodes.RTEWrongType)

    def EQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            if FRAMES.getType(arg2) == FRAMES.getType(arg3):
                FRAMES.setVal2(arg1, bool('xd'))
            else:
                FRAMES.setVal2(arg1, bool())
            return

        if FRAMES.getType(arg2) != FRAMES.getType(arg3):
            sys.stderr.write(f"ERROR-> WRONG TYPES EQ")
            sys.exit(ExitCodes.RTEWrongType)
        if FRAMES.getVal(arg2) == FRAMES.getVal(arg3):
            FRAMES.setVal2(arg1, bool('xd'))
        else:
            FRAMES.setVal2(arg1, bool())

    def AND(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if not isinstance(FRAMES.getVal(arg2), bool) or not isinstance(FRAMES.getVal(arg3), bool):
            sys.stderr.write(f"ERROR-> WRONG TYPES AND")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)

        if FRAMES.getVal(arg2) is True and FRAMES.getVal(arg3) is True:
            FRAMES.setVal2(arg1, bool('xd'))
        else:
            FRAMES.setVal2(arg1, bool())
        pass

    def OR(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if not isinstance(FRAMES.getVal(arg2), bool) or not isinstance(FRAMES.getVal(arg3), bool):
            sys.stderr.write(f"ERROR-> WRONG TYPES OR")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)

        if FRAMES.getVal(arg2) or FRAMES.getVal(arg3):
            FRAMES.setVal2(arg1, bool('xd'))
        else:
            FRAMES.setVal2(arg1, bool())
        pass

    def NOT(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        if not isinstance(FRAMES.getVal(arg2), bool):
            sys.stderr.write(f"ERROR-> WRONG TYPES NOT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)

        if FRAMES.getVal(arg2):
            FRAMES.setVal2(arg1, bool())
        else:
            FRAMES.setVal2(arg1, bool('xd'))
        pass

    def INT2CHAR(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        FRAMES.checkIfExists(arg1.text)
        if not isinstance(FRAMES.getVal(arg2), int) or FRAMES.getType(arg2) != 'int':
            sys.stderr.write(f"ERROR-> WRONG TYPES INT2CHAR")
            sys.exit(ExitCodes.RTEWrongType)
        try:
            char = chr(FRAMES.getVal(arg2))
        except ValueError:
            sys.stderr.write(f"ERROR-> NOT VALID UNICODE VALUE")
            sys.exit(58)
        FRAMES.setVal2(arg1, char)
        pass

    def STRI2INT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) != 'string' or FRAMES.getType(arg3) != 'int':
            sys.stderr.write(f"ERROR-> WRONG TYPES STRI2INT")
            sys.exit(ExitCodes.RTEWrongType)
        if not isinstance(FRAMES.getVal(arg2), str) or not isinstance(FRAMES.getVal(arg3), int):
            sys.stderr.write(f"ERROR-> WRONG TYPES STRI2INT")
            sys.exit(ExitCodes.RTEWrongType)
        arg2string = FRAMES.getVal(arg2)
        index = FRAMES.getVal(arg3)
        if index >= len(arg2string) or index < 0:
            sys.stderr.write(f"ERROR-> INVALID INDEX STRI2INT")
            sys.exit(58)
        char = arg2string[index]
        FRAMES.setVal2(arg1, ord(char))

        pass

    def CONCAT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getVal(arg2) == customNil or FRAMES.getVal(arg3) == customNil:
            sys.stderr.write(f"ERROR-> WRONG TYPES CONCAT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)

        if (not isinstance(FRAMES.getVal(arg2), str) and FRAMES.getVal(arg2) is not None) or (not isinstance(FRAMES.getVal(arg3), str) and FRAMES.getVal(arg3) is not None):
            sys.stderr.write(f"ERROR-> WRONG TYPES CONCAT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.setVal2(arg1, FRAMES.getVal(arg2) + FRAMES.getVal(arg3))
        pass

    def STRLEN(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) != 'string': #or not FRAMES.getVal(arg2):
            sys.stderr.write(f"ERROR-> WRONG TYPE STRLEN")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.setVal2(arg1, len(FRAMES.getVal(arg2)))
        pass

    def GETCHAR(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if (not isinstance(FRAMES.getVal(arg2), str) or FRAMES.getType(arg2) != 'string') or (not isinstance(FRAMES.getVal(arg3), int) or FRAMES.getType(arg3) != 'int'):
            sys.stderr.write(f"ERROR-> WRONG TYPES GETCHAR")
            sys.exit(ExitCodes.RTEWrongType)
        arg2string = FRAMES.getVal(arg2)
        index = FRAMES.getVal(arg3)
        if index >= len(arg2string) or index < 0:
            sys.stderr.write(f"ERROR-> INVALID INDEX GETCHAR")
            sys.exit(58)
        FRAMES.setVal2(arg1, arg2string[index])
        pass

    def SETCHAR(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getVal(arg1) == customNil or FRAMES.getVal(arg3) == customNil:
            sys.stderr.write(f"ERROR-> WRONG TYPES SETCHAR")
            sys.exit(ExitCodes.RTEWrongType)
        if FRAMES.getType(arg1) != 'string' or FRAMES.getType(arg2) != 'int' or FRAMES.getType(arg3) != 'string':
            sys.stderr.write(f"ERROR-> WRONG TYPES SETCHAR")
            sys.exit(ExitCodes.RTEWrongType)
        if not isinstance(FRAMES.getVal(arg1), str) or not isinstance(FRAMES.getVal(arg2), int) or not isinstance(
                FRAMES.getVal(arg3), str):
            sys.stderr.write(f"ERROR-> WRONG TYPES SETCHAR")
            sys.exit(ExitCodes.RTEWrongType)
        if not FRAMES.getVal(arg3):
            sys.stderr.write(f"ERROR-> EMPTY STRING")
            sys.exit(58)

        arg1string = FRAMES.getVal(arg1)
        index = FRAMES.getVal(arg2)
        char = FRAMES.getVal(arg3)[0]
        if index >= len(arg1string) or index < 0:
            sys.stderr.write(f"ERROR-> INVALID INDEX SETCHAR")
            sys.exit(58)
        arg1string = list(arg1string)
        arg1string[index] = char
        arg1string = "".join(arg1string)
        FRAMES.setVal2(arg1, arg1string)
        pass

    def TYPE(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        FRAMES.checkIfExists(arg1.text)

        if arg2.attrib['type'] == 'var':
            FRAMES.checkIfExists(arg2.text)
            FRAMES.frameType(arg2.text)
            if FRAMES.currentFrame[arg2.text[3:]] is None:
                FRAMES.setVal2(arg1, '')
            elif FRAMES.currentFrame[arg2.text[3:]] == customNil:
                FRAMES.setVal2(arg1, 'nil')
            elif isinstance(FRAMES.currentFrame[arg2.text[3:]], bool):
                FRAMES.setVal2(arg1, 'bool')
            elif isinstance(FRAMES.currentFrame[arg2.text[3:]], int):
                FRAMES.setVal2(arg1, 'int')
            else:
                FRAMES.setVal2(arg1, 'string')
            return
        FRAMES.setVal2(arg1, FRAMES.getType(arg2))
        pass

    def JUMP(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        LABELS.getLabPos(arg1.text)
        INST.currentPosition = LABELS.getLabPos(arg1.text)
        pass

    def JUMPIFEQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if arg1.attrib['type'] != 'label':
            sys.stderr.write(f"ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)
        LABELS.getLabPos(arg1.text)
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            if FRAMES.getVal(arg2) == FRAMES.getVal(arg3):
                INST.currentPosition = LABELS.getLabPos(arg1.text)
            return

        if FRAMES.getType(arg2) != FRAMES.getType(arg3):
            sys.stderr.write(f"ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)

        if FRAMES.getVal(arg2) == FRAMES.getVal(arg3):
            INST.currentPosition = LABELS.getLabPos(arg1.text)
        return

    def JUMPIFNEQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if arg1.attrib['type'] != 'label':
            sys.stderr.write(f"ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)
        LABELS.getLabPos(arg1.text)
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            if FRAMES.getType(arg2) != FRAMES.getType(arg3):
                INST.currentPosition = LABELS.getLabPos(arg1.text)
            return
        if FRAMES.getType(arg2) != FRAMES.getType(arg3):
            sys.stderr.write(f"ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)

        if FRAMES.getVal(arg2) != FRAMES.getVal(arg3):
            INST.currentPosition = LABELS.getLabPos(arg1.text)
        return

    def EXIT(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        if not isinstance(FRAMES.getVal(arg1), int) or FRAMES.getType(arg1) != 'int':
            sys.stderr.write(f"ERROR-> WRONG TYPES EXIT")
            sys.exit(ExitCodes.RTEWrongType)
        if 0 <= FRAMES.getVal(arg1) <= 49:
            sys.exit(FRAMES.getVal(arg1))
        else:
            sys.exit(57)

    def PUSHS(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        STACK.push(FRAMES.getVal(arg1))
        pass

    def POPS(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        FRAMES.checkIfExists(arg1.text)
        FRAMES.setVal2(arg1, STACK.pop())
        pass

    def CREATEFRAME(self):
        if len(self.instruction) != 0:
            sys.exit(ExitCodes.errorSemantic)
        FRAMES.createFrame()
        pass

    def PUSHFRAME(self):
        if len(self.instruction) != 0:
            sys.exit(ExitCodes.errorSemantic)
        FRAMES.pushframe()
        pass

    def POPFRAME(self):
        if len(self.instruction) != 0:
            sys.exit(ExitCodes.errorSemantic)
        FRAMES.popframe()
        pass

    def CALL(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        try:
            arg1 = self.instruction.find('arg1')
            pos = LABELS.labels[arg1.text]
            self.callPositionStack.append(INST.currentPosition)
            INST.currentPosition = pos
        except:
            sys.exit(52)
        pass

    def RETURN(self):
        if len(self.instruction) != 0:
            sys.exit(ExitCodes.errorSemantic)
        if not self.callPositionStack:
            sys.exit(56)
        INST.currentPosition = self.callPositionStack.pop(len(self.callPositionStack) - 1)

    def READ(self):
        try:
            if len(self.instruction) != 2:
                sys.exit(ExitCodes.errorSemantic)
            FRAMES.checkIfExists(self.instruction.find('arg1').text)
            if self.instruction.find('arg2').attrib['type'] != 'type':
                sys.exit(53)
            if inputFile.list is None:
                value = input()
            else:
                try:
                    value = inputFile.list[inputFile.pos]
                    inputFile.increment()
                except IndexError:
                    value = customNil
            if self.instruction.find('arg2').text == 'int':
                try:
                    value = int(value)
                except ValueError:
                    value = customNil
            elif self.instruction.find('arg2').text == 'bool':
                if value.lower() == 'true':
                    value = bool('xd')
                elif value.lower() == 'false':
                    value = bool()
                else:
                    value = customNil
            elif self.instruction.find('arg2').text == 'string':
                try:
                    value = str(value)
                except:
                    value = customNil
            else:
                sys.stderr.write(f"ERROR-> '{self.instruction.find('arg2').text}' is not correct type")
                sys.exit(53)
            FRAMES.setVal2(self.instruction.find('arg1'), value)
            pass
        except:
            value = customNil
            FRAMES.setVal2(self.instruction.find('arg1'), value)

    def DPRINT(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        val = FRAMES.getVal(self.instruction.find('arg1'))
        if isinstance(val, bool):
            if val:
                val = 'true'
            else:
                val = 'false'
        elif val == customNil:
            print('', end='')
            return
        if isinstance(val, str):
            regex_esc = re.compile(r"\\(\d{1,3})")
            hex_esc = re.compile(r"\\x([0-9a-f]{2})")
            val = hex_esc.sub(self.replaceHex, val)
            val = regex_esc.sub(self.replace, val)
        sys.stderr.write(val)
        pass

    def BREAK(self):
        if len(self.instruction) != 0:
            sys.exit(ExitCodes.errorSemantic)
        position = INST.currentPosition
        order = self.instruction.attrib['order']
        TF = FRAMES.temporaryFrame
        LF = FRAMES.localFrame
        GF = FRAMES.globalFrame
        FSTACK = FRAMES.frameStack
        LABS = LABELS.labels
        STCK = STACK.dataStack
        STCKpos = STACK.dataPos
        try:
            nextInst = INST.instructionList[position + 1]
            nextInst = nextInst[1].attrib['opcode']
        except:
            nextInst = None
        sys.stderr.write(f"---DEBUG---\n"
                         f"---START---\n"
                         f"Order: {order}\n"
                         f"Instruction position: {position}\n"
                         f"Next instruction: {nextInst}\n"
                         f"Frames [GF,LF,TF]:\n"
                         f"\t{GF}\n"
                         f"\t{LF}\n"
                         f"\t{TF}\n"
                         f"Frame stack: {FSTACK}\n"
                         f"Stack [current position: {STCKpos}]: {STCK} \n"
                         f"Labels: {LABS}\n"
                         f"----END----\n"
                         f"---DEBUG---\n")
        pass


class LABELS:
    labels = {}

    # Zistí pozíciu návestia
    # param - názov návestia
    @classmethod
    def getLabPos(cls, name):
        if name not in cls.labels:
            sys.stderr.write(f"ERROR-> LABEL DOES NOT EXIST {name}")
            sys.exit(52)
        else:
            return cls.labels[name]


class STACK:
    dataStack = []
    dataPos = -1

    @classmethod
    def pop(cls):
        if not cls.dataStack:
            sys.stderr.write(f"ERROR-> CANNOT POP EMPTY STACK")
            sys.exit(56)
        else:
            return cls.dataStack.pop(len(cls.dataStack) - 1)
        pass

    @classmethod
    def push(cls, val):
        cls.dataStack.append(val)
        pass


def checkFile(file):
    if os.access(file, os.F_OK) == 0:
        sys.stderr.write("ERROR -> File does not exist")
        sys.exit(ExitCodes.errorOpeningFile)
    if os.access(file, os.R_OK) == 0:
        sys.stderr.write("ERROR -> Insufficient permission to READ file")
        sys.exit(ExitCodes.errorOpeningFile)
    if os.access(file, os.W_OK) == 0:
        sys.stderr.write("ERROR -> Insufficient permission to WRITE file")
        sys.exit(ExitCodes.errorWriteToFile)


def init():
    help_me = "Program načte XML reprezentaci programu a tento program s využitím vstupu dle parametrů" \
              "příkazové řádky interpretuje a generuje výstup. Vstupní XML reprezentace je" \
              "ze zdrojového kódu v IPPcode20"
    parser = argparse.ArgumentParser(
        description=help_me)
    parser.add_argument('--source',
                        help='Vstupní soubor s XML reprezentací zdrojového kódu')
    parser.add_argument('--input',
                        help='Soubor se vstupy pro samotnou interpretaci zadaného zdrojového kódu')
    args = parser.parse_args()
    if args.source is not None and args.input is not None:
        checkFile(args.source)
        checkFile(args.input)
        return ET.parse(args.source), args.input
    elif args.source is not None and args.input is None:
        checkFile(args.source)
        return ET.parse(args.source), False
    elif args.source is None and args.input is not None:
        checkFile(args.input)
        return ET.parse(sys.stdin), args.input
    else:
        sys.exit(ExitCodes.errorParam)


class inputFile:
    list = None
    pos = 0

    @classmethod
    def increment(cls):
        cls.pos += cls.pos



try:
    tree, inputSrc = init()
    if inputSrc is not False:
        with open(inputSrc) as f:
            content = f.readlines()
            while '\n' in content:
                content.remove('\n')
        content = [x.strip() for x in content]
        inputFile.list = content
    root = tree.getroot()
except IOError:
    sys.exit(11)
except ET.ParseError:
    sys.exit(31)


if root.tag != "program":
    sys.stderr.write("ERROR -> File's tag is not 'program'")
    sys.exit(32)

if root.attrib['language'].lower() != "ippcode20":
    sys.stderr.write("ERROR -> File's attribute is not 'ippcode20'")
    sys.exit(32)

MAIN.order(root)
