class Token:
    def __init__(self, tipo, valor):
        self.tipo = tipo
        self.valor = valor


class Tokenizer:
    def __init__(self, source):
        self.source = source
        self.position = 0
        self.next = Token(None, None)
        self.stringBool = False
        self.string = ""
        self.mapping = {
            "calma": ("NUM", 0),  # Operador lógico AND
            "raiva": ("NUM", 1),  # Palavra-chave para número 1
            "felicidade": ("NUM", 2),  # Palavra-chave para número 2
            "tristeza": ("NUM", 3),  # Palavra-chave para número 3
            "ansiedade": ("NUM", 4),  # Palavra-chave para número 4
            "nojo": ("NUM", 5),  # Palavra-chave para número 5
            "poder": ("ASSIGN", "="),  # Palavra-chave para '='
            "dever": ("ASSIGN", "="),  # Operador de multiplicação
            "realizar": ("OP", "+"),  # Operador de adição
            "tornar": ("OP", "-"),  # Operador de subtração
            "expressar": ("OP", "<"),  # Operador de menor que
            "encontrar": ("OP", ">"),  # Operador de maior que
            "esse": ("PREVAR", None),  # Palavra-chave para variável
            "essa": ("PREVAR", None),  # Palavra-chave para variável
            "concordar": ("PRINT", None),  # Comando print
            "aquele": ("TYPE", "int"),  # Tipo para variáveis inteiras
            "aquela": ("TYPE", "str"),  # Tipo para variáveis string
            "se": ("DIV", "("),  # Delimitador de início de expressão
            "ou": ("DIV", ")"),  # Delimitador de fim de expressão
            "para": ("DIV", "{"),  # Delimitador de string
            "vez": ("DIV", "}"),  # Delimitador de string
            "*" : ("DIV", "}"), #Delimitador
            "pois": ("STRDIV", None), # Delimitador de string
            "nada": ("STRDIV", None),  # Palavra-chave para if
            "sempre": ("COND", "if"),  # Palavra-chave para if
            "nunca": ("COND", "else"),  # Palavra-chave para else
            "parecer": ("COND", "while")  # Palavra-chave para while
        }
        self.selectNext()

    def selectNext(self):
        if self.position >= len(self.source):
            self.next = Token("EOF", None)
            return

        while self.position < len(self.source) and self.source[self.position].isspace():
            self.position += 1

        if self.position >= len(self.source):
            self.next = Token("EOF", None)
            return

        c = self.source[self.position]

        # Reconhecer delimitadores de fim de linha
        if c in ".":
            self.next = Token("ENDLINE", c)
            self.position += 1
            return
        # Captura próxima palavra
        word = ""
        while self.position < len(self.source) and not self.source[self.position].isspace():
            word += self.source[self.position]
            self.position += 1

        if self.stringBool:
            if word in self.mapping:
                if self.mapping[word][0] == "STRDIV":
                    self.stringBool = not self.stringBool
                    self.next = Token("STR", self.string)
                    self.string = ""
                    return
            self.string += word
            if self.source[self.position].isspace():
                self.string += " "
            self.selectNext()
            return

        if word[-1] in ".":
            word = word[:-1]
            self.position -= 1

        # Verificar mapeamento
        if word in self.mapping:
            token_type, token_value = self.mapping[word]
            if token_type == "STRDIV":
                self.stringBool = not self.stringBool
                self.selectNext()
                return
            self.next = Token(token_type, token_value)
            return
        elif self.next.tipo == "TYPE" or self.next.tipo == "PREVAR":
            self.next = Token("VAR", word)
            return
        else:
            self.selectNext()
            return
