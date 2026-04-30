import json
import unicodedata
from nltk.corpus import wordnet as wn

depth = {
    'e':1,'t':1,
    'a':2,'i':2,'n':2,'m':2,
    's':3,'u':3,'r':3,'w':3,'d':3,'k':3,'g':3,'o':3,
    'h':4,'v':4,'f':4,'l':4,'p':4,'j':4,'b':4,'x':4,'c':4,'y':4,'z':4,'q':4
}

def clean(text):
    return unicodedata.normalize('NFKD', text).encode('ascii','ignore').decode()

def score(word):
    # depth sum + length penalty so long shallow words rank harder than short deep words
    depth_sum = sum(depth.get(c, 3) for c in word)
    length_bonus = len(word) * 0.75  # tune this multiplier if needed
    return depth_sum + length_bonus

def collect_words():
    words = {}
    for syn in wn.all_synsets():
        for lemma in syn.lemmas():
            w = lemma.name().lower()
            if not w.isalpha():
                continue
            if len(w) < 3 or len(w) > 20: # TODO hard bound on word length, I found 15 to be the sweet spot
                continue
            if w in words:
                continue
            syns = wn.synsets(w)
            if not syns:
                continue
            words[w] = clean(syns[0].definition())
    return words

print("Collecting words...")
words = collect_words()
print(f"Total words found: {len(words)}")

# Score everything first
scored = [(w, defn, score(w)) for w, defn in words.items()]
scored.sort(key=lambda x: x[2])

# Percentile bucket — guarantees even split
n = len(scored)
easy_cut = int(n * 0.33)
medium_cut = int(n * 0.66)

output = []
for i, (w, defn, s) in enumerate(scored):
    if i < easy_cut:
        diff = "easy"
    elif i < medium_cut:
        diff = "medium"
    else:
        diff = "hard"
    output.append({"word": w, "difficulty": diff, "definition": defn})

# Shuffle so the file isn't sorted by score
import random
random.shuffle(output)

with open("words.json", "w") as f:
    json.dump(output, f)
