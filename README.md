# CodeTelling - Linguagem de Programação

## APS - Lógica Da Computação

## Por Caio Bôa e Pedro Civita

---

**CodeTelling** é uma linguagem de programação que tem como objetivo proporcionar ao programador a possibilidade de escrever seu código em narrativas. Ou seja, permite escrever o código de forma que conte uma história ou represente um texto escolhido pelo programador. Sua proposta inicial surge da possibilidade de programar de forma quase encriptada em textos literários, possibilitando a escrita de código em meio a um livro, música, poema, etc.

## Funcionamento

O **CodeTelling** opera através de tokens-chave específicos que representam comandos para a linguagem. A partir desses tokens, é gerada uma gramática capaz de ser interpretada pelo interpretador para comandos no computador. O principal diferencial do **CodeTelling** está em ignorar todos os tokens que não são identificados como tokens-chave, sem gerar erros. Isso permite que palavras quaisquer sejam introduzidas em meio aos tokens-chave sem problemas para o interpretador da linguagem.

## Utilização

Foram desenvolvidos três compiladores para a linguagem **CodeTelling**, sendo eles:

- Compilador em Python
- Compilador em C
- Compilador em LLVM

### Compilador em Python

O compilador em Python foi criado como um protótipo para a linguagem, sendo o mais simples dos compiladores desenvolvidos. Por se tratar de um protótipo, ele não possui algumas funcionalidades dos demais compiladores, como a remoção de comentários, interpretação de `else if` entre `if` e `else`, e utilização dos operadores booleanos `==` e `!=`.

Para utilizá-lo, basta executar o comando dentro do diretório */CompiladorPython*:

```bash
python3 Main.py <arquivo_de_entrada>
```

Exemplo:

```bash
python3 Main.py ../TestFiles/teste1.txt
```

### Compilador em C

O compilador em C realiza sua análise léxica em *Flex*, sua análise sintática em *Bison* e a geração de código em *C*. O compilador em C possui todas as funcionalidades implementadas para o CodeTelling até o momento.

Lembre-se de utilizar todos os comandos dentro do diretório */CompiladorFlexBison*.

Para sua utilização, caso o mesmo ainda não esteja compilado, primeiramente utilizar o seguinte comando:

```bash
make
```

Em seguida, para execução de um arquivo de entrada, utilizar o seguinte comando:

```bash
python3 remove_acentos.py < arquivo de entrada > | ./code_tel
```

Exemplo:

```bash
python3 remove_acentos.py < ../TestFiles/teste1.txt | ./code_tel
```

Tal compilador exige um pré processamento para correto funcionamento até a presente versão, que foi implementado utilizando python.

### Compilador em LLVM

O compilador em LLVM realiza, assim como o compilador em C, sua análise léxica em *Flex* e sua análise sintática em *Bison*, porém realiza a geração de código em *LLVM*. O compilador em LLVM possui todas as funcionalidades implementadas para o CodeTelling até o momento e representa a versão final do projeto de compiladores.

Lembre-se de utilizar todos os comandos dentro do diretório */CompiladorLLVM*.

Assim como o compilador em C, o mesmo necessita de um pré processamento para funcionamento e compilação utilizando o comando make:

```bash
make
```

Em seguida, para execução de um arquivo de entrada, utilizar o seguinte comando:

```bash
python3 remove_acentos.py < arquivo de entrada > | ./code_tel > output.ll
```

Exemplo:

```bash
python3 remove_acentos.py < ../TestFiles/teste1.txt | ./code_tel > output.ll
```

Por se tratar de um compilador em LLVM, o mesmo gera um arquivo de saída em LLVM, que pode ser compilado em qualquer máquina que possua o LLVM instalado, porém antes disso é necessário realizar um pós processamento no arquivo output.ll, uma vez que alguns caracteres especiais do texto podem interferir na geração do código LLVM.

```bash
python3 pos_processamento.py output.ll output_cleaned.ll
```

Com arquivo de saída limpo, agora ele pode ser executado em qualquer máquina que possua o LLVM instalado, utilizando o seguinte comando:

```bash
lli output_cleaned.ll
```

## Regras da linguagem

Alguns pontos importantes para descrição da versão atual da linguagem **CodeTelling** são:

1) A linguagem suporta apenas comentários ao final do código, sem comentários entre linhas.
2) A linguagem não suporta uso de acentos e virgulas em strings e varíaveis, embora as mesmas possam ser utilizadas como tokens não identificados.

### Descrição da Gramática da Linguagem

O alfabeto da linguagem **CodeTelling** essencialmente representa todas as palavras possíveis de serem utilizadas em texto, uma vez que nenhuma palavra essencialmente será recusada pelo analisador léxico, porém segue abaixo as palavras utilizadas como tokens chave para a linguagem:

{ calma, raiva, felicidade, tristeza, ansiedade, nojo, poder, dever, realizar, tornar, expressar, encontrar, esquecer, proceder, esse, essa, concordar, aquele, aquela, se, ou, para, *, vez, sempre, nunca, talvez, parecer, pois, nada, [0-9] }

Através desses tokens é possível montar qualquer código em **CodeTelling** seguindo a gramática da linguagem, que se assemelha muito a gramática da linguagem C, seguindo a seguinte **EBNF**:

```EBNF
<programa> ::= <bloco>

<bloco> ::= "para" <declarações> "vez"|"*"

<declarações> ::= <declarações> <declaração> | ε

<declaração> ::= <declaração_de_variável> "."
               | <declaração_de_variável_com_inicialização> "."
               | <atribuição_de_variável> "."
               | <declaração_print> "."
               | <declaração_if>
               | <declaração_while>

<declaração_de_variável> ::= <tipo> <variável>

<declaração_de_variável_com_inicialização> ::= <tipo> <variável> <ASSIGN> <expressão>

<atribuição_de_variável> ::= <PREVAR> <variável> <ASSIGN> <expressão>

<declaração_print> ::= "concordar" "se" <expressão> "ou"

<declaração_if> ::= "sempre" "se" <expressão> "ou" <bloco> <lista_else_if>

<lista_else_if> ::= "talvez" "se" <expressão> "ou" <bloco> <lista_else_if>
                  | "nunca" <bloco>
                  | ε

<declaração_while> ::= "parecer" "se" <expressão> "ou" <bloco>

<expressão> ::= <expressão_de_igualdade>

<expressão_de_igualdade> ::= <expressão_de_igualdade> ("esquecer" | "proceder") <expressão_relacional>
                           | <expressão_relacional>

<expressão_relacional> ::= <expressão_relacional> ("expressar" | "encontrar") <expressão_aditiva>
                          | <expressão_aditiva>

<expressão_aditiva> ::= <expressão_aditiva> ("realizar" | "tornar") <termo>
                      | <termo>

<termo> ::= <fator>

<fator> ::= <NUM> 
          | <STR> 
          | <referência_de_variável> 
          | "se" <expressão> "ou"

<referência_de_variável> ::= "esse" <variável>
                           | "essa" <variável>

<tipo> ::= "aquele" | "aquela"

<PREVAR> ::= "esse" | "essa"

<ASSIGN> ::= "poder" | "dever"

<variável> ::= <identificador>

<identificador> ::= [a-zA-Z_][a-zA-Z0-9_]*

<NUM> ::= "calma"
        | "raiva"
        | "felicidade"
        | "tristeza"
        | "ansiedade"
        | "nojo"
        | <dígito>+

<STR> ::= "pois"|"nada" <conteúdo_da_string> "pois"|"nada"

<conteúdo_da_string> ::= <caractere> | <caractere> <conteúdo_da_string>

<caractere> ::= qualquer caracter exceto "pois" e "nada"
```

## Guia de Programação em CodeTelling

Para facilitar a introdução ao **CodeTelling** é valido realizar um paralelo com a linguagem C, uma vez que a linguagem **CodeTelling** se assemelha muito a linguagem C em sua gramática e estrutura de programação.

Para tal primeiramente descreveremos os tokens da linguagem **CodeTelling** e sua equivalência em C:

Tokens da Linguagem **CodeTelling** e sua Equivalência em C
A seguir, uma tabela que mapeia os tokens utilizados na linguagem CodeTelling para seus equivalentes na linguagem C:

| **Token CodeTelling** | **Descrição**                          | **Equivalente em C**                                     |
|-----------------------|----------------------------------------|----------------------------------------------------------|
| calma                 | Valor numérico 0                        | `0`                                                      |
| raiva                 | Valor numérico 1                        | `1`                                                      |
| felicidade            | Valor numérico 2                        | `2`                                                      |
| tristeza              | Valor numérico 3                        | `3`                                                      |
| ansiedade             | Valor numérico 4                        | `4`                                                      |
| nojo                  | Valor numérico 5                        | `5`                                                      |
| poder / dever         | Operadores de atribuição                | `=`                                                      |
| realizar              | Operador de adição                      | `+`                                                      |
| tornar                | Operador de subtração                   | `-`                                                      |
| expressar             | Operador de menor que                   | `<`                                                      |
| encontrar             | Operador de maior que                   | `>`                                                      |
| esquecer              | Operador de igualdade                   | `==`                                                     |
| proceder              | Operador de desigualdade                | `!=`                                                     |
| esse / essa           | Prefixo para referência de variáveis    | Nenhum equivalente direto; usado para referenciar variáveis |
| aquele / aquela       | Tipos de dados int e str                | `int` e `char*` (para strings)                           |
| sempre                | Estrutura condicional if                | `if`                                                     |
| talvez                | Estrutura condicional else if           | `else if`                                                |
| nunca                 | Estrutura condicional else              | `else`                                                   |
| parecer               | Estrutura de repetição while            | `while`                                                  |
| concordar             | Função de impressão printf              | `printf`                                                 |
| se                    | Parêntese esquerdo                      | `(`                                                      |
| ou                    | Parêntese direito                       | `)`                                                      |
| para                  | Chave esquerda                          | `{`                                                      |
| vez / *               | Chave direita                           | `}`                                                      |
| ///                   | Comentário                              | `//`                                                     |
| pois / nada           | Delimitadores de strings                 | `"`                                                      |
| NUM                   | Literal numérico (inteiro)              | Inteiros (`0`, `1`, etc.)                                |
| VAR                   | Identificador de variável               | Nomes de variáveis (`x`, `y`)                             |
| STR                   | Literal de string                       | Strings (`"gato"`, `"cachorro"`)                          |
| ENDLINE               | Final de declaração                     | `;`                                                      |

Em relação a **NUM**, se trata de qualquer valor numérico inserido no código, sendo automaticamente convertido para um token de valor inteiro.

Para **VAR**, se trata de qualquer palavra inserida logo depois de um identificador de varíavel, sendo ela identificada como uma varíavel.

Se tratando de **STR**, se trata de tudo que for inserido entre tokens de delimitação de strings, como pois e nada.

A partir de tais tokens é possível realizar a escrita de um código em **CodeTelling**, seguindo a gramática da linguagem e utilizando os tokens chave para a escrita do código.

```CodeTelling
Para aquele x poder nojo.
Sempre se esse x encontrar tristeza ou
Para concordar se nada PRINT pois ou. Vez nunca
Para concordar se nada NOP pois ou. Vez *
```

Pode ser traduzido para o seguinte código em C:

```C
{
    int x = 5;
    if (x > 3)
    {
        printf("PRINT");
    }
    else
    {
        printf("NOP");
    }
}
```

Porém a mágica da linguagem **CodeTelling** esta em sua capacidade de ser escrita em meio a diversos tipos textuais, como poesias, músicas, livros, etc. Permitindo que o programador escreva seu código em meio a um texto qualquer, por exemplo:

```CodeTelling
Para que aquele homem viva em plenitude ele tem o poder de fazer o que for necessário a fim de buscar a felicidade.
Esse homem dia sim dia não tem o dever de tentar, esse homem não deve desistir até realizar que não existe futuro em sua raiva.
Portanto, concordar que se esse homem tem forças pra levantar ele deve ao menos tentar pode ser visto como um fato ou necessidade. *
```

O código acima pode ser traduzido para o seguinte código em C:

```C
{
    int homem = 2;
    homem = homem + 1;
    printf(homem)
}
```
## Vídeo de Explicação

[Assista ao vídeo demonstrativo aqui](https://youtu.be/Gn_bO5Hm15M?si=v1hILJgHUHfdV_6z)

Neste vídeo, apresentamos uma demonstração abrangente da **CodeTelling**, detalhando todo o processo de desenvolvimento e as funcionalidades da linguagem, assim como aplicações e demonstrações de uso.

## Contato

Caio Bôa - caioob - [caioob@al.insper.edu.br](mailto:caioob@al.insper.edu.br)

Pedro Civita - pedrotpc - [pedrotpc@al.insper.edu.br](mailto:pedrotpc@al.insper.edu.br)

---

