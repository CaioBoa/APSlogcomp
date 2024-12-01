Linguagem Personalizada - Documentação
Bem-vindo à documentação da linguagem personalizada desenvolvida. Esta linguagem foi criada com o objetivo de explorar conceitos de compiladores e interpretadores, utilizando uma sintaxe única baseada em palavras-chave específicas. Este documento fornece uma visão geral do funcionamento da linguagem, incluindo sua sintaxe, semântica e um diagrama EBNF completo.

Sumário
Introdução
Características da Linguagem
Sintaxe da Linguagem
Tipos de Dados
Variáveis
Operadores
Estruturas de Controle
Funções de Entrada e Saída
Gramática EBNF
Exemplos de Código
Exemplo 1: Programa Simples
Exemplo 2: Estruturas Condicionais
Exemplo 3: Loop While
Como Executar um Programa
Considerações Finais
Introdução
Esta linguagem personalizada foi desenvolvida para fins educacionais, permitindo a exploração de conceitos de análise léxica, análise sintática e interpretação de código. A linguagem utiliza palavras-chave em português que representam ações e conceitos comuns em programação, proporcionando uma sintaxe diferenciada e intuitiva.

Características da Linguagem
Sintaxe Baseada em Palavras-Chave Especiais: Utiliza palavras como 'sempre', 'nunca', 'talvez', 'realizar', entre outras.
Tipagem Estática: Variáveis devem ser declaradas com seus tipos antes do uso.
Tipos de Dados Suportados:
Inteiros (int): Representados pela palavra 'aquele'.
Strings (str): Representadas pela palavra 'aquela'.
Operadores Aritméticos e Relacionais: Suporte a operações como adição, subtração, comparações, etc.
Estruturas de Controle:
Condicionais: if, else if, else.
Loops: while.
Comentários: Comentários iniciados com /// são ignorados pelo compilador.
Sintaxe da Linguagem
Tipos de Dados
Inteiro (int):

Declarado com a palavra-chave 'aquele'.
Valores podem ser números literais ou palavras que representam números:
'calma': 0
'raiva': 1
'felicidade': 2
'tristeza': 3
'ansiedade': 4
'nojo': 5
String (str):

Declarado com a palavra-chave 'aquela'.
Valores são definidos entre as palavras 'pois' ou 'nada'.
Variáveis
Declaração de Variáveis:

Sintaxe: TYPE VAR_NAME ENDLINE
Exemplo:
plaintext
Copiar código
aquele x.
Atribuição de Valores:

Sintaxe: TYPE VAR_NAME ASSIGN EXPRESSION ENDLINE
Exemplo:
plaintext
Copiar código
aquele x poder raiva.
Atribuição a Variáveis Existentes:

Sintaxe: PREVAR VAR_NAME ASSIGN EXPRESSION ENDLINE
Exemplo:
plaintext
Copiar código
esse x poder x realizar raiva.
Operadores
Aritméticos:

Adição: 'realizar' (equivale a +)
Subtração: 'tornar' (equivale a -)
Relacionais:

Menor que: 'expressar' (equivale a <)
Maior que: 'encontrar' (equivale a >)
Igual a: 'esquecer' (equivale a ==)
Diferente de: 'proceder' (equivale a !=)
Estruturas de Controle
Condicionais:

If:
plaintext
Copiar código
sempre se ( CONDIÇÃO ) ou
PARA_BLOCO
vez
Else If:
plaintext
Copiar código
talvez se ( CONDIÇÃO ) ou
PARA_BLOCO
vez
Else:
plaintext
Copiar código
nunca
PARA_BLOCO
vez
Loop While:

plaintext
Copiar código
parecer se ( CONDIÇÃO ) ou
PARA_BLOCO
vez
Funções de Entrada e Saída
Imprimir Valores:
Sintaxe: concordar ( EXPRESSION ) ENDLINE
Exemplo:
plaintext
Copiar código
concordar ( pois "Olá, mundo!" pois ).
Gramática EBNF
ebnf
Copiar código
(* Início da gramática *)
program        ::= block ;

block          ::= 'para' statements 'vez' ;

statements     ::= statement statements
                 | ε ;

statement      ::= TYPE VAR ENDLINE
                 | TYPE VAR ASSIGN expression ENDLINE
                 | PREVAR VAR ASSIGN expression ENDLINE
                 | PRINT '(' expression ')' ENDLINE
                 | IF '(' expression ')' block elseif_list
                 | IF '(' expression ')' block elseif_list ELSE block
                 | WHILE '(' expression ')' block ;

elseif_list    ::= ELSEIF '(' expression ')' block elseif_list
                 | ε ;

expression     ::= equality_expression ;

equality_expression ::= relational_expression
                      | equality_expression ( 'esquecer' | 'proceder' ) relational_expression ;

relational_expression ::= additive_expression
                        | relational_expression ( 'expressar' | 'encontrar' ) additive_expression ;

additive_expression ::= additive_expression ( 'realizar' | 'tornar' ) factor
                      | factor ;

factor         ::= NUM
                 | STR
                 | PREVAR VAR
                 | '(' expression ')' ;

(* Definições de tokens e símbolos *)
TYPE           ::= 'aquele'          (* int *)
                 | 'aquela'          (* str *) ;

VAR            ::= identifier ;

ASSIGN         ::= 'poder'
                 | 'dever' ;

PREVAR         ::= 'esse'
                 | 'essa' ;

PRINT          ::= 'concordar' ;

IF             ::= 'sempre' ;

ELSEIF         ::= 'talvez' ;

ELSE           ::= 'nunca' ;

WHILE          ::= 'parecer' ;

OP_ADD         ::= 'realizar' ;

OP_SUB         ::= 'tornar' ;

OP_LT          ::= 'expressar' ;

OP_GT          ::= 'encontrar' ;

OP_EQ          ::= 'esquecer' ;

OP_NE          ::= 'proceder' ;

LPAREN         ::= 'se' ;

RPAREN         ::= 'ou' ;

LBRACE         ::= 'para' ;

RBRACE         ::= 'vez' | '*' ;

ENDLINE        ::= '.' ;

NUM            ::= integer_literal
                 | 'calma'           (* 0 *)
                 | 'raiva'           (* 1 *)
                 | 'felicidade'      (* 2 *)
                 | 'tristeza'        (* 3 *)
                 | 'ansiedade'       (* 4 *)
                 | 'nojo'            (* 5 *) ;

integer_literal ::= digit {digit} ;

digit          ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' ;

identifier     ::= letter { letter | digit | '_' } ;

letter         ::= 'a' | ... | 'z' | 'A' | ... | 'Z' ;

STR            ::= ('pois' | 'nada') string_content ('pois' | 'nada') ;

string_content ::= { any_character_except_pois_or_nada } ;

(* Fim da gramática *)
Exemplos de Código
Exemplo 1: Programa Simples
plaintext
Copiar código
para
    aquele x poder raiva.
    concordar ( esse x ).
vez
Descrição: Declara uma variável x do tipo int com valor 1 e imprime seu valor.

Exemplo 2: Estruturas Condicionais
plaintext
Copiar código
para
    aquele nota poder 85.
    sempre se ( esse nota expressar 60 ) ou
    para
        concordar ( pois "Aprovado" pois ).
    vez
    nunca
    para
        concordar ( pois "Reprovado" pois ).
    vez
vez
Descrição: Verifica se a nota é maior que 60. Se for, imprime "Aprovado", caso contrário, imprime "Reprovado".

Exemplo 3: Loop While
plaintext
Copiar código
para
    aquele contador poder calma.
    parecer se ( esse contador expressar raiva ) ou
    para
        concordar ( esse contador ).
        esse contador poder esse contador realizar raiva.
    vez
vez
Descrição: Inicia um contador em 0 e, enquanto o contador for menor que 1, imprime seu valor e incrementa em 1.

Como Executar um Programa
Escreva o código em um arquivo de texto, por exemplo, meu_programa.txt.

Compile o programa (se necessário), utilizando as ferramentas fornecidas (como o bison, flex, e o compilador C):

bash
Copiar código
bison -d parser.y
flex lexer.l
gcc -o minha_linguagem lex.yy.c parser.tab.c nodes.c symbol_table.c main.c -lfl
Execute o programa passando o arquivo de código como entrada:

bash
Copiar código
./minha_linguagem < meu_programa.txt
Considerações Finais
Esta linguagem personalizada oferece uma maneira única de explorar conceitos fundamentais de compiladores e linguagens de programação. Com uma sintaxe baseada em palavras em português e uma estrutura clara definida pela gramática EBNF, ela serve como uma ferramenta educacional para entender como linguagens de programação são construídas e interpretadas.

Sinta-se à vontade para experimentar, estender e modificar a linguagem para atender às suas necessidades e curiosidades. A exploração e a prática são caminhos excelentes para aprofundar o conhecimento em ciência da computação e desenvolvimento de software.