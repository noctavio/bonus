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
            if len(w) < 3 or len(w) > 15: # hard bound on word length
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
easy_cut   = int(n * 0.33)
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

# Print score range per tier so you can sanity check
easy_scores   = [s for _, _, s in scored[:easy_cut]]
medium_scores = [s for _, _, s in scored[easy_cut:medium_cut]]
hard_scores   = [s for _, _, s in scored[medium_cut:]]

print(f"\nEasy:   {len(easy_scores)} words, score range {easy_scores[0]:.1f} – {easy_scores[-1]:.1f}")
print(f"Medium: {len(medium_scores)} words, score range {medium_scores[0]:.1f} – {medium_scores[-1]:.1f}")
print(f"Hard:   {len(hard_scores)} words, score range {hard_scores[0]:.1f} – {hard_scores[-1]:.1f}")

print("=== 10 EASIEST ===")
for w, defn, s in scored[:10]:
    print(f"  {w:<20} score: {s:.1f}  ({defn[:50]})")

print("\n=== 10 HARDEST ===")
for w, defn, s in scored[-10:]:
    print(f"  {w:<20} score: {s:.1f}  ({defn[:50]})")