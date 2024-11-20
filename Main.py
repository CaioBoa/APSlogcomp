import sys
from Utils import PrePro, Print
from Parser import Parser
from Tokenizer import Tokenizer
from SymbolTable import SymbolTable

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python main.py arquivo.txt")
        sys.exit(1)

    # Ler e pré-processar o código
    with open(sys.argv[1], 'r') as file:
        codigo = file.read()

    codigo = PrePro.filter(codigo)

    try:
        # Inicializar parser e analisar o código
        parser = Parser(Tokenizer(codigo))
        ast = parser.run(codigo)
        
        # Criar a SymbolTable global
        symbol_table = SymbolTable()

        # Avaliar o AST, passando a tabela de símbolos global
        ast.Evaluate(symbol_table)

    except Exception as e:
        print(f"Erro: {e}", file=sys.stderr)
        sys.exit(1)