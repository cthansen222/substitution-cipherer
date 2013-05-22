substitution-cipherer
=====================

A simple program that works to either generate or crack substitution ciphered text.

Usage:  cipher -e "<plain text>"   // encrypt some text
        cipher <corpus_file> <random_key_guesses> <swaps_per_key> <ciphered_text> // decrypt some text

The original text that was encrypted should be in the language of the corpus file; that said, the only support is
alphabetic characters.  Other characters are stripped.  Therefore, for ths version, the input text should not have
have spaces or other punctuation in it.
