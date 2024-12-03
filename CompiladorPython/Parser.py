from Tokenizer import Tokenizer
from Utils import IntVal, Var, Block, Assignment, Print, StrVal, BinOp, While, If

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
            
        elif token.tipo == "DIV" and token.valor == "(":
            self.tokenizer.selectNext()
            node = self.parseExpression()
            if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == ")":
                self.tokenizer.selectNext()
                return node
            else:
                raise ValueError("')' esperado após expressão")
            
        else:
            raise ValueError("Número, string ou variável esperados")
        
    def parseSummExpression(self):
        result = self.parseFactor()
        while self.tokenizer.next.tipo == "OP" and self.tokenizer.next.valor in "+-":
            op = self.tokenizer.next.valor
            self.tokenizer.selectNext()
            result = BinOp(op, result, self.parseFactor())
        return result
    
    def parseRelationalExpression(self):
        result = self.parseSummExpression()
        while self.tokenizer.next.tipo == "OP" and self.tokenizer.next.valor in ["<", ">", "<=", ">=", "==", "!="]:
            op = self.tokenizer.next.valor
            self.tokenizer.selectNext()
            result = BinOp(op, result, self.parseSummExpression())
        return result

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
                    if self.tokenizer.next.tipo == "ENDLINE":
                        self.tokenizer.selectNext()
                        return Assignment(Var(var_name), None, var_type)
            else:
                raise ValueError("Nome de variável esperado após tipo")
            
        elif self.tokenizer.next.tipo == "PREVAR":
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "VAR":
                var_name = self.tokenizer.next.valor
                self.tokenizer.selectNext()
                if self.tokenizer.next.tipo == "ASSIGN":
                    self.tokenizer.selectNext()
                    value_node = self.parseExpression()
                    if self.tokenizer.next.tipo == "ENDLINE":
                        self.tokenizer.selectNext()
                        return Assignment(Var(var_name), value_node)
                    else:
                        raise ValueError("',' ou '.' esperado após atribuição")
                else:
                    raise ValueError("'=' esperado após nome da variável")
            else:
                raise ValueError("Nome de variável esperado após '$'")
            
        elif self.tokenizer.next.tipo == "PRINT":
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
            
        elif self.tokenizer.next.tipo == "COND" and self.tokenizer.next.valor == "while":
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "(":
                self.tokenizer.selectNext()
                condition = self.parseExpression()
                if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == ")":
                    self.tokenizer.selectNext()
                    block = self.parseBlock()
                    return While(condition, block)
                else:
                    raise ValueError("Erro de sintaxe: ')' esperado após condição do 'while'")
            else:
                raise ValueError("Erro de sintaxe: '(' esperado após 'while'")
            
        elif self.tokenizer.next.tipo == "COND" and self.tokenizer.next.valor == "if":
            self.tokenizer.selectNext()
            if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "(":
                condition = self.parseExpression()
                if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "{":
                    if_block = self.parseBlock()
                else:
                    if_block = self.parseStatement()
                else_block = None
                if self.tokenizer.next.tipo == "COND" and self.tokenizer.next.valor == "else":
                    self.tokenizer.selectNext()
                    if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "{":
                        else_block = self.parseBlock()
                    else:
                        else_block = self.parseStatement()
                return If(condition, if_block, else_block)
            else:
                raise ValueError("Erro de sintaxe: '(' esperado após 'if'")
            
        if self.tokenizer.next.tipo == "ENDLINE":
            self.tokenizer.selectNext()
            
        else:
            raise ValueError("Declaração de variável ou comando 'print' esperado")

    def parseExpression(self):
        return self.parseRelationalExpression()  # Para simplificação, usamos apenas fatores por enquanto

    def parseBlock(self):
        if self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "{":
            self.tokenizer.selectNext()  # Consumir '{'
            block = Block()
            while not (self.tokenizer.next.tipo == "DIV" and self.tokenizer.next.valor == "}"):
                statement = self.parseStatement()
                block.children.append(statement)
            self.tokenizer.selectNext()  # Consumir '}'
            return block
        else:
            raise ValueError("Erro de sintaxe: '{' esperado no início do bloco")
    
    def parseProgram(self):
        program = self.parseBlock()
        if self.tokenizer.next.tipo != "EOF":
            raise ValueError("Token inesperado após o programa")
        return program

    def run(self, source):
        # Inicializa o Tokenizer e analisa o programa
        self.tokenizer = Tokenizer(source)
        return self.parseProgram()

