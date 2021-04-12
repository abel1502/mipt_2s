TRANSLIT = {
    '�': 'a',
    '�': 'b',
    '�': 'v',
    '�': 'g',
    '�': 'd',
    '�': 'e',
    '�': 'jo',
    '�': 'zh',
    '�': 'z',
    '�': 'i',
    '�': 'jj',
    '�': 'k',
    '�': 'l',
    '�': 'm',
    '�': 'n',
    '�': 'o',
    '�': 'p',
    '�': 'r',
    '�': 's',
    '�': 't',
    '�': 'u',
    '�': 'f',
    '�': 'kh',
    '�': 'c',
    '�': 'ch',
    '�': 'sh',
    '�': 'shh',
    '�': '"',
    '�': 'y',
    '�': '\'',
    '�': 'eh',
    '�': 'yu',
    '�': 'ya',
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

