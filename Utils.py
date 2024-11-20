from abc import ABC, abstractmethod

class PrePro:
    @staticmethod
    def filter(source):
        return source.lower()

class variable:
    def __init__(self, type, value):
        self.type = type
        self.value = value

class Node(ABC):
    def __init__(self, value):
        self.value = value
        self.children = []

    @abstractmethod
    def Evaluate(self, symbol_table):
        pass

class Assignment(Node):
    def __init__(self, var_node, value_node=None, var_type=None):
        super().__init__("=")
        self.var_node = var_node       # Nó da variável (Var)
        self.value_node = value_node   # Nó do valor (pode ser uma expressão)
        self.var_type = var_type       # Tipo da variável (None se não for declaração)

    def Evaluate(self, symbol_table):
        var_name = self.var_node.value
        # Avalia o valor, se existir
        if self.value_node is not None:
            value = self.value_node.Evaluate(symbol_table)
        else:
            value = variable(self.var_type, None)

        if self.var_type:
            # Declaração de variável
            if symbol_table.exists_in_current_scope(var_name):
                raise NameError(f"Variável '{var_name}' já declarada no escopo atual")
            # Se o tipo não corresponder ao valor, levanta um erro
            if value.type and value.type != self.var_type:
                raise TypeError(f"Tipo incompatível em atribuição para '{var_name}'")
            var = variable(self.var_type, value.value)
            symbol_table.assign(var_name, var)
        else:
            # Atribuição a variável existente
            if not symbol_table.exists(var_name):
                raise NameError(f"Variável '{var_name}' não declarada")
            existing_var = symbol_table.get(var_name)
            # Verifica se o tipo do novo valor corresponde ao da variável
            if existing_var.type != value.type:
                raise TypeError(f"Tipo incompatível em atribuição para '{var_name}'")
            existing_var.value = value.value
            symbol_table.set(var_name, existing_var)

class IntVal(Node):
    def __init__(self, value):
        super().__init__(value)

    def Evaluate(self, symbol_table):
        return variable("int", self.value)

class StrVal(Node):
    def __init__(self, value):
        super().__init__(value)

    def Evaluate(self, symbol_table):
        return variable("str", self.value)

class Var(Node):
    def __init__(self, value):
        super().__init__(value)

    def Evaluate(self, symbol_table):
        return symbol_table.get(self.value)

class Print(Node):
    def __init__(self, expr_node):
        super().__init__("Falei")
        self.children = [expr_node]

    def Evaluate(self, symbol_table):
        value = self.children[0].Evaluate(symbol_table)
        print(value.value)

class Block(Node):
    def __init__(self):
        super().__init__("block")
        self.children = []

    def Evaluate(self, symbol_table):
        result = None
        for child in self.children:
            result = child.Evaluate(symbol_table)
        return result

class NoOp(Node):
    def __init__(self):
        super().__init__(None)

    def Evaluate(self, symbol_table):
        return None
