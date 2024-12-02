import sys
import unicodedata

def remove_acentos(input_str):
    nfkd_form = unicodedata.normalize('NFKD', input_str)
    # Remove acentos e também remove vírgulas
    return ''.join([c for c in nfkd_form if not unicodedata.combining(c)])

for line in sys.stdin:
    print(remove_acentos(line), end='')
