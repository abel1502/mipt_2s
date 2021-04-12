TRANSLIT = {
    'à': 'a',
    'á': 'b',
    'â': 'v',
    'ã': 'g',
    'ä': 'd',
    'å': 'e',
    '¸': 'jo',
    'æ': 'zh',
    'ç': 'z',
    'è': 'i',
    'é': 'jj',
    'ê': 'k',
    'ë': 'l',
    'ì': 'm',
    'í': 'n',
    'î': 'o',
    'ï': 'p',
    'ð': 'r',
    'ñ': 's',
    'ò': 't',
    'ó': 'u',
    'ô': 'f',
    'õ': 'kh',
    'ö': 'c',
    '÷': 'ch',
    'ø': 'sh',
    'ù': 'shh',
    'ú': '"',
    'û': 'y',
    'ü': '\'',
    'ý': 'eh',
    'þ': 'yu',
    'ÿ': 'ya',
}

for rus, eng in list(TRANSLIT.items()):
    TRANSLIT[rus.upper()] = eng.capitalize()

TRANSLIT.update({
    '\u201c': '"',
    '\u201d': '"',
    '\xab': '"',
    '\xbb': '"',
    '\u2026': '...',
    '\u2018': "'",
    '\u2019': "'",
})

TRANSLIT = str.maketrans(TRANSLIT)

with open("dict-en-rus.dic", "r", encoding="windows-1251") as f:
    data = f.read()

data = data.translate(TRANSLIT)

with open("dict-en-rus.dic", "w", encoding="ASCII") as f:
    f.write(data)

