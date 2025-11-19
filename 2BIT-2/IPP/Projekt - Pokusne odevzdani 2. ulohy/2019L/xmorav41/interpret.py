#!/usr/bin/env python3
import sys
import os
import re
import xml.etree.ElementTree as ET


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


'''
class Opcodes:
    MOVE = 1
    CREATEFRAME = 2
    PUSHFRAME = 3
    POPFRAME = 4
    DEFVAR = 5
    CALL = 6
    RETURN = 7
    PUSHS = 8
    POPS = 9
    ADD = 10
    SUB = 11
    MUL = 12
    IDIV = 13
    LT = 14
    GT = 15
    EQ = 16
    AND = 17
    OR = 18
    NOT = 19
    INT2CHAR = 20
    STRI2INT = 21
    READ = 21
    WRITE = 22
    CONCAT = 23
    STRLEN = 24
    GETCHAR = 25
    SETCHAR = 26
    TYPE = 27
    LABEL = 28
    JUMP = 29
    JUMPIFEQ = 30
    JUMPIFNEQ = 31
    EXIT = 32
    DPRINT = 33
    BREAK = 34
'''

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

varTypes = ['string', 'bool', 'int', 'nil']

# :^)
customNil = '#(X}k8ge3X'


class FRAMES:
    globalFrame = {}
    localFrame = None
    temporaryFrame = None
    frameStack = []
    currentFrame = None

    @classmethod
    def getVal(cls, arg):
        if cls.isItVar(arg):
            cls.checkIfExists(arg.text)
            if cls.currentFrame[arg.text[3:]] is None:
                print("ERROR-> VAR HAS NO VALUE", arg.text)
                sys.exit(ExitCodes.RTEValueNotExist)
            else:
                return cls.currentFrame[arg.text[3:]]
        else:  # TODO check hodnoty a type
            if arg.attrib['type'] == 'int':
                return int(arg.text)
            if arg.attrib['type'] == 'nil':
                return customNil
            if arg.attrib['type'] == 'bool':
                if arg.text == 'true':
                    return bool('xd')
                else:
                    return bool()
            if arg.attrib['type'] == 'string':
                return arg.text

    @classmethod
    def getType(cls, arg):
        if arg.attrib['type'] == 'var':
            tmpVal = cls.getVal(arg)
            if isinstance(tmpVal, int):
                return 'int'
            elif tmpVal == customNil:
                return customNil
            elif isinstance(tmpVal, str):
                return 'string'
            elif isinstance(tmpVal, bool):
                return 'bool'
        elif arg.attrib['type'] in varTypes:
            return arg.attrib['type']
        else:
            print("ERROR-> WRONG TYPE ", arg.attrib['type'])
            sys.exit(ExitCodes.errorSynLex)

    @classmethod
    def addVar(cls, name):
        if cls.currentFrame is None:
            print("ERROR-> FRAME NOT INITIALIZED: ", name[:2])
            sys.exit(ExitCodes.RTEFrameNotExist)
        if name[3:] in cls.currentFrame:
            print("ERROR-> REDEFINING VARIABLE ", name)
            sys.exit(ExitCodes.errorSemantic)
        cls.currentFrame[name[3:]] = None
        pass

    @classmethod
    def frameType(cls, var):
        if var[:3].upper() == 'GF@':
            cls.currentFrame = cls.globalFrame
        elif var[:3].upper() == 'LF@':
            cls.currentFrame = cls.localFrame
        elif var[:3].upper() == 'TF@':
            cls.currentFrame = cls.temporaryFrame
        else:
            print("ERROR-> WRONG FRAME in ", var)
            sys.exit(ExitCodes.errorUnexpectedStruct)

    @classmethod
    def checkIfExists(cls, var):
        cls.frameType(var)
        if var[3:] not in cls.currentFrame:
            print("ERROR-> VAR DOES NOT EXIST ", var)
            sys.exit(ExitCodes.RTEVarNotExist)
        else:
            return True

    @classmethod
    def isItVar(cls, arg):
        #
        # uwu = arg.attrib['type']
        if arg.attrib['type'] == 'var':
            return True
        elif arg.attrib['type'] in varTypes:
            return False
        else:
            print("ERROR-> WRONG TYPE ", arg.attrib['type'])
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
        else:  # TODO ošetriť actual hodnoty
            cls.frameType(arg1.text)
            if arg2.attrib['type'] == 'int':
                cls.currentFrame[arg1.text[3:]] = int(arg2.text)
            elif arg2.attrib['type'] == 'bool':
                if arg2.text == 'true':
                    cls.currentFrame[arg1.text[3:]] = bool(arg2.text)  #  Pozor! python to prevedie na velke True/False
                elif arg2.text == 'false':
                    cls.currentFrame[arg1.text[3:]] = bool()  #  Pozor! python to prevedie na velke True/False
                else:
                    sys.exit(ExitCodes.errorSynLex)
            elif arg2.attrib['type'] == 'string':
                cls.currentFrame[arg1.text[3:]] = str(arg2.text)
            elif arg2.attrib['type'] == 'nil':
                cls.currentFrame[arg1.text[3:]] = customNil
            pass

    # Direct value insert
    @classmethod
    def setVal2(cls, arg1, val):
        cls.checkIfExists(arg1.text)
        cls.currentFrame[arg1.text[3:]] = val
        pass


class VARS:
    fullname = None
    value = None
    type = None

    def checkVar(self, fullname):
        #  '/(L|G|T)F@(_|\-|\$|&|%|\*|!|\?|[a-zA-Z])[\S]*/'
        pass

    def setName(self, varName):
        self.fullname = varName
        FRAMES.frameType(self.fullname)
        FRAMES.addVar(self.fullname)

    def getNameVar(self):
        return self.fullname

    def getValVar(self):
        return self.value

    def newVar(self):
        pass


class MAIN:

    @classmethod
    def order(cls, root):
        #   zoradenie všetkých inštrukcií podľa orderu ##
        instructionsRaw = root.findall("./")
        # instructionList = []
        keyList = []
        for elem in instructionsRaw:
            cls.checkArg(elem, len(elem))
            key = elem.attrib['order']
            INST.instructionList.append((int(key), elem))
            keyList.append(int(key))
        # kontrola unikatnosti orderov
        if len(keyList) != len(set(keyList)):
            sys.exit(32)
        INST.instructionList.sort()
        # labels
        labelList = LABELS()
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
        # currentPosition = 0
        pepe = len(INST.instructionList)
        oof = INST.currentPosition
        while INST.currentPosition < len(INST.instructionList):
            # for instruct in INST.instructionList:
            pee = INST.currentPosition
            instruct = INST.instructionList[INST.currentPosition]
            instruct[1].attrib['opcode'] = instruct[1].attrib['opcode'].upper()
            if instruct[1].attrib['opcode'] not in opcodes:
                print("ERROR-> order", instruct[0], ":", instruct[1].attrib['opcode'])
                sys.exit(ExitCodes.errorWrongOpcode)
            cls.opcode = instruct[1]
            instruction = INST(instruct[1])
            instruction.process()
            INST.currentPosition += 1
            # INST.process()

        pass
    @classmethod
    def checkArg(cls, arg, count):
        if not (0 < count < 4):
            sys.exit(32)
        if arg.tag != 'instruction':
            sys.exit(32)
        if 'order' not in arg.attrib or 'opcode' not in arg.attrib:
            sys.exit(32)
        if count == 1:
            if arg.find('arg1') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg1').attrib:
                sys.exit(32)
        if count == 2:
            if arg.find('arg1') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg1').attrib:
                sys.exit(32)
            if arg.find('arg2') is None:
                sys.exit(32)
            elif 'type' not in arg.find('arg2').attrib:
                sys.exit(32)
        if count == 3:
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
        pass


class INST:
    instructionList = []
    currentPosition = 0
    opcode = None
    instruction = None

    def __init__(self, inst):
        self.instruction = inst

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

    def DEFVAR(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        currVar = VARS()
        # pepe = self.instruction.find('arg1').text
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
                print("ERROR-> DIVISION BY ZERO ", self.instruction.attrib['order'], self.instruction.attrib['opcode'])
                sys.exit(ExitCodes.RTEWrongValue)
            suma = int(FRAMES.getVal(arg2)) // int(FRAMES.getVal(arg2))
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
            val = regex_esc.sub(self.replace, val)
        print(val)

    def replace(self, match):
        return chr(int(match.group(1)))

    def LT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            print("ERROR-> WRONG TYPES LT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) == FRAMES.getType(arg3):
            if FRAMES.getVal(arg2) < FRAMES.getVal(arg3):
                FRAMES.setVal2(arg1, bool('xd'))
            else:
                FRAMES.setVal2(arg1, bool())
        else:
            print("ERROR-> WRONG TYPES LT")
            sys.exit(ExitCodes.RTEWrongType)

    def GT(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            print("ERROR-> WRONG TYPES GT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)
        if FRAMES.getType(arg2) == FRAMES.getType(arg3):
            if FRAMES.getVal(arg2) > FRAMES.getVal(arg3):
                FRAMES.setVal2(arg1, bool('xd'))
            else:
                FRAMES.setVal2(arg1, bool())
        else:
            print("ERROR-> WRONG TYPES GT")
            sys.exit(ExitCodes.RTEWrongType)

    def EQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if FRAMES.getType(arg2) == 'nil' or FRAMES.getType(arg3) == 'nil':
            pass
        FRAMES.checkIfExists(arg1.text)

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
            print("ERROR-> WRONG TYPES AND")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.checkIfExists(arg1.text)

        if FRAMES.getVal(arg2) == FRAMES.getVal(arg3):
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
            print("ERROR-> WRONG TYPES OR")
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
            print("ERROR-> WRONG TYPES NOT")
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
        if not isinstance(FRAMES.getVal(arg2), int):
            print("ERROR-> WRONG TYPES INT2CHAR")
            sys.exit(ExitCodes.RTEWrongType)
        try:
            char = chr(FRAMES.getVal(arg2))
        except ValueError:
            print("ERROR-> NOT VALID UNICODE VALUE")
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
        if not isinstance(FRAMES.getVal(arg2), str) or not isinstance(FRAMES.getVal(arg3), int):
            print("ERROR-> WRONG TYPES STRI2INT")
            sys.exit(ExitCodes.RTEWrongType)
        arg2string = FRAMES.getVal(arg2)
        index = FRAMES.getVal(arg3)
        if index >= len(arg2string) or index < 0:
            print("ERROR-> INVALID INDEX STRI2INT")
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
        FRAMES.checkIfExists(arg1.text)
        if not isinstance(FRAMES.getVal(arg2), str) or not isinstance(FRAMES.getVal(arg3), str):
            print("ERROR-> WRONG TYPES CONCAT")
            sys.exit(ExitCodes.RTEWrongType)
        FRAMES.setVal2(arg1, FRAMES.getVal(arg2) + FRAMES.getVal(arg3))
        pass

    def STRLEN(self):
        if len(self.instruction) != 2:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        FRAMES.checkIfExists(arg1.text)
        if not isinstance(FRAMES.getVal(arg2), str):
            print("ERROR-> WRONG TYPE STRLEN")
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
        if not isinstance(FRAMES.getVal(arg2), str) or not isinstance(FRAMES.getVal(arg3), int):
            print("ERROR-> WRONG TYPES CONCAT")
            sys.exit(ExitCodes.RTEWrongType)
        arg2string = FRAMES.getVal(arg2)
        index = FRAMES.getVal(arg3)
        if index >= len(arg2string) or index < 0:
            print("ERROR-> INVALID INDEX GETCHAR")
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
        if not isinstance(FRAMES.getVal(arg1), str) or not isinstance(FRAMES.getVal(arg2), int) or not isinstance(
                FRAMES.getVal(arg3), str):
            print("ERROR-> WRONG TYPES SETCHAR")
            sys.exit(ExitCodes.RTEWrongType)
        arg1string = FRAMES.getVal(arg1)
        index = FRAMES.getVal(arg2)
        char = FRAMES.getVal(arg3)[0]
        if index >= len(arg1string) or index < 0:
            print("ERROR-> INVALID INDEX SETCHAR")
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
            FRAMES.frameType(arg2.text)
            if FRAMES.currentFrame[arg2.text[3:]] is None:
                FRAMES.setVal2(arg1, 'string')
            elif FRAMES.currentFrame[arg2.text[3:]] == customNil:
                FRAMES.setVal2(arg1, 'nil')
            return
        FRAMES.setVal2(arg1, FRAMES.getType(arg2))
        pass

    def JUMP(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        INST.currentPosition = LABELS.getLabPos(arg1.text)
        pass

    def JUMPIFEQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if arg1.attrib['type'] != 'label':
            print("ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)

        if FRAMES.getType(arg2) == 'nil' and FRAMES.getType(arg3) == 'nil':
            INST.currentPosition = LABELS.getLabPos(arg1.text)
            return

        if FRAMES.getVal(arg2) == FRAMES.getVal(arg3):
            INST.currentPosition = LABELS.getLabPos(arg1.text)
        else:
            pass
        pass

    def JUMPIFNEQ(self):
        if len(self.instruction) != 3:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        arg2 = self.instruction.find('arg2')
        arg3 = self.instruction.find('arg3')
        if arg1.attrib['type'] != 'label':
            print("ERROR-> WRONG TYPES JUMPIFEQ")
            sys.exit(ExitCodes.RTEWrongType)

        if FRAMES.getType(arg2) == 'nil' and FRAMES.getType(arg3) == 'nil':
            return

        if FRAMES.getVal(arg2) != FRAMES.getVal(arg3):
            INST.currentPosition = LABELS.getLabPos(arg1.text)
        else:
            pass
        pass

    def EXIT(self):
        if len(self.instruction) != 1:
            sys.exit(ExitCodes.errorSemantic)
        arg1 = self.instruction.find('arg1')
        if not isinstance(FRAMES.getVal(arg1), int):
            print("ERROR-> WRONG TYPES EXIT")
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


class LABELS:
    labels = {}

    @classmethod
    def getLabPos(cls, name):
        if name not in cls.labels:
            print("ERROR-> LABEL DOES NOT EXIST", name)
            sys.exit(52)
        else:
            return cls.labels[name]


class STACK:
    dataStack = []
    dataPos = -1

    @classmethod
    def pop(cls):
        if not cls.dataStack:
            print("ERROR-> CANNOT POP EMPTY STACK")
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
    if len(sys.argv) == 1:
        ##print("STDIN")
        return ET.parse(sys.stdin)
    elif sys.argv[1][:9] == "--source=" and len(sys.argv) == 2:
        ##print("SOURCE")
        checkFile(sys.argv[1][9:])
        return ET.parse(sys.argv[1][9:])
    elif sys.argv[1][:8] == "--input=" and len(sys.argv) == 2:
        ##print("INPUT")
        checkFile(sys.argv[1][8:])
        return ET.parse(sys.argv[1][8:])
    elif sys.argv[1] == "--help" and len(sys.argv) == 2:
        print("--help")
        print("Program načte XML reprezentaci programu a tento program s využitím vstupu dle parametrů")
        print("příkazové řádky interpretuje a generuje výstup. Vstupní XML reprezentace je")
        print("ze zdrojového kódu v IPPcode20\n\n")
        print("Parametre:")
        print("[--source=FILE]")
        print("vstupní soubor s XML reprezentací zdrojového kódu")
        print("[--input=FILE]")
        print("soubor se vstupy pro samotnou interpretaci zadaného zdrojového kódu")
        print("[bez parametrov]")
        print("Data načítaná ze standardního vstupu\n\n")
        sys.exit(0)
    else:
        sys.exit(ExitCodes.errorParam)

try:
    tree = init()
    root = tree.getroot()
except IOError:
    sys.exit(11)
except ET.ParseError:
    sys.exit(31)

''' 'instruction', 'order', 'opcode', 'arg1', 'arg2', 'arg3', 'type' '''
programAtrib = ['language', 'name', 'description']

if root.tag != "program":
    sys.stderr.write("ERROR -> File's tag is not 'program'")
    sys.exit(32)
#if not all(elem in root.attrib for elem in programAtrib):
#    sys.exit(31)

if root.attrib['language'].lower() != "ippcode20":
    sys.stderr.write("ERROR -> File's attribute is not 'ippcode20'")
    sys.exit(32)


'''
pepe = None
pepe['kek1'] = None
pepe['kek2'] = None
pepe['kek1'] = 42
pepe['kek2'] = '42'
pepe['kek2'] = False
'''

MAIN.order(root)

'''
##  zoradenie všetkých inštrukcií podľa orderu ##
instructionsRaw = root.findall("./")
##instructionList = []
keyList = []
for elem in instructionsRaw:
    key = elem.attrib['order']
    INST.instructionList.append((int(key), elem))
    keyList.append(int(key))
## kontrola unikatnosti orderov
if len(keyList) != len(set(keyList)):
    sys.exit(ExitCodes.errorWrongOpcode)
INST.instructionList.sort()

insOrder = 1

#####
for instruct in INST.instructionList:
    instruct[1].attrib['opcode'] = instruct[1].attrib['opcode'].upper()
    if instruct[1].attrib['opcode'] not in opcodes:
        print("ERROR-> order", instruct[0], ":", instruct[1].attrib['opcode'])
        sys.exit(ExitCodes.errorWrongOpcode)
    INST.process(instruct[1])
    pass
'''
