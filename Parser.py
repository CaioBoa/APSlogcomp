from Tokenizer import Tokenizer
from Utils import IntVal, Var, Block, Assignment, Print, StrVal, NoOp

class Parser:
    def __init__(self, tokenizer):
        self.tokenizer = tokenizer

    def parseFactor(self):
        token = self.tokenizer.next
        if token.tipo == "NUM":
            self.tokenizer.selectNext()
            return IntVal(token.valor)
        elif token.tipo == "STR":
            self.tokenizer.selectNext()
            return StrVal(token.valor)
        elif token.tipo == "VAR":
            var_name = token.valor
            self.tokenizer.selectNext()
            return Var(var_name)
        elif token.tipo == "PREVAR":
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "VAR":
                var_name = self.tokenizer.next.valor
                self.tokenizer.selectNext()
                return Var(var_name)
            else:
                raise ValueError("Nome de variável esperado após '$'")
        else:
            raise ValueError("Número, string ou variável esperados")

    def parseStatement(self):
        if self.tokenizer.next.tipo == "TYPE":
            var_type = self.tokenizer.next.valor
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "VAR":
                var_name = self.tokenizer.next.valor
                self.tokenizer.selectNext()
                if self.tokenizer.next.tipo == "ASSIGN":
                    self.tokenizer.selectNext()
                    value_node = self.parseExpression()
                    if self.tokenizer.next.tipo == "ENDLINE":
                        self.tokenizer.selectNext()
                        return Assignment(Var(var_name), value_node, var_type)
                    else:
                        raise ValueError("',' ou '.' esperado após atribuição")
                else:
                    raise ValueError("'=' esperado após nome da variável")
            else:
                raise ValueError("Nome de variável esperado após tipo")
        elif self.tokenizer.next.tipo == "PRINT":
            # Comando print
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "(":
                self.tokenizer.selectNext()
                expression_node = self.parseExpression()
                if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == ")":
                    self.tokenizer.selectNext()
                    if self.tokenizer.next.tipo == "ENDLINE":
                        self.tokenizer.selectNext()
                        return Print(expression_node)
                    else:
                        raise ValueError("',' ou '.' esperado após 'print'")
                else:
                    raise ValueError("')' esperado após expressão em 'print'")
            else:
                raise ValueError("'(' esperado após 'print'")
        else:
            raise ValueError("Declaração de variável ou comando 'print' esperado")

    def parseExpression(self):
        return self.parseFactor()  # Para simplificação, usamos apenas fatores por enquanto

    def parseBlock(self):
        block = Block()
        while self.tokenizer.next.tipo != "EOF":
            statement = self.parseStatement()
            block.children.append(statement)
        return block
    
    def parseProgram(self):
        program = self.parseBlock()
        if self.tokenizer.next.tipo != "EOF":
            raise ValueError("Token inesperado após o programa")
        return program

    def run(self, source):
        # Inicializa o Tokenizer e analisa o programa
        self.tokenizer = Tokenizer(source)
        return self.parseProgram()

