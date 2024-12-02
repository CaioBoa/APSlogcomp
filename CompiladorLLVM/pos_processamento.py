import sys

def process_file(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            # Remove espaços em branco no início e fim, e ignora linhas vazias
            cleaned_line = line.strip()
            if cleaned_line and not cleaned_line.startswith(','):
                outfile.write(cleaned_line + '\n')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Uso: python pos_processamento.py <arquivo_input.ll> <arquivo_output.ll>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    process_file(input_file, output_file)
