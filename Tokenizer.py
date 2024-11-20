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
            "pedro": ("NUM", 1),  # Palavra-chave para número 1
            "caio": ("NUM", 2),  # Palavra-chave para número 2
            "pizza": ("NUM", 3),  # Palavra-chave para número 3
            "tende": ("ASSIGN", "="),  # Palavra-chave para '='
            "camelo": ("OP", "+"),  # Operador de adição
            "trump": ("OP", "-"),  # Operador de subtração
            "maluco": ("PREVAR", None),  # Palavra-chave para variável
            "falei": ("PRINT", None),  # Comando print
            "futebol": ("TYPE", "int"),  # Tipo para variáveis inteiras
            "sabado": ("TYPE", "str"),  # Tipo para variáveis string
            "hoje": ("DIV", "("),  # Delimitador de início de expressão
            "canalha": ("DIV", ")"),  # Delimitador de fim de expressão
            "pequeno": ("STRDIV", None) # Delimitador de string
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
        if c in ",.":
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

        if word[-1] in ",.":
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
