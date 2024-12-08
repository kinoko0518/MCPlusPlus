import random

namespace = "MCPP.var"

def random_name(longness:int):
    res = ""
    pattern = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 1 2 3 4 5 6 7 8 9 0".split(" ")
    for i in range(longness):
        res += pattern[random.randint(0, len(pattern)-1)]
    return res

class Scoreboard:
    def __init__(self, var_name:str, scope:"list[str]", mcpp_type:type = None) -> None:
        self.name = var_name
        self.scope = scope
        self.type = mcpp_type
    
    @property
    def mc_name(self):
        return "#MCPP." + (".".join(self.scope) + "." if self.scope else "") + self.name

    def operation(self, operater:str, source):
        if type(source) == int:
            if operater == "+=":
                return self + source
            elif operater == "-=":
                return self - source
            temp = Scoreboard("TEMP", ["SYS"])
            return "\n".join([(temp << source), operate(self, operater, temp)])
        if type(source) == Scoreboard:
            return operate(self, operater, source)

    # 代入
    def __lshift__(self, source) -> str:
        if self.type == None:
            if is_bool(source):
                self.type = bool
            elif is_int(source):
                self.type = int
        
        # ToDo
        # ・LogicalFormulaを計算できるようにする
        if self.type is bool:
            if type(source) == bool:
                return "scoreboard players set {} {} {}".format(self.mc_name, namespace, 1 if source else 0)
        elif self.type is int:
            if type(source) == int:
                return "scoreboard players set {} {} {}".format(self.mc_name, namespace, str(source))
            if type(source) == Scoreboard:
                return operate(self, "=", source)
        else:
            print("Undefined assignment : {} and {}".format(self.type, type(source)))
    
    # 足し算
    def __add__(self, source) -> str:
        if self.type is int:
            if type(source) == int:
                return "scoreboard players add {} {} {}".format(self.mc_name, namespace, str(source))
            if type(source) == Scoreboard:
                return self.operation("+=", source)
    
    # 引き算
    def __sub__(self, source) -> str:
        if type(source) == int:
            if type(source) == int: return "scoreboard players remove {} {} {}".format(self.mc_name, namespace, str(source))
        if type(source) == Scoreboard:
            return self.operation("-=", source)
    
    # 掛け算
    def __mul__(self, source) -> str:
        if is_int(source):
            return self.operation("*=", source)
    
    # 割り算
    def __truediv__(self, source) -> str:
        if is_int(source):
            return self.operation("/=", source)
    
    # 比較演算
    def __eq__(self, other):
        if type(other) is int:
            return "if score {} {} matches {}".format(self.mc_name, namespace, str(other))
        if type(other) is Scoreboard:
            return "if score {} {} = {} {}".format(self.mc_name, namespace, other.mc_name, namespace)
    
    # 比較演算(否定)
    def __ne__(self, other):
        if type(other) is int:
            return "unless score {} {} matches {}".format(self.mc_name, namespace, str(other))
        if type(other) is Scoreboard:
            return "unless score {} {} = {} {}".format(self.mc_name, namespace, other.mc_name, namespace)
    
    # 比較演算(以上)
    def __le__(self, other):
        if type(other) is int:
            return "if score {} {} matches {}..".format(self.mc_name, namespace, str(other))
        if type(other) is Scoreboard:
            return "if score {} {} >= {} {}".format(self.mc_name, namespace, other.mc_name, namespace)
    
    # 比較演算(以下)
    def __ge__(self, other):
        if type(other) is int:
            return "if score {} {} matches ..{}".format(self.mc_name, namespace, str(other))
        if type(other) is Scoreboard:
            return "if score {} {} <= {} {}".format(self.mc_name, namespace, other.mc_name, namespace)
    
    @property
    def freement(self) -> str:
        return "scoreboard players reset {} {}".format(self.mc_name, namespace)


def solve_formula(target:Scoreboard, raw:"list[str|int|Scoreboard]"):
    res:list[str] = []
    for i in range(len(raw)-1):
        if i % 2 == 0:
            if raw[i] == "+":
                res.append(target + raw[i+1])
            elif raw[i] == "-":
                res.append(target - raw[i+1])
            elif raw[i] == "*":
                res.append(target * raw[i+1])
            elif raw[i] == "/":
                res.append(target / raw[i+1])
            elif raw[i] == "%":
                res.append(target % raw[i+1])
    return "\n".join(res)

def is_int(source):
    return type(source) is int or (type(source) is Scoreboard and source.type is int)

def is_bool(source):
    return type(source) is bool or (type(source) is Scoreboard and source.type is bool)

def operate(target:Scoreboard, operation:str, source:Scoreboard):
    return "scoreboard players operation {} {} {} {} {}".format(target.mc_name, namespace, operation, source.mc_name, namespace)