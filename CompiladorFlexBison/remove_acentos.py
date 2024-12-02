import sys
import unicodedata

def remove_acentos(input_str):
    nfkd_form = unicodedata.normalize('NFKD', input_str)
    return ''.join([c for c in nfkd_form if not unicodedata.combining(c)])

for line in sys.stdin:
    print(remove_acentos(line), end='')