# bonus
`This project was inspired by a toy I saw online` https://www.amazon.com/Professional-Interactive-Learning-Telegraph-Encryption/dp/B0GYMXPN3S?th=1

Using a python library we generated a set of random word and definitions, consider it not part of the assignment but I couldn't find
a csv/json file containing thousands of words and definition so I created my own. 

On program start we display how many words are in the json file, the 10 easiest words to spell vs the 10 most difficult for sake of intuition on 
the difficulty system.

The game is a `morse code spelling game`, you are given a word and a definition (purely for fun) then using the Z and X keys 
you navigate a visual morse binary tree to build the word letter by letter. Z's are dits(.) and X's are dahs(-) 

You can `choose a difficulty` for words based on categorizations determined by two values 1. The length of the word 2. 
The average depth of the individual letters in the word. If a word is long and or the letters are less common (lower in the tree) it is more difficult to spell flawlessly. 

Some basic `stats are also tracked` like your current streak (without resets), highest streak, resets, and words completed
Basic controls are provided on the start up screen 

Since we don't want to load 75k words we just select 999 from each set since the ordering is random it will always be a new order 
